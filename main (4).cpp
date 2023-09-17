#include "mbed.h"                                   //Include the Mbed libraries
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
    
class LED                                           //Begin LED class definition
{

protected:                                          //Protected (Private) data member declaration
    DigitalOut outputSignal;                        //Declaration of DigitalOut object
    bool status;                                    //Variable to recall the state of the LED

public:                                             //Public declarations
    LED(PinName pin) : outputSignal(pin)
    {
        off();   //Constructor - user provides the pin name, which is assigned to the DigitalOut
    }

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

    void toggle(void)                               //Public member function for toggling the LED
    {
        if (status)                                 //Check if the LED is currently on
            off();                                  //Turn off if so
        else                                        //Otherwise...
            on();                                   //Turn the LED on
    }

    bool getStatus(void)                            //Public member function for returning the status of the LED
    {
        return status;                              //Returns whether the LED is currently on or off
    }
};

class Potentiometer                                 //Begin Potentiometer class definition
{
private:                                            //Private data member declaration
    AnalogIn inputSignal;                           //Declaration of AnalogIn object
    float VDD, currentSampleNorm, currentSampleVolts; //Float variables to speficy the value of VDD and most recent samples

public:                                             // Public declarations
    Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}   //Constructor - user provided pin name assigned to AnalogIn...
                                                                        //VDD is also provided to determine maximum measurable voltage
    float amplitudeVolts(void)                      //Public member function to measure the amplitude in volts
    {
        return (inputSignal.read()*VDD);            //Scales the 0.0-1.0 value by VDD to read the input in volts
    }
    
    float amplitudeNorm(void)                       //Public member function to measure the normalised amplitude
    {
        return inputSignal.read();                  //Returns the ADC value normalised to range 0.0 - 1.0
    }
    
    void sample(void)                               //Public member function to sample an analogue voltage
    {
        currentSampleNorm = inputSignal.read();       //Stores the current ADC value to the class's data member for normalised values (0.0 - 1.0)
        currentSampleVolts = currentSampleNorm * VDD; //Converts the normalised value to the equivalent voltage (0.0 - 3.3 V) and stores this information
    }
    
    float getCurrentSampleVolts(void)               //Public member function to return the most recent sample from the potentiometer (in volts)
    {
        return currentSampleVolts;                  //Return the contents of the data member currentSampleVolts
    }
    
    float getCurrentSampleNorm(void)                //Public member function to return the most recent sample from the potentiometer (normalised)
    {
        return currentSampleNorm;                   //Return the contents of the data member currentSampleNorm  
    }

};


class SamplingPotentiometer : public Potentiometer {
private:
 float samplingFrequency, samplingPeriod;
 Ticker sampler;
public:
 SamplingPotentiometer(PinName p, float v, float fs): Potentiometer(p,v), samplingFrequency(fs){
     sampler.attach(callback(this,&Potentiometer::sample), 1/samplingFrequency);} // pins and VDD will already be configured by the Potentiometer class, frequency will be inputed by this class
};


    typedef enum {Starting,Set, Setting, Running, Continue, Paused, Complete} ProgramState;    //enum (user-defined type) 
    ProgramState state;                                    //Use the enum to define 'state' which holds the current state of the sequencer
    InterruptIn centre(D4);
    
    int remaining_minutes;
    int remaining_seconds;
    
    void runISR(){                               // ISR for when centre button is pressed. This allows state transitions depending on the current state 
        if (state == Set){
            state = Setting;}
        else if (state == Continue) {
            state = Paused;}
        else if (state == Paused) {
            state = Running;}
        else if (state == Complete) {
            state = Starting;}
            }
            
            void countingdownISR(){
         if (remaining_minutes > 0){
             if (remaining_seconds > 0) {
                 remaining_seconds = remaining_seconds -1; }
             else {remaining_minutes= remaining_minutes -1;
                   remaining_seconds=59;}
                 }
         else {remaining_seconds = remaining_seconds -1;}} // ISR that counts down in mins and seconds

      void countcompleteISR(){                            // This ISR allows to go to state Complete once the timer (countdown) finishes 
         state = Complete; }
         
         

int main() 
{
    LED red (D5), green (D9), blue (D8);
    Ticker toggleLED;
    C12832 lcd(D11, D13, D12, D7, D10);
    SamplingPotentiometer left (A0, 3.3, 100);
    SamplingPotentiometer right (A1, 3.3, 100);
    int  minutes;
    int seconds ;
    Speaker myspeaker(D6);
    Ticker togglespeaker;
    
     Ticker count;
     centre.rise(&runISR);
     Timeout countcomplete;
     

    
    
    while(1) {
 switch (state) {
     
 case (Starting) :
    lcd.cls(); // clear screen to begin program
    red.off(); // All leds are off at first before the user is able to input
    blue.off();
    green.off();
    togglespeaker.detach(); // to stop speaker on next cycle 
    state= Set; // transisition to next state
    break;
 
 case (Set) :
 red.on(); // only red is on, now user inputs 
 blue.off();
 green.off();
 minutes= (left.getCurrentSampleNorm()*9.5);
 seconds= (right.getCurrentSampleNorm()*59.5); // this allows the minutes and seconds to reach max without flickering 
 lcd.locate(0,0);
 lcd.printf("Set time duration = %d : %d", minutes,seconds); // prints the input by the user
break;

 case (Setting) : // Here the user has already inputed and it is time to start counting down
  red.on();
  blue.off();
  green.off();
  remaining_minutes= minutes;
  remaining_seconds= seconds;
  lcd.cls(); // important to clear the screen so that no outputs interfere 
  state = Running;
break;
 
 case(Running) : // Timer is counting down and green LED is flickering
 blue.off();
 red.off();
 green.off();
 toggleLED.attach(&green,&LED::toggle,1/0.5); // flickering green LED, 0.5 Hz 
 count.attach(&countingdownISR,1); // this allows the ISR to be executed every 1s like a real clock
 countcomplete.attach(&countcompleteISR, (remaining_minutes*60 + remaining_seconds)); // since the countdown occurs every one sec then when the countdown is complete the time in this callback will be 0 and it will move to the next state
 state = Continue;
break;
  case (Continue) :
  lcd.locate(0,0);
  lcd.printf("Time duration = %d : %d \n Time remaining = %d : %d",minutes,seconds, remaining_minutes, remaining_seconds);
break; // keeps looping so that the time continously updates
  
  
 case (Paused) :
 toggleLED.detach();
 count.detach();
 countcomplete.detach();
 green.on(); // LED is orange 
 blue.off();
 red.on();
 lcd.locate(0,0);
 lcd.printf("Timer is paused");
break; // keeps looping unless button is pressed

 
 
 case (Complete) : // this is the final state, Here the timer has finished and the buzzer turns on 
 count.detach();
 countcomplete.detach(); // detach all timers so that they do not keep calling
 toggleLED.detach();
 green.off();              
 blue.on();
 red.off(); // only blue LED is on 
 togglespeaker.attach(&myspeaker, &Speaker::toggle, 1.0/3000);        // turn speaker on                       
 lcd.locate(0,0);
 lcd.printf("Timer duration reached \n press the centre button to continue"); // The user now know that time has finished. Should press centre button to restart
 break;
                             
 
 default : // no default because the program will always start from the same state 
                          
 }
 }
    
    }