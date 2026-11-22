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
  double elapsed=0; float value=0,target=0;
  explicit Mod(unsigned seed):rng(seed){}
  float next(double sr,float hz) {
   elapsed+=1.0/sr;
   if(elapsed>=1.0/hz){elapsed-=1.0/hz;target=dist(rng);}
   const float step=(float)(1.0/(sr*juce::jmax(0.02,0.7/(double)hz)));
   value+=juce::jlimit(-step,step,target-value);
   return value;
  }
 } modL{0x18a1},modR{0x18b2};
 juce::AudioBuffer<float> history;
 int write=0; double sampleRate=48000;
 juce::dsp::IIR::Filter<float> hp[2],lp[2];
 juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> mixSmooth;
 static juce::AudioProcessorValueTreeState::ParameterLayout parameters();
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADTProcessor)
};
