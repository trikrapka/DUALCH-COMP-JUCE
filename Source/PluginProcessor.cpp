#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/include/Compressor.h"

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
	dryWetMixer(),
	leftGainReduction(0.0f), rightGainReduction(0.0f)
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

	parameters.addParameterListener("high_pass_freq_slider", this);
	parameters.addParameterListener("low_pass_freq_slider",this);

	parameters.addParameterListener("sidechain", this);

	parameters.addParameterListener("mix", this);
	//AlertWindow alertWindow("Sidechain error", "No sidechain input found.", MessageBoxIconType::WarningIcon, nullptr);
}

DualChannelAudioProcessor::~DualChannelAudioProcessor()
{
}

AudioProcessor::BusesProperties DualChannelAudioProcessor::makeBusesProperties()
{
	BusesProperties bp;

	bp.addBus(true, "Input", AudioChannelSet::stereo(), true);
	bp.addBus(false, "Output", AudioChannelSet::stereo(), true);
	bp.addBus(true, "Sidechain", AudioChannelSet::stereo(), true);

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

	leftCompressor.setRelease(5);
	rightCompressor.setRelease(5);
	
    leftCompressor.setThreshold(-32);
	rightCompressor.setThreshold(-32);
	
	leftCompressor.setRatio(2);
	rightCompressor.setRatio(2);
	
	linkCompressor.setRelease(5);	

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
			{
				return false;
			}
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

void DualChannelAudioProcessor::setOversamplingFactor(int value)
{
	oversamplingFactor = value;
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

void DualChannelAudioProcessor::toggleOversampling()
{
	applyOversampling = !applyOversampling;
}

void encodeLRToMS(AudioBlock<float> block)
{
	const int numSamples = block.getNumSamples();

	for (int i = 0; i < numSamples; i++)
	{
		float leftSample = block.getSample(0, i);
		float rightSample = block.getSample(1, i);

		float midSample = (leftSample + rightSample) * 0.5f;
		float sideSample = (leftSample - rightSample) * 0.5f;

		block.setSample(0, i, midSample);
		block.setSample(1, i, sideSample);
	}
}

void decodeMSToLR(AudioBlock<float> block)
{
	const int numSamples = block.getNumSamples();

	for (int i = 0; i < numSamples; i++)
	{
		float midSample = block.getSample(0, i);
		float sideSample = block.getSample(1, i);

		float leftSample = midSample + sideSample;
		float rightSample = midSample - sideSample;

		block.setSample(0, i, leftSample);
		block.setSample(1, i, rightSample);
	}
}

float calculateQFactor(float desiredDbOctave)
{
	if (desiredDbOctave == 6)
		return 0.707f;
	else if (desiredDbOctave == 12)
		return 2.0f;
	else if (desiredDbOctave == 18)
		return 2.414f;
	else
		return 4.236f;
}

void DualChannelAudioProcessor::process(AudioBlock<float> block)
{
	const int numSamples = block.getNumSamples();

	if (applyLinking && !JUCEApplicationBase::isStandaloneApp())
	{
		for (int i = 0; i < numSamples; i++)
		{
			float leftSample = block.getSample(0, i);
			float rightSample = block.getSample(1, i);

			float processedLeftSample = linkInputGainComputer.processSample(leftSample);
			processedLeftSample = linkCompressor.processSample(0, processedLeftSample);
			processedLeftSample = linkOutputGainComputer.processSample(processedLeftSample);

			float processedRightSample = linkInputGainComputer.processSample(rightSample);
			processedRightSample = linkCompressor.processSample(1, processedRightSample);
			processedRightSample = linkOutputGainComputer.processSample(processedRightSample);

			block.setSample(0, i, processedLeftSample);
			block.setSample(1, i, processedRightSample);

			if (leftSample == 0) leftGainReduction.set(0);
			else leftGainReduction.set(Decibels::gainToDecibels(processedLeftSample / leftSample));

			if (rightSample == 0) rightGainReduction.set(0);
			else rightGainReduction.set(Decibels::gainToDecibels(processedRightSample / rightSample));
		}
	}
	else
	{
		for (int i = 0; i < numSamples; i++)
		{
			float leftSample = block.getSample(0, i);
			float rightSample = block.getSample(1, i);

			float processedLeftSample = leftInputGainComputer.processSample(leftSample);
			processedLeftSample = leftCompressor.processSample(0, processedLeftSample);
			processedLeftSample = leftOutputGainComputer.processSample(processedLeftSample);

			float processedRightSample = rightInuptGainComputer.processSample(rightSample);
			processedRightSample = rightCompressor.processSample(0, processedRightSample);
			processedRightSample = rightOutputGainComputer.processSample(processedRightSample);

			block.setSample(0, i, processedLeftSample);
			block.setSample(1, i, processedRightSample);

			if (leftSample == 0) leftGainReduction.set(0);
			else leftGainReduction.set(Decibels::gainToDecibels(processedLeftSample / leftSample));

			if (rightSample == 0) rightGainReduction.set(0);
			else rightGainReduction.set(Decibels::gainToDecibels(processedRightSample / rightSample));
		}
	}
}

void DualChannelAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
	const int numSamples = buffer.getNumSamples();
	const double sampleRate = getSampleRate();

	AudioBuffer<float> mainBuffer = getBusBuffer(buffer, true, 0);
	AudioBlock<float> audioBlock(mainBuffer);

	IIR::Filter<float> highPassFilter;
	IIR::Filter<float> lowPassFilter;

	Oversampling<float> oversampling(2, oversamplingFactor, Oversampling<float>::FilterType::filterHalfBandFIREquiripple);

	if (applyHighPass)
	{
		float qFactor = calculateQFactor(hightPassValue);
		highPassFilter.coefficients = IIR::Coefficients<float>::makeHighPass(sampleRate, cutoffFrquencyHP, qFactor);
	}

	if (applyLowPass)
	{
		float qFactor = calculateQFactor(lowPassValue);
		lowPassFilter.coefficients = IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFrquencyLP, qFactor); 
	}

	dryWetMixer.pushDrySamples(audioBlock);

	if (applyOversampling)
	{
		oversampling.initProcessing(numSamples);
		AudioBlock<float> upsampledBlock = oversampling.processSamplesUp(mainBuffer);
		audioBlock.swap(upsampledBlock);
	}

	if (applyLRMode)
	{
		process(audioBlock);
	}
	else
	{
		encodeLRToMS(audioBlock);
		process(audioBlock);
		decodeMSToLR(audioBlock);
	}

	if (applyHighPass)
	{
		highPassFilter.process(ProcessContextReplacing<float>(audioBlock));
	}

	if (applyLowPass)
	{
		lowPassFilter.process(ProcessContextReplacing<float>(audioBlock));
	}

	if (applyOversampling)
	{
		AudioBlock<float> downsampledBlock(mainBuffer);
		oversampling.processSamplesDown(downsampledBlock);
		audioBlock.swap(downsampledBlock);
	}

	dryWetMixer.mixWetSamples(audioBlock);

	if (applySidechain)
	{
		AudioBuffer<float> sidechainBuffer = getBusBuffer(buffer, true, 1);
		AudioBlock<float> sidechainBlock(sidechainBuffer);
		ProcessContextReplacing<float> sidechainContextReplacing(sidechainBlock);
		
		auto sidechainInputs = sidechainBuffer.getNumChannels();
		
		if (sidechainInputs > 0)
		{
			sidechainGainComputer.process(sidechainContextReplacing);

			AudioBlock<float> mainBlock(mainBuffer);

			for (int i = 0; i < numSamples; i++)
			{
				float leftSample = mainBlock.getSample(0, i);
				float rightSample = mainBlock.getSample(1, i);

				float leftSidechainSample = sidechainBlock.getSample(0, i);
				float rightSidechainSample = sidechainBlock.getSample(1, i);

				float resultLeftSample = leftSample + leftSidechainSample;
				float resultRightSample = rightSample + rightSidechainSample;

				mainBlock.setSample(0, i, resultLeftSample);
				mainBlock.setSample(1, i, resultRightSample);
			}
		}
		else
		{
			alertWindow = std::make_unique<juce::AlertWindow>("Sidechain Error", "Sidechain input not found", MessageBoxIconType::WarningIcon, nullptr);
			alertWindow->showMessageBox(MessageBoxIconType::WarningIcon, "Sidechain Error", "Sidechain input not found", "OK");
			applySidechain = false;
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
	else if (parameterID == "high_pass_freq_slider") cutoffFrquencyHP = newValue;
	else if (parameterID == "low_pass_freq_slider") cutoffFrquencyLP = newValue;

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

	params.push_back(std::make_unique<AudioParameterFloat>("high_pass_freq_slider", "hpFreqSlider",
	NormalisableRange<float>(
		Constants::Parameter::highPassFreqStart,
		Constants::Parameter::highPassFreqEnd,
		Constants::Parameter::highPassFreqInterval), 20.0f,
	String(), AudioProcessorParameter::genericParameter,
	[](float value, float)
	{
		return String(value, 1) + " Hz";
	}));

	params.push_back(std::make_unique<AudioParameterFloat>("low_pass_freq_slider", "lpFreqSlider",
NormalisableRange<float>(
	Constants::Parameter::lowPassFreqStart,
	Constants::Parameter::lowPassFreqEnd,
	Constants::Parameter::lowPassFreqInterval), 20.0f,
String(), AudioProcessorParameter::genericParameter,
[](float value, float)
{
	return String(value, 1) + " Hz";
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