#include "delay.h"
#include <math.h>
#include <fstream>

#define DEFAULT_PROG "Default"
#define UNIQUE_ID 'rgmd'

#define EFFECT_NAME "Delay"
#define PRODUCT_STRING "Delay"
#define VENDOR_STRING "TOBY STOKES"
#define VENDOR_VERSION 1

#define CIRCULAR_BUFFER_LENGTH 44100
#define MAX_DELAY_SAMPLES 44100
#define MAX_FEEDBACK 1
#define MAX_DELAY_LEVEL 1

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
	return new delay (audioMaster);
}

// Constructor
delay::delay(audioMasterCallback audiomaster)
: AudioEffectX(audioMaster, 1,3)
{
	setNumInputs(1);
	setNumOutputs(1);
	setUniqueID(UNIQUE_ID);
	canProcessReplacing();

	sample_rate = getSampleRate();
	delay_samples = 20000;
	feedback = 0.5;
	delay_level = 0.5;

	cBuf.buffer =  new float [CIRCULAR_BUFFER_LENGTH];
	logfile.open("delay.log");
	resume();

	vst_strncpy(programName,DEFAULT_PROG,kVstMaxProgNameLen);
}
// Destructor
delay::~delay()
{
	if (cBuf.buffer) delete[] cBuf.buffer;
	logfile.close();
}

// Processing
void delay::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float *in = inputs[0];
	float *out = outputs[0];
	float delayed_sample;
	int delayed_sample_index;
	for (int frame_idx=0;frame_idx<=sampleFrames;frame_idx++)
	{
		delayed_sample_index = (((int)(cBuf.index-delay_samples) + CIRCULAR_BUFFER_LENGTH) % (int)CIRCULAR_BUFFER_LENGTH);
		logfile << "delayed_sample_index" << delayed_sample_index << "\n"; 
		delayed_sample = (cBuf.buffer)[delayed_sample_index];
		logfile << "delayed_sample" << delayed_sample << "\n";
		//write current sample to buffer
		(cBuf.buffer)[cBuf.index] = in[frame_idx] + (delayed_sample*feedback);
		logfile << "In sample" << (cBuf.buffer)[cBuf.index] << "\n";
		//Calculate + write output sample
		out[frame_idx] = in[frame_idx] + (delay_level*delayed_sample);
		logfile << "Out sample = In sample " << in[frame_idx] << "+ (delay_level" << delay_level <<" * delayed sample" << delayed_sample <<") giving" <<out[frame_idx] << "\n\n";
		//Increment buffer index
		cBuf.index = ((cBuf.index+1)) % CIRCULAR_BUFFER_LENGTH;
	}
}

void delay::resume()
{
	cBuf.index = 0;
	memset(cBuf.buffer,0,CIRCULAR_BUFFER_LENGTH * sizeof(float));
}

// Plugin name set/get
void delay::setProgramName(char* name)
{
	vst_strncpy(programName, name, kVstMaxProgNameLen);
}
void delay::getProgramName(char* name)
{
	vst_strncpy(name, programName, kVstMaxProgNameLen);
}

//Parameters
void delay::setParameter(VstInt32 index, float value)
{
	switch(index)
	{
		case DELAY_SAMPLES:
			delay_samples = value*MAX_DELAY_SAMPLES;
			break;
		case FEEDBACK:
			feedback = value*MAX_FEEDBACK;
			break;
		case DELAY_LEVEL:
			delay_level = value*MAX_DELAY_LEVEL;
	}
}

float delay::getParameter(VstInt32 index)
{
	float param = 0.0;
	switch(index)
	{
		case DELAY_SAMPLES:
			param = delay_samples/MAX_DELAY_SAMPLES;
			break;
		case FEEDBACK:
			param = feedback/MAX_FEEDBACK;
			break;
		case DELAY_LEVEL:
			param = delay_level/MAX_DELAY_LEVEL;
	}
	return param;
}

void delay::getParameterName(VstInt32 index, char* label)
{
	switch(index)
	{
		case DELAY_SAMPLES:
			vst_strncpy(label,"Delay Samples", kVstMaxParamStrLen);
			break;
		case FEEDBACK:
			vst_strncpy(label,"Feedback", kVstMaxParamStrLen);
			break;
		case DELAY_LEVEL:
			vst_strncpy(label,"Delay Level",kVstMaxParamStrLen);
	}
}

void delay::getParameterDisplay(VstInt32 index, char* text)
{
	switch(index)
	{
		case DELAY_SAMPLES:
			int2string(delay_samples, text, kVstMaxParamStrLen);
			break;
		case FEEDBACK:
			dB2string(feedback, text, kVstMaxParamStrLen);
			break;
		case DELAY_LEVEL:
			dB2string(delay_level, text, kVstMaxParamStrLen);
	}
}

void delay::getParameterLabel(VstInt32 index, char* label)
{
	switch(index)
	{
		case DELAY_SAMPLES:
			vst_strncpy(label, "samples", kVstMaxParamStrLen);
			break;
		case FEEDBACK:
			vst_strncpy(label, "dB", kVstMaxParamStrLen);
			break;
		case DELAY_LEVEL:
			vst_strncpy(label, "dB", kVstMaxParamStrLen);
	}
}

//VST host functions
bool delay::getEffectName(char* name) {
    vst_strncpy (name, EFFECT_NAME, kVstMaxEffectNameLen);
    return true;
}
bool delay::getProductString(char* text) {
    vst_strncpy (text, PRODUCT_STRING, kVstMaxProductStrLen);
    return true;
}
bool delay::getVendorString(char* text) {
    vst_strncpy (text, VENDOR_STRING, kVstMaxVendorStrLen);
    return true;
}
VstInt32 delay::getVendorVersion() {
    return VENDOR_VERSION;
}
