#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FT_VERSIONS 51
#define OCC_VERSIONS 8
#define FT_3_0BETA2 13
#define FT_4_6 49

//#include "ft_version.h"
//#include "0cc_version.h"

#include "misc_utils.h"
#include "0ccftm.h"
#include "0cc_decompiler.h"
#include "ft_decompiler.h"
//#include "file_list.h"

// Various 0CC modules are transposed


enum {
	DRV_ERROR,
	DRV_FT,
	DRV_0CC,
	DRV_UNKNOWN,
};

extern int version_list[];
extern short old_driver_table[];
extern short ft_driver_table[][6];
extern unsigned char ft_setd_table[];
extern unsigned char ft_resd_table[];
extern int occ_driver_table[][8];

static int drv_version;

int validate_ftm(char* filename)
{
	FT_File* File;
	File = Create_FTFile();
	if(!FT_LoadFile(File, filename)) return 0;
	return 1;
}

int detect_ft_driver(unsigned char* buf, int src_size)
{
	NSF_Header h;
	int driver_size;
	int t_NTSC;
	int t_PAL;
	int t_index;
	int dp;
	int adj;
	int bbbb;
	int i;

	memcpy(&h, buf, 0x80);

	/*printf("Expansion: 0x%02X\n", h.expansion);
	printf("load_addr: 0x%04X\n", h.load_addr);
	printf("init_addr: 0x%04X\n", h.init_addr);
	printf("predicted size: 0x%04X\n", 0xC000-h.init_addr);*/

	switch(h.expansion){
		case 0: t_index = 0; break;
		case 1: t_index = 1; break;
		case 2: t_index = 2; break;
		case 4: t_index = 3; break;
		case 8: t_index = 4; break;
		case 16: t_index = 5; break;
		default: return -1;
	}

	//if(h.n_songs == 1) return -1;

	if(h.init_addr==0x8000 || h.init_addr==0x8008){
		for(i=0;i<FT_VERSIONS;i++){
			adj = 8;
			if(i >= FT_3_0BETA2) adj++;
			if(h.expansion == 0x4) adj += 2;

			dp = 0x80 + adj + ft_driver_table[i][t_index];
			t_NTSC = buf[dp] + 256*buf[dp+1]; dp+=2;
			t_PAL = buf[dp] + 256*buf[dp+1]; dp+=2;
			driver_size = ft_driver_table[i][t_index];
			if((t_NTSC==0xE10 && t_PAL==0xBB8) || (t_NTSC>0xB00 && t_NTSC==t_PAL && t_NTSC%257 && !(t_NTSC%60))){
				drv_version = i;
				return i;
			}
		}
	}

	driver_size = 0xC000 - h.init_addr;
	for(i=0;i<FT_VERSIONS;i++){
		if(driver_size != ft_driver_table[i][t_index]) continue;

		if(driver_size == 3584){
			dp = 0xB32 + h.init_addr - h.load_addr;
			bbbb = buf[dp] + 256*buf[dp+1]; dp+=2;
			if(bbbb==0xA940 || bbbb==0x4008 || bbbb==0xAD1E) i++;
		}

		if(driver_size == 4288){
			dp = 0x84 + h.init_addr - h.load_addr;
			bbbb = buf[dp++];
			if(bbbb == 0x86) i++;
		}

		if(i == FT_4_6){
			dp = 0x80 + 0x27C + h.init_addr - h.load_addr;
			printf("[%02X %02X %02X %02X]\n", buf[dp], buf[dp+1], buf[dp+2], buf[dp+3]);
			if(memcmp(buf+dp, "\x29\xF0\xC9\xF0", 4)) continue;
		}

		drv_version = i;
		return i;
	}

	return -1;
}

int detect_0cc_driver(unsigned char* buf, int src_size)
{
	NSF_Header h;
	unsigned char* ptr;
	int t_index;
	int driver_size;
	int i;

	memcpy(&h, buf, 0x80);

	/*printf("Expansion: 0x%02X\n", h.expansion);
	printf("load_addr: 0x%04X\n", h.load_addr);
	printf("init_addr: 0x%04X\n", h.init_addr);
	printf("predicted size: 0x%04X\n", 0xC000-h.init_addr);*/

	switch(h.expansion){
		case 0: t_index = 0; break;
		case 1: t_index = 1; break;
		case 2: t_index = 2; break;
		case 4: t_index = 3; break;
		case 8: t_index = 4; break;
		case 16: t_index = 5; break;
		case 32: t_index = 6; break;
		default: t_index = 7; break;
	}

	//if(h.n_songs == 1) return -1;

	if(h.load_addr == 0x8000){
		ptr = memmem(buf+0x80, src_size-0x80, "\x00\x0C\x18\x25\x30\x3C\x47\x51\x5A\x62\x6A\x70\x76\x7A\x7D\x7F", 16);
		if(!ptr){
			//printf("Error: couldn't detect 0CC-FT driver signature\n");
			return -1;
		}
		//pp_data = 18 + ptr - buf;
		driver_size = ptr - buf - 110;
	} else {
		//pp_data = 0x80;
		driver_size = 0xC000-h.init_addr;
	}

	for(i=0;i<OCC_VERSIONS;i++){
		if(driver_size != occ_driver_table[i][t_index]) continue;
		drv_version = i;
		return i;
	}

	return -1;
}

int detect_driver(char* filename)
{
	unsigned char* buf;
	int src_size;

	if(!(buf = file2mem(filename))) return DRV_ERROR;
	src_size = getfsize(filename);

	if(detect_ft_driver(buf, src_size) != -1){
		//printf("Detected Famitracker!\n");
		//printf("Version: %i\n", drv_version);
		return DRV_FT;
	} else if(detect_0cc_driver(buf, src_size) != -1){
		printf("Detected 0CC-Famitracker!\n");
		printf("Version: %i\n", drv_version);
		return DRV_0CC;
	}

	free(buf);

	return DRV_UNKNOWN;
}

int main(int argc, char* argv[])
{
	char ftmname[64];
	char* nsfname;
	OCC_Decompiler* OCCDec;
	FT_Decompiler* FTDec;
	int driver_type;
	int i;

	if(argc == 2){
		nsfname = argv[1];
		strcpy(ftmname, nsfname);
		ftmname[strlen(ftmname)-4] = 0;

		//printf("%s - %s\n", ftmname, nsfname);

		driver_type = detect_driver(nsfname);
		switch(driver_type){
		case DRV_FT:
			if(!(FTDec = Create_FTDecompiler())) goto main_error;
			FTDec->version = drv_version;
			//printf("Version: %i\n", FTDec->version);
			//strcat(ftmname, _ft_version_list[drv_version]);
			strcat(ftmname, ".ftm");
			if(!FT_LoadNSF(FTDec, nsfname, drv_version)) goto main_error;
			if(!FT_ExportFTM(FTDec, ftmname)) goto main_error;
			printf("Created %s\n", ftmname);
			Free_FTDecompiler(FTDec);
			break;

		case DRV_0CC:
			if(!(OCCDec = Create_OCCDecompiler())) goto main_error;
			OCCDec->version = drv_version;
			//strcat(ftmname, _occ_version_list[drv_version]);
			strcat(ftmname, ".ftm");
			if(!OCC_LoadNSF(OCCDec, nsfname, drv_version)) goto main_error;
			if(!OCC_ExportFTM(OCCDec, ftmname)) goto main_error;
			printf("Created %s\n", ftmname);
			break;

		default:
			printf("Unknown driver type! (%i)\n", driver_type);
			break;
		}
	} else {
		printf("Usage: NSFDecompiler.exe [filename]\n");
	}


	//validate_ftm("NSF/n163multi.ftm");

	if(argc == 2) printf("Success! ^o^\n");
	system("pause");

	return 1;

main_error:

	printf("Fatal error! :(\n");
	system("pause");
	return 0;
}
