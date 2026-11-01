#pragma once
#include "PluginProcessor.h"
#include <array>

class DVAES_EQAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit DVAES_EQAudioProcessorEditor(DVAES_EQAudioProcessor&);
    ~DVAES_EQAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override { repaint(); }
    DVAES_EQAudioProcessor& processor;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    struct BandControls
    {
        juce::Slider freq, gain, q;
        juce::ToggleButton enabled;
        juce::Label label, freqLabel, gainLabel, qLabel;
        std::unique_ptr<Attachment> freqLink, gainLink, qLink;
        std::unique_ptr<ButtonAttachment> enabledLink;
    };
    std::array<BandControls, DVAES_EQAudioProcessor::bandCount> bands;
    juce::Slider output;
    juce::ToggleButton bypass;
    juce::Label outputLabel;
    std::unique_ptr<Attachment> outputLink;
    std::unique_ptr<ButtonAttachment> bypassLink;
    juce::Rectangle<int> responseArea;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DVAES_EQAudioProcessorEditor)
};
