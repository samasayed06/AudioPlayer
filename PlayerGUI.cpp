/*
  ==============================================================================

    PlayerGUI.cpp
    Created: 21 Oct 2025 4:33:31pm
    Author:  USER

  ==============================================================================
*/

#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& player) : audioPlayer(player)
{
    setupButtons();

    // إعداد السلايدر الخاص بتقدم الأغنية
    addAndMakeVisible(positionSlider);
    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.addListener(this);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // Labels لعرض نقاط A و B
    addAndMakeVisible(loopALabel);
    addAndMakeVisible(loopBLabel);
    loopALabel.setText("A: --:--", juce::dontSendNotification);
    loopBLabel.setText("B: --:--", juce::dontSendNotification);
    loopALabel.setColour(juce::Label::textColourId, juce::Colours::white);
    loopBLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    loopALabel.setJustificationType(juce::Justification::centred);
    loopBLabel.setJustificationType(juce::Justification::centred);

    startTimerHz(10); // التحديث كل 0.1 ثانية
}

void PlayerGUI::setupButtons()
{
    // إضافة كل الأزرار
    addAndMakeVisible(loadButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(restartButton);
    addAndMakeVisible(startButton);
    addAndMakeVisible(endButton);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(setAButton);
    addAndMakeVisible(setBButton);
    addAndMakeVisible(loopABButton);

    // تحميل ملف
    loadButton.onClick = [this]()
        {
            auto chooser = std::make_unique<juce::FileChooser>(
                "Select an audio file...", juce::File(), "*.mp3;*.wav;*.aiff");

            chooser->launchAsync(
                juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this](const juce::FileChooser& fc)
                {
                    auto file = fc.getResult();
                    if (file.existsAsFile())
                        audioPlayer.loadFile(file);
                });

            fileChooser = std::move(chooser);
        };

    // أزرار التشغيل العادية
    playButton.onClick = [this]() { audioPlayer.play(); };
    pauseButton.onClick = [this]() { audioPlayer.pause(); };
    restartButton.onClick = [this]() { audioPlayer.restart(); };
    startButton.onClick = [this]() { audioPlayer.start(); };
    endButton.onClick = [this]() { audioPlayer.end(); };

    // نقاط A-B
    setAButton.onClick = [this]() {
        audioPlayer.setLoopA();
        double a = audioPlayer.getCurrentPosition();
        loopALabel.setText("A: " + juce::String(a, 2) + "s", juce::dontSendNotification);
        };

    setBButton.onClick = [this]() {
        audioPlayer.setLoopB();
        double b = audioPlayer.getCurrentPosition();
        loopBLabel.setText("B: " + juce::String(b, 2) + "s", juce::dontSendNotification);
        };

    loopABButton.onClick = [this]() {
        audioPlayer.toggleABLoop();
        loopABButton.setButtonText(
            loopABButton.getButtonText() == "A-B Loop OFF" ? "A-B Loop ON" : "A-B Loop OFF");
        };
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds().reduced(10);

    int buttonHeight = 40;
    int spacing = 10;
    int buttonWidth = 80;
    int x = 10;
    int y = 10;

    // الصف الأول: أزرار التشغيل الرئيسية
    loadButton.setBounds(x, y, buttonWidth, buttonHeight);
    playButton.setBounds(x + (buttonWidth + spacing) * 1, y, buttonWidth, buttonHeight);
    pauseButton.setBounds(x + (buttonWidth + spacing) * 2, y, buttonWidth, buttonHeight);
    restartButton.setBounds(x + (buttonWidth + spacing) * 3, y, buttonWidth, buttonHeight);
    startButton.setBounds(x + (buttonWidth + spacing) * 4, y, buttonWidth, buttonHeight);
    endButton.setBounds(x + (buttonWidth + spacing) * 5, y, buttonWidth, buttonHeight);

    // الصف الثاني: أزرار A-B Loop
    int y2 = y + buttonHeight + spacing;
    setAButton.setBounds(x, y2, buttonWidth, buttonHeight);
    setBButton.setBounds(x + (buttonWidth + spacing) * 1, y2, buttonWidth, buttonHeight);
    loopABButton.setBounds(x + (buttonWidth + spacing) * 2, y2, buttonWidth * 2, buttonHeight);

    // الصف الثالث: الليبلز لعرض نقاط A و B
    int y3 = y2 + buttonHeight + spacing;
    loopALabel.setBounds(x, y3, buttonWidth * 2, buttonHeight);
    loopBLabel.setBounds(x + (buttonWidth + spacing) * 2, y3, buttonWidth * 2, buttonHeight);

    // الصف الرابع: السلايدر الخاص بالموضع
    int y4 = y3 + buttonHeight + spacing;
    positionSlider.setBounds(x, y4, (buttonWidth + spacing) * 6, 30);
}


void PlayerGUI::timerCallback()
{
    auto length = audioPlayer.getLengthInSeconds();
    if (length > 0)
    {
        double pos = audioPlayer.getCurrentPosition() / length;
        positionSlider.setValue(pos, juce::dontSendNotification);
    }

    // تحقق من التكرار A-B
    audioPlayer.checkAndLoop();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &positionSlider)
    {
        double newPos = slider->getValue() * audioPlayer.getLengthInSeconds();
        audioPlayer.setPosition(newPos);
    }
}
