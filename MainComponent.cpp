#include "MainComponent.h"
#include <cmath>

// ============================================================
//  Constructor
// ============================================================
MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    // أزرار التحكم
    for (auto* btn : { &loadButton, &restartButton, &playButton, &pauseButton,
                       &startButton, &endButton, &muteButton, &loopButton,
                       &setAButton, &setBButton, &abLoopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // السلايدرات
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(previousGain);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    progressSlider.setRange(0.0, 1.0, 0.001);
    progressSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    progressSlider.addListener(this);
    addAndMakeVisible(progressSlider);

    // الليبلز
    addAndMakeVisible(currentTimeLabel);
    addAndMakeVisible(totalTimeLabel);
    addAndMakeVisible(aLabel);
    addAndMakeVisible(bLabel);

    currentTimeLabel.setText("00:00", juce::dontSendNotification);
    totalTimeLabel.setText("00:00", juce::dontSendNotification);
    aLabel.setText("A: --:--", juce::dontSendNotification);
    bLabel.setText("B: --:--", juce::dontSendNotification);

    currentTimeLabel.setJustificationType(juce::Justification::centredLeft);
    totalTimeLabel.setJustificationType(juce::Justification::centredRight);
    aLabel.setJustificationType(juce::Justification::centred);
    bLabel.setJustificationType(juce::Justification::centred);

    setSize(750, 350);
    setAudioChannels(0, 2);
    startTimerHz(10);
}

// ============================================================
//  Destructor
// ============================================================
MainComponent::~MainComponent()
{
    shutdownAudio();
}

// ============================================================
//  Audio
// ============================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    transportSource.setGain(previousGain);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
        bufferToFill.clearActiveBufferRegion();
    else
        transportSource.getNextAudioBlock(bufferToFill);

    // تحقق من A–B Loop أثناء التشغيل
    if (isABLooping)
    {
        double pos = transportSource.getCurrentPosition();
        if (pos >= loopB)
            transportSource.setPosition(loopA);
    }
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}

// ============================================================
//  GUI
// ============================================================
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    int w = 80;
    int h = 40;
    int spacing = 10;
    int startX = 20;
    int startY = 20;

    loadButton.setBounds(startX, startY, w, h);
    restartButton.setBounds(startX + (w + spacing) * 1, startY, w, h);
    playButton.setBounds(startX + (w + spacing) * 2, startY, w, h);
    pauseButton.setBounds(startX + (w + spacing) * 3, startY, w, h);

    int sliderY = startY + h + 15;
    volumeSlider.setBounds(startX, sliderY, (w + spacing) * 4 - spacing, 30);

    int progressY = sliderY + 45;
    progressSlider.setBounds(startX + 65, progressY, (w + spacing) * 4 - spacing - 130, 30);

    int labelWidth = 60;
    currentTimeLabel.setBounds(progressSlider.getX() - labelWidth - 5, progressY, labelWidth, progressSlider.getHeight());
    totalTimeLabel.setBounds(progressSlider.getRight() + 5, progressY, labelWidth, progressSlider.getHeight());

    int secondRowY = progressY + 50;
    startButton.setBounds(startX, secondRowY, w, h);
    endButton.setBounds(startX + (w + spacing) * 1, secondRowY, w, h);
    loopButton.setBounds(startX + (w + spacing) * 2, secondRowY, w, h);

    int thirdRowY = secondRowY + h + 10;
    setAButton.setBounds(startX, thirdRowY, w, h);
    setBButton.setBounds(startX + (w + spacing) * 1, thirdRowY, w, h);
    abLoopButton.setBounds(startX + (w + spacing) * 2, thirdRowY, w * 2, h);

    int labelY = thirdRowY + h + 5;
    aLabel.setBounds(startX, labelY, w * 2, 25);
    bLabel.setBounds(startX + (w + spacing) * 2, labelY, w * 2, 25);

    int muteX = startX + (w + spacing) * 4 + 40;
    muteButton.setBounds(muteX, sliderY, w, h);
}

// ============================================================
//  Button Actions
// ============================================================
void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...", juce::File{}, "*.wav;*.mp3;*.aiff");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    if (auto* reader = formatManager.createReaderFor(file))
                    {
                        transportSource.stop();
                        transportSource.setSource(nullptr);
                        readerSource.reset();

                        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
                        transportSource.setGain(previousGain);
                    }
                }
            });
    }
    else if (button == &playButton) transportSource.start();
    else if (button == &pauseButton) transportSource.stop();
    else if (button == &restartButton) { transportSource.setPosition(0.0); transportSource.start(); }
    else if (button == &startButton) transportSource.setPosition(0.0);
    else if (button == &endButton) transportSource.setPosition(transportSource.getLengthInSeconds());
    else if (button == &muteButton)
    {
        if (!isMuted)
        {
            previousGain = (float)volumeSlider.getValue();
            transportSource.setGain(0.0f);
            volumeSlider.setValue(0.0, juce::dontSendNotification);
            muteButton.setButtonText("Unmute");
            isMuted = true;
        }
        else
        {
            transportSource.setGain(previousGain);
            volumeSlider.setValue(previousGain, juce::dontSendNotification);
            muteButton.setButtonText("Mute");
            isMuted = false;
        }
    }
    else if (button == &loopButton)
    {
        isLooping = !isLooping;
        loopButton.setButtonText(isLooping ? "Loop ON" : "Loop OFF");
    }
    else if (button == &setAButton)
    {
        loopA = transportSource.getCurrentPosition();
        aLabel.setText("A: " + juce::String(loopA, 2) + "s", juce::dontSendNotification);
    }
    else if (button == &setBButton)
    {
        loopB = transportSource.getCurrentPosition();
        bLabel.setText("B: " + juce::String(loopB, 2) + "s", juce::dontSendNotification);
        if (loopB < loopA) std::swap(loopA, loopB);
    }
    else if (button == &abLoopButton)
    {
        isABLooping = !isABLooping;
        abLoopButton.setButtonText(isABLooping ? "A–B Loop ON" : "A–B Loop OFF");
    }
}

// ============================================================
//  Sliders
// ============================================================
void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        float newGain = (float)slider->getValue();
        if (isMuted)
        {
            previousGain = newGain;
            transportSource.setGain(0.0f);
        }
        else
        {
            transportSource.setGain(newGain);
            previousGain = newGain;
        }
    }
    else if (slider == &progressSlider && readerSource.get() != nullptr)
    {
        double length = transportSource.getLengthInSeconds();
        double newPos = slider->getValue() * length;
        transportSource.setPosition(newPos);
    }
}

void MainComponent::sliderDragStarted(juce::Slider* slider)
{
    if (slider == &progressSlider)
        isDraggingSlider = true;
}

void MainComponent::sliderDragEnded(juce::Slider* slider)
{
    if (slider == &progressSlider)
        isDraggingSlider = false;
}

// ============================================================
//  Timer
// ============================================================
void MainComponent::timerCallback()
{
    if (readerSource.get() == nullptr)
        return;

    double length = transportSource.getLengthInSeconds();
    double position = transportSource.getCurrentPosition();

    if (!isDraggingSlider && length > 0)
    {
        double posRatio = position / length;
        progressSlider.setValue(posRatio, juce::dontSendNotification);
    }

    auto formatTime = [](double seconds)
        {
            int mins = (int)(seconds / 60);
            int secs = (int)std::fmod(seconds, 60);
            return juce::String::formatted("%02d:%02d", mins, secs);
        };

    currentTimeLabel.setText(formatTime(position), juce::dontSendNotification);
    totalTimeLabel.setText(formatTime(length), juce::dontSendNotification);
}
