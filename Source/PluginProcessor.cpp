#include "PluginProcessor.h"
#include "PluginEditor.h"
ADTProcessor::ADTProcessor():juce::AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"PARAMS",parameters()){}
juce::AudioProcessorValueTreeState::ParameterLayout ADTProcessor::parameters(){
 std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
 auto add=[&](const char* id,const char* name,float a,float b,float d,float skew=1.f){juce::NormalisableRange<float> r(a,b);r.skew=skew;if(juce::String(id)=="mix"||juce::String(id)=="hp"||juce::String(id)=="lp")r.interval=1.f;p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,r,d));};
 add("mix","ADT Mix",0,100,50);
 add("lfoL","LFO Left Hz",0.05f,6.f,0.12f,0.55f);
 add("lfoR","LFO Right Hz",0.05f,6.f,0.12f,0.55f);
 add("hp","Highpass Hz",0,300,0);
 add("lp","Lowpass Hz",1500,15000,15000);
 p.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"filter",1},"Filter Enabled",false));
 return {p.begin(),p.end()};
}
void ADTProcessor::prepareToPlay(double sr,int block){
 sampleRate=sr;history.setSize(1,(int)std::ceil(sr*0.04)+block+8);history.clear();write=0;
 mixSmooth.reset(sr,0.02);mixSmooth.setCurrentAndTargetValue(*state.getRawParameterValue("mix")/100.f);
 for(int i=0;i<2;++i){hp[i].reset();lp[i].reset();}
}
void ADTProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer&){
 juce::ScopedNoDenormals noDenormals;
 const int n=buffer.getNumSamples();if(history.getNumSamples()==0||buffer.getNumChannels()<2)return;
 const float lHz=*state.getRawParameterValue("lfoL"),rHz=*state.getRawParameterValue("lfoR");
 const float hpHz=*state.getRawParameterValue("hp"),lpHz=*state.getRawParameterValue("lp");
 const bool filtering=*state.getRawParameterValue("filter")>0.5f;
 const auto hpCoeff=juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate,juce::jmax(1.f,hpHz),0.7071f);
 const auto lpCoeff=juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate,lpHz,0.7071f);
 for(int c=0;c<2;++c){*hp[c].coefficients=*hpCoeff;*lp[c].coefficients=*lpCoeff;}
 mixSmooth.setTargetValue(*state.getRawParameterValue("mix")/100.f);
 auto* left=buffer.getWritePointer(0);auto* right=buffer.getWritePointer(1);
 const int len=history.getNumSamples();
 for(int i=0;i<n;++i){
  const float inL=left[i],inR=right[i];
  history.setSample(0,write,0.5f*(inL+inR));
  const float dl=5.05f+4.95f*modL.next(sampleRate,lHz);
  const float dr=5.05f+4.95f*modR.next(sampleRate,rHz);
  currentL.store(dl,std::memory_order_relaxed);currentR.store(dr,std::memory_order_relaxed);
  auto readDelay=[&](float ms){
   double pos=(double)write-(double)ms*sampleRate/1000.0;
   double wrapped=std::fmod(pos+(double)len*2.0,(double)len);
   int idx=(int)wrapped;float frac=(float)(wrapped-idx);
   float a=history.getSample(0,idx),b=history.getSample(0,(idx+1)%len);
   return a+(b-a)*frac;
  };
  float wetL=readDelay(dl),wetR=readDelay(dr);
  if(filtering){
   // 12 dB/oct Butterworth high/low-pass on wet signals only.
   // Always advance filter states even at HP=0 to avoid stale-state transitions.
   if(hpHz>0.01f){wetL=hp[0].processSample(wetL);wetR=hp[1].processSample(wetR);}
   wetL=lp[0].processSample(wetL);wetR=lp[1].processSample(wetR);
  }
  const float m=mixSmooth.getNextValue();
  left[i]=inL+wetL*m;right[i]=inR+wetR*m;
  if(++write==len)write=0;
 }
}
void ADTProcessor::getStateInformation(juce::MemoryBlock& dest){auto xml=state.copyState().createXml();copyXmlToBinary(*xml,dest);}
void ADTProcessor::setStateInformation(const void* data,int size){auto xml=getXmlFromBinary(data,size);if(xml&&xml->hasTagName(state.state.getType()))state.replaceState(juce::ValueTree::fromXml(*xml));}
juce::AudioProcessorEditor* ADTProcessor::createEditor(){return new ADTEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new ADTProcessor();}
