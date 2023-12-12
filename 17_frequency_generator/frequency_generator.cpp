#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
// Our assembled program:
#include "stream_bits.pio.h"

uint32_t bit_buffer[256]={
0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,

0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,

0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,

0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

PIO pio = pio0;
uint sm = pio_claim_unused_sm(pio, true);
int nco_dma_ping;
dma_channel_config nco_ping_cfg;
int nco_dma_pong;
dma_channel_config nco_pong_cfg;

void dma_handler() {

    printf("asdfsadf");

    //When a DMA completes, reconfigure it but don't start it yet, wait for DMA chaining to trigger
    if(dma_hw->ints0 & (1u << nco_dma_ping))
    {
      dma_channel_configure(nco_dma_ping, &nco_ping_cfg, bit_buffer, &pio->txf[sm], 256, false);
      dma_hw->ints0 = 1u << nco_dma_ping; //acknowledge interrupt
    }

    if(dma_hw->ints0 & (1u << nco_dma_pong))
    {
      dma_channel_configure(nco_dma_pong, &nco_pong_cfg, bit_buffer, &pio->txf[sm], 256, false);
      dma_hw->ints0 = 1u << nco_dma_pong; //acknowledge interrupt
    }

}

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

    // Choose which PIO instance to use (there are two instances)
    PIO pio = pio0;

    // Our assembled program needs to be loaded into this PIO's instruction
    // memory. This SDK function will find a location (offset) in the
    // instruction memory where there is enough space for our program. We need
    // to remember this location!
    uint offset = pio_add_program(pio, &stream_bits_program);

    // Find a free state machine on our chosen PIO (erroring if there are
    // none). Configure it to run our program, and start it, using the
    // helper function we included in our .pio file.
    uint sm = pio_claim_unused_sm(pio, true);
    stream_bits_program_init(pio, sm, offset, 0);

    // The state machine is now running. Any value we push to its TX FIFO will
    // appear on the LED pin.
    while (true) {
        // Blink
        pio_sm_put_blocking(pio, sm, 0xffffffff);
        sleep_ms(500);
        // Blonk
        pio_sm_put_blocking(pio, sm, 0);
        sleep_ms(500);
    }

    //configure PIO
    //uint offset = pio_add_program(pio, &stream_bits_program);
    //stream_bits_program_init(pio, sm, offset, 3);

    //confgure DMA from memory to PIO TX FIFO
    //nco_dma_ping = dma_claim_unused_channel(true);
    //nco_ping_cfg = dma_channel_get_default_config(nco_dma_ping);
    //channel_config_set_transfer_data_size(&nco_ping_cfg, DMA_SIZE_32);
    //channel_config_set_read_increment(&nco_ping_cfg, true);
    //channel_config_set_write_increment(&nco_ping_cfg, false);
    //channel_config_set_dreq(&nco_ping_cfg, pio_get_dreq(pio, sm, true));
    //channel_config_set_chain_to(&nco_ping_cfg, nco_dma_pong);
    //dma_channel_configure(nco_dma_ping, &nco_ping_cfg, bit_buffer, &pio->txf[sm], 256, false);

    //nco_dma_pong = dma_claim_unused_channel(true);
    //nco_pong_cfg = dma_channel_get_default_config(nco_dma_pong);
    //channel_config_set_transfer_data_size(&nco_pong_cfg, DMA_SIZE_32);
    //channel_config_set_read_increment(&nco_pong_cfg, true);
    //channel_config_set_write_increment(&nco_pong_cfg, false);
    //channel_config_set_dreq(&nco_pong_cfg, pio_get_dreq(pio, sm, true));
    //channel_config_set_chain_to(&nco_pong_cfg, nco_dma_ping);
    //dma_channel_configure(nco_dma_pong, &nco_pong_cfg, bit_buffer, &pio->txf[sm], 256, false);

    //DMA triggers an interrupt upon completion
    //dma_set_irq0_channel_mask_enabled((1u<<nco_dma_ping) | (1u<<nco_dma_pong), true);
    //irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    //irq_set_enabled(DMA_IRQ_0, true);

    //sleep_us(10000000);
    //while(1){
    //  printf("start\n");
    //  dma_start_channel_mask(1u << nco_dma_ping);
    //  dma_channel_wait_for_finish_blocking(nco_dma_ping);
    //  dma_channel_configure(nco_dma_ping, &nco_ping_cfg, bit_buffer, &pio->txf[sm], 256, false);
    //  printf("stop\n");
   // }

}
