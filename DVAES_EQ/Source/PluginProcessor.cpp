#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
const char* ids[] = {"LS", "B1", "B2", "B3", "HS"};
constexpr float defaultFreq[] = {90.f, 250.f, 1000.f, 4000.f, 12000.f};
}

juce::AudioProcessorValueTreeState::ParameterLayout DVAES_EQAudioProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    const juce::NormalisableRange<float> frequencyRange (20.0f, 20000.0f, 0.0f, 0.23f);
    for (int i = 0; i < bandCount; ++i)
    {
        const auto prefix = juce::String(ids[i]);
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{prefix + "_FREQ", 1}, prefix + " Frequency", frequencyRange, defaultFreq[i]));
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{prefix + "_GAIN", 1}, prefix + " Gain", juce::NormalisableRange<float>(-18.f, 18.f, 0.1f), 0.f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{prefix + "_Q", 1}, prefix + " Q", juce::NormalisableRange<float>(0.2f, 10.f, 0.f, 0.4f), 0.707f));
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{prefix + "_ON", 1}, prefix + " Enabled", true));
    }
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"OUTPUT", 1}, "Output", juce::NormalisableRange<float>(-18.f, 18.f, 0.1f), 0.f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"BYPASS", 1}, "Bypass", false));
    return layout;
}

DVAES_EQAudioProcessor::DVAES_EQAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createLayout())
{}

DVAES_EQAudioProcessor::Coefficients::Ptr DVAES_EQAudioProcessor::makeCoefficients(int band, float frequency, float gainDb, float q, double sampleRate)
{
    const double freq = juce::jlimit(20.0, sampleRate * 0.49, (double) frequency);
    const float linearGain = juce::Decibels::decibelsToGain(gainDb);
    const float safeQ = juce::jmax(0.2f, q);
    if (band == 0) return Coefficients::makeLowShelf(sampleRate, freq, safeQ, linearGain);
    if (band == 4) return Coefficients::makeHighShelf(sampleRate, freq, safeQ, linearGain);
    return Coefficients::makePeakFilter(sampleRate, freq, safeQ, linearGain);
}

void DVAES_EQAudioProcessor::prepareToPlay(double sampleRate, int)
{
    for (auto& band : filters)
        for (auto& filter : band)
            filter.reset();
    updateFilters();
    juce::ignoreUnused(sampleRate);
}

void DVAES_EQAudioProcessor::updateFilters()
{
    for (int i = 0; i < bandCount; ++i)
    {
        auto p = juce::String(ids[i]);
        auto coeff = makeCoefficients(i,
            parameters.getRawParameterValue(p + "_FREQ")->load(),
            parameters.getRawParameterValue(p + "_GAIN")->load(),
            parameters.getRawParameterValue(p + "_Q")->load(), getSampleRate() > 0 ? getSampleRate() : 44100.0);
        for (auto& filter : filters[(size_t)i])
            *filter.coefficients = *coeff;
    }
}

void DVAES_EQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalInput = getTotalNumInputChannels();
    for (int ch = totalInput; ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());
    if (parameters.getRawParameterValue("BYPASS")->load() > 0.5f) return;
    updateFilters();
    const auto samples = buffer.getNumSamples();
    for (int band = 0; band < bandCount; ++band)
    {
        if (parameters.getRawParameterValue(juce::String(ids[band]) + "_ON")->load() < 0.5f) continue;
        for (int ch = 0; ch < juce::jmin(2, totalInput); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            auto& filter = filters[(size_t)band][(size_t)ch];
            for (int s = 0; s < samples; ++s)
                data[s] = filter.processSample(data[s]);
        }
    }
    buffer.applyGain(juce::Decibels::decibelsToGain(parameters.getRawParameterValue("OUTPUT")->load()));
}

bool DVAES_EQAudioProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    if (layout.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layout.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;
    return layout.getMainOutputChannelSet() == layout.getMainInputChannelSet();
}

void DVAES_EQAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    if (auto xml = parameters.copyState().createXml()) copyXmlToBinary(*xml, dest);
}
void DVAES_EQAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessorEditor* DVAES_EQAudioProcessor::createEditor() { return new DVAES_EQAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new DVAES_EQAudioProcessor(); }
