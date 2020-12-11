/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicSynthAudioProcessor::BasicSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
tree (*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    mySynth.clearVoices();
    
    for (int i = 0; i < 5; ++i)
    {
        mySynth.addVoice(new SynthVoice());
    }
    
    mySynth.clearSounds();
    mySynth.addSound(new SynthSound());
}

//converting from real attack value to decimal between 0 and 1
float customConvertTo0To1Func(float rangeStart, float rangeEnd, float valueToRemap)
{
    
//    const midpoint
//
//    auto proportion = (valueToRemap - rangeStart) / (rangeEnd - rangeStart);
//    auto clampedProportion = jlimit (0.0f, 1.0f, proportion);
//
//    if (clampedProportion < .5) {
//        return
//    }
//
//    auto skew = std::log (.5) / std::log ((midpoint - min) / (max - min));
//    return std::pow (clampedProportion, skew);
    return 1;
}

//converting from a decimal between 0 and 1 to a real attack value
float customConvertFrom0To1Func(float rangeStart, float rangeEnd, float valueToRemap)
{
    
    float firstMax = 100.0f;

    if (valueToRemap <= .5) {
        return rangeStart + (firstMax - rangeStart) * valueToRemap;
    } else {
        auto skew = std::log (.5) / std::log ((50.0f - rangeStart) / (rangeEnd - rangeStart));
        auto skewedProportion = std::exp (std::log (valueToRemap) / skew);

        return rangeStart + (rangeEnd - rangeStart) * skewedProportion;
    }
}

AudioProcessorValueTreeState::ParameterLayout BasicSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::Parameter>> layout;
    
    NormalisableRange<float> attackRange(0, 20000.0f, customConvertFrom0To1Func, customConvertTo0To1Func);
    
    NormalisableRange<float> decayRange(0, 60000.0f, .001f);
    decayRange.setSkewForCentre(1000.0);
    
    NormalisableRange<float> releaseRange(0, 60000.0f, .001f);
    releaseRange.setSkewForCentre(1000.0);
      
 
    auto attackParam = std::make_unique<juce::AudioProcessorValueTreeState::Parameter> ("attack", "Attack", "Attack", attackRange, 0, nullptr, nullptr);
    auto decayParam = std::make_unique<juce::AudioProcessorValueTreeState::Parameter> ("decay", "Decay", "Decay", decayRange, 600.0f, nullptr, nullptr);
    auto sustainParam = std::make_unique<juce::AudioProcessorValueTreeState::Parameter> ("sustain", "Sustain", "Sustain", NormalisableRange<float>(0, 1.0f, .01f, 1), 1.0, nullptr, nullptr);
    auto releaseParam = std::make_unique<juce::AudioProcessorValueTreeState::Parameter> ("release", "Release", "Release", releaseRange, 2000.0f, nullptr, nullptr);
    auto wavetypeParam = std::make_unique<juce::AudioProcessorValueTreeState::Parameter> ("wavetype", "Wavetype", "Wavetype", NormalisableRange<float>(1, 4), 1, nullptr, nullptr);
    auto filtertypeParam = std::make_unique<juce::AudioProcessorValueTreeState::Parameter> ("filtertype", "Filtertype", "Filtertype", NormalisableRange<float>(1, 3), 1, nullptr, nullptr);
    auto cutoffParam = std::make_unique<juce::AudioProcessorValueTreeState::Parameter> ("cutoff", "Cutoff", "Cutoff", NormalisableRange<float>(26.0f, 20000.0f), 400.0f, nullptr, nullptr);
    auto resParam = std::make_unique<juce::AudioProcessorValueTreeState::Parameter> ("res", "Res", "Res", NormalisableRange<float>(0.0f, 100.0f), 0.0f, nullptr, nullptr);
    
    layout.push_back(std::move(attackParam));
    layout.push_back(std::move(decayParam));
    layout.push_back(std::move(sustainParam));
    layout.push_back(std::move(releaseParam));
    layout.push_back(std::move(wavetypeParam));
    layout.push_back(std::move(filtertypeParam));
    layout.push_back(std::move(cutoffParam));
    layout.push_back(std::move(resParam));
    
    return { layout.begin(), layout.end() };
}

BasicSynthAudioProcessor::~BasicSynthAudioProcessor()
{
}

//==============================================================================
const String BasicSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicSynthAudioProcessor::setCurrentProgram (int index)
{
}

const String BasicSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicSynthAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BasicSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    ignoreUnused(samplesPerBlock);
    lastSampleRate = sampleRate;
    mySynth.setCurrentPlaybackSampleRate(lastSampleRate);
    
}

void BasicSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicSynthAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    
    for (int i = 0; i < mySynth.getNumVoices(); ++i)
    {
        if ((myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(i))))
        {
            myVoice->getEnvelopeParams(tree.getRawParameterValue("attack"),
                                     tree.getRawParameterValue("decay"),
                                     tree.getRawParameterValue("sustain"),
                                     tree.getRawParameterValue("release"));
            
            myVoice->getWavetypeParam(tree.getRawParameterValue("wavetype"));
            
            myVoice->getFilterParams(tree.getRawParameterValue("filtertype"),
                                     tree.getRawParameterValue("cutoff"),
                                     tree.getRawParameterValue("res"));

        }
    }
    
    buffer.clear();

    // send to synth
    mySynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool BasicSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BasicSynthAudioProcessor::createEditor()
{
    return new BasicSynthAudioProcessorEditor (*this);
}

//==============================================================================
void BasicSynthAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicSynthAudioProcessor();
}