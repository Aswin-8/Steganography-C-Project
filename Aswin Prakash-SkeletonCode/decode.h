#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding stego file to decode file
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

/* Get file extention size by decoding 32 bytes */
#define DECODE_FILE_EXTN_SIZE 32

#define PASSCODE_SIZE (strlen(MAGIC_STRING))

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    int image_data_size;
    char image_data[MAX_IMAGE_BUF_SIZE];

     

    /* Decode File Info */
    char *decode_fname;
    FILE *fptr_decode_image;
    char *decode_image_fname;
    char decode_data[MAX_IMAGE_BUF_SIZE];
    int decode_file_size;
    int decode_file_extn_size;

    /* MAGIC STRING */
    char *passcode;

} DecodeInfo; 


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo); 

/* Get File pointer for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Performs Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get the magic string */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_image_to_data( int size,DecodeInfo *decInfo);

/* Decode a array of LSB into byte */
Status decode_byte_from_lsb(char * image_buffer);

/* Decode file extention size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode size from lsb */
Status decode_size_from_lsb(DecodeInfo *decInfo);

/* Decode file extenstion */
Status decode_secret_file_extn(int extn_size, DecodeInfo *decInfo);

/* Decode file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decoding file data */
Status decode_data_to_file(DecodeInfo *decInfo);

#endif 