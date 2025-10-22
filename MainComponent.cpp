
/*#include "MainComponent.h"

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
}*/


/*#include "MainComponent.h"

MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    // Add buttons (Load, Restart, Play, Pause, Start, End, Mute)
    for (auto* btn : { &loadButton, &restartButton, &playButton, &pauseButton, &startButton, &endButton, &muteButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider setup
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(previousGain);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    setSize(600, 250);
    setAudioChannels(0, 2); // no inputs, stereo output
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    // ensure transport gain follows slider initial value (unless muted)
    if (!isMuted)
        transportSource.setGain((float)volumeSlider.getValue());
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
    endButton.setBounds(x, y, w, h); x += 80;
    muteButton.setBounds(x, y, w, h);

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
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
                        // Disconnect previous source
                        transportSource.stop();
                        transportSource.setSource(nullptr);
                        readerSource.reset();

                        // Create and attach new source
                        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

                        // apply current gain state
                        if (isMuted)
                            transportSource.setGain(0.0f);
                        else
                            transportSource.setGain((float)volumeSlider.getValue());
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
        // Toggle mute / unmute
        if (!isMuted)
        {
            // going to muted state
            previousGain = (float)volumeSlider.getValue();   // save current slider value
            transportSource.setGain(0.0f);
            volumeSlider.setValue(0.0, juce::dontSendNotification); // visually show 0
            muteButton.setButtonText("Unmute");
            isMuted = true;
        }
        else
        {
            // unmute: restore previousGain
            isMuted = false;
            volumeSlider.setValue(previousGain, juce::dontSendNotification);
            transportSource.setGain(previousGain);
            muteButton.setButtonText("Mute");
        }
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        float v = (float)slider->getValue();

        if (isMuted)
        {
            // if muted and user moves slider, update previousGain so unmute restores to this
            previousGain = v;
            // keep transport gain at 0 while muted
            transportSource.setGain(0.0f);
        }
        else
        {
            transportSource.setGain(v);
        }
    }
}*/


#include "MainComponent.h"

MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    // Add buttons (Load, Restart, Play, Pause, Start, End, Mute)
    for (auto* btn : { &loadButton, &restartButton, &playButton, &pauseButton, &startButton, &endButton, &muteButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider setup
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    setSize(700, 250);
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
    endButton.setBounds(x, y, w, h); x += 80;
    muteButton.setBounds(x, y, w, h); // زرار الميوت

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

   
    if (button == &muteButton)
    {
        if (!isMuted)
        {
            previousVolume = (float)volumeSlider.getValue();
            transportSource.setGain(0.0f);
            isMuted = true;
            muteButton.setButtonText("Unmute");
        }
        else
        {
            transportSource.setGain(previousVolume);
            isMuted = false;
            muteButton.setButtonText("Mute");
        }
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        if (!isMuted)
            transportSource.setGain((float)slider->getValue());
    }
}
