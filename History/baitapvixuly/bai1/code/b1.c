#include <b1.h>
void bai1(void)
{
   int i;
   unsigned int8 a[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
   for(i=0;i<=7;i++){
      OUTPUT_B(a[i]);
      delay_ms((i+1)*1000);
   }
   OUTPUT_B(0xFF);
   delay_ms(3000);
   OUTPUT_B(0x00);
}
void main()
{
   while(true){
         bai1();
      }
}

