#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc_utils.h"
#include "ft_stuff.h"
#include "ft_version.h"
#include "0ccftm.h"
#include "ft_decompiler.h"

FT_Decompiler* Create_FTDecompiler()
{
	FT_Decompiler* Dec;
	Dec = calloc(1, sizeof(FT_Decompiler));
	Dec->dpcm_inst = -1;
	return Dec;
}

void Free_FTDecompiler(FT_Decompiler* Dec)
{
	int i;
	for(i=0;i<Dec->n_songs;i++) free(Dec->Song[i].Pattern);
	free(Dec->Song);
	free(Dec);
}

int FT_LoadNSF(FT_Decompiler* Dec, char* filename, int version)
{
	NSF_Header h;
	NSF_Song* Song;
	NSF_Pattern* Pattern;
	NSF_Item* Item;
	NSF_Frame* Frame;
	NSF_Instrument* Inst;
	NSF_Sequence* Seq;
	NSF_Entry* Entry;
	//NSF_Groove* Groove;
	NSF_Sample* Sample;
	unsigned char* ptr;
	//unsigned char* music;
	unsigned char* buf;
	int* pp_pattern;
	int src_size;
	int pp_data, pp_song, pp_inst, pp_entry, pp_sample, pp_fds, flags;
	//int pp_groove;
	int pp_dpcm;
	int div_ntsc;
	int offset;
	int channels;
	int bank;
	int jmax;
	int last, index;
	int t_index;
	int ch;
	int i, j, k, l;
	//unsigned char byte_in;

	printf("Version: %s\n", ft_version_list[version]);
	Dec->version = version;

	src_size = getfsize(filename);
	if(!(buf = file2mem(filename))) return 0;
	memcpy(&h, buf, 0x80);

	switch(h.expansion){
		case 0: t_index = 0; break;
		case 1: t_index = 1; break;
		case 2: t_index = 2; break;
		case 4: t_index = 3; break;
		case 8: t_index = 4; break;
		case 16: t_index = 5; break;
		//case 32: t_index = 6; break;
		default: return 0;
	}

	//printf("Detected Famitracker version: %s (%i)\n", ft_version_list[Dec->version], Dec->version);
	//printf("t_index: %i\n", t_index);

	if(h.load_addr == 0x8000){
		pp_data = 0x80 + ft_driver_table[Dec->version][t_index];
	} else {
		pp_data = 0x80;
	}

	ch = 5; for(i=0;i<5;i++) {Dec->ch_type[i] = 1; Dec->ch_id[i] = i;}

	if(h.expansion & 0x01) for(i=0;i<3;i++,ch++) {Dec->ch_type[ch] = 2; Dec->ch_id[ch] = 0x05+i;}
	if(h.expansion & 0x02) for(i=0;i<6;i++,ch++) {Dec->ch_type[ch] = 3; Dec->ch_id[ch] = 0x14+i;}
	if(h.expansion & 0x04) {Dec->ch_type[ch++] = 4; Dec->ch_id[ch] = 0x13;}
	if(h.expansion & 0x08) for(i=0;i<2;i++,ch++) {Dec->ch_type[ch] = 1; Dec->ch_id[ch] = 0x08+i;}
	if(h.expansion & 0x10) for(i=0;i<8;i++,ch++) {Dec->ch_type[ch] = 5; Dec->ch_id[ch] = 0x0B+i;}
	//if(h.expansion & 0x20) for(i=0;i<3;i++,ch++) {Dec->ch_type[ch] = 6; Dec->ch_id[ch] = 0x1A+i;}

	//printf("pp_data: 0x%04X\n", pp_data);

	ptr = buf + pp_data;

	pp_song = pp_data + ptr[0] + 256*ptr[1];
	pp_inst = pp_data + ptr[2] + 256*ptr[3];
	pp_entry = pp_data + ptr[4] + 256*ptr[5];
	pp_sample = pp_data + ptr[6] + 256*ptr[7];
	if(Dec->version >= FT_3_0BETA2) flags = ptr[8];
	pp_fds = pp_data + ptr[9] + 256*ptr[10];
	pp_dpcm = 0x80 + 0xC000 - h.load_addr;

	ptr += 8;
	ptr += (Dec->version >= FT_3_0BETA2)? 1: 0;
	ptr += (h.expansion & 0x04)? 2: 0;
	div_ntsc = ptr[0] + 256 * ptr[1];
	Dec->e_speed = (div_ntsc != 0xE10)? div_ntsc/60: 0;

	if(h.expansion & 0x10){
		ch -= 8;
		ch += ptr[4];
		Dec->n163_channels = ptr[4];
		//printf("%02X %02X [%02X] %02X %02X\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4]);
	}

	Dec->n_channels = channels = ch;

	//printf("Channels: %i\n", Dec->n_channels);

	//printf("pp_inst: 0x%04X\n", pp_inst);
	//printf("pp_entry: 0x%04X\n", pp_entry);
	//printf("pp_sample: 0x%04X\n", pp_sample);
	//printf("pp_song: 0x%04X\n", pp_song);
	//printf("pp_fds: 0x%04X\n", pp_fds);

	Dec->Song = calloc(h.n_songs, sizeof(NSF_Song));
	Dec->n_songs = h.n_songs;
	Dec->expansion = h.expansion;

	for(i=0;i<h.n_songs;i++){
		Song = Dec->Song + i;

		//printf("Decompiling song %i\n", i);

		memcpy(Song->ch_id, Dec->ch_id, 32);

		ptr = buf + pp_song + 2*i;
		Song->offset = pp_data + ptr[0] + 256*ptr[1];
		ptr = buf + Song->offset;
		Song->pp_frame = pp_data + ptr[0] + 256*ptr[1];
		Song->n_frames = ptr[2];
		Song->p_length = ptr[3];
		Song->speed = ptr[4];
		Song->tempo = ptr[5];
		Song->bank = ptr[6];

		if(flags & 1){
			bank = Song->bank;
			if(bank >= 3) bank -= 3;
			Song->pp_frame += 4096 * bank;
		}

		if(!Song->p_length) Song->p_length = 256;
		pp_pattern = calloc(Song->n_frames * channels, 4);

		//printf("Song %i offset: 0x%04X\n", i, Song->offset);
		//printf("Song %i # of frames: %i pp_frame: 0x%04X\n", i, Song->n_frames, Song->pp_frame);

		for(j=0;j<Song->n_frames;j++){
			//printf("j: %i\n", j);
			Frame = Song->Frame + j;
			ptr = buf + Song->pp_frame + 2*j;
			Frame->offset = pp_data + ptr[0] + 256*ptr[1];
			//printf("Song %i frame %i offset: 0x%04X\n", i, j, Frame->offset);

			if(flags & 1){
				bank = Song->bank;
				if(bank >= 3) bank -= 3;
				Frame->offset += 4096 * bank;
			}

			ptr = buf + Frame->offset;
			for(k=0;k<channels;k++){
				Frame->pp_pattern[k] = pp_data + ptr[0] + 256*ptr[1] + (k << 24);
				ptr += 2;
			}
			if(flags&1){
				for(k=0;k<channels;k++){
					bank = *(ptr++);
					if(bank >= 3) bank -= 3;
					Frame->pp_pattern[k] += 4096 * bank;
				}
			}
			memcpy(pp_pattern + j * channels, Frame->pp_pattern, 4 * channels);
		}

		//printf("Pass 1\n");

		jmax = Song->n_frames * channels;

		qsort(pp_pattern, jmax, sizeof(int), cmp_int);

		last = 0;

		for(j=0;j<jmax;j++){
			if(pp_pattern[j] != last){
				Song->n_patterns++;
			}
			last = pp_pattern[j];
		}

		//printf("Song %i # of patterns: %i\n", i, Song->n_patterns);

		Song->Pattern = calloc(Song->n_patterns, sizeof(NSF_Pattern));
		Pattern = Song->Pattern;

		for(j=0;j<jmax;j++){
			if(pp_pattern[j] != last){
				Pattern->offset = pp_pattern[j] & 0xFFFFFF;
				Pattern->channel = pp_pattern[j] >> 24;
				Pattern++;
			}
			last = pp_pattern[j];
		}

		free(pp_pattern);

		for(j=0;j<channels;j++){
			index = 0;
			for(k=0;k<Song->n_patterns;k++){
				Pattern = Song->Pattern + k;
				if(Pattern->channel != j) continue;
				Pattern->index = index;
				index++;
			}
		}

		for(j=0;j<Song->n_frames;j++){
			Frame = Song->Frame + j;
			for(k=0;k<channels;k++){
				offset = Frame->pp_pattern[k] & 0xFFFFFF;
				for(l=0;l<Song->n_patterns;l++){
					Pattern = Song->Pattern + l;
					if(Pattern->channel != k) continue;
					if(Pattern->offset != offset) continue;
					ch = k;
					if(ch == channels-1){
						ch = 4;
					} else if(ch >= 4){
						ch += 1;
					}
					Frame->data[ch] = Pattern->index;
					break;
				}
			}
		}

		/*for(j=0;j<Song->n_frames;j++){
			Frame = Song->Frame + j;
			printf("%02X:", j);
			for(k=0;k<channels;k++){
				printf(" %02X", Frame->data[k]);
			}
			printf("\n");
		}*/

		for(j=0;j<Song->n_patterns;j++){
			Pattern = Song->Pattern + j;
			Pattern->rows = Song->p_length;
			ch = Pattern->channel;

			if(ch == channels-1){
				Pattern->channel = 4;
			} else if(ch >= 4){
				Pattern->channel += 1;
			}

			//if(Pattern->offset == pp_data) continue;

			//if(j<200) printf("Decoding pattern %i at offset: 0x%04X\n", j, Pattern->offset);

			if(!FT_DecompilePattern(Dec, Pattern, buf + Pattern->offset)){
				printf("Error decoding pattern %i channel: %i index: %i offset: 0x%04X\n", j, Pattern->channel, Pattern->index, Pattern->offset);
				return 0;
			}

			//printf("channel: %i\n", Pattern->channel);
			t_index = Dec->ch_type[Pattern->channel];
			//printf("t_index: %i\n", t_index);

			for(k=0;k<Pattern->items;k++){
				Item = Pattern->Item + k;
				if(Item->instrument == 64) continue;
				Dec->Inst[Item->instrument].type_matches[t_index]++;
				if(Item->instrument >= Dec->n_instruments) Dec->n_instruments = Item->instrument+1;
			}
		}

		for(j=0;j<channels;j++){
			for(k=0;k<Song->n_patterns;k++){
				Pattern = Song->Pattern + k;
				if(Pattern->channel != j) continue;
				if(Pattern->ch_fx > Song->ch_fx[j]) Song->ch_fx[j] = Pattern->ch_fx;
			}
			//printf("ch_id: %i ch_fx: %i\n", Song->ch_id[j], Song->ch_fx[j]);
		}
	}

	for(i=0;i<Dec->n_instruments;i++){
		Inst = Dec->Inst + i;
		Seq = Dec->Sequence + Dec->n_sequences;
		ptr = buf + pp_inst + 2*i;
		Inst->offset = pp_data + ptr[0] + 256*ptr[1]; ptr += 2;
		ptr = buf + Inst->offset;

		k = 0;

		for(j=1;j<=6;j++){
			if(Inst->type_matches[j] <= k) continue;
			k = Inst->type_matches[j];
			Inst->type = j;
		}

		if(!Inst->type) Inst->type = 1;
		//printf("Instrument %i offset: 0x%04X type: %i\n", i, Inst->offset, Inst->type);
		if(Dec->dpcm_inst == -1 && Inst->type == 1) Dec->dpcm_inst = i;

		switch(Inst->type){
		case 1: case 2: case 6:
			Inst->modswitch = *(ptr++);
			for(j=0;j<5;j++){
				if(!(Inst->modswitch & (1<<j))){
					Inst->pp_sequence[j] = 0;
					continue;
				}
				Inst->pp_sequence[j] = pp_data + ptr[0] + 256*ptr[1]; ptr += 2;
				Inst->seq[j] = Dec->n_sequences;
				Seq->offset = Inst->pp_sequence[j];
				Seq->index = i;
				Seq->type = j;
				Seq->expansion = Inst->type;
				//printf("New seq! type: %i ex: %i\n", Seq->type, Seq->expansion);
				Dec->n_sequences++;
				Seq++;
			}
			break;

		case 3:
			Inst->vrc7_patch = *(ptr++) >> 4;
			if(!Inst->vrc7_patch) memcpy(Inst->vrc7_data, ptr, 8);
			break;

		case 4:
			Inst->fds_index = *(ptr++);
			memcpy(Inst->fds_mtable, ptr, 16); ptr += 16;
			Inst->fds_delay = *(ptr++);
			Inst->fds_depth = *(ptr++);
			Inst->fds_speed = ptr[0] + 256*ptr[1]; ptr += 2;
			Inst->fds_modswitch = *(ptr++);
			for(j=0;j<3;j++){
				if(!(Inst->fds_modswitch & (1<<j))){
					Inst->pp_sequence[j] = 0;
					continue;
				}
				Inst->pp_sequence[j] = pp_data + ptr[0] + 256*ptr[1]; ptr += 2;
				Inst->seq[j] = Dec->n_sequences;
				Seq->offset = Inst->pp_sequence[j];
				Seq->index = i;
				Seq->type = j;
				Dec->n_sequences++;
				Seq++;
			}
			ptr = buf + pp_fds + 64 * Inst->fds_index;
			memcpy(Inst->fds_wave, ptr, 64);
			break;

		case 5:
			Inst->wave_count = 1;
			Inst->wave_size = *(ptr++);
			Inst->wave_pos = *(ptr++);
			Inst->wave_offset = pp_data + ptr[0] + 256*ptr[1]; ptr += 2;
			Inst->modswitch = *(ptr++);
			for(j=0;j<5;j++){
				if(!(Inst->modswitch & (1<<j))){
					Inst->pp_sequence[j] = 0;
					continue;
				}
				Inst->pp_sequence[j] = pp_data + ptr[0] + 256*ptr[1]; ptr += 2;
				Inst->seq[j] = Dec->n_sequences;
				Seq->offset = Inst->pp_sequence[j];
				Seq->index = i;
				Seq->type = j;
				Seq->expansion = 5;
				Dec->n_sequences++;
				Seq++;
			}
			break;

		default:
			printf("Error: Instrument %i has unknown type: %i\n", i, Inst->type);
			return 0;
		}
	}

	for(i=0;i<Dec->n_sequences;i++){
		Seq = Dec->Sequence + i;
		ptr = buf + Seq->offset;
		Seq->length = *(ptr++);
		Seq->loop = *(ptr++);
		Seq->release = (Dec->version >= FT_3_1BETA5)? *(ptr++): 0;
		Seq->setting = (Dec->version >= FT_3_8BETA2)? *(ptr++): 0;
		//printf("Sequence %i offset: 0x%04X type: %i ex: %i len: %i lp: %i: r: %i s: %i\n", i, Seq->offset, Seq->type, Seq->expansion, Seq->length, Seq->loop, Seq->release, Seq->setting);
		memcpy(Seq->data, ptr, Seq->length);
		Seq->release--;
		if(Seq->release == -1) Seq->release = 0xFF;
	}

	for(i=0;i<Dec->n_instruments;i++){
		Inst = Dec->Inst + i;
		/*for(j=0;j<5;j++){
			if(!Inst->pp_sequence[j]) continue;
			for(k=0;k<Dec->n_sequences;k++){
				if(Inst->pp_sequence[j] != Dec->Sequence[k].offset) continue;
				Inst->seq[j] = k;
				break;
			}
		}*/

		if(Inst->type == 5){
			Inst->wave_count = 16;
			ptr = buf + Inst->wave_offset;
			for(k=0;k<Inst->wave_count;k++){
				memcpy(Inst->wave_data[k], ptr, Inst->wave_size);
				ptr += Inst->wave_size;
			}
		}
	}

	Dec->n_entries = (pp_sample - pp_entry) / ((Dec->version >= FT_4_2BETA)? 3: 2);
	Dec->n_samples = (pp_song - pp_sample) / ((Dec->version >= FT_3_8BETA2)? 3: 2);

	ptr = buf + pp_entry;

	for(i=0;i<Dec->n_entries;i++){
		Entry = Dec->Entry + i;
		Entry->pitch = *(ptr++);
		if(Dec->version >= FT_4_2BETA) Entry->earrape = *(ptr++);
		Entry->index = *(ptr++) / ((Dec->version >= FT_3_8BETA2)? 3: 2);
	}

	//printf("pp_sample: 0x%04X\n", pp_sample);

	for(i=0;i<Dec->n_samples;i++){
		ptr = buf + pp_sample + 3 * i;
		Sample = Dec->Sample + i;
		Sample->address = *(ptr++);
		Sample->size = *(ptr++);
		Sample->bank = (Dec->version >= FT_3_8BETA2)? *(ptr++): 0;
		if(Sample->bank){
			Sample->offset = 4096*Sample->bank + 64*Sample->address + 128;
		} else {
			Sample->offset = pp_dpcm + 64*Sample->address;
		}
		if(Sample->offset < 0 || Sample->offset > src_size){
			printf("Error: Sample %i has invalid offset: 0x%04X (bank: %i addr: %i)\n", i, Sample->offset, Sample->bank, Sample->address);
			return 0;
		}
		ptr = buf + Sample->offset;
		memcpy(Sample->data, ptr, 16*Sample->size);
	}

	for(i=0;i<Dec->n_instruments;i++){
		if(Dec->dpcm_inst != -1) break;
		if(Dec->Inst[i].type) continue;
		Dec->Inst[i].type = 1;
		Dec->dpcm_inst = i;
		break;
	}

	for(i=0;i<Dec->n_songs;i++){
		Song = Dec->Song + i;
		for(j=0;j<Song->n_patterns;j++){
			Pattern = Song->Pattern + j;
			if(Pattern->channel != 4) continue;
			for(k=0;k<Pattern->items;k++){
				if(!Pattern->Item[k].note) continue;
				Pattern->Item[k].instrument = Dec->dpcm_inst;
			}
		}
	}

	free(buf);

	return 1;
}

int FT_DecompilePattern(FT_Decompiler* Dec, NSF_Pattern* Pattern, unsigned char* ptr)
{
	NSF_Item Item = {0};
	NSF_Item Empty = {0};
	//unsigned char* buf;
	int row = 0;
	int items = 0;
	int inst = 64;
	int new_fx = 0;
	//int ch_fx = 0;
	int fixed = 0;
	int duration = 1;
	int setd_byte = ft_setd_table[Dec->version];
	int resd_byte = ft_resd_table[Dec->version];
	int version = Dec->version;
	//int i;
	unsigned char byte_in, t_fxtype, t_fxvalue;

	//buf = ptr;
	Empty.instrument = 64;
	Empty.volume = 16;
	Item = Empty;

	while(row < Pattern->rows){
		byte_in = *(ptr++);

		switch(byte_in >> 4){
		case 0x0: case 0x1: case 0x2: case 0x3: case 0x4: case 0x5: case 0x6: case 0x7:
			Item.row = row;
			if(byte_in){
				Item.note = 1+(byte_in-1)%12;
				Item.octave = (byte_in-1)/12;
				Item.instrument = inst;
			}
			if(byte_in > 0x70){
				Item.note = 13;
				Item.octave = 0;
				Item.instrument = 64;
				Item.volume = 16;
				if(byte_in==0x7F) Item.note = 14;
			}

			Pattern->Item[items++] = Item;
			Item = Empty;
			//printf("old row: %i\n", row);
			if(fixed) row += 1 + duration;
			else row += 1 + *(ptr++);
			//printf("new row: %i\n", row);
			new_fx = 0;
			break;

		case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD:
			if(byte_in == setd_byte){
				duration = *(ptr++);
				fixed = 1;
				break;
			} else if(byte_in == resd_byte){
				fixed = 0;
				break;
			}

			if(version >= FT_4_3BETA1B && version < FT_4_3){
				if(byte_in>=0x86) byte_in -= 2;
			}

			//printf("byte_in: 0x%02X items: %i row: %i\n", byte_in, items, row);

			t_fxtype = version >= FT_4_3? convert_new_cmd(byte_in): convert_cmd(byte_in);
			switch(t_fxtype)
			{
			case 99: break;
			case 100:
				t_fxvalue = *(ptr++);
				inst = (t_fxvalue/2)&63;
				break;
			case 102: break;
			case 103:
				printf("Error: unrecognized command! (0x%02X)\n", byte_in);
				printf("items: %i row: %i\n", items, row);
				return 0;

			case 104: break;
			case 110: // EF_CLEAR
				new_fx += 1;
				if(new_fx > Pattern->ch_fx + 1){
					if(new_fx > 4){printf("Error! Too many fx found in one cell. :(\n"); return 0;}
					Pattern->ch_fx = new_fx-1;
				}
				Item.fxdata[2*new_fx-2] = EF_PORTA_DOWN;
				Item.fxdata[2*new_fx-1] = 0;
				break;

			case 111: // EF_RESET_PITCH
				new_fx += 1;
				if(new_fx > Pattern->ch_fx + 1){
					if(new_fx > 4){printf("Error! Too many fx found in one cell. :(\n"); return 0;}
					Pattern->ch_fx = new_fx-1;
				}
				Item.fxdata[2*new_fx-2] = EF_PITCH;
				Item.fxdata[2*new_fx-1] = 0x80;
				break;

			default:
				t_fxvalue = *(ptr++);
				new_fx += 1;
				if(new_fx > Pattern->ch_fx + 1){
					if(new_fx > 4){printf("Error! Too many fx found in one cell. :(\n"); return 0;}
					Pattern->ch_fx = new_fx-1;
				}

				switch(t_fxtype){
				case EF_SKIP: case EF_NOTE_CUT: case EF_JUMP:
					//if(t_fxtype == EF_SKIP) printf("Skip!!!\n");
					//if(t_fxtype == EF_JUMP) printf("Jump!!!\n");
					t_fxvalue--; break;

				case 101:
					if(!(t_fxvalue&0x80)) break;
					t_fxtype = (t_fxvalue & 0x08)? EF_SWEEPUP: EF_SWEEPDOWN;
					t_fxvalue = t_fxvalue & 0x77;
					break;

				case EF_VIBRATO: case EF_TREMOLO:
					t_fxvalue = ((t_fxvalue << 4) & 0xF0) | ((t_fxvalue >> 4) & 0x0F);
					break;

				default:
					break;
				}
				Item.fxdata[2*new_fx-2] = t_fxtype;
				Item.fxdata[2*new_fx-1] = t_fxvalue;
				break;
			}

			break;

		case 0xE:
			inst = byte_in-0xE0;
			break;

		case 0xF:
			Item.volume = byte_in-0xF0;
			break;
		}
	}

	Pattern->items = items;

	return 1;
}

int FT_ExportFTM(FT_Decompiler* Dec, char* filename)
{
	FT_File* File;
	FT_Track* Track;
	NSF_Song* Song;
	NSF_Pattern* PSrc;
	FT_Pattern* PDst;
	NSF_Frame* Frame;
	NSF_Instrument* ISrc;
	FT_Instrument* IDst;
	NSF_Sequence* SSrc;
	FT_Sequence* SDst;
	//NSF_Entry* Entry;
	//NSF_Sample* Sample;
	int mult, sub, idx;
	int i, j, k;
	int count;

	if(!(File = Create_FTFile())) return 0;

	count = 0;
	for(i=0;i<Dec->n_songs;i++){
		count += Dec->Song[i].n_patterns;
	}

	File->Track = calloc(Dec->n_songs, sizeof(FT_Track));
	File->Pattern = calloc(count, sizeof(FT_Pattern));
	File->n_patterns = count;

	//File->version = 0x440;
	File->expansion = Dec->expansion;
	File->n_channels = Dec->n_channels;
	File->machine = 0;
	File->e_speed = Dec->e_speed;
	File->v_style = 1;
	File->highlight1 = 4;
	File->highlight2 = 16;
	File->n163_channels = Dec->n163_channels;
	File->speed_split = 0x20;

	File->n_tracks = Dec->n_songs;
	File->n_instruments = Dec->n_instruments;
	File->n_samples = Dec->n_samples;

	memcpy(File->ch_id, Dec->ch_id, 32);

	count = 0;

	for(i=0;i<Dec->n_songs;i++){
		Song = Dec->Song + i;
		Track = File->Track + i;
		Track->n_channels = Dec->n_channels;
		Track->n_frames = Song->n_frames;
		Track->speed = Song->speed;
		Track->tempo = Song->tempo;
		Track->p_length = Song->p_length;
		sprintf(Track->name, "New song");
		Track->name_length = strlen(Track->name);
		for(j=0;j<Song->n_frames;j++){
			Frame = Song->Frame + j;
			memcpy(Track->fdata[j], Frame->data, Track->n_channels);
		}

		for(j=0;j<Song->n_patterns;j++,count++){
			PSrc = Song->Pattern + j;
			PDst = File->Pattern + count;

			PDst->track = i;
			PDst->channel = PSrc->channel;
			PDst->index = PSrc->index;
			PDst->items = PSrc->items;
			memcpy(PDst->Item, PSrc->Item, 16*PSrc->items);
		}

		memcpy(Track->ch_fx, Song->ch_fx, 32);
	}

	count = 0;

	for(i=0;i<Dec->n_instruments;i++){
		ISrc = Dec->Inst + i;
		IDst = File->Inst + i;
		if(ISrc->type == 1 || ISrc->type == 2 || ISrc->type == 5 || ISrc->type == 6){
			IDst->type = ISrc->type;
			for(j=0;j<5;j++){
				if(!ISrc->pp_sequence[j]) continue;
				SSrc = Dec->Sequence + ISrc->seq[j];
				SDst = File->Seq + count;
				IDst->seq_data[2*j] = 1;
				IDst->seq_data[2*j+1] = SSrc->index;
				SDst->index = SSrc->index;
				SDst->type = SSrc->type;
				//printf("Inst: %i seq: %i index: %i type: %i\n", i, ISrc->seq[j], SSrc->index, SDst->type);
				SDst->length = SSrc->length;
				memcpy(SDst->data, SSrc->data, SSrc->length);
				SDst->loop = SSrc->loop;
				if(SDst->loop == 0xFF) SDst->loop = -1;
				SDst->release = SSrc->release;
				if(SDst->release == 0xFF) SDst->release = -1;
				SDst->setting = SSrc->setting;
				SDst->expansion = SSrc->expansion;
				//printf("Seq expansion: %i\n", SDst->expansion);
				switch(SDst->expansion){
					case 1: File->n_2a03_seqs++; break;
					case 2: File->n_vrc6_seqs++; break;
					case 5: File->n_n163_seqs++; break;
					case 6: File->n_s5b_seqs++; break;
					default: printf("wtf!!!\n"); return 0;
				}
				count++;
			}
			IDst->wave_size = ISrc->wave_size * 2;
			IDst->wave_pos = ISrc->wave_pos;
			IDst->wave_count = ISrc->wave_count;
			for(j=0;j<IDst->wave_count;j++){
				for(k=0;k<IDst->wave_size;k++){
					IDst->wave_data[j][2*k] = ISrc->wave_data[j][k] & 15;
					IDst->wave_data[j][2*k+1] = ISrc->wave_data[j][k] >> 4;
				}
			}
		} else if(ISrc->type == 3){
			IDst->type = 3;
			IDst->vrc7_patch = ISrc->vrc7_patch;
			memcpy(IDst->vrc7_data, ISrc->vrc7_data, 8);
		} else if(ISrc->type == 4){
			IDst->type = 4;
			memcpy(IDst->fds_wave, ISrc->fds_wave, 64);
			for(j=0;j<16;j++){
				IDst->fds_mtable[2*j] = ISrc->fds_mtable[j]&7;
				IDst->fds_mtable[2*j+1] = ISrc->fds_mtable[j]>>3;
			}
			for(j=0;j<3;j++){
				if(!(ISrc->fds_modswitch & (1<<j))) continue;
				SSrc = Dec->Sequence + ISrc->seq[j];
				SDst = IDst->Seq + j;
				SDst->length = SSrc->length;
				memcpy(SDst->data, SSrc->data, SSrc->length);
				SDst->loop = SSrc->loop;
				if(SDst->loop == 0xFF) SDst->loop = -1;
				SDst->release = SSrc->release;
				if(SDst->release == 0xFF) SDst->release = -1;
				SDst->setting = SSrc->setting;
				SDst->expansion = SSrc->expansion;
			}
			IDst->fds_mdelay = ISrc->fds_delay;
			IDst->fds_mdepth = ISrc->fds_depth;
			IDst->fds_mspeed = ISrc->fds_speed;;
		}

		IDst->index = i;
		IDst->seq_count = 5;
		IDst->name_length = 14;
		memcpy(IDst->name, "New instrument\0", 15);
	}

	IDst = File->Inst + Dec->dpcm_inst;

	for(i=0;i<Dec->n_entries;i++){
		mult = (Dec->version >= FT_4_2BETA)? 3: 2;
		sub = (Dec->version <= FT_3_8BETA2)? 2: 0;
		if(Dec->version >= FT_4_3BETA1B){
			mult=1; sub=1;
		}
		idx = 3*(mult + i*mult - sub);
		if(idx < 0) continue;
		IDst->dpcm_data[idx] = Dec->Entry[i].index + 1;
		IDst->dpcm_data[idx+1] = Dec->Entry[i].pitch;
		IDst->dpcm_data[idx+2] = 0xFF;
	}

	for(i=0;i<Dec->n_samples;i++){
		File->Sample[i].index = i;
		File->Sample[i].name_length = 0xE;
		sprintf(File->Sample[i].name, "New instrument");
		File->Sample[i].data_length = 16*Dec->Sample[i].size;
		memcpy(File->Sample[i].data, Dec->Sample[i].data, 16*Dec->Sample[i].size);
	}

	File->n_grooves = 0;

	//printf("n_2a03_seqs: %i\n", File->n_2a03_seqs);
	//printf("n_s5b_seqs: %i\n", File->n_s5b_seqs);

	if(!FT_SaveFile(File, filename)) return 0;
	Free_FTFile(File);
	return 1;
}
