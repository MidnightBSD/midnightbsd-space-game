/*
 *  sound.h
 *  Allenroids
 *
 *  It allows us to make noise.
 */

#if defined(__LINUX__)
	#include "SDL/SDL.h"
	#include "SDL/SDL_audio.h"
#else
	#include "SDL.h"
	#include "SDL_audio.h"
#endif

#define NUMBER_CHANNELS 8

typedef struct soundSample{
    Uint8 *data;
    Uint32 dataPosition;
    Uint32 dataLength;
	int soundID;
} SoundSample;

//The Sounds
#define POP_SOUND				0
#define POP_ALIEN_SOUND			1
#define SHIELDS_SOUND			2
#define THRUST_SOUND			3
#define STOP_THRUSTING_SOUND	4
#define EXPLOSION_SMALL_SOUND	5
#define EXPLOSION_MEDIUM_SOUND	6
#define EXPLOSION_LARGE_SOUND	7
#define EXPLOSION_HUGE_SOUND	8
#define WARNING_PLAYER1_SOUND	9
#define WARNING_PLAYER2_SOUND	10
#define RELEASE_CRYSTAL_SOUND	11
#define RETRIEVE_CRYSTAL_SOUND	12
#define CRYSTAL_SPLATTER_SOUND	13
#define WARNING_ALIEN_SOUND		14
#define SHOT_TUNK_SOUND			15

#define NUMBER_SOUNDS 16

//Macros
#define WARNING_SOUND(x) (\
	(x==1) ? WARNING_PLAYER1_SOUND:WARNING_PLAYER2_SOUND\
)

//Prototypes
extern void soundInit();
extern void loadSounds(char **, SDL_AudioCVT *, unsigned int);
extern void mixaudio(void *, Uint8 *, int);
extern void playSound(SDL_AudioCVT *, int);