#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <random>
class ADTProcessor : public juce::AudioProcessor {
public:
 ADTProcessor();
 void prepareToPlay(double,int) override;
 void releaseResources() override {}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override {return true;}
 const juce::String getName() const override {return "LM18 ADT";}
 bool acceptsMidi() const override {return false;}
 bool producesMidi() const override {return false;}
 bool isMidiEffect() const override {return false;}
 double getTailLengthSeconds() const override {return 0.03;}
 int getNumPrograms() override {return 1;}
 int getCurrentProgram() override {return 0;}
 void setCurrentProgram(int) override {}
 const juce::String getProgramName(int) override {return {};}
 void changeProgramName(int,const juce::String&) override {}
 void getStateInformation(juce::MemoryBlock&) override;
 void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState state;
 std::atomic<float> currentL {5.05f},currentR {5.05f};
private:
 struct Mod {
  std::mt19937 rng;
  std::uniform_real_distribution<float> dist{-1.f,1.f};
  double phase=0.0;
  float from=0.f,to=0.f;
  explicit Mod(unsigned seed):rng(seed){to=dist(rng);}
  float next(double sr,float hz) {
   phase += (double)hz/sr;
   if(phase>=1.0) {
    phase-=1.0;
    from=to;
    to=dist(rng);
   }
   // Raised-cosine interpolation: zero slope at both endpoints, no held plateau.
   const float t=(float)phase;
   const float shaped=0.5f-0.5f*std::cos(juce::MathConstants<float>::pi*t);
   return from+(to-from)*shaped;
  }
 } modL{0x18a1},modR{0x18b2};
 juce::AudioBuffer<float> history;
 int write=0; double sampleRate=48000;
 juce::dsp::IIR::Filter<float> hp[2],lp[2];
 juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> mixSmooth;
 static juce::AudioProcessorValueTreeState::ParameterLayout parameters();
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADTProcessor)
};
