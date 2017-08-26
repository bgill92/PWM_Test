//
// This file uses the libopencm3 library
// Bilal Gill 2017
//
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/timer.h"
#include "libopencm3/stm32/f3/nvic.h"
#include "libopencm3/stm32/exti.h"

// #define PIN_A0 0x0001

void clock_setup() {

	// The default clock for the STM32f3discovery is the 72 MHz
	// PLL clock, so we don't want to mess with that

	// We're enabling the clock for GPIOE, which is the register that
	// is connected to the LED
	// We're also starting the GPIOB clock because that register has the
	// pins we're controlling the motors with

	rcc_periph_clock_enable(RCC_GPIOA);

	rcc_periph_clock_enable(RCC_GPIOE);

	rcc_periph_clock_enable(RCC_GPIOB);

	rcc_periph_clock_enable(RCC_GPIOF);

	// We need to enable the clock for the timer module that will be used

	rcc_periph_clock_enable(RCC_TIM1); // for the LED

	rcc_periph_clock_enable(RCC_TIM8); // for the motor

	rcc_periph_clock_enable(RCC_TIM3); // for updating speed and direction of the
										// motor

	rcc_periph_clock_enable(RCC_SYSCFG); // Needed for the EXTI interrupts

}

void gpio_setup(){

	// This function sets up the GPIO mode. We want an alternative function
	// to be used so we can use the timer for controlling the LED with a PWM
	// signal

	// We're setting up GPIO register E (GPIOE), making it an alternate
	// function (GPIO_MODE_AF), setting it to have no pullup or pulldown
	// resistor activated (GPIO_PUPD_NONE), and this is for pin 9 in the
	// register (GPIO9)

	// We also do the same thing for the Pin 8 of GPIOB. We will also do the same
	// for pin 9, because the motor controller requires two PWM signals to control
	// one motor

	// Also need to use an extra GPIO pin to enable the motor controller

	gpio_mode_setup(GPIOE,GPIO_MODE_AF,GPIO_PUPD_NONE, GPIO9); // TIM for LED

	gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE, GPIO8); // IN1 for motor controller

	gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE, GPIO9); // IN2 for motor controller

	gpio_mode_setup(GPIOB,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1); // Enable signal to  
																	// motor controller

	gpio_mode_setup(GPIOA,GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO0); // Testing the Exti line

	gpio_mode_setup(GPIOB,GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO0); // Testing the Exti line 2

	// Each pin can have multiple alternate functions, in this case
	// we're using the GPIO_AF2 for this specific pin meaning it
	// is set to use the timer output
	// Also do the same for the PWM pins
	// Alternate functions and other information can be found at the following link
	// http://www.st.com/content/ccc/resource/technical/document/datasheet/f2/1f/e1/41/ef/59/4d/50/DM00058181.pdf/files/DM00058181.pdf/jcr:content/translations/en.DM00058181.pdf

	gpio_set_af(GPIOE,GPIO_AF2,GPIO9); // TIM1_CH1 alternate function

	gpio_set_af(GPIOB,GPIO_AF10,GPIO8); // TIM8_CH2 alternate function

	gpio_set_af(GPIOB,GPIO_AF10,GPIO9); // TIM8_CH3 alternate function

	// Turn on the enable pin

	gpio_set(GPIOB,GPIO1);

}

void timer_setup() {

	// reset timer 1 & 8 peripheral

	timer_reset(TIM1);
	timer_reset(TIM8);
	timer_reset(TIM3);

	// Set the mode for timer peripheral 1 (TIM1), Set there to be no clock division,
	// Set the timer to be edge mode,
	// and the counting direction to be up


	timer_set_mode(TIM1,TIM_CR1_CKD_CK_INT,TIM_CR1_CMS_EDGE,TIM_CR1_DIR_UP);

	timer_set_mode(TIM8,TIM_CR1_CKD_CK_INT,TIM_CR1_CMS_EDGE,TIM_CR1_DIR_UP);

	timer_set_mode(TIM3,TIM_CR1_CKD_CK_INT,TIM_CR1_CMS_EDGE,TIM_CR1_DIR_UP);


	/* We want the PWM signal to have a frequency of 20 ms, because a hobbyist servo
	   is controlled with a 20 ms signal with a duty cycle in between 1 ms and 2 ms.
	   With that, the two formulas we have at our disposal are: 

	   (1) Tick_Freq = Clock_Freq/prescalar; Clock_Freq = 72000000 Hz;

	   (2) PWM_Freq = Tick_Freq/(Tim_Period+1); PWM_Freq = .02 s;

		Using equation (2):

		50 = 120000/(Tim_Period + 1) (we set prescalar to 600);

		Also the prescalar is intrinsically whatever you set + 1, the prescalar we set
		should be 599

		Tim_period = 2399

	*/

	timer_set_prescaler(TIM1,599);

	timer_set_prescaler(TIM8,599);

	timer_set_prescaler(TIM3,60000);


	// We're setting the output compare mode for TIM_OC1N on peripheral TIM1 to PWM1
	// which if the timer value is less than the Output Compare value, the pin is high

	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);

	timer_set_oc_mode(TIM8, TIM_OC2, TIM_OCM_PWM1);

	timer_set_oc_mode(TIM8, TIM_OC3, TIM_OCM_PWM1);

	// timer_set_oc_mode(TIM3,TIM_OC1, TIM_OCM_FROZEN);

	// This function enables the GPIO to go high for the output compare 

	timer_enable_oc_output(TIM1, TIM_OC1);

	timer_enable_oc_output(TIM8, TIM_OC2);

	timer_enable_oc_output(TIM8, TIM_OC3);

	timer_enable_oc_output(TIM3, TIM_OC4);

	// If a Tim_period of 60000 gives a PWM frequency of 20 ms, and we want a duty cycle
	// of 1 ms, the oc value should be 3000

	timer_set_oc_value(TIM1, TIM_OC1, 12000);

	timer_set_oc_value(TIM8, TIM_OC2, 1200);

	timer_set_oc_value(TIM8, TIM_OC3, 0);

	// timer_set_period(TIM1, 2399);

	timer_set_period(TIM1, 23990);

	timer_set_period(TIM8, 2399);

	timer_set_period(TIM8, 2399);

	timer_set_period(TIM3, 1199);

	// Im not sure what this does

	// timer_set_oc_idle_state_set(TIM1,TIM_OC1);

	// This needs to be enabled for seeing the output on an advanced timer

	timer_enable_break_main_output(TIM1);

	timer_enable_break_main_output(TIM8);

	// The timer keeps accumulating after the first overflow

	timer_continuous_mode(TIM1);

	timer_continuous_mode(TIM8);

	timer_continuous_mode(TIM3);

	// enable the counter

	timer_enable_counter(TIM1);

	timer_enable_counter(TIM8);

	timer_enable_counter(TIM3);

	// enable interrupts

	nvic_enable_irq(NVIC_TIM3_IRQ);

	nvic_set_priority(NVIC_TIM3_IRQ, 1);

	timer_enable_irq(TIM3, TIM_DIER_UIE);

	nvic_enable_irq(NVIC_EXTI0_IRQ);

	nvic_set_priority(NVIC_EXTI0_IRQ, 1);


}

// void pwm_freqeuncy_set()
// {

//void tim3_isr(){
//
//	trace_printf("hi\n");
//
//}
// }

void exti_setup(void){

	exti_select_source(EXTI0, GPIOA);
	exti_select_source(EXTI0, GPIOB);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
	exti_enable_request(EXTI0);
	

}

volatile uint16_t pin_A0;

volatile bool prev_A0;

volatile uint16_t pin_B0;

volatile bool prev_B0;

void encoder_setup() {

	prev_A0 = (bool)(0x0001 & gpio_port_read(GPIOA)); // read the state of GPIOA0

	prev_B0 = (bool)(0x0001 & gpio_port_read(GPIOB)); // read the state of GPIOB0

}

void EXTI0_IRQHandler(void){

	if (exti_get_flag_status(EXTI0)){
		
//		 trace_printf("ow\n");

//		for (int i = 0; i < 100000; i++){
//
//			asm("nop");
//
//		}

		pin_A0 = 0x0001 & gpio_port_read(GPIOA); // Read the state of pin A0

//		pin_A0 = gpio_port_read(GPIOA); // Read the state of pin A0

		trace_printf("%u\n",pin_A0);

		if (pin_A0){

			trace_printf("HIGH\n");

		}
		else {

			trace_printf("LOW\n");

		}

		exti_reset_request(EXTI0);

	}

}

volatile bool state = false;

void TIM3_IRQHandler(void) {

	// IRQ handler works by checking if the flag has been set. Once, that is done 

	if(timer_get_flag(TIM3,TIM_SR_UIF)){

		// trace_printf("hi\n");

		timer_clear_flag(TIM3, TIM_SR_UIF);

		if (state == false) {

			state = true;

			timer_set_oc_value(TIM8, TIM_OC2, 1200);

			timer_set_oc_value(TIM8, TIM_OC3, 0);	

		}
		else if (state == true){

			state = false;

			timer_set_oc_value(TIM8, TIM_OC2, 0);

			timer_set_oc_value(TIM8, TIM_OC3, 1200);	

		}

	}


}



int main(int argc, char* argv[])
{
	clock_setup();
	gpio_setup();
	timer_setup();
	exti_setup();

	while(1){



	}

	return 0;
}


// ----------------------------------------------------------------------------




