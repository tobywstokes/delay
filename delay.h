#ifndef DELAY_H
#define DELAY_H

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include <fstream>

class delay:public AudioEffectX
{
	public:
		delay(audioMasterCallback audioMaster);
		~delay();
		void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);
		void resume();
    		// Programs for plug−in
    		void setProgramName (char* name);
    		void getProgramName (char* name);
    		// Parameters (use with GUI)
		// Constructor
		// Destructor
    		void setParameter (VstInt32 index, float value);
    		float getParameter (VstInt32 index);
    		void getParameterLabel (VstInt32 index, char* label);
    		void getParameterDisplay (VstInt32 index, char* text);
    		void getParameterName (VstInt32 index, char* text);
    		// Plug−in details (name, etc.) for VST host
    		bool getEffectName (char* name);
    		bool getVendorString (char* text);
    		bool getProductString (char* text);
    		VstInt32 getVendorVersion ();
		std::ofstream logfile;
	private:
		struct cBufS{
			int index;
			float *buffer;
		}cBuf;
		enum{DELAY_SAMPLES, FEEDBACK, DELAY_LEVEL};
		float sample_rate;
		float delay_samples;
		float feedback;
		float delay_level;
		char programName[kVstMaxProgNameLen + 1];
};
#endif


