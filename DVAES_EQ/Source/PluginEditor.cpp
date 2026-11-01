#include "PluginEditor.h"
#include <cmath>

namespace {
const char* prefixes[] = {"LS", "B1", "B2", "B3", "HS"};
const char* titles[] = {"LOW SHELF", "BELL 1", "BELL 2", "BELL 3", "HIGH SHELF"};
const juce::Colour accent[] = {
    juce::Colour(0xff4cc9f0), juce::Colour(0xff7c83ff),
    juce::Colour(0xffc77dff), juce::Colour(0xffffad66), juce::Colour(0xffff6584)
};
float xForHz(float f, juce::Rectangle<int> area)
{
    return area.getX() + (std::log(f / 20.f) / std::log(1000.f)) * area.getWidth();
}
float yForDb(float db, juce::Rectangle<int> area)
{
    return area.getCentreY() - db / 24.f * (area.getHeight() / 2.f);
}
}
DVAES_EQAudioProcessorEditor::DVAES_EQAudioProcessorEditor(DVAES_EQAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(940, 610);
    for (int i = 0; i < (int)bands.size(); ++i)
    {
        auto& b = bands[(size_t)i];
        const auto key = juce::String(prefixes[i]);
        b.label.setText(titles[i], juce::dontSendNotification);
        b.label.setJustificationType(juce::Justification::centred);
        b.label.setColour(juce::Label::textColourId, accent[i]);
        b.label.setFont(juce::FontOptions(13.f, juce::Font::bold));
        addAndMakeVisible(b.label);
        auto configure = [this](juce::Slider& slider)
        {
            slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 95, 20);
            slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff7a81ff));
            slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff333a4e));
            slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
            slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
            addAndMakeVisible(slider);
        };
        configure(b.freq); configure(b.gain); configure(b.q);
        b.freq.setTextValueSuffix(" Hz");
        b.gain.setTextValueSuffix(" dB");
        b.q.setNumDecimalPlacesToDisplay(2);
        b.freqLink = std::make_unique<Attachment>(processor.parameters, key + "_FREQ", b.freq);
        b.gainLink = std::make_unique<Attachment>(processor.parameters, key + "_GAIN", b.gain);
        b.qLink = std::make_unique<Attachment>(processor.parameters, key + "_Q", b.q);
        b.enabled.setButtonText("ON");
        b.enabled.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
        addAndMakeVisible(b.enabled);
        b.enabledLink = std::make_unique<ButtonAttachment>(processor.parameters, key + "_ON", b.enabled);
        for (auto* item : {&b.freqLabel, &b.gainLabel, &b.qLabel})
        {
            item->setJustificationType(juce::Justification::centred);
            item->setColour(juce::Label::textColourId, juce::Colour(0xff9da6bb));
            addAndMakeVisible(*item);
        }
        b.freqLabel.setText("FREQ", juce::dontSendNotification);
        b.gainLabel.setText("GAIN", juce::dontSendNotification);
        b.qLabel.setText("Q", juce::dontSendNotification);
    }
    output.setSliderStyle(juce::Slider::LinearHorizontal);
    output.setTextBoxStyle(juce::Slider::TextBoxRight, false, 85, 24);
    output.setTextValueSuffix(" dB");
    addAndMakeVisible(output);
    outputLink = std::make_unique<Attachment>(processor.parameters, "OUTPUT", output);
    outputLabel.setText("OUTPUT", juce::dontSendNotification);
    outputLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(outputLabel);
    bypass.setButtonText("BYPASS");
    bypass.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(bypass);
    bypassLink = std::make_unique<ButtonAttachment>(processor.parameters, "BYPASS", bypass);
    startTimerHz(25);
}
void DVAES_EQAudioProcessorEditor::resized()
{
    responseArea = {24, 92, getWidth() - 48, 285};
    const int usable = getWidth() - 48;
    const int colWidth = usable / 5;
    for (int i = 0; i < (int)bands.size(); ++i)
    {
        auto& b = bands[(size_t)i];
        const int x = 24 + i * colWidth;
        b.label.setBounds(x, 395, colWidth, 22);
        b.enabled.setBounds(x + colWidth / 2 - 24, 420, 65, 24);
        const int knobWidth = (colWidth - 8) / 3;
        juce::Slider* controls[] = {&b.freq, &b.gain, &b.q};
        juce::Label* labels[] = {&b.freqLabel, &b.gainLabel, &b.qLabel};
        for (int j = 0; j < 3; ++j)
        {
            controls[j]->setBounds(x + j * knobWidth, 457, knobWidth, 86);
            labels[j]->setBounds(x + j * knobWidth, 542, knobWidth, 18);
        }
    }
    outputLabel.setBounds(570, 35, 75, 30);
    output.setBounds(645, 35, 190, 30);
    bypass.setBounds(835, 35, 95, 30);
}
void DVAES_EQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff10131b));
    g.setColour(juce::Colour(0xfff2f4fc));
    g.setFont(juce::FontOptions(27.f, juce::Font::bold));
    g.drawText("DVAES", 24, 24, 115, 38, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff929afc));
    g.drawText("EQ", 142, 24, 75, 38, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff7d8598));
    g.setFont(juce::FontOptions(11.f));
    g.drawText("5-BAND PARAMETRIC EQUALIZER  /  V1.0", 235, 29, 300, 29, juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xff191e2a));
    g.fillRoundedRectangle(responseArea.toFloat(), 10.f);
    g.saveState();
    g.reduceClipRegion(responseArea);
    const float ticks[] = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    for (float hz : ticks)
    {
        float x = xForHz(hz, responseArea);
        g.setColour(juce::Colour(0xff2d3443));
        g.drawVerticalLine((int)x, (float)responseArea.getY(), (float)responseArea.getBottom());
        g.setColour(juce::Colour(0xff778197));
        g.setFont(juce::FontOptions(10.f));
        g.drawText(hz >= 1000 ? juce::String(hz / 1000.f, hz == 1000 || hz == 2000 || hz == 5000 || hz == 10000 || hz == 20000 ? 0 : 1) + "k" : juce::String((int)hz),
                   (int)x + 3, responseArea.getBottom() - 18, 35, 16, juce::Justification::left);
    }
    for (int db = -18; db <= 18; db += 6)
    {
        float y = yForDb((float)db, responseArea);
        g.setColour(db == 0 ? juce::Colour(0xff566071) : juce::Colour(0xff2d3443));
        g.drawHorizontalLine((int)y, (float)responseArea.getX(), (float)responseArea.getRight());
        g.setColour(juce::Colour(0xff98a0b3));
        g.drawText(juce::String(db) + " dB", responseArea.getX() + 5, (int)y - 16, 52, 15, juce::Justification::left);
    }
    juce::Path curve, fill;
    bool started = false;
    const double sr = processor.getSampleRate() > 0 ? processor.getSampleRate() : 44100.0;
    for (int x = responseArea.getX(); x <= responseArea.getRight(); x += 2)
    {
        const float hz = 20.f * std::pow(1000.f, (x - responseArea.getX()) / (float)responseArea.getWidth());
        double magnitude = 1.0;
        for (int i = 0; i < DVAES_EQAudioProcessor::bandCount; ++i)
        {
            const auto prefix = juce::String(prefixes[i]);
            if (processor.parameters.getRawParameterValue(prefix + "_ON")->load() < 0.5f) continue;
            auto coef = DVAES_EQAudioProcessor::makeCoefficients(i,
                processor.parameters.getRawParameterValue(prefix + "_FREQ")->load(),
                processor.parameters.getRawParameterValue(prefix + "_GAIN")->load(),
                processor.parameters.getRawParameterValue(prefix + "_Q")->load(), sr);
            magnitude *= coef->getMagnitudeForFrequency(hz, sr);
        }
        const float db = (float)juce::Decibels::gainToDecibels(magnitude, -60.0);
        const float y = yForDb(db, responseArea);
        if (!started) { curve.startNewSubPath((float)x, y); fill.startNewSubPath((float)x, y); started = true; }
        else { curve.lineTo((float)x, y); fill.lineTo((float)x, y); }
    }
    fill.lineTo((float)responseArea.getRight(), (float)responseArea.getBottom());
    fill.lineTo((float)responseArea.getX(), (float)responseArea.getBottom());
    fill.closeSubPath();
    g.setColour(juce::Colour(0x223fa9f5));
    g.fillPath(fill);
    g.setColour(juce::Colour(0xff76caff));
    g.strokePath(curve, juce::PathStrokeType(2.5f));
    for (int i = 0; i < DVAES_EQAudioProcessor::bandCount; ++i)
    {
        auto prefix = juce::String(prefixes[i]);
        if (processor.parameters.getRawParameterValue(prefix + "_ON")->load() < 0.5f) continue;
        float hz = processor.parameters.getRawParameterValue(prefix + "_FREQ")->load();
        float db = processor.parameters.getRawParameterValue(prefix + "_GAIN")->load();
        float x = xForHz(hz, responseArea), y = yForDb(db, responseArea);
        g.setColour(accent[i]);
        g.fillEllipse(x - 6, y - 6, 12, 12);
        g.setColour(juce::Colours::white);
        g.drawEllipse(x - 6, y - 6, 12, 12, 1.f);
    }
    g.restoreState();
    g.setColour(juce::Colour(0xff778197));
    g.setFont(juce::FontOptions(11.f));
    g.drawText("DVAES DIGITAL  •  PRECISION AUDIO TOOLS", 24, 577, 420, 18, juce::Justification::left);
}
