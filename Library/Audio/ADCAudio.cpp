#include "ADCAudio.h"
#include <stdio.h>

ADCAudio :: ADCAudio(const uint8_t audio_pin, const uint16_t audio_sample_rate)
{
    
    //ADC Configuration
    adc_init();
    adc_gpio_init(audio_pin);//I channel (0) - configure pin for ADC use
    adc_set_clkdiv((48000000/audio_sample_rate)-1);
  
    // Configure DMA for ADC transfers
    adc_dma = dma_claim_unused_channel(true);
    cfg = dma_channel_get_default_config(adc_dma);

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, DREQ_ADC);// Pace transfers based on availability of ADC samples

    //start ADC
    adc_select_input(0);
    hw_clear_bits(&adc_hw->fcs, ADC_FCS_UNDER_BITS);
    hw_clear_bits(&adc_hw->fcs, ADC_FCS_OVER_BITS);
    adc_fifo_setup(true, true, 1, false, false);
    adc_run(true);

    //pre-fill ping buffer
    dma_channel_configure(adc_dma, &cfg, ping, &adc_hw->fifo, 1024, true);
    ping_running = true;

}

//samples is a reference to a buffer containing block size samples
void ADCAudio :: InputSamples(uint16_t*& samples)
{
    if(ping_running){
        //wait for ping transfer to complete
        dma_channel_wait_for_finish_blocking(adc_dma);
        //start a transfer into pong buffer for next time
        dma_channel_configure(adc_dma, &cfg, pong, &adc_hw->fifo, 1024, true);
        samples = ping; //return ping buffer
        ping_running = false;
    }
    else
    {
        //wait for ping transfer to complete
        dma_channel_wait_for_finish_blocking(adc_dma);
        //start a transfer into pong buffer for next time
        dma_channel_configure(adc_dma, &cfg, ping, &adc_hw->fifo, 1024, true);
        samples = pong; //return pong buffer
        ping_running = true;
    }
}
