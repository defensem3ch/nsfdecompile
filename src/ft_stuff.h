/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2012  Jonathan Liss
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

// Part of this is directly taken from FamiTracker's source, hence the copyright notice. ;)
// -Slimeball

#define EX_NONE		0x00
#define EX_VRC6		0x01
#define EX_VRC7		0x02
#define EX_FDS		0x04
#define EX_MMC5		0x08
#define EX_N163		0x10
#define EX_S5B		0x20

enum{
	TYPE_2A03=1,
	TYPE_VRC6,
	TYPE_VRC7,
	TYPE_FDS,
	TYPE_N163,
	TYPE_S5B
};

#define CHANNELS_2A03 5
#define CHANNELS_MMC5 7
#define CHANNELS_VRC6 8
#define CHANNELS_VRC7 11
#define CHANNELS_FDS  6
#define CHANNELS_N163 13

#define MAX_I_SIZE_2A03		11
#define MAX_I_SIZE_MMC5		11
#define MAX_I_SIZE_VRC6		11
#define MAX_I_SIZE_VRC7		11
#define MAX_I_SIZE_FDS		28
#define MAX_I_SIZE_N163		125
#define MAX_I_SIZE_N163_OLD 125

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

//Versions
enum{
	FT_2_5,
	FT_2_6,
	FT_2_7BETA1,
	FT_2_7BETA2,
	FT_2_7BETA3,
	FT_2_7,
	FT_2_8BETA1,
	FT_2_8BETA2,
	FT_2_8BETA3A,
	FT_2_8BETA3B,
	FT_2_9,
	FT_3_0BETA1,
	FT_3_0BETA1A,
	FT_3_0BETA2,
	FT_3_0,
	FT_3_1BETA1,
	FT_3_1BETA2,
	FT_3_1BETA3,
	FT_3_1BETA4,
	FT_3_1BETA5,
	FT_3_5,
	FT_3_6BETA1,
	FT_3_6BETA2,
	FT_3_6,
	FT_3_7BETA1,
	FT_3_7BETA2A,
	FT_3_7BETA2B,
	FT_3_7,
	FT_3_8BETA1,
	FT_3_8BETA2,
	FT_3_8BETA3,
	FT_4_0,
	FT_4_1,
	FT_4_2BETA,
	FT_4_2,
	FT_4_3BETA1,
	FT_4_3BETA1A,
	FT_4_3BETA1B,
	FT_4_3BETA1C,
	FT_4_3BETA1D,
	FT_4_3,
	FT_4_3S,
	FT_4_4S_BETA,
	FT_4_4,
	FT_4_4S,
	FT_4_5_BETA,
	FT_4_5S_BETA,
	FT_4_5,
	FT_4_5S,
	FT_4_6,
	FT_4_6S,
	FT_VERSIONS
};

// Channel effects
#define DEF_CMD(x) ((x << 1) | 0x80)

enum {
	EF_NONE = 0,
	EF_SPEED,
	EF_JUMP,
	EF_SKIP,
	EF_HALT,
	EF_VOLUME,
	EF_PORTAMENTO,
	EF_PORTAOFF,				// unused!!
	EF_SWEEPUP,
	EF_SWEEPDOWN,
	EF_ARPEGGIO,
	EF_VIBRATO,
	EF_TREMOLO,
	EF_PITCH,
	EF_DELAY,
	EF_EARRAPE,
	EF_PORTA_UP,
	EF_PORTA_DOWN,
	EF_DUTY_CYCLE,
	EF_SAMPLE_OFFSET,
	EF_SLIDE_UP,
	EF_SLIDE_DOWN,
	EF_VOLUME_SLIDE,
	EF_NOTE_CUT,
	EF_RETRIGGER,
	EF_DELAYED_VOLUME,			// Unimplemented
	EF_FDS_MOD_DEPTH,
	EF_FDS_MOD_SPEED_HI,
	EF_FDS_MOD_SPEED_LO,
	EF_DPCM_PITCH,
	EF_SUNSOFT_ENV_LO,
	EF_SUNSOFT_ENV_HI,
	EF_SUNSOFT_ENV_TYPE,
	EF_COUNT
};

// Command table
enum {
	CMD_INSTRUMENT			= DEF_CMD(0),	// 80h
	CMD_EFF_SPEED			= DEF_CMD(1),	// 82h
	CMD_EFF_JUMP			= DEF_CMD(2),	// ...
	CMD_EFF_SKIP			= DEF_CMD(3),
	CMD_EFF_HALT			= DEF_CMD(4),	// 88h
	CMD_EFF_VOLUME			= DEF_CMD(5),
	CMD_EFF_PORTAMENTO		= DEF_CMD(6),
	CMD_EFF_PORTAUP			= DEF_CMD(7),
	CMD_EFF_PORTADOWN		= DEF_CMD(8),	// 90h
	CMD_EFF_SWEEP			= DEF_CMD(9),
	CMD_EFF_ARPEGGIO		= DEF_CMD(10),
	CMD_EFF_VIBRATO			= DEF_CMD(11),
	CMD_EFF_TREMOLO			= DEF_CMD(12),	// 98h
	CMD_EFF_PITCH			= DEF_CMD(13),
	CMD_EFF_DELAY			= DEF_CMD(14),
	CMD_EFF_EARRAPE			= DEF_CMD(15),
	CMD_EFF_DUTY			= DEF_CMD(16),	// A0h
	CMD_EFF_OFFSET			= DEF_CMD(17),
	CMD_EFF_SLIDE_UP		= DEF_CMD(18),
	CMD_EFF_SLIDE_DOWN		= DEF_CMD(19),
	CMD_EFF_VOL_SLIDE		= DEF_CMD(20),	// A8h
	CMD_EFF_NOTE_CUT		= DEF_CMD(21),
	CMD_EFF_RETRIGGER		= DEF_CMD(22),
	CMD_EFF_DPCM_PITCH		= DEF_CMD(23),

	CMD_SET_DURATION		= DEF_CMD(24),	// B0h (change these in init.s)
	CMD_RESET_DURATION		= DEF_CMD(25),	// B2h

	CMD_EFF_FDS_MOD_DEPTH	= DEF_CMD(26),
	CMD_EFF_FDS_MOD_RATE_HI = DEF_CMD(27),
	CMD_EFF_FDS_MOD_RATE_LO = DEF_CMD(28),
};


enum {
	CMD44_INSTRUMENT 			= DEF_CMD(0),	// 80h
	CMD44_SET_DURATION			= DEF_CMD(1),	// 82h
	CMD44_RESET_DURATION		= DEF_CMD(2),	// 84h
	CMD44_EFF_SPEED				= DEF_CMD(3),	// 86h
	CMD44_EFF_TEMPO				= DEF_CMD(4),	// 88h
	CMD44_EFF_JUMP				= DEF_CMD(5),	// 8Ah
	CMD44_EFF_SKIP				= DEF_CMD(6),	// 8Ch
	CMD44_EFF_HALT				= DEF_CMD(7),	// 8Eh
	CMD44_EFF_VOLUME			= DEF_CMD(8),	// 90h
	CMD44_EFF_CLEAR				= DEF_CMD(9),	// 92h
	CMD44_EFF_PORTAUP			= DEF_CMD(10),	// 94h
	CMD44_EFF_PORTADOWN			= DEF_CMD(11),	// 96h
	CMD44_EFF_PORTAMENTO		= DEF_CMD(12),	// 98h
	CMD44_EFF_ARPEGGIO			= DEF_CMD(13),	// 9Ah
	CMD44_EFF_VIBRATO			= DEF_CMD(14),	// 9Ch
	CMD44_EFF_TREMOLO			= DEF_CMD(15),	// 9Eh
	CMD44_EFF_PITCH				= DEF_CMD(16),	// A0h
	CMD44_EFF_RESET_PITCH		= DEF_CMD(17),	// A2h
	CMD44_EFF_DUTY				= DEF_CMD(18),	// A4h
	CMD44_EFF_DELAY				= DEF_CMD(19),	// A6h
	CMD44_EFF_SWEEP				= DEF_CMD(20),	// A8h
	CMD44_EFF_DAC				= DEF_CMD(21),	// AAh
	CMD44_EFF_OFFSET			= DEF_CMD(22),	// ACh
	CMD44_EFF_SLIDE_UP			= DEF_CMD(23),	// AEh
	CMD44_EFF_SLIDE_DOWN		= DEF_CMD(24),	// B0h
	CMD44_EFF_VOL_SLIDE			= DEF_CMD(25),	// B2h
	CMD44_EFF_NOTE_CUT			= DEF_CMD(26),	// B4h
	CMD44_EFF_RETRIGGER			= DEF_CMD(27),	// B6h
	CMD44_EFF_DPCM_PITCH		= DEF_CMD(28),	// B8h
	CMD44_EFF_FDS_MOD_DEPTH		= DEF_CMD(29),	// BAh
	CMD44_EFF_FDS_MOD_RATE_HI	= DEF_CMD(30),	// BCh
	CMD44_EFF_FDS_MOD_RATE_LO	= DEF_CMD(31),	// BEh
};

unsigned char convert_cmd(unsigned char cmd)
{
	switch(cmd){
	case 0xCE:						return 99;
	case 0x99:						return 99;
	case 0x97:						return 99;
	case 0x9F:						return 99;
	case 0x87:						return 99;
	case 0x8D:						return 99;
	case CMD_INSTRUMENT:			return 100;
	case CMD_EFF_SPEED:				return EF_SPEED;
	case CMD_EFF_JUMP:				return EF_JUMP;
	case CMD_EFF_SKIP:				return EF_SKIP;
	case CMD_EFF_HALT:				return EF_HALT;
	case CMD_EFF_VOLUME:			return EF_VOLUME;
	case CMD_EFF_PORTAMENTO:		return EF_PORTAMENTO;
	case CMD_EFF_PORTAUP:			return EF_PORTA_UP;
	case CMD_EFF_PORTADOWN:			return EF_PORTA_DOWN;
	case CMD_EFF_SWEEP:				return 101;
	case CMD_EFF_ARPEGGIO:			return EF_ARPEGGIO;
	case CMD_EFF_VIBRATO:			return EF_VIBRATO;
	case CMD_EFF_TREMOLO:			return EF_TREMOLO;
	case CMD_EFF_PITCH:				return EF_PITCH;
	case CMD_EFF_DELAY:				return EF_DELAY;
	case CMD_EFF_EARRAPE:			return EF_EARRAPE;
	case CMD_EFF_DUTY:				return EF_DUTY_CYCLE;
	case CMD_EFF_OFFSET:			return EF_SAMPLE_OFFSET;
	case CMD_EFF_SLIDE_UP:			return EF_SLIDE_UP;
	case CMD_EFF_SLIDE_DOWN:		return EF_SLIDE_DOWN;
	case CMD_EFF_VOL_SLIDE:			return EF_VOLUME_SLIDE;
	case CMD_EFF_NOTE_CUT:			return EF_NOTE_CUT;
	case CMD_EFF_RETRIGGER:			return EF_RETRIGGER;
	case CMD_EFF_DPCM_PITCH:		return EF_DPCM_PITCH;
	case CMD_EFF_FDS_MOD_DEPTH:		return EF_FDS_MOD_DEPTH;
	case CMD_EFF_FDS_MOD_RATE_HI:	return EF_FDS_MOD_SPEED_HI;
	case CMD_EFF_FDS_MOD_RATE_LO:	return EF_FDS_MOD_SPEED_LO;
	case CMD_SET_DURATION:			return EF_SPEED;
	//case CMD_EFF_VRC7_PATCH:		return 102;
	//case CMD_LOOP_POINT:			return 102;
	default:						return 103;
	}
}

unsigned char convert_new_cmd(unsigned char cmd)
{
	switch(cmd){
	case CMD44_INSTRUMENT: return 100;
	//case CMD44_SET_DURATION: return 100;
	//case CMD44_RESET_DURATION: return 100;
	case CMD44_EFF_SPEED: return EF_SPEED;
	case CMD44_EFF_TEMPO: return EF_SPEED;
	case CMD44_EFF_JUMP: return EF_JUMP;
	case CMD44_EFF_SKIP: return EF_SKIP;
	case CMD44_EFF_HALT: return EF_HALT;
	case CMD44_EFF_VOLUME: return EF_VOLUME;
	case CMD44_EFF_CLEAR: return 110;
	case CMD44_EFF_PORTAUP: return EF_PORTA_UP;
	case CMD44_EFF_PORTADOWN: return EF_PORTA_DOWN;
	case CMD44_EFF_PORTAMENTO: return EF_PORTAMENTO;
	case CMD44_EFF_ARPEGGIO: return EF_ARPEGGIO;
	case CMD44_EFF_VIBRATO: return EF_VIBRATO;
	case CMD44_EFF_TREMOLO: return EF_TREMOLO;
	case CMD44_EFF_PITCH: return EF_PITCH;
	case CMD44_EFF_RESET_PITCH: return 111;
	case CMD44_EFF_DUTY: return EF_DUTY_CYCLE;
	case CMD44_EFF_DELAY: return EF_DELAY;
	case CMD44_EFF_SWEEP: return 101;
	case CMD44_EFF_DAC: return EF_EARRAPE;
	case CMD44_EFF_OFFSET: return EF_SAMPLE_OFFSET;
	case CMD44_EFF_SLIDE_UP: return EF_SLIDE_UP;
	case CMD44_EFF_SLIDE_DOWN: return EF_SLIDE_DOWN;
	case CMD44_EFF_VOL_SLIDE: return EF_VOLUME_SLIDE;
	case CMD44_EFF_NOTE_CUT: return EF_NOTE_CUT;
	case CMD44_EFF_RETRIGGER: return EF_RETRIGGER;
	case CMD44_EFF_DPCM_PITCH: return EF_DPCM_PITCH;
	case CMD44_EFF_FDS_MOD_DEPTH: return EF_FDS_MOD_DEPTH;
	case CMD44_EFF_FDS_MOD_RATE_HI: return EF_FDS_MOD_SPEED_HI;
	case CMD44_EFF_FDS_MOD_RATE_LO: return EF_FDS_MOD_SPEED_LO;
	default:						return 103;
	}
}

unsigned char convert_old_cmd(unsigned char cmd)
{
	switch(cmd){
	case 130: return EF_ARPEGGIO;
	case 131: return EF_PORTAMENTO;
	case 132: return EF_PORTAMENTO;
	case 133: return EF_VIBRATO;
	case 134: return EF_TREMOLO;
	case 135: return EF_SPEED;
	case 136: return EF_JUMP;
	case 137: return EF_SKIP;
	case 138: return EF_HALT;
	case 139: return EF_VOLUME;
	case 140: return 101;
	case 141: return EF_PITCH;
	case 142: return EF_DELAY;
	case 143: return EF_EARRAPE;
	default:  return 103;
	}
}
