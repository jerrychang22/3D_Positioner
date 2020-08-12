#include "Encoder.h"

Encoder* ENC_LIST[NUM_ENCODERS] = {NULL, NULL, NULL};

Encoder::Encoder(int gpio_pin_a, int gpio_pin_b){
    
    pin_a = (gpio_num_t) gpio_pin_a;
    pin_b = (gpio_num_t) gpio_pin_b;
	gpio_pad_select_gpio(pin_a);
	gpio_pad_select_gpio(pin_b);
	gpio_set_direction(pin_a, GPIO_MODE_INPUT);
	gpio_set_direction(pin_b, GPIO_MODE_INPUT);
	gpio_pulldown_en(pin_a);
	gpio_pulldown_en(pin_b);
    
    int i = 0;
    for (; i < NUM_ENCODERS; i++){
        if (ENC_LIST[i] == NULL) {
            ENC_LIST[i] = this;
            break;
       }
    }
    if (i > NUM_ENCODERS){
        printf("Too many encoders!");
    }

    unit = (pcnt_unit_t) i;
    
    pcnt_config_t pcnt_config;
    // Set PCNT input signal and control GPIOs
    pcnt_config.pulse_gpio_num = pin_a;
    pcnt_config.ctrl_gpio_num = pin_b;
    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = unit;
    // What to do on the positive / negative edge of pulse input?
    pcnt_config.pos_mode = PCNT_COUNT_INC;   // Count up on the positive edge
    pcnt_config.neg_mode = PCNT_COUNT_DIS;   // Keep the counter value on the negative edge
    // What to do when control input is low or high?
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE; // Reverse counting direction if low
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;    // Keep the primary counter mode if high
    // Set the maximum and minimum limit values to watch
    pcnt_config.counter_h_lim = PCNT_H_LIM_VAL;
    pcnt_config.counter_l_lim = PCNT_L_LIM_VAL;
    //pcnt_config = pcnt_conf;
    pcnt_unit_config(&pcnt_config);

    //pcnt_set_filter_value(unit, 10);
    //pcnt_filter_enable(unit);
    
	pcnt_event_enable(unit, PCNT_EVT_H_LIM);
	pcnt_event_enable(unit, PCNT_EVT_L_LIM);

    /* Initialize PCNT's counter */
    pcnt_counter_pause(unit);
    pcnt_counter_clear(unit);

    /* Register ISR handler and enable interrupts for PCNT unit */
    //pcnt_isr_register(pcnt_isr_handle, NULL, 0, &Encoder::isr_handle);
    //pcnt_intr_enable(unit);

    /* Everything is set up, now go to counting */
    pcnt_counter_resume(unit); 

}

int64_t Encoder::GetCount(){
    int16_t raw_count;
	pcnt_get_counter_value(unit, &raw_count);
    return count + raw_count;
}

int64_t Encoder::SetCount(int64_t val){
    int16_t raw_count;
	pcnt_get_counter_value(unit, &raw_count);
    count = val - raw_count;
    return count;
}

int64_t Encoder::ClearCount(){
    count = 0;
	return pcnt_counter_clear(unit);
}

/*
static void IRAM_ATTR pcnt_isr_handle(void* arg){
    Encoder* enc;
    uint32_t intr_status = PCNT.int_st.val;
    int i = 0;

    for(; i < NUM_ENCODERS; i++){
       if (intr_status & (BIT(i))) {
			enc = ENC_LIST[i];

			int64_t status = 0;
			if(PCNT.status_unit[i].h_lim_lat){
				status=enc->pcnt_config.counter_h_lim;
			}
			if(PCNT.status_unit[i].l_lim_lat){
				status=enc->pcnt_config.counter_l_lim;
			}
			
            PCNT.int_clr.val = BIT(i); 
			//Handle overrun
            //enc->count = status + enc->count;
		} 
    }
}
*/

