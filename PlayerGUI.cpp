/*
  ==============================================================================

    PlayerGUI.cpp
    Created: 21 Oct 2025 4:33:31pm
    Author:  USER

  ==============================================================================
*/
#include "PlayerGUI.h"
#include "PlayerAudio.h"

// small helper
static inline bool inRange(int i, int n) noexcept { return i >= 0 && i < n; }

PlayerGUI::PlayerGUI()
{
    auto addB = [this](juce::Button& b)
        {
            addAndMakeVisible(b);
            b.addListener(this);

            b.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFD66A88));  
            b.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFFB04866)); 
            b.setColour(juce::TextButton::textColourOffId, juce::Colours::darkgrey);     
            b.setColour(juce::TextButton::textColourOnId, juce::Colours::darkgrey);

        };


    addB(loadButton); addB(playlistButton); addB(playButton); addB(pauseButton);
    addB(restartButton); addB(startButton); addB(endButton); addB(muteButton);
    addB(loopButton); addB(setAButton); addB(setBButton); addB(abLoopButton); addB(jumpBackButton);
    addB(jumpForwardButton);


    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(lastVolume);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    positionSlider.setRange(0.0, 1.0, 0.0001);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    addAndMakeVisible(currentTime); addAndMakeVisible(totalTime);
    addAndMakeVisible(infoLabel);
    addAndMakeVisible(aLabel); addAndMakeVisible(bLabel);

    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);

    setSize(920, 240);
    startTimerHz(20);
    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);
   


}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
}

void PlayerGUI::setAudioBackend(PlayerAudio* backend)
{
    audio = backend;
    if (audio) audio->setGain(lastVolume);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::grey);

    // Draw waveform
    if (!waveformPath.isEmpty())
    {
        juce::Rectangle<int> area(12, 120 + waveformYOffset, getWidth() - 24, 60); // waveform area moved
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f));

    }

    if (audio)
    {
        double len = audio->getLength();
        double pos = audio->getPosition();
        if (len > 0.0)
        {
            int waveformTop = 120;      // same top as waveform
            int waveformHeight = 60;    // same height as waveform
            float x = (float)(pos / len * (getWidth() - 24)); // same width as waveform
            g.setColour(juce::Colours::red);

            float yOffset = 30;        // move playhead up by 10 pixels

            g.drawLine(x + 12, waveformTop + yOffset, x + 12, waveformTop + waveformHeight + yOffset, 2.0f);
        }
    }


}

void PlayerGUI::resized()
{
    int x = 12, y = 8, w = 100, h = 30, s = 8;
    loadButton.setBounds(x, y, w, h);
    playlistButton.setBounds(x + (w + s), y, w + 30, h);
    playButton.setBounds(x + 2 * (w + s) + 30, y, w, h);
    pauseButton.setBounds(x + 3 * (w + s) + 30, y, w, h);
    restartButton.setBounds(x + 4 * (w + s) + 30, y, w, h);

    volumeSlider.setBounds(x, y + h + 10, 360, 22);
    speedSlider.setBounds(x + 480, y + h + 10, 260, h);
    positionSlider.setBounds(x + 70, y + 2 * (h + 10), 520, 18);
    int waveformY = y + 2 * (h + 10) + 20;
    // waveform is painted manually starting at waveformY

    muteButton.setBounds(x + 560, y + 3 * (h + 10), 90, h);

    positionSlider.setBounds(x + 70, y + 2 * (h + 10), 520, 18);
    currentTime.setBounds(x, y + 2 * (h + 10), 60, 18);
    totalTime.setBounds(x + 600, y + 2 * (h + 10), 60, 18);

    startButton.setBounds(x, y + 3 * (h + 10), w, h);
    endButton.setBounds(x + (w + s), y + 3 * (h + 10), w, h);
    loopButton.setBounds(x + 2 * (w + s), y + 3 * (h + 10), w + 20, h);

    setAButton.setBounds(x + 3 * (w + s) + 20, y + 3 * (h + 10), w, h);
    setBButton.setBounds(x + 4 * (w + s) + 20, y + 3 * (h + 10), w, h);
    abLoopButton.setBounds(x+570, y , w , h);

    playlistBox.setBounds(x, y + 5 * (h + 10), getWidth() - 24, 120);
    infoLabel.setBounds(12, getHeight() - 24, getWidth() - 24, 18);
    aLabel.setBounds(getWidth() - 220, y + 3 * (h + 10), 100, 18);
    bLabel.setBounds(getWidth() - 110, y + 3 * (h + 10), 100, 18);
    int y2 = y + 4 * (h + 10); // just below your main buttons
    jumpBackButton.setBounds(x + 680, y, w, h);
    jumpForwardButton.setBounds(x + 790, y, w, h);

}

// Button handler
void PlayerGUI::buttonClicked(juce::Button* b)
{
    if (!audio && b != &loadButton && b != &playlistButton) return;

    if (b == &loadButton)
    {
        fileChooser.reset(new juce::FileChooser("Select audio file...", juce::File(), "*.mp3;*.wav;*.aiff"));
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto f = fc.getResult();
                if (f.existsAsFile())
                {
                    if (audio->loadFile(f))
                    {
                        infoLabel.setText(audio->getDisplayInfo(), juce::dontSendNotification);

                        // --- Step 2: Generate waveform for display ---
                        juce::AudioFormatReader* reader = audio->getAudioFormatReader();
                        if (reader)
                        {
                            // Resize buffer and read samples
                            waveformBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
                            reader->read(&waveformBuffer, 0, (int)reader->lengthInSamples, 0, true, true);

                            // Build waveform path
                            waveformPath.clear();
                            int w = getWidth() - 24;      // width to fit GUI
                            int h = 60;                    // height of waveform display
                            waveformPath.startNewSubPath(0, h / 2);

                            int step = waveformBuffer.getNumSamples() / w;
                            if (step < 1) step = 1;

                            for (int i = 0; i < waveformBuffer.getNumSamples(); i += step)
                            {
                                float sample = waveformBuffer.getSample(0, i); // channel 0
                                float y = h / 2 - sample * h / 2 + waveformYOffset;

                                waveformPath.lineTo((float)(i / (float)waveformBuffer.getNumSamples() * w), y);
                            }
                        }

                        audio->play();
                    }
                }
            });
    }
    else if (b == &playlistButton)
    {
        fileChooser.reset(new juce::FileChooser("Select multiple audio files...", juce::File(), "*.mp3;*.wav;*.aiff"));
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode |
            juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                playlistFiles.clear();
                for (auto& f : results) playlistFiles.add(f);
                playlistBox.updateContent();
            });
    }
    else if (b == &playButton)
    {
        if (onPlayPressed) onPlayPressed(); // notify mix mode controller (if present)
        if (audio) audio->play();
    }
    else if (b == &jumpBackButton)
    {
        if (audio)
        {
            double newPos = juce::jmax(0.0, audio->getPosition() - 10.0);
            audio->setPosition(newPos);
            if (onSetPositionRequest) onSetPositionRequest(newPos); // for mix mode
        }
    }
    else if (b == &jumpForwardButton)
    {
        if (audio)
        {
            double len = audio->getLength();
            double newPos = juce::jmin(len, audio->getPosition() + 10.0);
            audio->setPosition(newPos);
            if (onSetPositionRequest) onSetPositionRequest(newPos); // for mix mode
        }
    }

    else if (b == &pauseButton)
    {
        if (onPausePressed) onPausePressed();
        if (audio) audio->pause();
    }
    else if (b == &restartButton)
    {
        if (onRestartPressed) onRestartPressed();
        if (audio) { audio->stop(); audio->play(); }
    }
    else if (b == &startButton)
    {
        if (onStartPressed) onStartPressed();
        if (audio) audio->setPosition(0.0);
    }
    else if (b == &endButton)
    {
        if (onEndPressed) onEndPressed();
        if (audio) { double len = audio->getLength(); if (len > 0.0) audio->setPosition(len - 0.01); }
    }
    else if (b == &muteButton)
    {
        if (onMuteToggled) onMuteToggled();
        if (audio) { audio->toggleMute(); muteButton.setButtonText(audio->isMuted() ? "Unmute" : "Mute"); }
    }
    else if (b == &loopButton)
    {
        if (onLoopToggled) onLoopToggled();
        if (audio) { bool v = !audio->isLoopingEnabled(); audio->setLooping(v); loopButton.setButtonText(v ? "Loop ON" : "Loop OFF"); }
    }
    else if (b == &setAButton)
    {
        double p = audio ? audio->getPosition() : 0.0;
        if (audio) audio->setMarkerA(p);
        aLabel.setText("A: " + juce::String(p, 2), juce::dontSendNotification);
    }
    else if (b == &setBButton)
    {
        double p = audio ? audio->getPosition() : 0.0;
        if (audio) audio->setMarkerB(p);
        bLabel.setText("B: " + juce::String(p, 2), juce::dontSendNotification);
    }
    else if (b == &abLoopButton)
    {
        if (onABLoopToggled) onABLoopToggled();
        if (audio) { bool v = !audio->isABLoopEnabled(); audio->setABLoopEnabled(v); abLoopButton.setButtonText(v ? "A–B Loop ON" : "A–B Loop OFF"); }
    }
}

// Slider handler
void PlayerGUI::sliderValueChanged(juce::Slider* s)
{
    if (!audio) return;
    if (s == &volumeSlider)
    {
        lastVolume = (float)volumeSlider.getValue();
        if (onVolumeChanged) onVolumeChanged(lastVolume);
        audio->setGain(lastVolume);
    }
    else if (s == &speedSlider)
    {
        double speed = speedSlider.getValue();
        if (onSpeedChanged) onSpeedChanged(speed);
        if (audio) audio->setSpeed(speed);
    }

    else if (s == &positionSlider && draggingPosition)
    {
        double len = audio->getLength();
        if (len > 0.0)
        {
            double pos = positionSlider.getValue() * len;
            // notify mix controller of position request (so it can set both)
            if (onSetPositionRequest) onSetPositionRequest(pos);
            // also set own audio position (original behavior)
            audio->setPosition(pos);
        }
    }
}

void PlayerGUI::sliderDragStarted(juce::Slider* s) { if (s == &positionSlider) draggingPosition = true; }
void PlayerGUI::sliderDragEnded(juce::Slider* s) { if (s == &positionSlider) draggingPosition = false; }

int PlayerGUI::getNumRows() { return playlistFiles.size(); }

void PlayerGUI::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected) g.fillAll(juce::Colours::lightblue);
    g.setColour(juce::Colours::white);
    if (inRange(row, playlistFiles.size()))
        g.drawText(playlistFiles[row].getFileName(), 6, 0, width - 10, height, juce::Justification::centredLeft);
}

void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
    if (!inRange(lastRowSelected, playlistFiles.size())) return;
    auto f = playlistFiles.getReference(lastRowSelected);
    if (audio->loadFile(f))
    {
        infoLabel.setText(audio->getDisplayInfo(), juce::dontSendNotification);
        audio->play();
    }
}

void PlayerGUI::timerCallback()
{
    if (!audio) return;

    double len = audio->getLength();
    double pos = audio->getPosition();

    if (!draggingPosition && len > 0.0)
        positionSlider.setValue(pos / len, juce::dontSendNotification);

    auto fmt = [](double s)
        {
            int m = int(s / 60), sec = int(std::fmod(s, 60.0));
            return juce::String::formatted("%02d:%02d", m, sec);
        };

    currentTime.setText(fmt(pos), juce::dontSendNotification);
    totalTime.setText(len > 0.0 ? fmt(len) : "00:00", juce::dontSendNotification);

    // update info label (metadata or filename)
    infoLabel.setText(audio->getDisplayInfo(), juce::dontSendNotification);
    repaint(); // will redraw waveform with updated playhead

}
