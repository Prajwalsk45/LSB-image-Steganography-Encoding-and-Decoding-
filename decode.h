#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 **decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    

    /* Secret File Info */
    char *secret_fname;             //secret.txt
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
    long extn_size;

    /* Stego Image Info */
    char *stego_image_fname;        //stego.bmp
    FILE *fptr_stego_image;

} DecodeInfo;


/* decoding function prototype */

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_file(DecodeInfo *decInfo);


/* skip bmp image header */ 
Status skip_bmp_header(FILE *fptr_stego_image);

/* decode Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

Status decode_secret_extn_size(DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* decode function, which does the real decoding */
Status decode_image_to_data(char *data,int size,FILE *fptr_stego_image);

/* decode a byte into LSB of image data array */
char decode_lsb_to_byte(char *image_buffer);


int decode_lsb_to_size(char *image_buffer);

#endif
