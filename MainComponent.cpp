#include "MainComponent.h"

MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    for (auto* btn : { &loadButton, &restartButton, &playButton, &pauseButton,
                       &startButton, &endButton, &muteButton, &loopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(previousGain);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    setSize(700, 300);
    setAudioChannels(0, 2); 
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

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

    if (isLooping && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds())
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}
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

    
    int secondRowY = sliderY + 45;
    startButton.setBounds(startX, secondRowY, w, h);
    endButton.setBounds(startX + (w + spacing) * 1, secondRowY, w, h);
    loopButton.setBounds(startX + (w + spacing) * 2, secondRowY, w, h);

    
    int muteX = startX + (w + spacing) * 4 + 40;
    muteButton.setBounds(muteX, sliderY, w, h);
}



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
    else if (button == &restartButton)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
    else if (button == &playButton)
    {
        transportSource.start();
    }
    else if (button == &pauseButton)
    {
        transportSource.stop();
    }
    else if (button == &startButton)
    {
        transportSource.setPosition(0.0);
    }
    else if (button == &endButton)
    {
        transportSource.setPosition(transportSource.getLengthInSeconds());
    }
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
        resized();
    }

}

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
}

