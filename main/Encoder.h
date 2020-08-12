#ifndef ENCODER_H
#define ENCODER_H

#include "driver/gpio.h"
#include "driver/pcnt.h"

const int NUM_ENCODERS = 3;
const int PCNT_H_LIM_VAL = 32766;
const int PCNT_L_LIM_VAL = -32666;

class Encoder{
    private:
        gpio_num_t          pin_a;
        gpio_num_t          pin_b;
        pcnt_unit_t         unit;
        volatile int64_t    count = 0;
//        pcnt_config_t       pcnt_config;

    public:

        Encoder(int gpio_pin_a, int gpio_pin_b);
        int64_t GetCount();
        int64_t SetCount(int64_t val);
        int64_t ClearCount();
 //       pcnt_isr_handle_t isr_handle = NULL;

};


#endif
