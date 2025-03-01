enum effect_t {
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
	EF_DAC,
	EF_PORTA_UP,
	EF_PORTA_DOWN,
	EF_DUTY_CYCLE,
	EF_SAMPLE_OFFSET,
	EF_SLIDE_UP,
	EF_SLIDE_DOWN,
	EF_VOLUME_SLIDE,
	EF_NOTE_CUT,
	EF_RETRIGGER,
	EF_DELAYED_VOLUME,			// // //
	EF_FDS_MOD_DEPTH,
	EF_FDS_MOD_SPEED_HI,
	EF_FDS_MOD_SPEED_LO,
	EF_DPCM_PITCH,
	EF_SUNSOFT_ENV_LO,
	EF_SUNSOFT_ENV_HI,
	EF_SUNSOFT_ENV_TYPE,
	EF_NOTE_RELEASE,			// // //
	EF_GROOVE,					// // //
	EF_TRANSPOSE,				// // //
//	EF_TARGET_VOLUME_SLIDE, 
/*
	EF_VRC7_MODULATOR,
	EF_VRC7_CARRIER,
	EF_VRC7_LEVELS,
*/
	EF_COUNT
};

#define DEF_CMD(a) (((a)<<1)|0x80)

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
	
	CMD_EFF_NOTE_RELEASE	= DEF_CMD(26),	// // //
	CMD_EFF_LINEAR_COUNTER	= DEF_CMD(27),	// // //
	CMD_EFF_GROOVE			= DEF_CMD(28),	// // //
	CMD_EFF_DELAYED_VOLUME	= DEF_CMD(29),	// // //	

	CMD_EFF_FDS_MOD_DEPTH	= DEF_CMD(30),
	CMD_EFF_FDS_MOD_RATE_HI = DEF_CMD(31),
	CMD_EFF_FDS_MOD_RATE_LO = DEF_CMD(32),
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
	CMD44_EFF_NOTE_RELEASE		= DEF_CMD(29),	// BAh
	CMD44_EFF_LINEAR_COUNTER	= DEF_CMD(30),	// BCh
	CMD44_EFF_GROOVE			= DEF_CMD(31),	// BEh
	CMD44_EFF_DELAYED_VOLUME	= DEF_CMD(32),	// C0h
	CMD44_EFF_TRANSPOSE			= DEF_CMD(33),	// C2h
	CMD44_EFF_FDS_MOD_DEPTH		= DEF_CMD(34),	// C4h
	CMD44_EFF_FDS_MOD_RATE_HI	= DEF_CMD(35),	// C6h
	CMD44_EFF_FDS_MOD_RATE_LO	= DEF_CMD(36),	// C8h
	CMD44_EFF_N163_FINE_PITCH	= DEF_CMD(37),	// CAh
	CMD44_EFF_S5B_ENV_TYPE		= DEF_CMD(38),	// CCh
	CMD44_EFF_S5B_ENV_RATE_HI	= DEF_CMD(39),	// CEh
	CMD44_EFF_S5B_ENV_RATE_LO	= DEF_CMD(40),	// D0h
};

unsigned char convert_0cc_cmd(unsigned char cmd)
{
	switch(cmd){
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
	case CMD_EFF_EARRAPE:			return EF_DAC;
	case CMD_EFF_DUTY:				return EF_DUTY_CYCLE;
	case CMD_EFF_OFFSET:			return EF_SAMPLE_OFFSET;
	case CMD_EFF_SLIDE_UP:			return EF_SLIDE_UP;
	case CMD_EFF_SLIDE_DOWN:		return EF_SLIDE_DOWN;
	case CMD_EFF_VOL_SLIDE:			return EF_VOLUME_SLIDE;
	case CMD_EFF_NOTE_CUT:			return EF_NOTE_CUT;
	case CMD_EFF_RETRIGGER:			return EF_RETRIGGER;
	case CMD_EFF_DPCM_PITCH:		return EF_DPCM_PITCH;
	case CMD_EFF_NOTE_RELEASE:		return EF_NOTE_RELEASE; // Note release (Lxx)
	case CMD_EFF_LINEAR_COUNTER:	return EF_NOTE_CUT; // Triangle only (Sxx)
	case CMD_EFF_GROOVE:			return EF_GROOVE; // Oxx
	case CMD_EFF_DELAYED_VOLUME:	return EF_DELAYED_VOLUME; // Mxy
	case CMD_EFF_FDS_MOD_DEPTH:		return EF_FDS_MOD_DEPTH;
	case CMD_EFF_FDS_MOD_RATE_HI:	return EF_FDS_MOD_SPEED_HI;
	case CMD_EFF_FDS_MOD_RATE_LO:	return EF_FDS_MOD_SPEED_LO;
	case CMD_SET_DURATION:			return EF_SPEED;
	//case CMD_EFF_VRC7_PATCH:		return 102;
	//case CMD_LOOP_POINT:			return 102;
	default:						return 103;
	}
}

unsigned char convert_new_0cc_cmd(unsigned char cmd)
{
	switch(cmd)
	{
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
	case CMD44_EFF_DAC: return EF_DAC;
	case CMD44_EFF_OFFSET: return EF_SAMPLE_OFFSET;
	case CMD44_EFF_SLIDE_UP: return EF_SLIDE_UP;
	case CMD44_EFF_SLIDE_DOWN: return EF_SLIDE_DOWN;
	case CMD44_EFF_VOL_SLIDE: return EF_VOLUME_SLIDE;
	case CMD44_EFF_NOTE_CUT: return EF_NOTE_CUT;
	case CMD44_EFF_RETRIGGER: return EF_RETRIGGER;
	case CMD44_EFF_DPCM_PITCH: return EF_DPCM_PITCH;
	case CMD44_EFF_NOTE_RELEASE: return EF_NOTE_RELEASE; // Note release (Lxx)
	case CMD44_EFF_LINEAR_COUNTER: return EF_NOTE_CUT; // Triangle only (Sxx)
	case CMD44_EFF_GROOVE: return EF_GROOVE; // Oxx
	case CMD44_EFF_DELAYED_VOLUME: return EF_DELAYED_VOLUME; // Mxy
	case CMD44_EFF_TRANSPOSE: return EF_TRANSPOSE; // Txy
	//case CMD44_EFF_FDS_MOD_DEPTH: return EF_FDS_MOD_DEPTH;
	case CMD44_EFF_FDS_MOD_DEPTH: return EF_SUNSOFT_ENV_TYPE;
	case CMD44_EFF_FDS_MOD_RATE_HI: return EF_SUNSOFT_ENV_HI;
	case CMD44_EFF_FDS_MOD_RATE_LO: return EF_SUNSOFT_ENV_LO;
	//case CMD44_EFF_FDS_MOD_RATE_LO: return EF_FDS_MOD_SPEED_LO;
	case CMD44_EFF_N163_FINE_PITCH: return 103; // ???
	//case CMD44_EFF_S5B_ENV_TYPE: return EF_FDS_MOD_SPEED_LO;
	//case CMD44_EFF_S5B_ENV_RATE_HI: return EF_FDS_MOD_DEPTH;
	//case CMD44_EFF_S5B_ENV_RATE_LO: return EF_FDS_MOD_SPEED_HI;
	
	default:						return 103;
	}
}