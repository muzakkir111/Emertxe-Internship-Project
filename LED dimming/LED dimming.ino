#define LED 3 

void setup() 
{
  pinMode(3, OUTPUT);

}
unsigned int i = 0;
void loop()
{
for (i =0 ; i < 256; i ++)
 {
  analogWrite(LED, i);
    delay(5);
 }  

}
