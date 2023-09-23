#include "PluginProcessor.h"
#include "PluginEditor.h"

DualChannelAudioProcessorEditor::DualChannelAudioProcessorEditor(DualChannelAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processor(p), valueTreeState(vts), backgroundApp(Colour(Constants::Colors::bg_App)),
    firstInputGainSlider(this), firstAttackSlider(this), firstReleaseSlider(this), firstThresholdSlider(this), firstRatioSlider(this), firstOutGainSlider(this),
    secondInputGainSlider(this), secondAttackSlider(this), secondReleaseSlider(this), secondThresholdSlider(this), secondRatioSlider(this), secondOutGainSlider(this),
    leftGainMeter(), rightGainMeter(),
    highPassSlider(this), lowPassSlider(this),
    highPassToggle("HP"), lowPassToggle("LP"),
    sidechainSlider(this), sidechainToggle("Sidechain"),
    mixSlider(this),
    lrToggle("MidSide"), linkingToggle("Linking"),
    oversamplingComboBox("Oversampling factor"), oversamplingToggle("Oversampling"), lowPassFreqSlider(this), highPassFreqSlider(this)
{
    setLookAndFeel(&LAF);
    initWidgets();
    setSize(900, 600);
    startTimerHz(60);
    
    firstInputGainSlider.slider.addListener(this);
    firstAttackSlider.slider.addListener(this);
    firstReleaseSlider.slider.addListener(this);
    firstThresholdSlider.slider.addListener(this);
    firstRatioSlider.slider.addListener(this);
    firstOutGainSlider.slider.addListener(this);

    secondInputGainSlider.slider.addListener(this);
    secondAttackSlider.slider.addListener(this);
    secondReleaseSlider.slider.addListener(this);
    secondThresholdSlider.slider.addListener(this);
    secondRatioSlider.slider.addListener(this);
    secondOutGainSlider.slider.addListener(this);

    firstThresholdSlider.slider.setValue(-32);
    firstRatioSlider.slider.setValue(2);
    
    secondThresholdSlider.slider.setValue(-32);
    secondRatioSlider.slider.setValue(2);
}


DualChannelAudioProcessorEditor::~DualChannelAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

void DualChannelAudioProcessorEditor::timerCallback()
{
    float leftGainReduction = processor.getLeftGainReduction();
    float rightGainReduction = processor.getRightGainReduction();

    leftGainMeter.update(leftGainReduction);
    rightGainMeter.update(rightGainReduction);
    
    if (!sidechainError)
    {
        sidechainError = false;
        sidechainToggle.setToggleState(false, dontSendNotification);
    } 
    
}

//==============================================================================
void DualChannelAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll(backgroundApp);
}

void DualChannelAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(Constants::Margins::big);

    const auto oneFifthHeight = area.getHeight() / 5;

    auto lmButtonsArea = area.removeFromTop(oneFifthHeight).reduced(Constants::Margins::small);
    auto rsButtonsArea = area.removeFromTop(oneFifthHeight).reduced(Constants::Margins::small);
    auto meterButtonsArea = area.removeFromTop(oneFifthHeight).reduced(Constants::Margins::small);
    auto filtersButtonsArea = area.removeFromBottom(oneFifthHeight).reduced(Constants::Margins::medium);
    auto otherButtonsArea = area.removeFromBottom(oneFifthHeight).reduced(Constants::Margins::medium);

    const auto knobMarginSmall = FlexItem::Margin(Constants::Margins::medium);

    FlexBox lmButtonsBox;
    lmButtonsBox.flexWrap = FlexBox::Wrap::noWrap;
    lmButtonsBox.flexDirection = FlexBox::Direction::row;
    lmButtonsBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    lmButtonsBox.items.add(FlexItem(firstInputGainSlider).withFlex(1).withMargin(knobMarginSmall));
    lmButtonsBox.items.add(FlexItem(firstAttackSlider).withFlex(1).withMargin(knobMarginSmall));
    lmButtonsBox.items.add(FlexItem(firstReleaseSlider).withFlex(1).withMargin(knobMarginSmall));
    lmButtonsBox.items.add(FlexItem(firstThresholdSlider).withFlex(1).withMargin(knobMarginSmall));
    lmButtonsBox.items.add(FlexItem(firstRatioSlider).withFlex(1).withMargin(knobMarginSmall));
    lmButtonsBox.items.add(FlexItem(firstOutGainSlider).withFlex(1).withMargin(knobMarginSmall));
    lmButtonsBox.performLayout(lmButtonsArea.toFloat());

    FlexBox rsBUttonsBox;
    rsBUttonsBox.flexWrap = FlexBox::Wrap::noWrap;
    rsBUttonsBox.flexDirection = FlexBox::Direction::row;
    rsBUttonsBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    rsBUttonsBox.items.add(FlexItem(secondInputGainSlider).withFlex(1).withMargin(knobMarginSmall));
    rsBUttonsBox.items.add(FlexItem(secondAttackSlider).withFlex(1).withMargin(knobMarginSmall));
    rsBUttonsBox.items.add(FlexItem(secondReleaseSlider).withFlex(1).withMargin(knobMarginSmall));
    rsBUttonsBox.items.add(FlexItem(secondThresholdSlider).withFlex(1).withMargin(knobMarginSmall));
    rsBUttonsBox.items.add(FlexItem(secondRatioSlider).withFlex(1).withMargin(knobMarginSmall));
    rsBUttonsBox.items.add(FlexItem(secondOutGainSlider).withFlex(1).withMargin(knobMarginSmall));
    rsBUttonsBox.performLayout(rsButtonsArea.toFloat());
    
    FlexBox meterButtonsBox;
    meterButtonsBox.flexWrap = FlexBox::Wrap::noWrap;
    meterButtonsBox.flexDirection = FlexBox::Direction::row;
    meterButtonsBox.justifyContent = FlexBox::JustifyContent::spaceAround;

    meterButtonsBox.items.add(FlexItem(leftGainMeter).withFlex(1).withMargin(knobMarginSmall));
    meterButtonsBox.items.add(FlexItem(rightGainMeter).withFlex(1).withMargin(knobMarginSmall));

    meterButtonsBox.performLayout(meterButtonsArea.toFloat());

    FlexBox filterButtonsBox;
    filterButtonsBox.flexWrap = FlexBox::Wrap::noWrap;
    filterButtonsBox.flexDirection = FlexBox::Direction::row;
    filterButtonsBox.justifyContent = FlexBox::JustifyContent::spaceAround;

    filterButtonsBox.items.add(FlexItem(highPassFreqSlider).withFlex(0.6).withMargin(knobMarginSmall));
    filterButtonsBox.items.add(FlexItem(highPassSlider).withFlex(1).withMargin(knobMarginSmall));
    filterButtonsBox.items.add(FlexItem(highPassToggle).withFlex(1).withMargin(knobMarginSmall));

    filterButtonsBox.items.add(FlexItem(lowPassFreqSlider).withFlex(0.6).withMargin(knobMarginSmall));
    filterButtonsBox.items.add(FlexItem(lowPassSlider).withFlex(1).withMargin(knobMarginSmall));
    filterButtonsBox.items.add(FlexItem(lowPassToggle).withFlex(1).withMargin(knobMarginSmall));

    filterButtonsBox.items.add(FlexItem(oversamplingComboBox).withFlex(1).withHeight(30).withMargin(40));
    filterButtonsBox.items.add(FlexItem(oversamplingToggle).withFlex(1).withMargin(knobMarginSmall));

    filterButtonsBox.performLayout(filtersButtonsArea.toFloat());

    FlexBox otherButtonsBox;
    otherButtonsBox.flexWrap = FlexBox::Wrap::noWrap;
    otherButtonsBox.flexDirection = FlexBox::Direction::row;
    otherButtonsBox.justifyContent = FlexBox::JustifyContent::spaceAround;

    otherButtonsBox.items.add(FlexItem(sidechainSlider).withFlex(1).withMargin(knobMarginSmall));
    otherButtonsBox.items.add(FlexItem(sidechainToggle).withFlex(1).withMargin(knobMarginSmall));
    
    otherButtonsBox.items.add(FlexItem(linkingToggle).withFlex(1).withMargin(knobMarginSmall));
    otherButtonsBox.items.add(FlexItem(lrToggle).withFlex(1).withMargin(knobMarginSmall));

    otherButtonsBox.items.add(FlexItem(mixSlider).withFlex(1).withMargin(knobMarginSmall));

    otherButtonsBox.performLayout(otherButtonsArea.toFloat());
}

void DualChannelAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (linkingApplied)
    {
        if (slider == &firstInputGainSlider.slider ||
            slider == &firstAttackSlider.slider ||
            slider == &firstReleaseSlider.slider ||
            slider == &firstThresholdSlider.slider ||
            slider == &firstRatioSlider.slider ||
            slider == &firstOutGainSlider.slider)
        {
            // The user is interacting with channel 1 sliders
            isChannel1Active = true;
        }
        else if (slider == &secondInputGainSlider.slider ||
                 slider == &secondAttackSlider.slider ||
                 slider == &secondReleaseSlider.slider ||
                 slider == &secondThresholdSlider.slider ||
                 slider == &secondRatioSlider.slider ||
                 slider == &secondOutGainSlider.slider)
        {
            // The user is interacting with channel 2 sliders
            isChannel1Active = false;
        }
        if (isChannel1Active)
        {
            // Synchronize values from the first channel to the second channel
            secondInputGainSlider.slider.setValue(firstInputGainSlider.slider.getValue());
            secondAttackSlider.slider.setValue(firstAttackSlider.slider.getValue());
            secondReleaseSlider.slider.setValue(firstReleaseSlider.slider.getValue());
            secondThresholdSlider.slider.setValue(firstThresholdSlider.slider.getValue());
            secondRatioSlider.slider.setValue(firstRatioSlider.slider.getValue());
            secondOutGainSlider.slider.setValue(firstOutGainSlider.slider.getValue());
        }
        else if (!isChannel1Active)
        {
            // Synchronize values from the second channel to the first channel
            firstInputGainSlider.slider.setValue(secondInputGainSlider.slider.getValue());
            firstAttackSlider.slider.setValue(secondAttackSlider.slider.getValue());
            firstReleaseSlider.slider.setValue(secondReleaseSlider.slider.getValue());
            firstThresholdSlider.slider.setValue(secondThresholdSlider.slider.getValue());
            firstRatioSlider.slider.setValue(secondRatioSlider.slider.getValue());
            firstOutGainSlider.slider.setValue(secondOutGainSlider.slider.getValue());
        }
    }
}
        
void DualChannelAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &linkingToggle)
    {
        processor.toggleLinking();
        linkingApplied = (clicked == 0); 
        clicked = (clicked == 0) ? 1 : 0; 
        sliderValueChanged(nullptr);
    }
    else if (button == &highPassToggle)
    {
        processor.toggleHighPass();
    }
    else if (button == &lowPassToggle)
    {
        processor.toggleLowPass();
    }
    else if (button == &sidechainToggle)
    {
        processor.toggleSidechain();
    }
    else if (button == &lrToggle)
    {
        processor.toggleLR();
    }
    else if (button == &oversamplingToggle)
    {
        processor.toggleOversampling();
    }
}

void DualChannelAudioProcessorEditor::comboBoxChanged(ComboBox* comboBox)
{
    if (comboBox == &oversamplingComboBox)
    {
        int selectedId = comboBox->getSelectedId();

        switch (selectedId)
        {
        case 1: { processor.setOversamplingFactor(1); break; }
        case 2: { processor.setOversamplingFactor(2); break; }
        case 3: { processor.setOversamplingFactor(4); break; }
        default: break;
        }
    }
}

void DualChannelAudioProcessorEditor::initWidgets()
{
    addAndMakeVisible(firstInputGainSlider);
    firstInputGainSlider.reset(valueTreeState, "f_inputgain");
    firstInputGainSlider.setLabelText("Input");

    addAndMakeVisible(firstAttackSlider);
    firstAttackSlider.reset(valueTreeState, "f_attack");
    firstAttackSlider.setLabelText("Attack");

    addAndMakeVisible(firstReleaseSlider);
    firstReleaseSlider.reset(valueTreeState, "f_release");
    firstReleaseSlider.setLabelText("Release");

    addAndMakeVisible(firstThresholdSlider);
    firstThresholdSlider.reset(valueTreeState, "f_threshold");
    firstThresholdSlider.setLabelText("Threshold");

    addAndMakeVisible(firstRatioSlider);
    firstRatioSlider.reset(valueTreeState, "f_ratio");
    firstRatioSlider.setLabelText("Ratio");

    addAndMakeVisible(firstOutGainSlider);
    firstOutGainSlider.reset(valueTreeState, "f_outputgain");
    firstOutGainSlider.setLabelText("Output");

    addAndMakeVisible(secondInputGainSlider);
    secondInputGainSlider.reset(valueTreeState, "s_inputgain");
    secondInputGainSlider.setLabelText("Input");

    addAndMakeVisible(secondAttackSlider);
    secondAttackSlider.reset(valueTreeState, "s_attack");
    secondAttackSlider.setLabelText("Attack");

    addAndMakeVisible(secondReleaseSlider);
    secondReleaseSlider.reset(valueTreeState, "s_release");
    secondReleaseSlider.setLabelText("Release");

    addAndMakeVisible(secondThresholdSlider);
    secondThresholdSlider.reset(valueTreeState, "s_threshold");
    secondThresholdSlider.setLabelText("Threshold");

    addAndMakeVisible(secondRatioSlider);
    secondRatioSlider.reset(valueTreeState, "s_ratio");
    secondRatioSlider.setLabelText("Ratio");

    addAndMakeVisible(secondOutGainSlider);
    secondOutGainSlider.reset(valueTreeState, "s_outputgain");
    secondOutGainSlider.setLabelText("Output");

    addAndMakeVisible(leftGainMeter);
    addAndMakeVisible(rightGainMeter);

    addAndMakeVisible(highPassSlider);
    highPassSlider.reset(valueTreeState, "high_pass");
    highPassSlider.setLabelText("Slope");

    addAndMakeVisible(highPassToggle);
    highPassToggle.addListener(this);

    addAndMakeVisible(lowPassSlider);
    lowPassSlider.reset(valueTreeState, "low_pass");
    lowPassSlider.setLabelText("Slope");

    addAndMakeVisible(lowPassToggle);
    lowPassToggle.addListener(this);

    addAndMakeVisible(sidechainSlider);
    sidechainSlider.reset(valueTreeState, "sidechain");
    sidechainSlider.setLabelText("Sidechain");

    addAndMakeVisible(sidechainToggle);
    sidechainToggle.addListener(this);

    addAndMakeVisible(mixSlider);
    mixSlider.reset(valueTreeState, "mix");
    mixSlider.setLabelText("Mix");

    addAndMakeVisible(lrToggle);
    lrToggle.addListener(this);

    addAndMakeVisible(linkingToggle);
    linkingToggle.addListener(this);

    addAndMakeVisible(oversamplingComboBox);
    oversamplingComboBox.addItem("2", 1);
    oversamplingComboBox.addItem("4", 2);
    oversamplingComboBox.addItem("8", 3);
    oversamplingComboBox.addListener(this);
    oversamplingComboBox.setSelectedId(1);

    addAndMakeVisible(oversamplingToggle);
    oversamplingToggle.addListener(this);

    addAndMakeVisible(lowPassFreqSlider);
    highPassFreqSlider.reset(valueTreeState, "high_pass_freq_slider");
    highPassFreqSlider.setLabelText("Cutoff");
    addAndMakeVisible(highPassFreqSlider);
    lowPassFreqSlider.reset(valueTreeState, "low_pass_freq_slider");
    lowPassFreqSlider.setLabelText("Cutoff");
}
