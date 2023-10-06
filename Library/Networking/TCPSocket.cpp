#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include "TCPSocket.h"

bool TCPSocket::open(const char * remote_ip_address, uint16_t port) {
    if (state != CONNECTION_CLOSED) {
      DEBUG_printf("Connection not closed\n");
      return false;
    }

    ip4_addr_t remote_addr;
    ip4addr_aton(remote_ip_address, &remote_addr);
    pcb = tcp_new_ip_type(IP_GET_TYPE(remote_addr));
    if (pcb==NULL) {
        DEBUG_printf("failed to create pcb\n");
        return false;
    }

    read_pointer = 0;
    write_pointer = 0;
    bytes_stored = 0;

    tcp_arg(pcb, this);
    tcp_sent(pcb, server_sent_callback);
    tcp_recv(pcb, server_recv_callback);
    tcp_poll(pcb, server_poll_callback, POLL_TIME_S * 2);
    tcp_err(pcb, server_err_callback);

    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(pcb, &remote_addr, port, connected_callback);
    cyw43_arch_lwip_end();
    if(err != ERR_OK) return false;

    state = CONNECTION_CONNECTING;
    while (state == CONNECTION_CONNECTING) {
  #if PICO_CYW43_ARCH_POLL
      // if you are using pico_cyw43_arch_poll, then you must poll periodically
      // from your main loop (not from a timer) to check for WiFi driver or lwIP
      // work that needs to be done.
      cyw43_arch_poll();
  #endif
      sleep_ms(1);
    }
    return state == CONNECTION_OPEN;

}

bool TCPSocket::listen(uint16_t port) {
  if (state != CONNECTION_CLOSED) {
    DEBUG_printf("Connection not closed\n");
    return false;
  }

  struct tcp_pcb *new_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
  if (new_pcb == NULL) {
    state = CONNECTION_CLOSED;
    return false;
  }

  err_t err = tcp_bind(new_pcb, NULL, port);
  if (err != ERR_OK) {
    state = CONNECTION_CLOSED;
    return false;
  }

  pcb = tcp_listen_with_backlog(new_pcb, 1);
  if (pcb == NULL) {
    if (new_pcb != NULL) {
      tcp_close(new_pcb);
    }
    state = CONNECTION_CLOSED;
    return false;
  }

  server_pcb = pcb;

  tcp_arg(pcb, this);
  tcp_accept(pcb, accept_callback);

  state = CONNECTION_LISTENING;
  while (state == CONNECTION_LISTENING) {
#if PICO_CYW43_ARCH_POLL
    // if you are using pico_cyw43_arch_poll, then you must poll periodically
    // from your main loop (not from a timer) to check for WiFi driver or lwIP
    // work that needs to be done.
    cyw43_arch_poll();
#endif
    sleep_ms(1);
  }
  return state == CONNECTION_OPEN;
}

err_t TCPSocket::on_connected(struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        state = CONNECTION_CLOSED;
        return close();
    }
    state = CONNECTION_OPEN;
    return ERR_OK;
}

uint16_t TCPSocket::receive(uint8_t *dest, uint16_t max_num_bytes) {
#if PICO_CYW43_ARCH_POLL
  // if you are using pico_cyw43_arch_poll, then you must poll periodically from
  // your main loop (not from a timer) to check for WiFi driver or lwIP work
  // that needs to be done.
  cyw43_arch_poll();
#endif

  const uint16_t transfer_bytes = std::min(bytes_stored, max_num_bytes);

  if (transfer_bytes) {
    const uint16_t byte_to_end_of_buffer = TCP_WND - read_pointer;
    const uint16_t first_transfer_bytes =
        std::min(byte_to_end_of_buffer, transfer_bytes);
    const uint16_t second_transfer_bytes =
        transfer_bytes - first_transfer_bytes;

    // make first transfer not beyond end of buffer
    memcpy(dest, receive_buffer + read_pointer, first_transfer_bytes);
    tcp_recved(pcb, first_transfer_bytes);
    read_pointer += first_transfer_bytes;
    if (read_pointer == TCP_WND)
      read_pointer = 0;
    bytes_stored -= first_transfer_bytes;

    // if more data still need to be transferred, make a second transfer from
    // the start
    if (second_transfer_bytes) {
      memcpy(dest + first_transfer_bytes, receive_buffer,
             second_transfer_bytes);
      tcp_recved(pcb, second_transfer_bytes);
      read_pointer += second_transfer_bytes;
      bytes_stored -= second_transfer_bytes;
    }
  }

  if (state == CONNECTION_CLOSING) {
    if (bytes_stored == 0)
      close();
  }

  return transfer_bytes;
}

uint16_t TCPSocket::send(uint8_t *src, uint16_t max_num_bytes) {
#if PICO_CYW43_ARCH_POLL
  // if you are using pico_cyw43_arch_poll, then you must poll periodically from
  // your main loop (not from a timer) to check for WiFi driver or lwIP work
  // that needs to be done.
  cyw43_arch_poll();
#endif
  cyw43_arch_lwip_check();

  const uint16_t transfer_bytes = std::min(tcp_sndbuf(pcb), max_num_bytes);
  err_t err = tcp_write(pcb, src, transfer_bytes, TCP_WRITE_FLAG_COPY);

  if (err == ERR_OK) {
    return transfer_bytes;
  } else {
    return 0; // not enough space to send data, try agin
  }
}

// close connection
bool TCPSocket::close() {
  err_t err = ERR_OK;
  if (pcb != NULL) {

    // disconnect callback functions
    tcp_arg(pcb, NULL);
    tcp_poll(pcb, NULL, 0);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_err(pcb, NULL);

    // close connection
    err = tcp_close(pcb);
    if (err != ERR_OK) {
      if (pcb != NULL) {
        tcp_abort(pcb);
      }
      err = ERR_ABRT;
    }
    pcb = NULL;
  }
  if (server_pcb != NULL) {
    tcp_arg(server_pcb, NULL);
    tcp_close(server_pcb);
    server_pcb = NULL;
  }
  state = CONNECTION_CLOSED;
  return err == ERR_OK;
}

// This function gets called by LWIP when the server has accepted data that we
// sent
err_t TCPSocket::on_sent(struct tcp_pcb *tpcb, u16_t len) { return ERR_OK; }

err_t TCPSocket::on_recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

  if (p == NULL) {
    state = CONNECTION_CLOSING;
    return ERR_OK;
  }

  // this method is callback from lwIP, so cyw43_arch_lwip_begin is not
  // required, however you can use this method to cause an assertion in debug
  // mode, if this method is called when cyw43_arch_lwip_begin IS needed
  cyw43_arch_lwip_check();

  if (p->tot_len > 0) {
    uint16_t bytesLeft = p->tot_len;

    // This code assumes that the buffer will always be large enough to hold
    // the received data and that TCP flow control will prevent the buffer
    // from overflowing

    const uint16_t space_left = TCP_WND - bytes_stored;
    if (space_left < p->tot_len) {
      printf("Insufficient space to store received data %u %u %u\n", space_left,
             bytes_stored, p->tot_len);
      return close();
    }

    const uint16_t byte_to_end_of_buffer = TCP_WND - write_pointer;
    const uint16_t first_transfer_bytes =
        std::min(byte_to_end_of_buffer, p->tot_len);
    const uint16_t second_transfer_bytes = p->tot_len - first_transfer_bytes;

    // make first transfer not beyond end of buffer
    pbuf_copy_partial(p, receive_buffer + write_pointer, first_transfer_bytes,
                      0);
    write_pointer += first_transfer_bytes;
    if (write_pointer == TCP_WND)
      write_pointer = 0;
    bytes_stored += first_transfer_bytes;

    // if more data still need to be transferred, make a second transfer from
    // the start
    if (second_transfer_bytes) {
      pbuf_copy_partial(p, receive_buffer, second_transfer_bytes,
                        first_transfer_bytes);
      write_pointer += second_transfer_bytes;
      bytes_stored += second_transfer_bytes;
    }
  }
  pbuf_free(p);

  return ERR_OK;
}

// called periodically by lwip when idle
err_t TCPSocket::on_poll(struct tcp_pcb *tpcb) { return ERR_OK; }

// called when lwip encounters an error
void TCPSocket::on_err(err_t err) {}

// This function gets called by LWIP when the server has accepted a connection
err_t TCPSocket::on_accept(struct tcp_pcb *new_pcb, err_t err) {
  if (err != ERR_OK || new_pcb == NULL) {
    state = CONNECTION_CLOSED;
    return ERR_VAL;
  }

  read_pointer = 0;
  write_pointer = 0;
  bytes_stored = 0;

  pcb = new_pcb;
  tcp_arg(pcb, this);
  tcp_sent(pcb, server_sent_callback);
  tcp_recv(pcb, server_recv_callback);
  tcp_poll(pcb, server_poll_callback, POLL_TIME_S * 2);
  tcp_err(pcb, server_err_callback);
  state = CONNECTION_OPEN;
  return ERR_OK;
}

// Implement LWIP callack functions
// All the callbacks use the server instance as their argument
// The callbacks all link back to a corresponding handler in the server class
err_t connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
  TCPSocket *server = (TCPSocket *)arg;
  return server->on_connected(tpcb, err);
}

err_t server_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  TCPSocket *server = (TCPSocket *)arg;
  return server->on_sent(tpcb, len);
}

err_t server_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p,
                           err_t err) {
  TCPSocket *server = (TCPSocket *)arg;
  return server->on_recv(tpcb, p, err);
}

err_t server_poll_callback(void *arg, struct tcp_pcb *tpcb) {
  TCPSocket *server = (TCPSocket *)arg;
  return server->on_poll(tpcb);
}

void server_err_callback(void *arg, err_t err) {
  TCPSocket *server = (TCPSocket *)arg;
  return server->on_err(err);
}

err_t accept_callback(void *arg, struct tcp_pcb *client_pcb, err_t err) {
  TCPSocket *server = (TCPSocket *)arg;
  return server->on_accept(client_pcb, err);
}
