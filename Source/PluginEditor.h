#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include "PluginProcessor.h"
#include <array>
class ADTEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
 explicit ADTEditor(ADTProcessor&);
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 using SliderAttachment=juce::AudioProcessorValueTreeState::SliderAttachment;
 using ButtonAttachment=juce::AudioProcessorValueTreeState::ButtonAttachment;
 ADTProcessor& proc;
 juce::Slider mix,lfoL,lfoR,hp,lp;
 juce::ToggleButton filter{"FILTER ON"};
 std::unique_ptr<SliderAttachment> aMix,aL,aR,aHp,aLp;
 std::unique_ptr<ButtonAttachment> aFilter;
 std::array<float,260> traceL{},traceR{};
 void setup(juce::Slider&,const juce::String&,std::unique_ptr<SliderAttachment>&);
 void timerCallback() override;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADTEditor)
};
