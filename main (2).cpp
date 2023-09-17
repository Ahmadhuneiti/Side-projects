#include "mbed.h"
#include "C12832.h"

class Speaker { // speaker class to control speaker output
    
private:

    DigitalOut outputSignal;
    char state;

public:

    Speaker(PinName pin): outputSignal (pin){}
    
    void on (void){
        outputSignal=1;
        state=1;}
        
    void off (void){
        outputSignal=0;
        state=0;}
    
    void toggle (void){
        if (state==1)
            off();
        else
            on();}
            
    bool getStatus (void){
        return state;}
    
    };
    
    C12832 lcd(D11, D13, D12, D7, D10); // lcd class to write on screen
    DigitalOut red_led(D5);                 
    DigitalOut green_led(D9); 
    InterruptIn centre(D4); // interrupt for centre button to be activated when pressed
    InterruptIn up(A2);
    InterruptIn down(A3);
    Speaker myspeaker(D6);
    volatile float frequency = 2000.0; // volatile float since it will change while running
    int increment = 200;
    Ticker myticker; // ticker API to control on/off periods 
    
    
    void centreISR()               // ISR for when centre button is pressed
{
    green_led = !green_led;
    red_led = !red_led;
    lcd.cls();
      if (green_led ==0){
      myticker.attach(&myspeaker,&Speaker::toggle,1.0/frequency);
      lcd.cls();
      lcd.locate (0,0);
      lcd.printf("Frequency = %f", frequency);
      }
      else  {myticker.detach();}
        
}

   void upISR () {
    if (green_led ==0){
    frequency = frequency + increment;
    myticker.attach(&myspeaker,&Speaker::toggle,1.0/frequency);
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Frequency = %f", frequency);}
    }
    
    
    
    
    void downISR (){
      if (green_led==0) {
        frequency = frequency - increment;
        myticker.attach(&myspeaker,&Speaker::toggle,1.0/frequency);
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Frequency = %f", frequency);}
    }
    
    
    int main() 
    {
    
        green_led = 1 ;// green is off at the start 
        red_led = 0; // red is on at the start
        
        centre.rise(&centreISR);
        up.rise (&upISR);
        down.rise (&downISR);
        
      
        
        while (1){
            
            }
    }