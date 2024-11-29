#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Stego Image file and decode file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_decode(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
    	return e_failure;
    }

        // Decode Image file
    decInfo->fptr_decode_image = fopen(decInfo->decode_image_fname, "w");
    // Do Error handling
    if (decInfo->fptr_decode_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_image_fname);
    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *encInfo)
{
    if((strstr(argv[2], ".bmp"))!=NULL)         //checking if there is a bmp file
        encInfo-> stego_image_fname = argv[2];         //storing the bmp file in stego_image_fname
    else
        return e_failure;
    if(argv[3]!=NULL)
        encInfo ->decode_image_fname =argv[3];                  //checking if there is a 3rd argument in CLA, if so adding in it
    else
        encInfo -> decode_image_fname= "decode.txt";            //if not storing a .txt file in decode_image_fname
    return e_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    
    decode_image_to_data(PASSCODE_SIZE, decInfo);                   //calling the decode_image_to_data length of magic string times
    // No failure return e_success
    return e_success;
}

Status decode_image_to_data( int size,DecodeInfo *decInfo)
{
    int i=0;
    for(i=0;i<size;i++)
    {
        fread(decInfo->image_data,8,1,decInfo->fptr_stego_image);                //reading 8 bytes of data from stego.bmp
        decInfo->decode_data[i] =decode_byte_from_lsb(decInfo->image_data);      //calling the decode_byte_from_lsb
    } 
    decInfo->decode_data[i]='\0'; 
}

Status decode_byte_from_lsb(char *image_data)
{
    char decoded_bytes=0;
    for(int i=0;i<MAX_IMAGE_BUF_SIZE;i++)
    {
        decoded_bytes |=  (image_data[i] & 0x01) <<i;                            //storing the lsb of each byte in decode_bytes
    } 
    return decoded_bytes;
} 

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    decode_size_from_lsb(decInfo);                                        //calling decode_size_from_lsb function
    return e_success;
}

 Status decode_secret_file_extn(int extn_size, DecodeInfo *decInfo)
{
    decInfo->decode_file_extn_size = decode_image_to_data(decInfo->image_data_size,decInfo);    //storing the file extension size after decoding in decode_file_extn_size
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    decInfo->decode_file_size= decode_size_from_lsb(decInfo);              //storing the file  size after decoding in decode_file_size
    return e_success;
} 

Status decode_size_from_lsb(DecodeInfo *decInfo)
{
    
    char file_size[32];
    fread(file_size,32,1,decInfo->fptr_stego_image);
    decInfo->image_data_size=0;
    for(int i=0;i<MAX_IMAGE_BUF_SIZE;i++)
    {
        decInfo->image_data_size |=  (file_size[i] & 0x01) <<i;               //storing the lsb of each byte in image_data_size
    } 
    return decInfo->image_data_size;
}

Status decode_data_to_file(DecodeInfo *decInfo)
{
    int file_size=decInfo->decode_file_size;
    decode_image_to_data(decInfo->decode_file_size,decInfo);   
    fwrite(decInfo->decode_data,file_size,1,decInfo->fptr_decode_image);            //writing the decoded characters to decode.txt file
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_files_decode(decInfo)==e_success)                             //checking the opening the file is a success
    {
        printf("Open file is a success\n");
    }
    else
    {
        printf("Failed to open files successfully\n");
        return e_failure;
    }

    fseek(decInfo->fptr_stego_image,54,SEEK_SET);                        //making the stego file pointer point toward 54th position

    if(decode_magic_string(MAGIC_STRING,decInfo)==e_success)                      //checking the magic string function  is a success
    {
        printf("decoded magic string successfully\n");
        if(strcmp(MAGIC_STRING,decInfo->decode_data)==0)                     //checking the magic string is a match or not
        printf("decoded magic string is matching\n");
        else
        {
            printf("decoded magic string is not matching\n");
            return e_failure;
        }

    }
    else
    {
        printf("Failed to decode magic string \n");
        return e_failure;
    }
    
    if(decode_secret_file_extn_size(decInfo)==e_success)                  
    {
        printf("Decoded secret file extension size successfully\n");
    }
    else
    {
        printf("Failed to decode secret file extension size \n");
        return e_failure;
    }

    char str[10];
    strcpy(str, strchr(decInfo->decode_image_fname,'.'));                            //storing the extension in str

    if(decode_secret_file_extn(decInfo->image_data_size,decInfo)==e_success)
    {
        printf("Decoded secret file extension successfully\n");
        if(strcmp(str,decInfo->decode_data)==0)                                      //checking if the extension is matching with user given file
        {
            printf("File Extensions are matching\n");
        }
        else
        {
            printf("File Extensions are not matching\n");
            return e_failure;
        }
    }
    else
    {
        printf("Failed to decode secret file extension \n");
        return e_failure;
    }

    if(decode_secret_file_size(decInfo)==e_success)
    {
        printf("Decoded secret file size successfully\n");
    }
    else
    {
        printf("Failed to decode secret file size \n");
        return e_failure;
    }

    if(decode_data_to_file(decInfo)==e_success)
    {
        printf("Decoded secret file data successfully\n");
    }
    else
    {
        printf("Failed to decode secret file data \n");
        return e_failure;
    }

    return e_success;
}