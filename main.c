/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/
/*============================ INCLUDES ======================================*/
#include "pico/stdlib.h"
#include "perf_counter.h"

#include "arm_2d.h"
#include "arm_2d_disp_adapter_0.h"

#define __PICO_USE_LCD_1IN3__ 1

#if defined(__PICO_USE_LCD_1IN3__) && __PICO_USE_LCD_1IN3__
#include "DEV_Config.h"
#include "LCD_1In3.h"
#include "GLCD_Config.h"
#endif

#include <stdio.h>

#include "RTE_Components.h"
#if defined(RTE_Compiler_EventRecorder) && defined(USE_EVR_FOR_STDOUR)
#   include <EventRecorder.h>
#endif

/*============================ MACROS ========================================*/
#define TOP         (0x1FFF)

/*============================ MACROFIED FUNCTIONS ===========================*/
//#define ABS(__N)    ((__N) < 0 ? -(__N) : (__N))
//#define _BV(__N)    ((uint32_t)1<<(__N))



/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void SysTick_Handler(void)
{

}

/*! \brief set the 16-level led gradation
 *! \param hwLevel gradation
 *! \return none
 */
static void set_led_gradation(uint16_t hwLevel)
{
    static uint16_t s_hwCounter = 0;
    
    if (hwLevel >= s_hwCounter) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
    } else {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
    }
    
    s_hwCounter++;
    s_hwCounter &= TOP;
}

static void breath_led(void)
{
    static uint16_t s_hwCounter = 0;
    static int16_t s_nGray = (TOP >> 1);
    
    s_hwCounter++;
    if (!(s_hwCounter & (_BV(10)-1))) {
        s_nGray++; 
        if (s_nGray == TOP) {
            s_nGray = 0;
        }
    }
    
    set_led_gradation(ABS(s_nGray - (TOP >> 1)));
}

static void system_init(void)
{
    extern void SystemCoreClockUpdate();

    SystemCoreClockUpdate();
    /*! \note if you do want to use SysTick in your application, please use 
     *!       init_cycle_counter(true); 
     *!       instead of 
     *!       init_cycle_counter(false); 
     */
    init_cycle_counter(false);

#if defined(RTE_Compiler_EventRecorder) && defined(USE_EVR_FOR_STDOUR)
    EventRecorderInitialize(0, 1);
#endif

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

#if defined(__PICO_USE_LCD_1IN3__) && __PICO_USE_LCD_1IN3__
    DEV_Delay_ms(100);

    if(DEV_Module_Init()!=0){
        //assert(0);
    }
   
    DEV_SET_PWM(50);
    /* LCD Init */
    
    LCD_1IN3_Init(HORIZONTAL);
    LCD_1IN3_Clear(GLCD_COLOR_BLUE);
    
    for (int n = 0; n < KEY_NUM; n++) {
        dev_key_init(n);
    }
#endif
}


int main(void) 
{
    system_init();

    printf("Hello Pico-Template\r\n");
    arm_irq_safe {
        arm_2d_init(); // init arm-2d
    }

    // Init Display Adapter 0
    disp_adapter0_init();

    uint32_t n = 0;
    
    while (true) {
        breath_led();
        //gpio_put(PICO_DEFAULT_LED_PIN, 1);
        //sleep_ms(500);
        //gpio_put(PICO_DEFAULT_LED_PIN, 0);
        //sleep_ms(500);
        disp_adapter0_task();
    }
    //return 0;
}
