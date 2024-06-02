//TODO: Alignment tests! This will likely be very useful for certain things... ;)

typedef struct {
	unsigned char data[256];
	int index;
	int type;
	int length;
	int loop;
	int release;
	int setting;
	int expansion;
	int ex_index;
} FT_Sequence;

typedef struct {
	FT_Sequence Seq[3];
	unsigned char wave_data[16][32];
	unsigned char dpcm_data[288];
	char name[64];
	unsigned char fds_wave[64];
	unsigned char fds_mtable[32];
	unsigned char seq_data[16];
	unsigned char vrc7_data[8];
	int index;
	int type;
	int seq_count;
	int name_length;
	int fds_mspeed;
	int fds_mdepth;
	int fds_mdelay;
	int vrc7_patch;
	int wave_size;
	int wave_pos;
	int wave_count;
} FT_Instrument;

typedef struct {
	unsigned char data[4096];
	char name[64];
	int index;
	int name_length;
	int data_length;
} FT_Sample;

typedef struct {
	int row;
	unsigned char note;
	unsigned char octave;
	unsigned char instrument;
	unsigned char volume;
	unsigned char fxdata[8];
} FT_Item;

typedef struct {
	FT_Item Item[256];
	int track;
	int channel;
	int index;
	int items;
} FT_Pattern;

typedef struct {
	unsigned char fdata[128][32];
	char name[64];
	char ch_fx[32];
	
	int n_channels;
	int n_frames;
	int speed;
	int tempo;
	int p_length;
	int name_length;
} FT_Track;

typedef struct {
	unsigned char data[256];
	int offset;
	int index;
	int length;
} FT_Groove;

typedef struct {
	FT_Sequence Seq[320];
	FT_Instrument Inst[64];
	FT_Sample Sample[96];
	FT_Groove Groove[32];
	
	char author[32];
	char title[32];
	char copyright[32];
	char ch_id[32];
	
	FT_Track* Track;
	FT_Pattern* Pattern;
	
	int file_version;
	int expansion;
	int n_channels;
	int machine;
	int e_speed;
	int v_style;
	int highlight1;
	int highlight2;
	int n163_channels;
	int speed_split;
	
	int n_tracks;
	int n_patterns;
	int n_instruments;
	int n_samples;
	int n_grooves;
	int n_2a03_seqs;
	int n_vrc6_seqs;
	int n_n163_seqs;
	int n_s5b_seqs;
} FT_File;

FT_File* Create_FTFile();
void Free_FTFile(FT_File* File);
//void Reset_FTFile(FT_File* File);
int FT_LoadFile(FT_File* File, char* filename);
int FT_SaveFile(FT_File* File, char* filename);

int FT_ReadParams(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadInfo(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadHeader(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadInstruments(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadSequences(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadFrames(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadPatterns(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadSamples(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadVRC6Sequences(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadN163Sequences(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadS5BSequences(FT_File* File, unsigned char* buf, int version, int size);
int FT_ReadGrooves(FT_File* File, unsigned char* buf, int version, int size);




