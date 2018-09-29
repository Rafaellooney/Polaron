#include "Bounce2.h"
#include "FastLED.h"
#include "SequencerTrack.h"
#include "SequencerStep.h"
#include "Sequencer.h"
#include "mixer.h"

#include <Audio.h>

#include "AudioChannel1.h"
#include "SimpleDrumChannel.h"

#define PULSE_WIDTH_USEC 5

#define SHIFT_IN_PLOAD_PIN 0 // 2  // Connects to Parallel load pin the 165
#define SHIFT_IN_DATA_PIN 1 // 4 // Connects to the Q7 pin the 165
#define SHIFT_IN_CLOCK_PIN 2 //5 // Connects to the Clock pin the 165
//pin used to send the serial data to the array of leds (via fastLED)
#define DATA_PIN 6
#define POTI_PIN_1 A16
#define POTI_PIN_2 A17

AudioOutputAnalogStereo  dacs1;          

AudioMixer8              mixer1;
AudioMixer8              mixer2;

SimpleDrumChannel channel1;
AudioChannel1 channel2;
AudioChannel1 channel3;
AudioChannel1 channel4;
AudioChannel1 channel5;
AudioChannel1 channel6;


AudioConnection          patchCord8(*channel1.getOutput1(), 0, mixer1, 0);
AudioConnection          patchCord9(*channel2.getOutput1(), 0, mixer1, 1);
AudioConnection          patchCord10(*channel3.getOutput1(), 0, mixer1, 2);
AudioConnection          patchCord11(*channel4.getOutput1(), 0, mixer1, 3);
AudioConnection          patchCord12(*channel5.getOutput1(), 0, mixer1, 4);
AudioConnection          patchCord13(*channel6.getOutput1(), 0, mixer1, 5);
AudioConnection          patchCord14(*channel1.getOutput2(), 0, mixer2, 0);
AudioConnection          patchCord15(*channel2.getOutput2(), 0, mixer2, 1);
AudioConnection          patchCord16(*channel3.getOutput2(), 0, mixer2, 2);
AudioConnection          patchCord17(*channel4.getOutput2(), 0, mixer2, 3);
AudioConnection          patchCord18(*channel5.getOutput2(), 0, mixer2, 4);
AudioConnection          patchCord19(*channel6.getOutput2(), 0, mixer2, 5);

AudioConnection          patchCord20(mixer1, 0, dacs1, 0);
AudioConnection          patchCord21(mixer2, 0, dacs1, 1);

// GUItool: end automatically generated code

AudioChannel *players [NUMBER_OF_INSTRUMENTTRACKS] = {
    &channel1, &channel2, &channel3, &channel4, &channel5, &channel6
};

unsigned long last_step_time = millis();

unsigned int step_length = 20;
static bool externalClockReceived = false;
static bool externalSync = false;


Sequencer sequencer;

void setup() {

    pinMode(SHIFT_IN_PLOAD_PIN, OUTPUT);
    //pinMode(inClockEnablePin, OUTPUT);
    pinMode(SHIFT_IN_CLOCK_PIN, OUTPUT);
    pinMode(SHIFT_IN_DATA_PIN, INPUT);
    // init input shift register
    digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
    digitalWrite(SHIFT_IN_PLOAD_PIN, HIGH);
        
    AudioMemory(20);
    //dacs1.analogReference(EXTERNAL);
        
    mixer1.gain(0, 0.3f);
    mixer1.gain(1, 0.3f);
    mixer1.gain(2, 0.1f);
    mixer1.gain(3, 0.3f);
    mixer1.gain(4, 0.1f);
    mixer1.gain(5, 0.3f);
    
    mixer2.gain(0, 0.3f);
    mixer2.gain(1, 0.3f);
    mixer2.gain(2, 0.3f);
    mixer2.gain(3, 0.1f);
    mixer2.gain(4, 0.3f);
    mixer2.gain(5, 0.1f);
    
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(sequencer.leds, NUM_LEDS);
    FastLED.setBrightness(5);
    
    
    for (int i = 0; i < NUM_LEDS; i++){
        for (int j = 0; j < NUM_LEDS; j++){
            sequencer.leds[j] = CRGB::Black;
        }
        sequencer.leds[i] = CRGB::Red;
        FastLED.show();
        delay(20);
    }
    for (int i = NUM_LEDS-1; i >= 0; i--){
        for (int j = 0; j < NUM_LEDS; j++){
            sequencer.leds[j] = CRGB::Black;
        }
        sequencer.leds[i] = CRGB::Red;
        FastLED.show();
        delay(20);
    }
    sequencer.leds[0] = CRGB::Black;
    FastLED.show();
    
    
}


CRGB colorForStepState(uint8_t state){
    switch (state) {
        case 1:
            //trigger on / plock rec off
            return CRGB::CornflowerBlue;
        case 2:
            //trigger off / plock rec on
            return CRGB::Green;
        case 3:
            //trigger on / plock rec on
            return CRGB::DarkOrange;
        default:
            //trigger off / plock rec off
            return CRGB::Black;
    }
}

inline void readButtonState(Bounce &button) {
  button.update();
  //Pulse the Clock (rising edge shifts the next bit).
  digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
  //delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
}

/*
 * updates all button states by reading in the values from the shift registers
 */
void readButtonStates(){
    // Trigger a parallel Load to latch the state of the data lines in the input shift register.
    digitalWrite(SHIFT_IN_PLOAD_PIN, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(SHIFT_IN_PLOAD_PIN, HIGH);

    digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
    digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
    digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
    digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);

    readButtonState(sequencer.trackButtons[5]);
    readButtonState(sequencer.trackButtons[4]);
    readButtonState(sequencer.trackButtons[3]);
    readButtonState(sequencer.trackButtons[2]);
    readButtonState(sequencer.trackButtons[1]);
    readButtonState(sequencer.trackButtons[0]);

    readButtonState(sequencer.stepButtons[7]);
    readButtonState(sequencer.stepButtons[6]);
    readButtonState(sequencer.stepButtons[5]);
    readButtonState(sequencer.stepButtons[4]);
    readButtonState(sequencer.stepButtons[3]);
    readButtonState(sequencer.stepButtons[2]);
    readButtonState(sequencer.stepButtons[1]);
    readButtonState(sequencer.stepButtons[0]);

    readButtonState(sequencer.stepButtons[15]);
    readButtonState(sequencer.stepButtons[14]);
    readButtonState(sequencer.stepButtons[13]);
    readButtonState(sequencer.stepButtons[12]);
    readButtonState(sequencer.stepButtons[11]);
    readButtonState(sequencer.stepButtons[10]);
    readButtonState(sequencer.stepButtons[9]);
    readButtonState(sequencer.stepButtons[8]);

    readButtonState(sequencer.functionButtons[7]);
    readButtonState(sequencer.functionButtons[6]);
    readButtonState(sequencer.functionButtons[5]);
    readButtonState(sequencer.functionButtons[4]);
    readButtonState(sequencer.functionButtons[3]);
    readButtonState(sequencer.functionButtons[2]);
    readButtonState(sequencer.functionButtons[1]);
    readButtonState(sequencer.functionButtons[0]);
}



void updateControl() {
    
    readButtonStates();
    sequencer.updateState();
    FastLED.show();
    
}





void updateAudio() {

    sequencer.tick();   
      
    uint16_t sensorValue1 = (uint16_t) (1024 - analogRead(POTI_PIN_1));
    uint16_t sensorValue2 = (uint16_t) analogRead(POTI_PIN_2);
    
    if (sequencer.pulseCount == 0){
        for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
            
            //advance one step;
            sequencer.tracks[i].doStep();
            // checks if the bit at position of the current_step is set to 1 in the step on/off integer
            SequencerStep &step = sequencer.tracks[i].getCurrentStep();
            
            if (step.isParameterLockOn()){
                step.parameter1 = sensorValue1;
                step.parameter2 = sensorValue2;
            }
        }
    }

    // punch in style recording of the trigger pattern for the currently selected track
    uint8_t triggerPattern = 0b00000001;
    if (sequencer.functionButtons[BUTTON_TRIGGER_PATTERN].read()){
        if (sensorValue1 < 200){
            triggerPattern = 0b00000001;
        } else if (sensorValue1 < 400){
            triggerPattern = 0b00000101;
        } else if (sensorValue1 < 600){
            triggerPattern = 0b00000111;
        } else if (sensorValue1 < 800){
            triggerPattern = 0b00010101;
        } else {
            triggerPattern = 0b00111111;
        }
        sequencer.getSelectedTrack().getCurrentStep().triggerPattern = triggerPattern;
    }

    // trigger tracks if step is set to on
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        SequencerStep step = sequencer.tracks[i].getCurrentStep();
        
        
        if (!sequencer.tracks[i].isMuted() && step.isTriggerOn()
            //check if the bit for the n-th pulse of this step is set
            && (step.triggerPattern & (1 << sequencer.pulseCount))){
                if (sequencer.functionButtons[BUTTON_INSTANT_PLOCK].read()){         
                    players[i]->setParam1(sensorValue1);
                    players[i]->setParam2(sensorValue2);
                } else {
                    players[i]->setParam1(step.parameter1);
                    players[i]->setParam2(step.parameter2);

                }
                players[i]->trigger();
        }
    }
}




void loop() {
    
    //usbMIDI.read();
    //long starttime = micros();
    
    FastLED.clearData();
    readButtonStates();
    sequencer.updateState();
    FastLED.show();
    
    if (sequencer.isRunning()){
        
        //if (externalSync && externalClockReceived){
        //    externalClockReceived = false;
        //    updateAudio();
        //} else 
        if (!externalSync && (millis() - last_step_time >= step_length)){
            updateAudio();
            last_step_time = millis();
        }
        
        if (0){

            Serial.print("CPU: ");
            Serial.print("all=");
            Serial.print(AudioProcessorUsage());
            Serial.print(",");
            Serial.print(AudioProcessorUsageMax());
            Serial.print("    ");
            Serial.print("Memory: ");
            Serial.print(AudioMemoryUsage());
            Serial.print(",");
            Serial.print(AudioMemoryUsageMax());
            Serial.println();

        }
    }  
}