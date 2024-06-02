#ifndef _FTNSF_STRUCT
#define _FTNSF_STRUCT

typedef struct {
	char file_id[5];
	unsigned char version;
	unsigned char n_songs;
	unsigned char first;
	unsigned short load_addr;
	unsigned short init_addr;
	unsigned short play_addr;
	char title[32];
	char author[32];
	char copyright[32];
	unsigned short ntsc_speed;
	unsigned char banks_init[8];
	unsigned short pal_speed;
	unsigned char machine;
	unsigned char expansion;
	char reserved[4];
} NSF_Header;

typedef struct {
	int row;
	unsigned char note;
	unsigned char octave;
	unsigned char instrument;
	unsigned char volume;
	unsigned char fxdata[8];
} NSF_Item;

typedef struct {
	NSF_Item Item[256];
	int offset;
	int channel;
	int index;
	int track;
	int ft_channel;
	int ch_fx;
	int items;
	int rows;
} NSF_Pattern;

typedef struct {
	int pp_pattern[32];
	unsigned char data[32];
	int offset;
} NSF_Frame;

typedef struct {
	unsigned char data[256];
	int index;
	int type;
	int length;
	int loop;
	int release;
	int setting;
	int offset;
	int pad;
	int expansion;
} NSF_Sequence;

typedef struct {
	NSF_Sequence FDS_Seq[3];
	unsigned char fds_wave[64];
	unsigned char fds_mtable[16];
	unsigned char wave_data[16][16];
	int type_matches[8];
	int pp_sequence[5];
	int seq[5];
	unsigned char vrc7_data[8];
	int channel;
	int type;
	int offset;
	int modswitch;
	int fds_index;
	int fds_delay;
	int fds_depth;
	int fds_speed;
	int fds_modswitch;
	int wave_size;
	int wave_count;
	int wave_offset;
	int wave_pos;
	int vrc7_patch;
} NSF_Instrument;

typedef struct {
	int pitch;
	int earrape;
	int index;
} NSF_Entry;

typedef struct {
	unsigned char data[4096];
	int offset;
	int address;
	int size;
	int bank;
} NSF_Sample;

typedef struct {
	unsigned char data[256];
	int offset;
	int index;
	int length;
} NSF_Groove;

typedef struct {
	NSF_Frame Frame[128];
	unsigned char ch_id[32];
	unsigned char ch_fx[32];
	NSF_Pattern* Pattern;
	int offset;
	int pp_frame;
	int n_frames;
	int p_length;
	int speed;
	int tempo;
	int bank;
	int n_patterns;
} NSF_Song;

#endif

typedef struct {
	NSF_Instrument Inst[64];
	NSF_Sequence Sequence[320];
	NSF_Sample Sample[96];
	NSF_Entry Entry[128];
	NSF_Groove Groove[32];
	unsigned char ch_id[32];
	int ch_type[32];
	NSF_Song* Song;
	int n_songs;
	int n_instruments;
	int n_sequences;
	int n_entries;
	int n_samples;
	int n_grooves;
	int n_channels;
	int n163_channels;
	int dpcm_inst;
	int version;
	int expansion;
	int e_speed;
} FT_Decompiler;

FT_Decompiler* Create_FTDecompiler();
void Free_FTDecompiler(FT_Decompiler* Dec);

int FT_LoadNSF(FT_Decompiler* Dec, char* filename, int version);
int FT_ExportFTM(FT_Decompiler* Dec, char* filename);
int FT_DecompilePattern(FT_Decompiler* Dec, NSF_Pattern* Pattern, unsigned char* ptr);
