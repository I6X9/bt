// oad_convert.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "string.h"
#include "malloc.h"
#include "types.h"

#define INFO_LEN 16 
#define SWAP_ENDIAN16(Value) ((((Value) & 0x00FF) << 8) | \
                              (((Value) & 0xFF00) >> 8))

#define SWAP_ENDIAN32(Value) ((((Value) & 0x000000FF) << 24) | \
                              (((Value) & 0x0000FF00) << 8) | \
                              (((Value) & 0x00FF0000) >> 8) | \
                              (((Value) & 0xFF000000) >> 24))


uint32_t crc32_table[256];
uint8 image_flag[6] = {'B','K','3','2','6','6'};
static uint8_t oadhdr1[] = {0xff, 0xff, 0xff, 0xff, 0x30, 0x66, 0xfd, 0xfe, 0x53, 0x53, 0x53, 0x53, 0xff, 0xff, 0x05, 0x00};
static uint8_t oadhdr2[] = {0xff, 0xff, 0xff, 0xff, 0x31, 0x66, 0xfd, 0xff, 0x53, 0x53, 0x53, 0x53, 0xff, 0xff, 0x05, 0x00};

int make_crc32_table(void)
{
    uint32_t c;
    int i = 0;
    int bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (uint32_t)i;
        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c>>1)^(0xEDB88320);
            }
            else
            {
                c = c >> 1;
            }
        }
        crc32_table[i] = c;
    }
    return 0;
}

uint32_t make_crc32(uint32_t crc,unsigned char *string,uint32_t size)
{
    while(size--)
    {
        crc = (crc >> 8)^(crc32_table[(crc^*string++)&0xff]);
    }
    return crc;
}

int main(int argc, char *argv[])
{
    int i;
    int binfiles = argc - 1;
    
    for(i = 1; i <= binfiles; i++)
    {
        int  len;
        char name[128];
        
        FILE* fi;
        FILE* fo;

        sprintf(name, "%s", argv[i]);
        len = strlen(argv[i]);
        len = len > 4 ? len - 4 : len;
        sprintf(name + len, "_ota.bin");

        fi = fopen(argv[i], "rb");
        fo = fopen(name,    "wb");
		printf("%s,%s\r\n",argv[i],name);
        if(fi && fo)
        {
            uint8 * fbuf;
            int   flen;
            
            fseek(fi, 0, SEEK_END);
            flen = ftell(fi);
            fseek(fi, 0, SEEK_SET);
			
			if(flen > 0x100000)
		    {
		        flen -= 0x100000;
				fseek(fi, 0x100000, SEEK_SET);
		    }
			
			flen = 0xfffc*16;
            fbuf = (uint8 *)malloc(INFO_LEN + flen);
            memset(&fbuf[0], 0xFF, flen + INFO_LEN);
			
            if(fbuf)
            {
                fread(fbuf + INFO_LEN, 1, flen, fi);
                
                if(!memcmp(&fbuf[INFO_LEN + 0x110], image_flag, 6))
                {
                    flen -= 4096;
                    memcpy(fbuf, oadhdr1, INFO_LEN);
                    memmove(fbuf + INFO_LEN, fbuf + INFO_LEN + 4096, flen);
					fbuf[INFO_LEN+0x20] = 0xFF;
					fbuf[INFO_LEN+0x21] = 0x5A;
                }
                else
                {
					fbuf[INFO_LEN+0x20] = 0xFF;
					fbuf[INFO_LEN+0x21] = 0xA5;
                    memcpy(fbuf, oadhdr2, INFO_LEN);
                }

                // copy crc 
                make_crc32_table();
                *(uint32_t*)fbuf = make_crc32(0xFFFFFFFF, fbuf + INFO_LEN, flen);

                // write data back to file
                fwrite(fbuf, 1, flen + INFO_LEN, fo);

                free(fbuf);
            }
        }
        
        if(fi) fclose(fi);
        if(fo) fclose(fo);
    }
    
    return 0;
}

