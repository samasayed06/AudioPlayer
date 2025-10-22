
#include "MainComponent.h"

MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    // Add buttons (Load, Restart, Play, Pause, Start, End)
    for (auto* btn : { &loadButton, &restartButton, &playButton, &pauseButton, &startButton, &endButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider setup
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    setSize(600, 250);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
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
    int y = 20;
    int w = 70;
    int h = 40;
    int x = 20;

    loadButton.setBounds(x, y, w, h); x += 80;
    restartButton.setBounds(x, y, w, h); x += 80;
    playButton.setBounds(x, y, w, h); x += 80;
    pauseButton.setBounds(x, y, w, h); x += 80;
    startButton.setBounds(x, y, w, h); x += 80;
    endButton.setBounds(x, y, w, h);

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...", juce::File{}, "*.wav;*.mp3");

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
                    }
                }
            });
    }

    if (button == &restartButton)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }

    if (button == &playButton)
    {
        transportSource.start();
    }

    if (button == &pauseButton)
    {
        transportSource.stop();
    }

    if (button == &startButton)
    {
        transportSource.setPosition(0.0);
    }

    if (button == &endButton)
    {
        transportSource.setPosition(transportSource.getLengthInSeconds());
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        transportSource.setGain((float)slider->getValue());
}


