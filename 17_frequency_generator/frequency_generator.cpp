#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
// Our assembled program:
#include "stream_bits.pio.h"

uint32_t bit_buffer[256] __attribute__((aligned(1024))) ={
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
int nco_dma;
dma_channel_config nco_dma_cfg;
int chain_dma;
dma_channel_config chain_dma_cfg;

void dma_handler() {


    //When a DMA completes, reconfigure it but don't start it yet, wait for DMA chaining to trigger
    if(dma_hw->ints0 & (1u << nco_dma))
    {
      printf("a\n");
      dma_channel_configure(nco_dma, &nco_dma_cfg, &pio->txf[sm], bit_buffer, 256, false);
      dma_hw->ints0 = 1u << nco_dma; //acknowledge interrupt
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

    nco_dma = dma_claim_unused_channel(true);
    chain_dma = dma_claim_unused_channel(true);

    //confgure DMA from memory to PIO TX FIFO
    nco_dma_cfg = dma_channel_get_default_config(nco_dma);
    channel_config_set_transfer_data_size(&nco_dma_cfg, DMA_SIZE_32);
    channel_config_set_read_increment(&nco_dma_cfg, true);
    channel_config_set_write_increment(&nco_dma_cfg, false);
    channel_config_set_dreq(&nco_dma_cfg, pio_get_dreq(pio, sm, true));
    channel_config_set_chain_to(&nco_dma_cfg, chain_dma);
    channel_config_set_ring(&nco_dma_cfg, false, 10); // 1 << 10 = 1024 bytes = 256 ints read addresses wrap
    dma_channel_configure(nco_dma, &nco_dma_cfg, &pio->txf[sm], bit_buffer, 256, false);
    
    //use a second DMA to reconfigure the first
    chain_dma_cfg = dma_channel_get_default_config(chain_dma);
    channel_config_set_transfer_data_size(&chain_dma_cfg, DMA_SIZE_32);
    channel_config_set_read_increment(&chain_dma_cfg, false); //no increment, avoid the need to reprogram
    channel_config_set_write_increment(&chain_dma_cfg, false);
    channel_config_set_chain_to(&chain_dma_cfg, nco_dma);

    //second DMA doesn't transfer any data, it just restarts the first
    uint32_t null_transfer_data;
    dma_channel_configure(
      chain_dma,
      &chain_dma_cfg,
      &null_transfer_data, // Initial write address
      &null_transfer_data, // Initial read address
      1, // Halt after each control block
      false // Don't start yet
    );

    //DMA triggers an interrupt upon completion
    //dma_set_irq0_channel_mask_enabled((1u<<nco_dma) | (1u<<chain_dma), true);
    ////irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    ////irq_set_enabled(DMA_IRQ_0, true);

    sleep_us(10000000);
    printf("starting\n");
    //dma_channel_start(nco_dma);
    dma_start_channel_mask(1u << nco_dma);

    // The state machine is now running. Any value we push to its TX FIFO will
    // appear on the LED pin.
    while (true) {}

    //configure PIO
    //uint offset = pio_add_program(pio, &stream_bits_program);
    //stream_bits_program_init(pio, sm, offset, 3);

    //confgure DMA from memory to PIO TX FIFO
    //nco_dma = dma_claim_unused_channel(true);
    //nco_dma_cfg = dma_channel_get_default_config(nco_dma);
    //channel_config_set_transfer_data_size(&nco_dma_cfg, DMA_SIZE_32);
    //channel_config_set_read_increment(&nco_dma_cfg, true);
    //channel_config_set_write_increment(&nco_dma_cfg, false);
    //channel_config_set_dreq(&nco_dma_cfg, pio_get_dreq(pio, sm, true));
    //channel_config_set_chain_to(&nco_dma_cfg, chain_dma);
    //dma_channel_configure(nco_dma, &nco_dma_cfg, bit_buffer, &pio->txf[sm], 256, false);

    //chain_dma = dma_claim_unused_channel(true);
    //chain_dma_cfg = dma_channel_get_default_config(chain_dma);
    //channel_config_set_transfer_data_size(&chain_dma_cfg, DMA_SIZE_32);
    //channel_config_set_read_increment(&chain_dma_cfg, true);
    //channel_config_set_write_increment(&chain_dma_cfg, false);
    //channel_config_set_dreq(&chain_dma_cfg, pio_get_dreq(pio, sm, true));
    //channel_config_set_chain_to(&chain_dma_cfg, nco_dma);
    //dma_channel_configure(chain_dma, &chain_dma_cfg, bit_buffer, &pio->txf[sm], 256, false);

    //DMA triggers an interrupt upon completion
    //dma_set_irq0_channel_mask_enabled((1u<<nco_dma) | (1u<<chain_dma), true);
    //irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    //irq_set_enabled(DMA_IRQ_0, true);

    //sleep_us(10000000);
    //while(1){
    //  printf("start\n");
    //  dma_start_channel_mask(1u << nco_dma);
    //  printf("stop\n");
   // }

}
