///////////////////////////////////////////////////////////
//
// AudioEngine.h: Core audio functionality
// Copyright (c) 2010 - iZotope, Inc.  All Rights Reserved
//
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "portaudio.h"
#include <v8.h>
#include <vector>
#include <node_internals.h>
#include <node_object_wrap.h>
using namespace v8; using namespace std;

#define SAMPLE_RATE			(44100)
#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (256)

namespace Audio {

	//////////////////////////////////////////////////////////////////////////
	//! Core audio functionality
	class AudioEngine : public node::ObjectWrap {
	public:
		AudioEngine( Local<Function>& audioCallback ); //!< Initalize

		static void Init(v8::Handle<v8::Object> target);	//!< Our node.js instantiation function
		static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);	//!< Instantiate from instance factory
		
		//! Our static audio callback function. This is static, and we pass a pointer to ourselves in as userData,
		//! allowing us to call a non-static function on the correct instance of AudioEngine.
		static int audioCallbackSource( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, 
								 const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) {
			return ((AudioEngine*)userData)->audioCallback(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags);
		} // end audioCallbackSource()

		//!< Our main audio callback
		int audioCallback( const void *input, void *output, unsigned long uSampleFrames, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags );

		~AudioEngine();	//!< Destructor

	private:
		static v8::Persistent<v8::Function> constructor;				
		static v8::Handle<v8::Value> New( const v8::Arguments& args );					//!< Our V8 new operator

		static v8::Handle<v8::Value> ProcessIfNewData( const v8::Arguments& args );		//!< Hands audio to a javascript callback if we have new data
		
		static v8::Handle<v8::Value> IsActive( const v8::Arguments& args );				//!< Returns whether the audio stream active
		static v8::Handle<v8::Value> GetNumInputChannels( const v8::Arguments& args );	//!< Returns the number of input channels
		static v8::Handle<v8::Value> GetNumOutputChannels( const v8::Arguments& args );	//!< Returns the number of output channels
		static v8::Handle<v8::Value> GetSampleRate( const v8::Arguments& args );		//!< Returns the sample rate
		static v8::Handle<v8::Value> GetInputDeviceIndex( const v8::Arguments& args );	//!< Returns the index of the input device
		static v8::Handle<v8::Value> GetOutputDeviceIndex( const v8::Arguments& args );	//!< Returns the index of the output device
		static v8::Handle<v8::Value> GetDeviceName( const v8::Arguments& args );		//!< Returns the name of the device corresponding to a given ID number
		static v8::Handle<v8::Value> GetNumDevices( const v8::Arguments& args );		//!< Returns the number of devices we have available

		static v8::Handle<v8::Value> SetInputDevice( const v8::Arguments& args );		//!< Changes the input device
		static v8::Handle<v8::Value> SetOutputDevice( const v8::Arguments& args );		//!< Changes the output device

		void copyBuffer( int uSampleFrames, const float* sourceBuffer, float* destBuffer ); //!< Copy one buffer into another
		void wrapObject( v8::Handle<v8::Object> object ); //!< Wraps a handle into an object
		void restartStream( AudioEngine* engine );	//!< Stops and restarts our audio stream

		PaStream* m_pStream;	//!< Our audio stream

		PaStreamParameters m_inputParameters,		//!< Our stream parameters
						   m_outputParameters;
		
		Persistent<Function> m_audioCallback;	//!< We call this with audio data whenever we get it

		Handle<Value> m_uSampleFrames;			//!< Number of sample frames in this buffer (set in audioCallbackSource)

		v8::Local<v8::Array> m_inputBuffer,		//!< Our v8 input and output buffers
							 m_outputBuffer;

		bool m_bNewAudioData;	//!< Our new audio data flag

		int m_uSleepTime;		//!< The number of milliseconds we put the audio thread to 
								//!< sleep when we're waiting for new data from the non-process thread

		vector<vector<float> > m_tempBuffer,
							   m_fInputNonProcessSamples,	//!< Samples we can change from the main thread
							   m_fOutputNonProcessSamples;	//!< Samples we can change from the main thread

	}; // end class AudioEngine
	
} // end namespace Audio
