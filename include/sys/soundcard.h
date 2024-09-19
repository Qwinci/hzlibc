#ifndef _SYS_SOUNDCARD_H
#define _SYS_SOUNDCARD_H

#include <sys/ioctl.h>

#define SNDCTL_DSP_RESET _IO('P', 0)
#define SNDCTL_DSP_SYNC _IO('P', 1)
#define SNDCTL_DSP_SPEED _IOWR('P', 2, int)
#define SNDCTL_DSP_STEREO _IOWR('P', 3, int)
#define SNDCTL_DSP_GETBLKSIZE _IOWR('P', 4, int)
#define SNDCTL_DSP_SETFMT _IOWR('P', 5, int)
#define SNDCTL_DSP_SAMPLESIZE SNDCTL_DSP_SETFMT
#define SNDCTL_DSP_CHANNELS _IOWR('P', 6, int)
#define SNDCTL_DSP_POST _IO('P', 8)
#define SNDCTL_DSP_SUBDIVIDE _IOWR('P', 9, int)
#define SNDCTL_DSP_SETFRAGMENT _IOWR('P', 10, int)
#define SNDCTL_DSP_GETFMTS _IOR('P', 11, int)
#define SNDCTL_DSP_GETOSPACE _IOR('P', 12, audio_buf_info)
#define SNDCTL_DSP_GETISPACE _IOR('P', 13, audio_buf_info)
#define SNDCTL_DSP_NONBLOCK _IO('P', 14)
#define SNDCTL_DSP_GETCAPS _IOR('P', 15, int)
#define SNDCTL_DSP_GETTRIGGER _IOR('P', 16, int)
#define SNDCTL_DSP_SETTRIGGER _IOW('P', 16, int)
#define SNDCTL_DSP_GETIPTR _IOR('P', 17, count_info)
#define SNDCTL_DSP_GETOPTR _IOR('P', 18, count_info)
#define SNDCTL_DSP_SETSYNCRO _IO('P', 21)
#define SNDCTL_DSP_SETDUPLEX _IO('P', 22)

#define SOUND_DEVICE_LABELS	{"Vol  ", "Bass ", "Trebl", "Synth", "Pcm  ", "Spkr ", "Line ", \
	"Mic  ", "CD   ", "Mix  ", "Pcm2 ", "Rec  ", "IGain", "OGain", \
	"Line1", "Line2", "Line3", "Digital1", "Digital2", "Digital3", \
	"PhoneIn", "PhoneOut", "Video", "Radio", "Monitor"}
#define SOUND_DEVICE_NAMES	{"vol", "bass", "treble", "synth", "pcm", "speaker", "line", \
	"mic", "cd", "mix", "pcm2", "rec", "igain", "ogain", \
	"line1", "line2", "line3", "dig1", "dig2", "dig3", \
	"phin", "phout", "video", "radio", "monitor"}

#define SOUND_MIXER_VOLUME 0
#define SOUND_MIXER_BASS 1
#define SOUND_MIXER_TREBLE 2
#define SOUND_MIXER_SYNTH 3
#define SOUND_MIXER_PCM 4
#define SOUND_MIXER_SPEAKER 5
#define SOUND_MIXER_LINE 6
#define SOUND_MIXER_MIC 7
#define SOUND_MIXER_CD 8
#define SOUND_MIXER_IMIX 9
#define SOUND_MIXER_ALTPCM 10
#define SOUND_MIXER_RECLEV 11
#define SOUND_MIXER_IGAIN 12
#define SOUND_MIXER_OGAIN 13
#define SOUND_MIXER_LINE1 14
#define SOUND_MIXER_LINE2 15
#define SOUND_MIXER_LINE3 16
#define SOUND_MIXER_DIGITAL1 17
#define SOUND_MIXER_DIGITAL2 18
#define SOUND_MIXER_DIGITAL3 19
#define SOUND_MIXER_PHONEIN 20
#define SOUND_MIXER_PHONEOUT 21
#define SOUND_MIXER_VIDEO 22
#define SOUND_MIXER_RADIO 23
#define SOUND_MIXER_MONITOR 24
#define SOUND_MIXER_NRDEVICES 25

#define MIXER_READ(dev) _IOR('M', dev, int)
#define MIXER_WRITE(dev) _IOWR('M', dev, int)

#define SOUND_MIXER_STEREODEVS 0xFB
#define SOUND_MIXER_RECMASK 0xFD
#define SOUND_MIXER_DEVMASK 0xFE
#define SOUND_MIXER_RECSRC 0xFF

#define SOUND_MIXER_READ_RECSRC MIXER_READ(SOUND_MIXER_RECSRC)
#define SOUND_MIXER_READ_DEVMASK MIXER_READ(SOUND_MIXER_DEVMASK)
#define SOUND_MIXER_READ_RECMASK MIXER_READ(SOUND_MIXER_RECMASK)
#define SOUND_MIXER_READ_STEREODEVS MIXER_READ(SOUND_MIXER_STEREODEVS)
#define SOUND_MIXER_WRITE_RECSRC MIXER_WRITE(SOUND_MIXER_RECSRC)

// data formats
#define AFMT_QUERY 0
#define AFMT_MU_LAW 1
#define AFMT_A_LAW 2
#define AFMT_IMA_ADPCM 4
#define AFMT_U8 8
#define AFMT_S16_LE 0x10
#define AFMT_S16_BE 0x20
#define AFMT_S8 0x40
#define AFMT_U16_LE 0x80
#define AFMT_U16_BE 0x100
#define AFMT_MPEG 0x200
#define AFMT_AC3 0x400

typedef struct audio_buf_info {
	int fragments;
	int fragstotal;
	int fragsize;
	int bytes;
} audio_buf_info;

typedef struct count_info {
	int bytes;
	int blocks;
	int ptr;
} count_info;

#endif
