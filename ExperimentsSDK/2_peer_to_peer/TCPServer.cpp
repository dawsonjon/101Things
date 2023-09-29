#include <string.h>
#include <stdlib.h>
#include <algorithm>

#include "TCPServer.h"


bool TCPServer::listen(uint16_t port)
{
    if(state != CONNECTION_CLOSED)
    {
        DEBUG_printf("Connection not closed\n");
        return false;
    }    

    DEBUG_printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), port);



    struct tcp_pcb *new_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (new_pcb == NULL) {
        DEBUG_printf("failed to create pcb\n");
        state=CONNECTION_CLOSED;
        return false;
    }

    err_t err = tcp_bind(new_pcb, NULL, port);
    if (err != ERR_OK) {
        DEBUG_printf("failed to bind to port %u %u\n", port, err);
        state=CONNECTION_CLOSED;
        return false;
    }

    pcb = tcp_listen_with_backlog(new_pcb, 1);
    if (pcb == NULL) {
        DEBUG_printf("failed to listen\n");
        if (new_pcb!=NULL) {
            tcp_close(new_pcb);
        }
        state=CONNECTION_CLOSED;
        return false;
    }

    server_pcb = pcb;

    tcp_arg(pcb, this);
    tcp_accept(pcb, accept_callback);

    state=CONNECTION_LISTENING;
    return true;
}

bool TCPServer::accept()
{
    while(state == CONNECTION_LISTENING){
#if PICO_CYW43_ARCH_POLL
        // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
        // main loop (not from a timer) to check for WiFi driver or lwIP work that needs to be done.
        cyw43_arch_poll();
#endif
        sleep_ms(1);
    }
    return state == CONNECTION_OPEN;
}


uint16_t TCPServer::receive(uint8_t *dest, uint16_t maxNumBytes) {
    #if PICO_CYW43_ARCH_POLL
            // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
            // main loop (not from a timer) to check for WiFi driver or lwIP work that needs to be done.
            cyw43_arch_poll();
    #endif


    const uint16_t transferBytes = std::min(bytesStored, maxNumBytes);

    if(transferBytes)
    {
        const uint16_t bytesToEndOfBuffer = TCP_WND-readPointer;      
        const uint16_t firstTransferBytes = std::min(bytesToEndOfBuffer, transferBytes);
        const uint16_t secondTransferBytes = transferBytes - firstTransferBytes;

        //make first transfer not beyond end of buffer
        memcpy(dest, receiveBuffer + readPointer, firstTransferBytes);
        tcp_recved(pcb, firstTransferBytes);
        bytesAcknowledged += firstTransferBytes;
        readPointer+=firstTransferBytes; if(readPointer == TCP_WND) readPointer = 0;
        bytesStored-=firstTransferBytes;

        //if more data still need to be transferred, make a second transfer from the start
        if(secondTransferBytes)
        {
            memcpy(dest + firstTransferBytes, receiveBuffer, secondTransferBytes);
            tcp_recved(pcb, secondTransferBytes);
            bytesAcknowledged+=secondTransferBytes;
            readPointer+=secondTransferBytes;
            bytesStored-=secondTransferBytes;
        }
    }

    if(state == CONNECTION_CLOSING)
    {
        DEBUG_printf("waiting for data to complete %u\n", bytesStored);
        if(bytesStored == 0) close();
    }

    //printf("receive %u %u %u %u\n", totalBytesStored, bytesAcknowledged, totalBytesStored - bytesAcknowledged, bytesStored);

    return transferBytes;
}

//close connection
bool TCPServer::close()
{
    DEBUG_printf("closing \n");
    err_t err = ERR_OK;
    if (pcb != NULL) {

        DEBUG_printf("disable callbacks \n");

        //disconnect callback functions
        tcp_arg(pcb, NULL);
        tcp_poll(pcb, NULL, 0);
        tcp_sent(pcb, NULL);
        tcp_recv(pcb, NULL);
        tcp_err(pcb, NULL);

        //close connection
        err = tcp_close(pcb);
        if (err != ERR_OK) {
            DEBUG_printf("close failed %d, calling abort\n", err);
            
            if(pcb != NULL)
            {
                tcp_abort(pcb);
            }
            err = ERR_ABRT;
        }
        pcb = NULL;
    }
    if (server_pcb!=NULL) {
        tcp_arg(server_pcb, NULL);
        tcp_close(server_pcb);
        server_pcb = NULL;
    }
    state=CONNECTION_CLOSED;
    return err == ERR_OK;
}

//This function gets called by LWIP when the server has accepted data that we sent
err_t TCPServer::on_sent(struct tcp_pcb *tpcb, u16_t len) {  
    return ERR_OK;
}

err_t TCPServer::on_recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
   
    if (p == NULL) {
        DEBUG_printf("client disconnected");
        state = CONNECTION_CLOSING;
        return ERR_OK;
    }

    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();

    if (p->tot_len > 0) {        
        uint16_t bytesLeft = p->tot_len;

        //This code assumes that the buffer will always be large enough to hold
        //the received data and that TCP flow control will prevent the buffer
        //from overflowing

        const uint16_t spaceLeft = TCP_WND - bytesStored;
        if(spaceLeft < p->tot_len)
        {
            printf("Insufficient space to store received data %u %u %u\n", spaceLeft, bytesStored, p->tot_len);
            return close();
        } 

        const uint16_t bytesToEndOfBuffer = TCP_WND-writePointer;      
        const uint16_t firstTransferBytes = std::min(bytesToEndOfBuffer, p->tot_len);
        const uint16_t secondTransferBytes = p->tot_len - firstTransferBytes;

        //make first transfer not beyond end of buffer
        pbuf_copy_partial(p, receiveBuffer + writePointer, firstTransferBytes, 0);
        totalBytesStored += firstTransferBytes;
        writePointer+=firstTransferBytes; if(writePointer == TCP_WND) writePointer = 0;
        bytesStored+=firstTransferBytes;

        //if more data still need to be transferred, make a second transfer from the start
        if(secondTransferBytes)
        {
            printf("wrap\n");
            pbuf_copy_partial(p, receiveBuffer, secondTransferBytes, firstTransferBytes);
            totalBytesStored+=secondTransferBytes;
            writePointer+=secondTransferBytes;
            bytesStored+=secondTransferBytes;
        }
    }
    pbuf_free(p);

    DEBUG_printf("on_recv read %u write %u\n", readPointer, writePointer);
    //printf("on_recv %u %u %u %u\n", totalBytesStored, bytesAcknowledged, totalBytesStored - bytesAcknowledged, bytesStored);


    return ERR_OK;
}

//called periodically by lwip when idle
err_t TCPServer::on_poll(struct tcp_pcb *tpcb) {
    DEBUG_printf("tcp_server_poll_fn\n");

    return ERR_OK;
}


//called when lwip encounters an error
void TCPServer::on_err(err_t err) {
    if (err != ERR_ABRT) {
        DEBUG_printf("tcp_client_err_fn %d\n", err);
    }
}

//This function gets called by LWIP when the server has accepted a connection
err_t TCPServer::on_accept(struct tcp_pcb *new_pcb, err_t err) {
    if (err != ERR_OK || new_pcb == NULL) {
        DEBUG_printf("Failure in accept\n");
        state = CONNECTION_CLOSED;
        return ERR_VAL;
    }

    readPointer=0;
    writePointer=0;
    bytesStored=0;
    bytesAcknowledged=0;
    totalBytesStored=0;

    DEBUG_printf("Client connected\n");

    pcb = new_pcb;
    tcp_arg(pcb, this);
    tcp_sent(pcb, server_sent_callback);
    tcp_recv(pcb, server_recv_callback);
    tcp_poll(pcb, server_poll_callback, POLL_TIME_S * 2);
    tcp_err(pcb, server_err_callback);
    state = CONNECTION_OPEN;
    return ERR_OK;
}


//Implement LWIP callack functions
//All the callbacks use the server instance as their argument
//The callbacks all link back to a corresponding handler in the server class

err_t server_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    TCPServer *server = (TCPServer*)arg;
    return server->on_sent(tpcb, len);
}

err_t server_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    TCPServer *server = (TCPServer*)arg;
    return server->on_recv(tpcb, p, err);
}

err_t server_poll_callback(void *arg, struct tcp_pcb *tpcb) {
    TCPServer *server = (TCPServer*)arg;
    return server->on_poll(tpcb);
}

void server_err_callback(void *arg, err_t err) {
    TCPServer *server = (TCPServer*)arg;
    return server->on_err(err);
}

err_t accept_callback(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    TCPServer *server = (TCPServer*)arg;
    return server->on_accept(client_pcb, err);
}

