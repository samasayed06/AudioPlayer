#include <JuceHeader.h>
#include "PlayerGUI.h"
#include "PlayerAudio.h"


// Our application class
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Simple Audio Player"; }
    const juce::String getApplicationVersion() override { return "1.0"; }

    void initialise(const juce::String&) override
    {
        // Create and show the main window
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr; // Clean up
    }

private:
    // The main window of the app
    class MainWindow : public juce::DocumentWindow,
        public juce::AudioAppComponent

    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::lightgrey,
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            audio = std::make_unique<PlayerAudio>();
            gui = std::make_unique<PlayerGUI>(*audio);
            setContentOwned(gui.get(), false);

            setAudioChannels(0, 2);

            
            transportSource = audio->getTransportSource();
            audioSourcePlayer.setSource(transportSource);
            deviceManager.addAudioCallback(&audioSourcePlayer);


            juce::DocumentWindow::centreWithSize(400, 200);

            juce::DocumentWindow::setVisible(true);

           
        }

        ~MainWindow() override
        {
            juce::AudioTransportSource* transportSource = nullptr;
            deviceManager.removeAudioCallback(&audioSourcePlayer);
            shutdownAudio();
        }

        

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
        {
            if (transportSource)
                transportSource->prepareToPlay(samplesPerBlockExpected, sampleRate);

            DBG("Audio device sample rate = " << sampleRate);
        }


        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
        {
            if (transportSource)
                transportSource->getNextAudioBlock(bufferToFill);
        }

        void releaseResources() override
        {
            if (transportSource)
                transportSource->releaseResources();
        }

    private:
        std::unique_ptr<PlayerAudio> audio;
        std::unique_ptr<PlayerGUI> gui;
        juce::AudioTransportSource* transportSource = nullptr;
        juce::AudioSourcePlayer audioSourcePlayer;
    };

    std::unique_ptr<MainWindow> mainWindow;
    

};

// This macro starts the app
START_JUCE_APPLICATION(SimpleAudioPlayer)
