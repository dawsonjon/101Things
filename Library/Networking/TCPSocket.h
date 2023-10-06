#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define DEBUG_printf
#define POLL_TIME_S 5

enum estate 
{
CONNECTION_CLOSED,
CONNECTION_CONNECTING,
CONNECTION_LISTENING,
CONNECTION_OPEN,
CONNECTION_CLOSING
};

class TCPSocket
{

    public:

    //event handlers
    err_t on_sent(struct tcp_pcb *tpcb, u16_t len);
    err_t on_recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    err_t on_poll(struct tcp_pcb *tpcb);
    void on_err(err_t err);
    err_t on_accept(struct tcp_pcb *cpcb, err_t err);
    err_t on_connected(struct tcp_pcb *tpcb, err_t err);

    //user interface

    //listen for a connection on specified port
    //return true if successful.
    bool listen(uint16_t port);

    //connect to a remote server
    //return true if successful.
    bool open(const char * remote_ip_address, uint16_t port);

    //recieve bytes
    //returns number of bytes recieved which will be maxNumBytes at most
    uint16_t receive(uint8_t dest[], uint16_t maxNumBytes);

    //send bytes
    uint16_t send(uint8_t *src, uint16_t maxNumBytes);

    //close the connection
    bool close();

    bool is_connected() {
        return (state==CONNECTION_OPEN) || (state==CONNECTION_CLOSING);
    }


    private:

    struct tcp_pcb *pcb = NULL;
    struct tcp_pcb *server_pcb = NULL;
    estate state = CONNECTION_CLOSED;
    uint8_t receive_buffer[TCP_WND];
    uint16_t read_pointer=0;
    uint16_t write_pointer=0;
    uint16_t bytes_stored=0;

};

//This function gets called by LWIP when the server has accepted data that we sent
err_t server_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
err_t server_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
err_t server_poll_callback(void *arg, struct tcp_pcb *tpcb);
void server_err_callback(void *arg, err_t err);
err_t accept_callback(void *arg, struct tcp_pcb *client_pcb, err_t err);
err_t connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err);

#endif
