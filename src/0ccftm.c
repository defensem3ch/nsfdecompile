#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc_utils.h"
#include "0ccftm.h"

FT_File* Create_FTFile()
{
	FT_File* File;
	File = calloc(1, sizeof(FT_File));
	return File;
}

void Free_FTFile(FT_File* File)
{
	if(!File) return;
	if(File->Track) free(File->Track);
	if(File->Pattern) free(File->Pattern);
	free(File);
}

int FT_LoadFile(FT_File* File, char* filename)
{
	char block_id[16];
	unsigned char* buf;
	int ftm_version;
	int block_version;
	int block_size;
	int src_size;
	int status;
	int dp;

	dp = 0;
	src_size = getfsize(filename);
	if(!(buf = file2mem(filename))) return 0;

	if(memcmp(buf + dp, "FamiTracker Module", 18)){
		printf("Error loading %s: header mismatch\n", filename);
		free(buf);
		return 0;
	}

	memcpy(&ftm_version, buf+0x12, 4);
	printf("Version: 0x%X\n", ftm_version);

	dp = 0x16;

	while(dp < src_size-16){
		memcpy(block_id, buf+dp, 16); dp += 16;
		memcpy(&block_version, buf+dp, 4); dp += 4;
		memcpy(&block_size, buf+dp, 4); dp += 4;

		printf("Currently reading block id: %s\n", block_id);

		if(!strcmp(block_id, "PARAMS")){
			status = FT_ReadParams(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "INFO")){
			status = FT_ReadInfo(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "HEADER")){
			status = FT_ReadHeader(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "INSTRUMENTS")){
			status = FT_ReadInstruments(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "SEQUENCES")){
			status = FT_ReadSequences(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "FRAMES")){
			status = FT_ReadFrames(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "PATTERNS")){
			status = FT_ReadPatterns(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "DPCM SAMPLES")){
			status = FT_ReadSamples(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "SEQUENCES_VRC6")){
			status = FT_ReadVRC6Sequences(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "SEQUENCES_N163")){
			status = FT_ReadN163Sequences(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "SEQUENCES_S5B")){
			status = FT_ReadS5BSequences(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "GROOVES")){
			status = FT_ReadGrooves(File, buf+dp, block_version, block_size);
		} else if(!strcmp(block_id, "COMMENTS")){
			status = 1;
		} else {
			printf("Error loading %s: encountered unknown block %.16s @ 0x%X\n", filename, block_id, dp-24);
			free(buf);
			return 0;
		}

		if(!status){
			free(buf);
			return 0;
		}

		dp += block_size;
	}

	printf("%.3s\n", buf+dp);

	free(buf);
	return 1;
}

int FT_SaveFile(FT_File* File, char* filename)
{
	FT_Track* Track;
	FT_Instrument* Inst;
	//FT_Sequence* FirstSeq;
	FT_Sequence* Seq;
	FT_Pattern* Pattern;
	FT_Sample* Sample;
	FT_Groove* Groove;
	FILE* fp_dst;
	unsigned char* buf;
	//unsigned char* ptr;
	int size1, size2;
	//int version;
	int blocksize;
	int item_size;
	int count;
	//int namelen;
	int i, j;
	int dp;

	//printf("Exporting FTM... tracks: %i instruments: %i patterns: %i\n", File->n_tracks, File->n_instruments, File->n_patterns);

	size1 = File->n_patterns * sizeof(FT_Pattern);
	size2 = File->n_samples * sizeof(FT_Sample);
	blocksize = (size1 > size2)? size1: size2;
	if(blocksize < 65536) blocksize = 65536;

	//printf("Block size: %i\n", blocksize);

	buf = malloc(blocksize);
	fp_dst = fopen(filename,"wb"); if(!fp_dst) {printf("Error: Couldn't open FTM file for writing!\n"); return 0;}

	memcpy(buf, "FamiTracker Module\x40\x04\x00\x00PARAMS\0\0\0\0\0\0\0\0\0\0\x06\0\0\0\x13\0\0\0\0", 0x2D);
	buf[0x2A] = (File->expansion&0x10)? 0x21: 0x1D;
	dp = 0x2E;

	buf[dp++] = File->expansion;
	memcpy(buf+dp, &File->n_channels, 4); dp += 4;
	memcpy(buf+dp, &File->machine, 4); dp += 4;
	memcpy(buf+dp, &File->e_speed, 4); dp += 4;
	memcpy(buf+dp, &File->v_style, 4); dp += 4;
	memcpy(buf+dp, &File->highlight1, 4); dp += 4;
	memcpy(buf+dp, &File->highlight2, 4); dp += 4;
	if(File->expansion&0x10){
		memcpy(buf+dp, &File->n163_channels, 4);
		dp += 4;
	}
	memcpy(buf+dp, &File->speed_split, 4); dp += 4;
	fwrite(buf, dp, 1, fp_dst); dp = 0;

	memcpy(buf, "INFO\0\0\0\0\0\0\0\0\0\0\0\0\x01\0\0\0\x60\0\0\0", 0x18);
	memcpy(buf+0x18, File->title, 0x20);
	memcpy(buf+0x38, File->author, 0x20);
	memcpy(buf+0x58, File->copyright, 0x20);
	fwrite(buf, 0x78, 1, fp_dst); dp = 0;


	dp = 0x18;
	memcpy(buf, "HEADER\0\0\0\0\0\0\0\0\0\0\x03\0\0\0", 0x14);
	buf[dp++] = File->n_tracks - 1;

	for(i=0;i<File->n_tracks;i++){
		Track = File->Track + i;
		Track->n_channels = File->n_channels;
		strcpy((char*)(buf+dp), Track->name); dp += Track->name_length + 1;
	}

	for(i=0;i<File->n_channels;i++){
		buf[dp++] = File->ch_id[i];
		for(j=0;j<File->n_tracks;j++){
			Track = File->Track + j;
			buf[dp++] = Track->ch_fx[i];
		}
	}

	blocksize = dp - 0x18;
	memcpy(buf+0x14, &blocksize, 4);
	fwrite(buf, dp, 1, fp_dst); dp = 0;

	dp = 0x18;
	memcpy(buf, "INSTRUMENTS\0\0\0\0\0\x06\0\0\0", 0x14);
	memcpy(buf+dp, &File->n_instruments, 4); dp += 4;

	for(i=0;i<File->n_instruments;i++){
		Inst = File->Inst + i;
		memcpy(buf+dp, &Inst->index, 4); dp += 4;
		//printf("Instrument %i type: %i (w)\n", i, Inst->type);
		buf[dp++] = Inst->type;
		switch(Inst->type)
		{
		case 4: // FDS
			memcpy(buf+dp, Inst->fds_wave, 64); dp += 64;
			memcpy(buf+dp, Inst->fds_mtable, 32); dp += 32;
			memcpy(buf+dp, &Inst->fds_mspeed, 4); dp += 4;
			memcpy(buf+dp, &Inst->fds_mdepth, 4); dp += 4;
			memcpy(buf+dp, &Inst->fds_mdelay, 4); dp += 4;

			for(j=0;j<3;j++){
				Seq = Inst->Seq + j;
				buf[dp++] = Seq->length;
				memcpy(buf+dp, &Seq->loop, 4); dp += 4;
				memcpy(buf+dp, &Seq->release, 4); dp += 4;
				memcpy(buf+dp, &Seq->setting, 4); dp += 4;
				memcpy(buf+dp, Seq->data, Seq->length); dp += Seq->length;
			}
			break;

		case 3: // VRC7
			memcpy(buf+dp, &Inst->vrc7_patch, 4); dp += 4;
			memcpy(buf+dp, Inst->vrc7_data, 8); dp += 8;
			break;

		case 1: // 2A03/MMC5
		case 2: // VRC6
		case 5: // N163
		case 6: // S5B
			memcpy(buf+dp, &Inst->seq_count, 4); dp += 4;
			memcpy(buf+dp, Inst->seq_data, 2*Inst->seq_count); dp += 2*Inst->seq_count;
			if(Inst->type == 1){
				memcpy(buf+dp, Inst->dpcm_data, 288); dp += 288;
			} else if(Inst->type == 5){
				memcpy(buf+dp, &Inst->wave_size, 4); dp += 4;
				memcpy(buf+dp, &Inst->wave_pos, 4); dp += 4;
				memcpy(buf+dp, &Inst->wave_count, 4); dp += 4;
				//printf("N163 wave size: %i pos: %i count: %i\n", Inst->wave_size, Inst->wave_pos, Inst->wave_count);
				for(j=0;j<Inst->wave_count;j++){
					memcpy(buf+dp, Inst->wave_data[j], Inst->wave_size); dp += Inst->wave_size;
				}
			}
			break;

		default:
			printf("Error: Instrument %i has unknown type: %i\n", i, Inst->type);
			return 0;
		}
		memcpy(buf+dp, &Inst->name_length, 4); dp += 4;
		memcpy(buf+dp, Inst->name, Inst->name_length); dp += Inst->name_length;
	}

	blocksize = dp - 0x18;
	memcpy(buf+0x14, &blocksize, 4);
	fwrite(buf, dp, 1, fp_dst); dp = 0;

	dp = 0x18;
	memcpy(buf, "SEQUENCES\0\0\0\0\0\0\0\x06\0\0\0", 0x14);
	memcpy(buf+dp, &File->n_2a03_seqs, 4); dp += 4;

	count = File->n_2a03_seqs + File->n_vrc6_seqs + File->n_n163_seqs + File->n_s5b_seqs;

	//printf("2a03 sequences (w): %i\n", File->n_2a03_seqs);

	for(i=0;i<count;i++){
		Seq = File->Seq + i;
		if(Seq->expansion != 1) continue;
		memcpy(buf+dp, &Seq->index, 4); dp += 4;
		memcpy(buf+dp, &Seq->type, 4); dp += 4;
		buf[dp++] = Seq->length;
		memcpy(buf+dp, &Seq->loop, 4); dp += 4;
		memcpy(buf+dp, Seq->data, Seq->length); dp += Seq->length;
	}

	for(i=0;i<count;i++){
		Seq = File->Seq + i;
		if(Seq->expansion != 1) continue;
		memcpy(buf+dp, &Seq->release, 4); dp += 4;
		memcpy(buf+dp, &Seq->setting, 4); dp += 4;
	}

	blocksize = dp - 0x18;
	memcpy(buf+0x14, &blocksize, 4);
	fwrite(buf, dp, 1, fp_dst); dp = 0;

	dp = 0x18;
	memcpy(buf, "FRAMES\0\0\0\0\0\0\0\0\0\0\x03\0\0\0", 0x14);

	for(i=0;i<File->n_tracks;i++){
		Track = File->Track + i;
		//printf("Track %i frames: %i\n", i, Track->n_frames);
		memcpy(buf+dp, &Track->n_frames, 4); dp += 4;
		memcpy(buf+dp, &Track->speed, 4); dp += 4;
		memcpy(buf+dp, &Track->tempo, 4); dp += 4;
		memcpy(buf+dp, &Track->p_length, 4); dp += 4;
		for(j=0;j<Track->n_frames;j++){
			//printf("Track %i channels: %i\n", i, Track->n_channels);
			memcpy(buf+dp, Track->fdata[j], Track->n_channels);
			dp += Track->n_channels;
		}
	}

	blocksize = dp - 0x18;
	memcpy(buf+0x14, &blocksize, 4);
	fwrite(buf, dp, 1, fp_dst); dp = 0;

	dp = 0x18;
	memcpy(buf, "PATTERNS\0\0\0\0\0\0\0\0\x05\0\0\0", 0x14);

	for(i=0;i<File->n_patterns;i++){
		Pattern = File->Pattern + i;
		if(Pattern->track >= File->n_tracks) continue;
		if(Pattern->channel >= File->n_channels) continue;
		//printf("Pattern %i items: %i\n", i, Pattern->items);
		memcpy(buf+dp, &Pattern->track, 4); dp += 4;
		memcpy(buf+dp, &Pattern->channel, 4); dp += 4;
		memcpy(buf+dp, &Pattern->index, 4); dp += 4;
		memcpy(buf+dp, &Pattern->items, 4); dp += 4;
		Track = File->Track + Pattern->track;
		//printf("Pattern %i track: %i channel: %i\n", i, Pattern->track, Pattern->channel);
		item_size = 10 + 2 * Track->ch_fx[Pattern->channel];
		for(j=0;j<Pattern->items;j++){
			//printf("wow!\n");
			memcpy(buf+dp, &Pattern->Item[j], item_size);
			dp += item_size;
		}
	}

	//printf("Written %i patterns\n", File->n_patterns);

	blocksize = dp - 0x18;
	memcpy(buf+0x14, &blocksize, 4);
	fwrite(buf, dp, 1, fp_dst); dp = 0;

	dp = 0x18;
	memcpy(buf, "DPCM SAMPLES\0\0\0\0\x01\0\0\0", 0x14);
	buf[dp++] = File->n_samples;

	for(i=0;i<File->n_samples;i++){
		Sample = File->Sample + i;
		buf[dp++] = Sample->index;
		memcpy(buf+dp, &Sample->name_length,  4); dp += 4;
		memcpy(buf+dp, Sample->name, Sample->name_length); dp += Sample->name_length;
		memcpy(buf+dp, &Sample->data_length, 4); dp += 4;
		memcpy(buf+dp, Sample->data, Sample->data_length); dp += Sample->data_length;
	}

	blocksize = dp - 0x18;
	memcpy(buf+0x14, &blocksize, 4);
	fwrite(buf, dp, 1, fp_dst); dp = 0;

	count = File->n_2a03_seqs + File->n_vrc6_seqs + File->n_n163_seqs + File->n_s5b_seqs;

	if(File->expansion & 0x01){
		dp = 0x18;
		memcpy(buf, "SEQUENCES_VRC6\0\0\x06\0\0\0", 0x14);
		memcpy(buf+dp, &File->n_vrc6_seqs, 4); dp += 4;

		for(i=0;i<count;i++){
			Seq = File->Seq + i;
			if(Seq->expansion != 2) continue;
			memcpy(buf+dp, &Seq->index, 4); dp += 4;
			memcpy(buf+dp, &Seq->type, 4); dp += 4;
			buf[dp++] = Seq->length;
			memcpy(buf+dp, &Seq->loop, 4); dp += 4;
			memcpy(buf+dp, Seq->data, Seq->length); dp += Seq->length;
		}

		for(i=0;i<count;i++){
			Seq = File->Seq + i;
			if(Seq->expansion != 2) continue;
			memcpy(buf+dp, &Seq->release, 4); dp += 4;
			memcpy(buf+dp, &Seq->setting, 4); dp += 4;
		}

		blocksize = dp - 0x18;
		memcpy(buf+0x14, &blocksize, 4);
		fwrite(buf, dp, 1, fp_dst); dp = 0;
	}

	if(File->expansion & 0x10){
		dp = 0x18;
		memcpy(buf, "SEQUENCES_N163\0\0\x01\0\0\0", 0x14);
		memcpy(buf+dp, &File->n_n163_seqs, 4); dp += 4;

		for(i=0;i<count;i++){
			Seq = File->Seq + i;
			if(Seq->expansion != 5) continue;
			memcpy(buf+dp, &Seq->index, 4); dp += 4;
			memcpy(buf+dp, &Seq->type, 4); dp += 4;
			buf[dp++] = Seq->length;
			memcpy(buf+dp, &Seq->loop, 4); dp += 4;
			memcpy(buf+dp, &Seq->release, 4); dp += 4;
			memcpy(buf+dp, &Seq->setting, 4); dp += 4;
			memcpy(buf+dp, Seq->data, Seq->length); dp += Seq->length;
		}

		blocksize = dp - 0x18;
		memcpy(buf+0x14, &blocksize, 4);
		fwrite(buf, dp, 1, fp_dst); dp = 0;
	}

	if(File->expansion & 0x20){
		dp = 0x18;
		memcpy(buf, "SEQUENCES_S5B\0\0\0\x01\0\0\0", 0x14);
		memcpy(buf+dp, &File->n_s5b_seqs, 4); dp += 4;

		for(i=0;i<count;i++){
			Seq = File->Seq + i;
			if(Seq->expansion != 6) continue;
			memcpy(buf+dp, &Seq->index, 4); dp += 4;
			memcpy(buf+dp, &Seq->type, 4); dp += 4;
			buf[dp++] = Seq->length;
			memcpy(buf+dp, &Seq->loop, 4); dp += 4;
			memcpy(buf+dp, &Seq->release, 4); dp += 4;
			memcpy(buf+dp, &Seq->setting, 4); dp += 4;
			//printf("S5BSeq %i index: %i type: %i len: %i lp: %i rel: %i s: %i\n", i, Seq->index, Seq->type, Seq->length, Seq->loop, Seq->release, Seq->setting);
			memcpy(buf+dp, Seq->data, Seq->length); dp += Seq->length;
		}

		blocksize = dp - 0x18;
		memcpy(buf+0x14, &blocksize, 4);
		fwrite(buf, dp, 1, fp_dst); dp = 0;
	}

	if(File->n_grooves){
		dp = 0x18;
		memcpy(buf, "GROOVES\0\0\0\0\0\0\0\0\0\x01\0\0\0", 0x14);
		buf[dp++] = File->n_grooves;

		for(i=0;i<File->n_grooves;i++){
			Groove = File->Groove + i;
			buf[dp++] = Groove->index;
			buf[dp++] = Groove->length;
			memcpy(buf+dp, Groove->data, Groove->length); dp += Groove->length;
		}

		buf[dp++] = 1;
		buf[dp++] = 1;

		blocksize = dp - 0x18;
		memcpy(buf+0x14, &blocksize, 4);
		fwrite(buf, dp, 1, fp_dst);
	}

	putc('E', fp_dst);
	putc('N', fp_dst);
	putc('D', fp_dst);

	//fwrite("\x01\x01END", 5, 1, fp_dst);
	fclose(fp_dst);
	free(buf);

	return 1;
}

int FT_ReadParams(FT_File* File, unsigned char* buf, int version, int size)
{
	int dp = 0;

	File->expansion = buf[dp++];
	memcpy(&File->n_channels, &buf[dp], 4); dp += 4;
	memcpy(&File->machine, &buf[dp], 4); dp += 4;
	memcpy(&File->e_speed, &buf[dp], 4); dp += 4;
	memcpy(&File->v_style, &buf[dp], 4); dp += 4;
	memcpy(&File->highlight1, &buf[dp], 4); dp += 4;
	memcpy(&File->highlight2, &buf[dp], 4); dp += 4;
	if(File->expansion&0x10){
		memcpy(&File->n163_channels, &buf[dp], 4);
		dp += 4;
	}
	memcpy(&File->speed_split, &buf[dp], 4); dp += 4;

	return 1;
}

int FT_ReadInfo(FT_File* File, unsigned char* buf, int version, int size)
{
	memcpy(File->title, buf, 32);
	memcpy(File->author, buf+32, 32);
	memcpy(File->copyright, buf+64, 32);

	return 1;
}

int FT_ReadHeader(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Track* Track;
	int i, j;
	int dp = 0;

	File->n_tracks = 1 + buf[dp++];

	File->Track = calloc(File->n_tracks, sizeof(FT_Track));

	for(i=0;i<File->n_tracks;i++){
		Track = File->Track + i;
		Track->n_channels = File->n_channels;

		for(j=0;1;j++){
			if(!(Track->name[j] = buf[dp++])) break;
			Track->name_length++;
		}
	}

	for(i=0;i<File->n_channels;i++){
		File->ch_id[i] = buf[dp++];
		for(j=0;j<File->n_tracks;j++){
			Track = File->Track + j;
			Track->ch_fx[i] = buf[dp++];
		}
	}
	return 1;
}

int FT_ReadInstruments(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Instrument* Inst;
	FT_Sequence* Seq;
	int dp;
	int i, j;

	dp = 0;

	memcpy(&File->n_instruments, buf+dp, 4); dp += 4;

	for(i=0;i<File->n_instruments;i++){
		Inst = &File->Inst[i];
		memcpy(&Inst->index, buf+dp, 4); dp += 4;
		memcpy(&Inst->type, buf+dp, 1); dp += 1;

		switch(Inst->type){
		case 1: // 2A03/MMC5
		case 2: // VRC6
		case 5: // N163
		case 6: // S5B
			memcpy(&Inst->seq_count, buf+dp, 4); dp += 4;
			memcpy(Inst->seq_data, buf+dp, 2*Inst->seq_count); dp += 2*Inst->seq_count;
			if(Inst->type == 1) { // 2A03/MMC5
				memcpy(Inst->dpcm_data, buf+dp, 288);
				dp += 288;
			} else if(Inst->type == 5){ // N163
				memcpy(&Inst->wave_size, &buf[dp], 4); dp += 4;
				memcpy(&Inst->wave_pos, &buf[dp], 4); dp += 4;
				memcpy(&Inst->wave_count, &buf[dp], 4); dp += 4;
				for(j=0;j<Inst->wave_count;j++){
					memcpy(Inst->wave_data[j], &buf[dp], Inst->wave_size);
					dp += Inst->wave_size;
				}
			}
			break;

		case 3: // VRC7
			memcpy(&Inst->vrc7_patch, &buf[dp], 4); dp += 4;
			memcpy(Inst->vrc7_data, &buf[dp], 8); dp += 8;
			break;

		case 4: // FDS
			memcpy(Inst->fds_wave, &buf[dp], 64); dp += 64;
			memcpy(Inst->fds_mtable, &buf[dp], 32); dp += 32;
			memcpy(&Inst->fds_mspeed, &buf[dp], 4); dp += 4;
			memcpy(&Inst->fds_mdepth, &buf[dp], 4); dp += 4;
			memcpy(&Inst->fds_mdelay, &buf[dp], 4); dp += 4;
			for(j=0;j<3;j++){
				Seq = Inst->Seq + j;
				Seq->length = buf[dp++];
				memcpy(&Seq->loop, &buf[dp], 4); dp += 4;
				memcpy(&Seq->release, &buf[dp], 4); dp += 4;
				memcpy(&Seq->setting, &buf[dp], 4); dp += 4;
				memcpy(Seq->data, &buf[dp], Seq->length); dp += Seq->length;
			}
			break;

		default:
			printf("Error: Instrument %i has unknown type: %i\n", i, Inst->type);
			return 0;
		}

		memcpy(&Inst->name_length, buf+dp, 4); dp += 4;
		memcpy(Inst->name, buf+dp, Inst->name_length); dp += Inst->name_length;

		//printf("Instrument %i name: %s\n", i, Inst->name);
	}

	return 1;
}

int FT_ReadSequences(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Sequence* Seq;
	int i;
	int dp = 0;

	memcpy(&File->n_2a03_seqs, &buf[dp], 4); dp += 4;

	printf("2a03 sequences (r): %i\n", File->n_2a03_seqs);

	for(i=0;i<File->n_2a03_seqs;i++){
		Seq = File->Seq + i;
		memcpy(&Seq->index, &buf[dp], 4); dp += 4;
		memcpy(&Seq->type, &buf[dp], 4); dp += 4;
		Seq->length = buf[dp++];
		memcpy(&Seq->loop, &buf[dp], 4); dp += 4;
		memcpy(Seq->data, &buf[dp], Seq->length); dp += Seq->length;
	}

	for(i=0;i<File->n_2a03_seqs;i++){
		Seq = File->Seq + i;
		memcpy(&Seq->release, &buf[dp], 4); dp += 4;
		memcpy(&Seq->setting, &buf[dp], 4); dp += 4;
		Seq->expansion = 1;
	}
	return 1;
}

int FT_ReadFrames(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Track* Track;
	int i, j;
	int dp = 0;

	for(i=0;i<File->n_tracks;i++){
		Track = File->Track + i;
		memcpy(&Track->n_frames, &buf[dp], 4); dp += 4;
		memcpy(&Track->speed, &buf[dp], 4); dp += 4;
		memcpy(&Track->tempo, &buf[dp], 4); dp += 4;
		memcpy(&Track->p_length, &buf[dp], 4); dp += 4;
		for(j=0;j<Track->n_frames;j++){
			memcpy(Track->fdata[j], &buf[dp], Track->n_channels);
			dp += Track->n_channels;
		}
	}

	return 1;
}

int FT_ReadPatterns(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Pattern* Pattern;
	FT_Track* Track;
	int n_patterns, item_size;
	int trk, ch, items, psize;
	int i, j;
	int dp = 0;

	for(i=0;dp<size;i++){
		memcpy(&trk, &buf[dp], 4); dp += 4;
		memcpy(&ch, &buf[dp], 4); dp += 8;
		memcpy(&items, &buf[dp], 4); dp += 4;
		Track = File->Track + trk;
		//printf("Pattern %i track: %i channel: %i items: %i item_size: %i\n", i, trk, ch, items, 10 + 2 * Track->ch_fx[ch]);
		psize = items * (10 + 2 * Track->ch_fx[ch]);
		dp += psize;
	}

	File->n_patterns = n_patterns = i;
	printf("Number of patterns: %i\n", n_patterns);
	File->Pattern = calloc(n_patterns, sizeof(FT_Pattern));
	dp = 0;

	for(i=0;i<n_patterns;i++){
		Pattern = File->Pattern + i;
		memcpy(&Pattern->track, &buf[dp], 4); dp += 4;
		memcpy(&Pattern->channel, &buf[dp], 4); dp += 4;
		memcpy(&Pattern->index, &buf[dp], 4); dp += 4;
		memcpy(&Pattern->items, &buf[dp], 4); dp += 4;
		//printf("Pattern %i track: %i channel: %i index: %i items: %i\n", i, Pattern->track, Pattern->channel, Pattern->index, Pattern->items);
		Track = File->Track + Pattern->track;
		item_size = 10 + 2 * Track->ch_fx[Pattern->channel];
		for(j=0;j<Pattern->items;j++){
			memcpy(&Pattern->Item[j], &buf[dp], item_size);
			//printf("Pattern %i item %i row: %i note: %i octave: %i inst: %i vol: %i fx: %i-%02X\n", i, j, Pattern->Item[j].row, Pattern->Item[j].note, Pattern->Item[j].octave, Pattern->Item[j].instrument, Pattern->Item[j].volume, Pattern->Item[j].fxdata[0], Pattern->Item[j].fxdata[1]);
			dp += item_size;
		}
	}
	return 1;
}

int FT_ReadSamples(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Sample* Sample;
	int i;
	int dp = 0;

	File->n_samples = buf[dp++];

	for(i=0;i<File->n_samples;i++){
		Sample = File->Sample + i;
		Sample->index = buf[dp++];
		memcpy(&Sample->name_length, &buf[dp], 4); dp += 4;
		memcpy(Sample->name, &buf[dp], Sample->name_length); dp += Sample->name_length;
		memcpy(&Sample->data_length, &buf[dp], 4); dp += 4;
		memcpy(Sample->data, &buf[dp], Sample->data_length); dp += Sample->data_length;
	}
	return 1;
}

int FT_ReadVRC6Sequences(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Sequence* FirstSeq;
	FT_Sequence* Seq;
	int i;
	int dp = 0;

	FirstSeq = File->Seq + File->n_2a03_seqs;

	memcpy(&File->n_vrc6_seqs, &buf[dp], 4); dp += 4;

	for(i=0;i<File->n_vrc6_seqs;i++){
		Seq = FirstSeq + i;
		memcpy(&Seq->index, &buf[dp], 4); dp += 4;
		memcpy(&Seq->type, &buf[dp], 4); dp += 4;
		Seq->length = buf[dp++];
		memcpy(&Seq->loop, &buf[dp], 4); dp += 4;
		memcpy(Seq->data, &buf[dp], Seq->length); dp += Seq->length;
	}

	for(i=0;i<File->n_vrc6_seqs;i++){
		Seq = FirstSeq + i;
		memcpy(&Seq->release, &buf[dp], 4); dp += 4;
		memcpy(&Seq->setting, &buf[dp], 4); dp += 4;
		Seq->expansion = 2;
	}
	return 1;
}

int FT_ReadN163Sequences(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Sequence* FirstSeq;
	FT_Sequence* Seq;
	int i;
	int dp = 0;

	FirstSeq = File->Seq + File->n_2a03_seqs + File->n_vrc6_seqs;

	memcpy(&File->n_n163_seqs, &buf[dp], 4); dp += 4;

	for(i=0;i<File->n_n163_seqs;i++){
		Seq = FirstSeq + i;
		memcpy(&Seq->index, &buf[dp], 4); dp += 4;
		memcpy(&Seq->type, &buf[dp], 4); dp += 4;
		Seq->length = buf[dp++];
		memcpy(&Seq->loop, &buf[dp], 4); dp += 4;
		memcpy(&Seq->release, &buf[dp], 4); dp += 4;
		memcpy(&Seq->setting, &buf[dp], 4); dp += 4;
		memcpy(Seq->data, &buf[dp], Seq->length); dp += Seq->length;
		Seq->expansion = 5;
	}
	return 1;
}

int FT_ReadS5BSequences(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Sequence* FirstSeq;
	FT_Sequence* Seq;
	int i;
	int dp = 0;

	FirstSeq = File->Seq + File->n_2a03_seqs + File->n_vrc6_seqs + File->n_n163_seqs;

	memcpy(&File->n_s5b_seqs, &buf[dp], 4); dp += 4;

	for(i=0;i<File->n_s5b_seqs;i++){
		Seq = FirstSeq + i;
		memcpy(&Seq->index, &buf[dp], 4); dp += 4;
		memcpy(&Seq->type, &buf[dp], 4); dp += 4;
		Seq->length = buf[dp++];
		memcpy(&Seq->loop, &buf[dp], 4); dp += 4;
		memcpy(&Seq->release, &buf[dp], 4); dp += 4;
		memcpy(&Seq->setting, &buf[dp], 4); dp += 4;
		memcpy(Seq->data, &buf[dp], Seq->length); dp += Seq->length;
		Seq->expansion = 6;
	}
	return 1;
}

int FT_ReadGrooves(FT_File* File, unsigned char* buf, int version, int size)
{
	FT_Groove* Groove;
	int i;
	int dp = 0;

	File->n_grooves = buf[dp++];

	for(i=0;i<File->n_grooves;i++){
		Groove = File->Groove + i;
		Groove->index = buf[dp++];
		Groove->length = buf[dp++];
		memcpy(Groove->data, &buf[dp], Groove->length); dp += Groove->length;
	}

	return 1;
}

