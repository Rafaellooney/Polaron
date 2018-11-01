// Copyright (c) 2018 Thomas Zueblin
//
// Author: Thomas Zueblin (thomas.zueblin@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "AudioChannel.h"
#include "mixer.h"
#include <Audio.h>

#ifndef HatsChannel_h
#define HatsChannel_h

class HatsChannel : public AudioChannel
{
  public:
    HatsChannel() : w1m(w1, 0, mixer, 0),
                    w2m(w2, 0, mixer, 1),
                    w3m(w3, 0, mixer, 2),
                    w4m(w4, 0, mixer, 3),
                    w5m(w5, 0, mixer, 4),
                    w6m(w6, 0, mixer, 5),
                    w7m(w7, 0, mixer, 6),
                    w8m(w8, 0, mixer, 7),
                    mixerFilter(mixer, 0, filter, 0),
                    filterEnv(filter, 0, envelope, 0)
    {

        envelope.delay(0);
        envelope.attack(1);
        envelope.hold(0);
        envelope.decay(5);
        envelope.sustain(0.0f);

        mixer.gain(0, 0.5f);
        mixer.gain(1, 0.5f);
        mixer.gain(2, 0.5f);
        mixer.gain(3, 0.5f);
        mixer.gain(4, 0.5f);
        mixer.gain(5, 0.5f);
        mixer.gain(6, 0.5f);
        mixer.gain(7, 0.0f);

        w1.begin(1.0, baseFreq, WAVEFORM_SQUARE);
        w2.begin(1.0, baseFreq * ratio1, WAVEFORM_SQUARE);
        w3.begin(1.0, baseFreq * ratio2, WAVEFORM_SQUARE);
        w4.begin(1.0, baseFreq * ratio3, WAVEFORM_SQUARE);
        w5.begin(1.0, baseFreq * ratio4, WAVEFORM_SQUARE);
        w6.begin(1.0, baseFreq * ratio5, WAVEFORM_SQUARE);
        w7.begin(1.0, baseFreq * ratio6, WAVEFORM_SQUARE);
        w8.begin(1.0, baseFreq * ratio7, WAVEFORM_SQUARE);

        filter.setHighpass(0, 8000, 0.700);
        filter.setHighpass(1, 8000, 0.700);
        filter.setHighpass(2, 8000, 0.700);
        //filter.setHighpass(3, 8000, 0.700 );
    }
    AudioStream *getOutput1() { return &envelope; }
    AudioStream *getOutput2() { return &envelope; }

    void trigger()
    {
        envelope.noteOn();
    }
    void setParam1(int value)
    {
        baseFreq = (float)map(value, 0, 1024, 40, 80);
        w1.frequency(baseFreq);
        w2.frequency(baseFreq * ratio1);
        w3.frequency(baseFreq * ratio2);
        w4.frequency(baseFreq * ratio3);
        w5.frequency(baseFreq * ratio4);
        w6.frequency(baseFreq * ratio5);
        w7.frequency(baseFreq * ratio6);
        w8.frequency(baseFreq * ratio7);
    }
    void setParam2(int value) {}
    void setParam3(int value)
    {
        float mappedValue = (float)map(value, 0, 1024, 2000, 10000);
        filter.setHighpass(0, mappedValue, 0.700);
        filter.setHighpass(1, mappedValue, 0.700);
        filter.setHighpass(2, mappedValue, 0.700);
    }
    void setParam4(int value) {}
    void setParam5(int value) {}
    void setParam6(int value) { envelope.decay((float)map(value, 0, 1024, 1, 100)); }

  private:
    float baseFreq = 40;
    float ratio1 = 2.0;
    float ratio2 = 3.0;
    float ratio3 = 4.16;
    float ratio4 = 5.43;
    float ratio5 = 6.79;
    float ratio6 = 8.21;
    float ratio7 = 10.0;
    //float ratio2 = 2.0;

    AudioMixer8 mixer;
    AudioSynthWaveform w1;
    AudioSynthWaveform w2;
    AudioSynthWaveform w3;
    AudioSynthWaveform w4;
    AudioSynthWaveform w5;
    AudioSynthWaveform w6;
    AudioSynthWaveform w7;
    AudioSynthWaveform w8;
    AudioFilterBiquad filter;
    AudioEffectEnvelope envelope;
    AudioConnection w1m;
    AudioConnection w2m;
    AudioConnection w3m;
    AudioConnection w4m;
    AudioConnection w5m;
    AudioConnection w6m;
    AudioConnection w7m;
    AudioConnection w8m;
    AudioConnection mixerFilter;
    AudioConnection filterEnv;
};
#endif
