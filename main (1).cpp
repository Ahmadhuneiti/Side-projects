#include "mbed.h"

class Joystick {
private:
DigitalIn up, down, left, right, fire;
public:
Joystick(PinName u, PinName d, PinName l, PinName r, PinName f):up(u),down(d),left(l),right(r),fire(f){}

bool upPressed(void){
    if (up==1){
       return true;}
    else {
      return  false;}
      }
     
bool downPressed(void){
    if (down==1){
       return true;}
    else{
       return false;}
       }       
bool leftPressed(void){
    if(left==1){
       return true;}
    else{
        return false;}
        }
bool rightPressed(void){
    if(right==1){
       return true;}
    else{
       return false;}
       }
bool firePressed(void){
    if(fire==1){
      return true;}
    else{
      return false;}
      }
};

class LED                                          
{

private:                                          
    DigitalOut outputSignal;                        //Declaration of DigitalOut object
    bool status;                                    //Variable to recall the state of the LED

public:                                             //Public declarations
    LED(PinName pin) : outputSignal(pin){off();}    //Constructor - user provided pin name is assigned to the DigitalOut

    void on(void)                                   //Public member function for turning the LED on
    {
        outputSignal = 0;                           //Set output to 0 (LED is active low)
        status = true;                              //Set the status variable to show the LED is on
    }

    void off(void)                                  //Public member function for turning the LED off
    {
        outputSignal = 1;                           //Set output to 1 (LED is active low)
        status = false;                             //Set the status variable to show the LED is off
    }

   
};

int main(){
    LED RED(D5);
    LED BLUE(D8);
    LED GREEN(D9);
    
    
    Joystick myjoystick((A2),(A3),(A4),(A5),(D4));
    
     while (1){
         
         if (myjoystick.upPressed()==1){
            RED.on();
            GREEN.on();
            BLUE.on();}
        else {RED.off();
              GREEN.off();
              BLUE.off();}
    
         
         if (myjoystick.downPressed()==1)
            RED.on();
      else {GREEN.off();
            RED.off();
            BLUE.off();}
        
         if(myjoystick.leftPressed()==1)
            GREEN.on();
     else {BLUE.off();
           RED.off();
           GREEN.off();}    
         if (myjoystick.rightPressed()==1){
            BLUE.on();
            }
        else { RED.off();
               GREEN.off();
               BLUE.off();}
        
            
        if (myjoystick.firePressed()==1){
            RED.on();
            BLUE.on();}
        else {RED.off();
              GREEN.off();
              BLUE.off();}
        
        }
    }
         
         