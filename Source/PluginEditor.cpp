#include "PluginEditor.h"
ADTEditor::ADTEditor(ADTProcessor& p):AudioProcessorEditor(&p),proc(p){
 setSize(960,570);
 setup(mix,"mix",aMix);setup(lfoL,"lfoL",aL);setup(lfoR,"lfoR",aR);
 setup(hp,"hp",aHp);setup(lp,"lp",aLp);
 addAndMakeVisible(filter);filter.setColour(juce::ToggleButton::tickColourId,juce::Colours::cyan);
 aFilter=std::make_unique<ButtonAttachment>(proc.state,"filter",filter);
 traceL.fill(5.05f);traceR.fill(5.05f);startTimerHz(30);
}
void ADTEditor::setup(juce::Slider& s,const juce::String& id,std::unique_ptr<SliderAttachment>& a){
 addAndMakeVisible(s);s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
 s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,110,25);
 s.setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(0xff35dce5));
 s.setColour(juce::Slider::textBoxTextColourId,juce::Colours::white);
 a=std::make_unique<SliderAttachment>(proc.state,id,s);
}
void ADTEditor::timerCallback(){
 std::move(traceL.begin()+1,traceL.end(),traceL.begin());
 std::move(traceR.begin()+1,traceR.end(),traceR.begin());
 traceL.back()=proc.currentL.load();traceR.back()=proc.currentR.load();repaint(juce::Rectangle<int>(235,112,490,240));
}
void ADTEditor::paint(juce::Graphics& g){
 g.fillAll(juce::Colour(0xff10151d));
 g.setColour(juce::Colour(0xff252e39));g.fillRoundedRectangle(12,12,936,546,12);
 g.setColour(juce::Colours::white);g.setFont(juce::Font(juce::FontOptions(38.f).withStyle("Bold")));g.drawText("ADT",32,20,130,48,juce::Justification::left);
 g.setFont(juce::Font(juce::FontOptions(18.f)));g.drawText("Artificial Double Tracking   |   LM18   |   V0.4",170,25,740,38,juce::Justification::left);
 auto panel=[&](int x,int y,int w,int h){g.setColour(juce::Colour(0xff151e28));g.fillRoundedRectangle((float)x,(float)y,(float)w,(float)h,9.f);g.setColour(juce::Colour(0xff445260));g.drawRoundedRectangle((float)x,(float)y,(float)w,(float)h,9.f,1.f);};
 panel(28,90,194,290);panel(235,90,490,290);panel(738,90,194,290);panel(28,395,904,145);
 g.setColour(juce::Colours::white);g.setFont(juce::Font(juce::FontOptions(19.f)));
 g.drawText("LFO 1   (L)",45,100,160,28,juce::Justification::centred);
 g.drawText("LFO 2   (R)",755,100,160,28,juce::Justification::centred);
 g.drawText("DELAY MODULATION  0.1–10 ms",250,100,440,28,juce::Justification::centredLeft);
 g.drawText("FILTER  •  ADT ONLY",45,405,340,26,juce::Justification::left);
 g.setFont(juce::Font(juce::FontOptions(15.f)));
 g.drawText("0.05–6 Hz",50,340,150,24,juce::Justification::centred);
 g.drawText("0.05–6 Hz",760,340,150,24,juce::Justification::centred);
 g.drawText("MIX",440,401,80,25,juce::Justification::centred);
 g.drawText("HIGH PASS  0–300 Hz",210,413,210,25,juce::Justification::centred);
 g.drawText("LOW PASS  1.5–15 kHz",615,413,240,25,juce::Justification::centred);
 auto graph=juce::Rectangle<float>(255,145,450,205);
 g.setColour(juce::Colour(0xff0b131b));g.fillRect(graph);
 g.setColour(juce::Colour(0xff283644));for(int i=0;i<=10;++i){float y=graph.getY()+i*graph.getHeight()/10.f;g.drawHorizontalLine((int)y,graph.getX(),graph.getRight());}
 auto draw=[&](const std::array<float,260>& a,juce::Colour c){juce::Path path;for(size_t i=0;i<a.size();++i){float x=graph.getX()+graph.getWidth()*(float)i/(a.size()-1);float y=graph.getBottom()-(a[i]-0.1f)/9.9f*graph.getHeight();if(i==0)path.startNewSubPath(x,y);else path.lineTo(x,y);}g.setColour(c);g.strokePath(path,juce::PathStrokeType(2.f));};
 draw(traceL,juce::Colours::cyan);draw(traceR,juce::Colour(0xffe68ce8));
 g.setColour(juce::Colours::cyan);g.drawText("L",660,130,20,20,juce::Justification::centred);
 g.setColour(juce::Colour(0xffe68ce8));g.drawText("R",684,130,20,20,juce::Justification::centred);
}
void ADTEditor::resized(){
 lfoL.setBounds(50,155,150,180);lfoR.setBounds(760,155,150,180);
 mix.setBounds(426,418,108,112);
 hp.setBounds(244,442,120,90);lp.setBounds(665,442,120,90);
 filter.setBounds(45,455,150,38);
}
