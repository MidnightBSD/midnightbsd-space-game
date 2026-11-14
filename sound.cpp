/*
 *  sound.c
 *  Allenroids
 *
 *  Provides sound playback via SDL.
 *
 */

#include "sound.h"
#include "Allenroids.h"
#include <array>
#include <cstdlib>
#include <limits>
#include <new>
#include <vector>

boolean audioOn;
SoundSample channels[NUMBER_CHANNELS];

namespace {
bool audioSubsystemInitializedByGame = false;
bool audioShutdownRegistered = false;

bool queueSoundLocked(SDL_AudioCVT soundList[], int idToPlay) {
    for (int index = 0; index < NUMBER_CHANNELS; ++index) {
        if (channels[index].dataPosition >= channels[index].dataLength) {
            channels[index].data = soundList[idToPlay].buf;
            channels[index].dataLength = soundList[idToPlay].len_cvt;
            channels[index].dataPosition = 0;
            channels[index].soundID = idToPlay;
            return true;
        }
    }
    return false;
}
}

const char *soundName[]=
{								//soundBuffer Index:
	"Sounds/Pop.wav",				//	0
	"Sounds/PopAlien.wav",			//	1
	"Sounds/Shields.wav",			//	2
	"Sounds/Thrust.wav",			//	3
	"Sounds/StopThrusting.wav",		//	4
	"Sounds/ExplosionSmall.wav",	//	5
	"Sounds/ExplosionMedium.wav",	//	6
	"Sounds/ExplosionLarge.wav",	//	7
	"Sounds/ExplosionHuge.wav",		//	8
	"Sounds/WarningPlayer1.wav",	//	9
	"Sounds/WarningPlayer2.wav",	//	10
	"Sounds/ReleaseCrystal.wav",	//	11
	"Sounds/RetrieveCrystal.wav",	//	12
	"Sounds/CrystalSplatter.wav",	//	13
	"Sounds/WarningAlien.wav",		//	14
	"Sounds/ShotTunk.wav"			//  15
};

SDL_AudioCVT soundBuffer[NUMBER_SOUNDS];
std::array<std::vector<Uint8>, NUMBER_SOUNDS> soundStorage;

/****************************************************
 * soundInit
 *  set up SDL sound
 ***************************************************/
void soundInit(){
	if(audioOn)
		return;

	if(SDL_WasInit(SDL_INIT_AUDIO) == 0) {
		if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
			audioOn = false;
			return;
		}
		audioSubsystemInitializedByGame = true;
	}

	SDL_AudioSpec soundFormat = {};
	
    /* Set 16-bit stereo audio at 22Khz */
    soundFormat.freq = 22050;
//    soundFormat.freq = 11127;
    soundFormat.format = AUDIO_S16;
    soundFormat.channels = 2; // 1 = mono, 2 = stereo
    soundFormat.samples = 1024; // A good value for games
    soundFormat.callback = mixaudio; //a function!
    soundFormat.userdata = NULL;
	
	if( SDL_OpenAudio(&soundFormat, NULL) < 0) {
		//printf("Warning: %s\n", SDL_GetError());
		audioOn = false;
		if(audioSubsystemInitializedByGame) {
			SDL_QuitSubSystem(SDL_INIT_AUDIO);
			audioSubsystemInitializedByGame = false;
		}
		return;
	}
	else
		audioOn = true;

	if(!audioShutdownRegistered) {
		std::atexit(soundShutdown);
		audioShutdownRegistered = true;
	}
	
	loadSounds(soundName, soundBuffer, NUMBER_SOUNDS);
	
	SDL_PauseAudio(false); //activate sound
	//load all the sounds here
}

void soundShutdown(){
	if(audioOn) {
		SDL_CloseAudio();
		audioOn = false;
	}

	if(audioSubsystemInitializedByGame) {
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		audioSubsystemInitializedByGame = false;
	}
}


/****************************************************
 * loadSounds
 *  copies the specified sound names to memory
 ***************************************************/
void loadSounds(const char * const nameBuffer[], SDL_AudioCVT soundBuffer[], unsigned int numberSounds){
	unsigned int counter;
    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dataLength;	
	
	for(counter = 0; counter < numberSounds; counter++){
		// Load the sound file and convert it to 16-bit stereo at 22kHz
		if ( SDL_LoadWAV(nameBuffer[counter], &wave, &data, &dataLength) == NULL ) {
			//fprintf(stderr, "Couldn't load %s: %s\n", nameBuffer[counter], SDL_GetError());
			return;
		}
		if(SDL_BuildAudioCVT(&soundBuffer[counter],
				wave.format,	wave.channels,	wave.freq,
				AUDIO_S16,		2,				22050) < 0) {
			SDL_FreeWAV(data);
			return;
		}
		if(soundBuffer[counter].len_mult <= 0 ||
			dataLength > std::numeric_limits<size_t>::max() / static_cast<size_t>(soundBuffer[counter].len_mult)) {
			SDL_FreeWAV(data);
			return;
		}
		try {
			soundStorage[counter].resize(dataLength * static_cast<size_t>(soundBuffer[counter].len_mult));
		}
		catch(const std::bad_alloc &) {
			SDL_FreeWAV(data);
			return;
		}
		soundBuffer[counter].buf = soundStorage[counter].data();
		memcpy(soundBuffer[counter].buf, data, dataLength);
		soundBuffer[counter].len = dataLength;
		SDL_ConvertAudio(&soundBuffer[counter]);
		SDL_FreeWAV(data);
	}//end for

}

/****************************************************
 * mixAudio
 *  plays all the sounds on the docket at once.
 ***************************************************/
void mixaudio(void *unused, Uint8 *stream, int length)
{
    unsigned int counter;
    //Uint32 amount; //as suggested by tutorial code; creates signed/unsigned complaints
	int amount;

	SDL_memset(stream, 0, length);
	
    for ( counter = 0; counter < NUMBER_CHANNELS; counter++ ){
		if(channels[counter].data == NULL ||
			channels[counter].dataPosition >= channels[counter].dataLength)
			continue;
		amount = (channels[counter].dataLength - channels[counter].dataPosition);
		if ( amount > length ){
			amount = length;
		}
		SDL_MixAudio(stream, &channels[counter].data[channels[counter].dataPosition], amount, SDL_MIX_MAXVOLUME);
		channels[counter].dataPosition += amount;
    }
}


/****************************************************
 * playSound
 *  run a sound
 *	adapted from http://www.libsdl.org/intro/usingsound.html
 ***************************************************/
void playSound(SDL_AudioCVT soundList[], int idToPlay){
    SDL_LockAudio();
    queueSoundLocked(soundList, idToPlay);
    SDL_UnlockAudio();
}

bool playSoundIfFewerThan(SDL_AudioCVT soundList[], int idToPlay, unsigned int maximumInstances){
    unsigned int activeInstances = 0;

    SDL_LockAudio();
    for (unsigned int index = 0; index < NUMBER_CHANNELS; ++index) {
        if (channels[index].soundID == idToPlay &&
            channels[index].dataPosition < channels[index].dataLength) {
            ++activeInstances;
        }
    }
    const bool queued = activeInstances < maximumInstances && queueSoundLocked(soundList, idToPlay);
    SDL_UnlockAudio();
    return queued;
}
