#include <main.h>
#int_TIMER1
void TIMER1_isr(void){
   set_timer1(40535); 
    output_toggle(pin_B0);
    delay_ms(2); 
}
#int_TIMER0
void TIMER0_isr(void){
  
    output_bit(pin_B2,input(pin_b0)||input(pin_b1));
}
void main()
{
  
   setup_timer_1(T1_|T1_DIV_BY_8);  
    set_timer1(40535); 
    
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   set_timer0(0);
     enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   while(TRUE)
   {
      for(int i = 0;i<2;i++)
         {
            OUTPUT_HIGH(pin_b1);
            delay_ms(i*2+2);
            OUTPUT_LOW(pin_b1);
            delay_ms(i*2+1);
             }
         }
}
