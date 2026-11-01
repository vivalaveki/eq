#pragma once
#include <JuceHeader.h>
#include <array>

class DVAES_EQAudioProcessor : public juce::AudioProcessor
{
public:
    DVAES_EQAudioProcessor();
    ~DVAES_EQAudioProcessor() override = default;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    static Coefficients::Ptr makeCoefficients(int band, float frequency, float gainDb, float q, double sampleRate);
    juce::AudioProcessorValueTreeState parameters;
    static constexpr int bandCount = 5;
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
private:
    using Filter = juce::dsp::IIR::Filter<float>;
    std::array<std::array<Filter, 2>, bandCount> filters;
    void updateFilters();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DVAES_EQAudioProcessor)
};
