/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DualChannelAudioProcessor::DualChannelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(makeBusesProperties()),
	parameters(*this, nullptr, "PARAMETERS", createParameterLayout()),
	leftInputGainComputer(), rightInuptGainComputer(),
	leftOutputGainComputer(), rightOutputGainComputer(),
	sidechainGainComputer(),
	leftCompressor(), rightCompressor(),
	linkInputGainComputer(), linkOutputGainComputer(),
	linkCompressor(),
	dryWetMixer()
#endif
{
	parameters.addParameterListener("f_inputgain", this);
	parameters.addParameterListener("f_attack", this);
	parameters.addParameterListener("f_release", this);
	parameters.addParameterListener("f_threshold", this);
	parameters.addParameterListener("f_ratio", this);
	parameters.addParameterListener("f_outputgain", this);

	parameters.addParameterListener("s_inputgain", this);
	parameters.addParameterListener("s_attack", this);
	parameters.addParameterListener("s_release", this);
	parameters.addParameterListener("s_threshold", this);
	parameters.addParameterListener("s_ratio", this);
	parameters.addParameterListener("s_outputgain", this);

	parameters.addParameterListener("high_pass", this);
	parameters.addParameterListener("low_pass", this);

	parameters.addParameterListener("sidechain", this);

	parameters.addParameterListener("mix", this);
}

DualChannelAudioProcessor::~DualChannelAudioProcessor()
{
}

AudioProcessor::BusesProperties DualChannelAudioProcessor::makeBusesProperties()
{
	BusesProperties bp;

	bp.addBus(true, "Input", AudioChannelSet::stereo(), true);
	bp.addBus(false, "Output", AudioChannelSet::stereo(), true);

	if (!JUCEApplicationBase::isStandaloneApp())
	{
		bp.addBus(true, "Sidechain", AudioChannelSet::stereo(), true);
	}

	return bp;
}

//==============================================================================
const String DualChannelAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool DualChannelAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool DualChannelAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool DualChannelAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double DualChannelAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int DualChannelAudioProcessor::getNumPrograms()
{
	return 1;
}

int DualChannelAudioProcessor::getCurrentProgram()
{
	return 0;
}

void DualChannelAudioProcessor::setCurrentProgram(int index)
{
}

const String DualChannelAudioProcessor::getProgramName(int index)
{
	return {};
}

void DualChannelAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void DualChannelAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	ProcessSpec spec1{};
	spec1.sampleRate = sampleRate;
	spec1.numChannels = 3;
	spec1.maximumBlockSize = samplesPerBlock;

	leftInputGainComputer.prepare(spec1);
	rightInuptGainComputer.prepare(spec1);
	leftOutputGainComputer.prepare(spec1);
	rightOutputGainComputer.prepare(spec1);

	leftCompressor.prepare(spec1);
	rightCompressor.prepare(spec1);

	ProcessSpec spec2{};
	spec2.sampleRate = sampleRate;
	spec2.numChannels = 4;
	spec2.maximumBlockSize = samplesPerBlock;

	linkInputGainComputer.prepare(spec2);
	linkOutputGainComputer.prepare(spec2);

	linkCompressor.prepare(spec2);

	dryWetMixer.prepare(spec2);

	leftInputGainComputer.setGainDecibels(0);
	rightInuptGainComputer.setGainDecibels(0);
	leftOutputGainComputer.setGainDecibels(0);
	rightOutputGainComputer.setGainDecibels(0);

	linkInputGainComputer.setGainDecibels(0);
	linkOutputGainComputer.setGainDecibels(0);

	dryWetMixer.setWetMixProportion(1);
}

void DualChannelAudioProcessor::releaseResources()
{
   //release res
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DualChannelAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
	const auto mono = AudioChannelSet::mono();
	const auto stereo = AudioChannelSet::stereo();

	const auto mainIn = layouts.getMainInputChannelSet();
	const auto mainOut = layouts.getMainOutputChannelSet();

	if (mainIn != mainOut)
		return false;

	if (mainOut != stereo && mainOut != mono)
		return false;

	if (wrapperType != wrapperType_Standalone)
	{
		const auto sidechainIn = layouts.getChannelSet(true, 1);
		if (!sidechainIn.isDisabled())
			if (sidechainIn != stereo && sidechainIn != mono)
				return false;
	}

	return true;
}
#endif

bool DualChannelAudioProcessor::canAddBus(bool isInput) const
{
	if (wrapperType == wrapperType_Standalone)
		return false;

	return isInput;
}

float DualChannelAudioProcessor::getLeftGainReduction()
{
	return leftGainReduction.get();
}

float DualChannelAudioProcessor::getRightGainReduction()
{
	return rightGainReduction.get();
}

void DualChannelAudioProcessor::setHighPass(float value)
{
	hightPassValue = value;
}

void DualChannelAudioProcessor::setLowPass(float value)
{
	lowPassValue = value;
}

void DualChannelAudioProcessor::toggleLR()
{
	applyLRMode = !applyLRMode;
}

void DualChannelAudioProcessor::toggleLinking()
{
	applyLinking = !applyLinking;
}

void DualChannelAudioProcessor::toggleHighPass()
{
	applyHighPass = !applyHighPass;
}

void DualChannelAudioProcessor::toggleLowPass()
{
	applyLowPass = !applyLowPass;
}

void DualChannelAudioProcessor::toggleSidechain()
{
	applySidechain = !applySidechain;
}

void encodeMS(float* const* samples, int numSamples, int ch0) noexcept
{
	const auto ch1 = ch0 + 1;

	for (auto s = 0; s < numSamples; ++s)
	{
		const auto mid = samples[ch0][s] + samples[ch1][s];
		const auto side = samples[ch0][s] - samples[ch1][s];

		samples[ch0][s] = mid;
		samples[ch1][s] = side;
	}

	for (auto ch = ch0; ch < ch0 + 2; ++ch)
		FloatVectorOperations::multiply(samples[ch], .5f, numSamples);
}

void decodeMS(float* const* samples, int numSamples, int ch0) noexcept
{
	const auto ch1 = ch0 + 1;

	for (auto s = 0; s < numSamples; ++s)
	{
		const auto left = samples[ch0][s] + samples[ch1][s];
		const auto right = samples[ch0][s] - samples[ch1][s];

		samples[ch0][s] = left;
		samples[ch1][s] = right;
	}
}

void DualChannelAudioProcessor::process(AudioBuffer<float> buffer)
{
	const int numSamples = buffer.getNumSamples();

	if (applyLinking)
	{
		auto leftReadData = buffer.getReadPointer(0);
		auto rightReadData = buffer.getReadPointer(1);
		auto leftWriteData = buffer.getWritePointer(0);
		auto rightWriteData = buffer.getWritePointer(1);

		for (int i = 0; i < numSamples; i++)
		{
			float leftSample = leftReadData[i];
			float rightSample = rightReadData[i];

			float processedLeftSample = linkInputGainComputer.processSample(leftSample);
			processedLeftSample = linkCompressor.processSample(0, processedLeftSample);
			processedLeftSample = linkOutputGainComputer.processSample(processedLeftSample);

			float processedRightSample = linkInputGainComputer.processSample(rightSample);
			processedRightSample = linkCompressor.processSample(1, processedRightSample);
			processedRightSample = linkOutputGainComputer.processSample(processedRightSample);

			leftWriteData[i] = processedLeftSample;
			rightWriteData[i] = processedRightSample;

			leftGainReduction.set(Decibels::gainToDecibels(processedLeftSample / leftSample));
			rightGainReduction.set(Decibels::gainToDecibels(processedRightSample / rightSample));
		}
	}
	else
	{		
		auto leftReadData = buffer.getReadPointer(0);
		auto rightReadData = buffer.getReadPointer(1);
		auto leftWriteData = buffer.getWritePointer(0);
		auto rightWriteData = buffer.getWritePointer(1);

		for (int i = 0; i < numSamples; i++)
		{
			float leftSample = leftReadData[i];
			float rightSample = rightReadData[i];

			float processedLeftSample = leftInputGainComputer.processSample(leftSample);
			processedLeftSample = leftCompressor.processSample(0, processedLeftSample);
			processedLeftSample = leftOutputGainComputer.processSample(processedLeftSample);

			float processedRightSample = rightInuptGainComputer.processSample(rightSample);
			processedRightSample = rightCompressor.processSample(0, processedRightSample);
			processedRightSample = rightOutputGainComputer.processSample(processedRightSample);

			leftWriteData[i] = processedLeftSample;
			rightWriteData[i] = processedRightSample;

			leftGainReduction.set(Decibels::gainToDecibels(processedLeftSample / leftSample));
			rightGainReduction.set(Decibels::gainToDecibels(processedRightSample / rightSample));
		}
	}
}

void DualChannelAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	const int numSamples = buffer.getNumSamples();
	const double sampleRate = getSampleRate();

	dsp::ProcessorChain<dsp::IIR::Filter<float>, dsp::IIR::Filter<float>> filter;

	if (applyHighPass)
	{
		filter.get<0>().coefficients = dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 5000.0f, hightPassValue); // 5 kHz
	}

	if (applyLowPass)
	{
		filter.get<1>().coefficients = dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 200.0f, lowPassValue); // 200 Hz
	}

	AudioBlock<float> audioBlock(buffer);

	dryWetMixer.pushDrySamples(audioBlock);

	if (applyLRMode)
	{
		process(buffer);
	}
	else
	{
		encodeMS(buffer.getArrayOfWritePointers(), numSamples, 0);
		process(buffer);
		decodeMS(buffer.getArrayOfWritePointers(), numSamples, 0);
	}

	if (applyHighPass || applyLowPass)
	{
		filter.process(ProcessContextReplacing<float>(audioBlock));
	}

	dryWetMixer.mixWetSamples(audioBlock);

	if (applySidechain && !JUCEApplicationBase::isStandaloneApp())
	{
		AudioBuffer<float> sidechainBuffer(2, numSamples);
		sidechainBuffer.copyFrom(0, 0, buffer.getReadPointer(2), numSamples);
		sidechainBuffer.copyFrom(1, 0, buffer.getReadPointer(3), numSamples);
		AudioBlock<float> sidechainBlock(sidechainBuffer);
		ProcessContextReplacing<float> sidechainContextReplacing(sidechainBlock);

		sidechainGainComputer.process(sidechainContextReplacing);

		auto leftInputChannelData = buffer.getReadPointer(0);
		auto rightInputChannelData = buffer.getReadPointer(1);

		auto leftOutputChannelData = buffer.getWritePointer(0);
		auto rightOutputChannelData = buffer.getWritePointer(1);

		auto leftSidechainChannelData = sidechainBuffer.getReadPointer(0);
		auto rightSidechainChannelData = sidechainBuffer.getReadPointer(1);

		for (int i = 0; i < numSamples; i++)
		{
			float leftSample = leftInputChannelData[i];
			float rightSample = rightInputChannelData[i];

			float leftSidechainSample = leftSidechainChannelData[i];
			float rightSidechainSample = rightSidechainChannelData[i];

			float resultLeftSample = leftSample + leftSidechainSample;
			float resultRightSample = rightSample + rightSidechainSample;

			leftOutputChannelData[i] = resultLeftSample;
			rightOutputChannelData[i] = resultRightSample;
		}
	}
}

//==============================================================================
bool DualChannelAudioProcessor::hasEditor() const
{
	return true; // change this to false if you want to not supply an editor
}

AudioProcessorEditor* DualChannelAudioProcessor::createEditor()
{
	return new DualChannelAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void DualChannelAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void DualChannelAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new DualChannelAudioProcessor();
}

void DualChannelAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
	if (parameterID == "f_inputgain") { leftInputGainComputer.setGainDecibels(newValue); linkInputGainComputer.setGainDecibels(newValue); }
	else if (parameterID == "f_attack") { leftCompressor.setAttack(newValue); linkCompressor.setAttack(newValue); }
	else if (parameterID == "f_release") { leftCompressor.setRelease(newValue); linkCompressor.setRelease(newValue); }
	else if (parameterID == "f_threshold") { leftCompressor.setThreshold(newValue); linkCompressor.setThreshold(newValue); }
	else if (parameterID == "f_ratio") { leftCompressor.setRatio(newValue); linkCompressor.setRatio(newValue); }
	else if (parameterID == "f_outputgain") { leftOutputGainComputer.setGainDecibels(newValue); linkOutputGainComputer.setGainDecibels(newValue); }

	else if (parameterID == "s_inputgain") { rightInuptGainComputer.setGainDecibels(newValue); linkInputGainComputer.setGainDecibels(newValue); }
	else if (parameterID == "s_attack") { rightCompressor.setAttack(newValue); linkCompressor.setAttack(newValue); }
	else if (parameterID == "s_release") { rightCompressor.setRelease(newValue); linkCompressor.setRelease(newValue); }
	else if (parameterID == "s_threshold") { rightCompressor.setThreshold(newValue); linkCompressor.setThreshold(newValue); }
	else if (parameterID == "s_ratio") { rightCompressor.setRatio(newValue); linkCompressor.setRatio(newValue); }
	else if (parameterID == "s_outputgain") { rightOutputGainComputer.setGainDecibels(newValue); linkOutputGainComputer.setGainDecibels(newValue); }

	else if (parameterID == "low_pass") lowPassValue = newValue;
	else if (parameterID == "high_pass") hightPassValue = newValue;

	else if (parameterID == "sidechain") sidechainGainComputer.setGainDecibels(newValue);

	else if (parameterID == "mix") dryWetMixer.setWetMixProportion(newValue);
}

AudioProcessorValueTreeState::ParameterLayout DualChannelAudioProcessor::createParameterLayout()
{
	std::vector<std::unique_ptr<RangedAudioParameter>> params;

	params.push_back(std::make_unique<AudioParameterBool>("power", "Power", true));

	params.push_back(std::make_unique<AudioParameterFloat>("f_inputgain", "Input",
		NormalisableRange<float>(
			Constants::Parameter::inputStart,
			Constants::Parameter::inputEnd,
			Constants::Parameter::inputInterval), 0.0f,
		String(),
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			return String(value, 1) + " dB";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("f_attack", "Attack",
		NormalisableRange<float>(
			Constants::Parameter::attackStart,
			Constants::Parameter::attackEnd,
			Constants::Parameter::attackInterval, 0.5f), 1.0f,
		"ms",
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			if (value == 100.0f) return String(value, 0) + " ms";
			return String(value, 2) + " ms";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("f_release", "Release",
		NormalisableRange<float>(
			Constants::Parameter::releaseStart,
			Constants::Parameter::releaseEnd,
			Constants::Parameter::releaseInterval, 0.35f),
		5.0f,
		String(),
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			if (value <= 100) return String(value, 2) + " ms";
			if (value >= 1000)
				return String(value * 0.001f, 2) + " s";
			return String(value, 1) + " ms";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("f_threshold", "Tresh",
		NormalisableRange<float>(
			Constants::Parameter::thresholdStart,
			Constants::Parameter::thresholdEnd,
			Constants::Parameter::thresholdInterval), 0.0f,
		String(), AudioProcessorParameter::genericParameter,
		[](float value, float maxStrLen)
		{
			return String(value, 1) + " dB";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("f_ratio", "Ratio",
		NormalisableRange<float>(
			Constants::Parameter::ratioStart,
			Constants::Parameter::ratioEnd,
			Constants::Parameter::ratioInterval, 0.5f), 1.0f,
		String(), AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			if (value > 23.9f)return String("Infinity") + ":1";
			return String(value, 1) + ":1";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("f_outputgain", "Output",
		NormalisableRange<float>(
			Constants::Parameter::inputStart,
			Constants::Parameter::inputEnd,
			Constants::Parameter::inputInterval), 0.0f,
		String(),
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			return String(value, 1) + " dB";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("s_inputgain", "Input",
		NormalisableRange<float>(
			Constants::Parameter::inputStart,
			Constants::Parameter::inputEnd,
			Constants::Parameter::inputInterval), 0.0f,
		String(),
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			return String(value, 1) + " dB";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("s_attack", "Attack",
		NormalisableRange<float>(
			Constants::Parameter::attackStart,
			Constants::Parameter::attackEnd,
			Constants::Parameter::attackInterval, 0.5f), 1.0f,
		"ms",
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			if (value == 100.0f) return String(value, 0) + " ms";
			return String(value, 2) + " ms";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("s_release", "Release",
		NormalisableRange<float>(
			Constants::Parameter::releaseStart,
			Constants::Parameter::releaseEnd,
			Constants::Parameter::releaseInterval, 0.35f),
		5.0f,
		String(),
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			if (value <= 100) return String(value, 2) + " ms";
			if (value >= 1000)
				return String(value * 0.001f, 2) + " s";
			return String(value, 1) + " ms";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("s_threshold", "Tresh",
		NormalisableRange<float>(
			Constants::Parameter::thresholdStart,
			Constants::Parameter::thresholdEnd,
			Constants::Parameter::thresholdInterval), 0.0f,
		String(), AudioProcessorParameter::genericParameter,
		[](float value, float maxStrLen)
		{
			return String(value, 1) + " dB";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("s_ratio", "Ratio",
		NormalisableRange<float>(
			Constants::Parameter::ratioStart,
			Constants::Parameter::ratioEnd,
			Constants::Parameter::ratioInterval, 0.5f), 1.0f,
		String(), AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			if (value > 23.9f)return String("Infinity") + ":1";
			return String(value, 1) + ":1";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("s_outputgain", "Output",
		NormalisableRange<float>(
			Constants::Parameter::inputStart,
			Constants::Parameter::inputEnd,
			Constants::Parameter::inputInterval), 0.0f,
		String(),
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			return String(value, 1) + " dB";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("high_pass", "HP",
		NormalisableRange<float>(
			Constants::Parameter::highPassStart,
			Constants::Parameter::highPassEnd,
			Constants::Parameter::highPassInterval, 0.5f), 6.0f,
		String(), AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			return String(value, 1) + " db/Oct";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("low_pass", "LP",
		NormalisableRange<float>(
			Constants::Parameter::lowPassStart,
			Constants::Parameter::lowPassEnd,
			Constants::Parameter::lowPassInterval, 0.5f), 6.0f,
		String(), AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			return String(value, 1) + " db/Oct";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("sidechain", "Sidechain",
		NormalisableRange<float>(
			Constants::Parameter::sidechainStart,
			Constants::Parameter::sidechainEnd,
			Constants::Parameter::sidechainInterval), 0.0f,
		String(),
		AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			return String(value, 1) + " dB";
		}));

	params.push_back(std::make_unique<AudioParameterFloat>("mix", "Mix",
		NormalisableRange<float>(
			Constants::Parameter::mixStart,
			Constants::Parameter::mixEnd,
			Constants::Parameter::mixInterval),
		1.0f, "%", AudioProcessorParameter::genericParameter,
		[](float value, float)
		{
			return String(value * 100.0f, 1) + " %";
		}));

	lrmsModeParam = std::make_unique<AudioParameterChoice>("LRMS", "LRMS", StringArray("LR", "MS"), 0);
	params.push_back(std::make_unique<AudioParameterChoice>("LRMS", "LRMS", StringArray("LR", "MS"), 0));

	return { params.begin(), params.end() };
}