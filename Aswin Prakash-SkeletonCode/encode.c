#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if((strstr(argv[2], ".bmp"))!=NULL)
        encInfo-> src_image_fname = argv[2];                       //storing the source file name in src_image_fname
    else
            return e_failure;
    if((strstr(argv[3], ".txt"))!=NULL)
        encInfo-> secret_fname = argv[3];                         //storing the secret file name in secret_fname
    else
            return e_failure;
    if(argv[4]!=NULL)
        encInfo ->stego_image_fname =argv[4];                     //storing the stego file name in stego_image__fname
    else
        encInfo -> stego_image_fname= "stego.bmp";
    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo-> image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);      //getting the size of bmp file
    encInfo-> size_secret_file=get_file_size(encInfo->fptr_secret);                   //getting the size of bmp file
    if(encInfo->image_capacity >(16+32+32+32+(encInfo->size_secret_file*8)))        //checking whether the source image has greater size than requirement 
    {
        return e_success;
    }
    else
        return e_failure;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);                       //making the file pointer point at the beginning
    return ftell(fptr);
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image,0,SEEK_SET);
    char str[50];
    fread(str,54,1,fptr_src_image);                  //reading 54 bytes from beautiful.bmp
    fwrite(str,54,1,fptr_dest_image);                //writing 54 bytes to stego.bmp
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string,strlen(magic_string), encInfo);
    return e_success;
}

Status encode_data_to_image(char *data, int size,EncodeInfo *encInfo)
{
    int i;
    for(i=0;i<size;i++)
    {
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);            //reading 8 bytes from beautiful.bmp
        encode_byte_to_lsb(data[i],encInfo->image_data);                   //calling the encode_bye_to_lsb function
        fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);         //writing the 8 bytes to stego.bmp
    }
}

Status encode_byte_to_lsb(char data,char * image_buffer)
{
    int i;
    for(i=0;i<8;i++)
    {
        image_buffer[i]=(image_buffer[i] & 0xFE) | ((data>>i) & 1);         //toggling lsb of image buffer and storing lsb of data
        
    }
} 



Status encode_secret_file_extn_size(int size,EncodeInfo *encInfo)
{
    encode_size_to_lsb(size,encInfo);
    return e_success;
}

Status encode_size_to_lsb(int size,EncodeInfo *encinfo)
{
    int i;
    char str[32];
    fread(str,32,1,encinfo->fptr_src_image);
     for(i=0;i<32;i++)
    {
        str[i]=(str[i] & 0xFE) | ((size>>i) & 1);
    }
    fwrite(str,32,1,encinfo->fptr_stego_image);              //writing 32 bytes to stego.bmp
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo);
    return e_success;
}

Status encode_secret_file_size(int file_size,EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo->size_secret_file];
    fseek(encInfo->fptr_secret,0,SEEK_SET);                       //making the file pointer point to start
    fread(str,encInfo->size_secret_file,1,encInfo->fptr_secret);
    encode_data_to_image(str,encInfo->size_secret_file,encInfo);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src) >0 )
    {
        fwrite(&ch,1,1,fptr_dest);                  //writing all remaing data to stego.bmp
    }
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("Open file is a success\n");
    }
    else
    {
        printf("Failed to open files successfully\n");
        return e_failure;
    }
    if(check_capacity(encInfo)==e_success)
    {
        printf("Check capacity is a success\n");
    }
    else
    {
        printf("Check capacity is a failure\n");
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("Copied bmp header successfully\n");
    }
    else
    {
        printf("Failed to copy bmp header \n");
        return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
        printf("Encoded magic string successfully\n");
    }
    else
    {
        printf("Failed to encode magic string \n");
        return e_failure;
    }
    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname,'.'));
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success)
    {
        printf("Encoded secret file extension size successfully\n");
    }
    else
    {
        printf("Failed to encode secret file extension size \n");
        return e_failure;
    }

    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
    {
        printf("Encoded secret file extension successfully\n");
    }
    else
    {
        printf("Failed to encode secret file extension \n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
    {
        printf("Encoded secret file size successfully\n");
    }
    else
    {
        printf("Failed to encode secret file size \n");
        return e_failure;
    }
    
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("Encoded secret file data successfully\n");
    }
    else
    {
        printf("Failed to encode secret file data \n");
        return e_failure;
    }
    
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        printf("Copyed remaining data successfully\n");
    }
    else
    {
        printf("Failed to copy remaining data \n");
        return e_failure;
    }

    return e_success;
}


