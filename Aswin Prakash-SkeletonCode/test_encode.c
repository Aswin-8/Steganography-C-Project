#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>
int main(int argc, char *argv[])
{
    if(check_operation_type(argv)==e_encode)
    {
        printf("Selected encoding\n");
        EncodeInfo encode;
        if((read_and_validate_encode_args(argv, &encode))==e_success)
        {
            printf("Read and validate encode argument is a success\n");
            printf("<------Started encoding------>\n");
            if(do_encoding(&encode)==e_success)
            {
                printf("Encoding is successful \n");
            }
            else 
            {
                printf("Encoding is failure \n");
                return -2;
            }
        }
        else
        {
            printf("Read and validate encode argument is a failure\n");
            return -1;
        }
    }
    else if(check_operation_type(argv)== e_decode)
    {
        printf("Selected decoding\n");
        DecodeInfo decode;
        if((read_and_validate_decode_args(argv, &decode))==e_success)
        {
            printf("Read and validate encode argument is a success\n");
            printf("<------Started decoding------>\n");
             if(do_decoding(&decode)==e_success)
            {
                printf("Decoding is successful \n");
            }
            else 
            {
                printf("Decoding is failure \n");
                return -2;
            }
        }
        else
        {
            printf("Read and validate encode argument is a failure\n");
            return -1;
        }
    }
    else
    {
        printf("Invalid input\n");
        printf("For encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For decoding : ./a.out -d stego.bmp [decode.txt]\n");
    }
    
    return 0;
}
OperationType check_operation_type(char *argv[])
{
    if((strcmp(argv[1],"-e"))==0)
        return e_encode;    
    else if((strcmp(argv[1],"-d"))==0)
        return e_decode;    
    else
        return e_unsupported;
}
