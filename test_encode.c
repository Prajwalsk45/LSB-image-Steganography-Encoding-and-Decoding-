/*
Name        : Prajwal S K  
Date        : 23-12-2025
Project     : LSB Image Steganography (Encoding & Decoding)

Description : This project implements LSB (Least Significant Bit) based Steganography
              to securely hide and retrieve confidential data inside a BMP image.

              Features:
              1. Encodes any text/script file (.txt /.c /.cpp / .py etc..) inside a BMP image.
              2. Uses LSB bit-level encoding without affecting the visible image quality.
              3. Supports decoding to extract the hidden secret data from the stego image.
              4. Ensures data integrity by using a Magic String based validation.
              5. Capacity check is performed before encoding to avoid data overflow.
              6. Includes informative logs, argument validation and error handling.

              Components:
              ▪ Encoding  — Hides secret data into the image.
              ▪ Decoding  — Recovers the hidden data back from the image.
              ▪ Custom CLI interface supporting:
                    -e  for encoding operation
                    -d  for decoding operation

              Output:
              Generates a new BMP file (stego image) with encoded data during encoding
              and restores the original secret file during decoding.
*/


#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>


int main(int argc,char* argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    //1st to check encode or decode
    int res=check_operation_type(argv);
    
    if(res == e_encode)
    {
        printf("Encoding\n");
        if((read_and_validate_encode_args(argc,argv,&encInfo)) == e_success)
        {
            printf("read and vaildate argumnet is success\n");
            do_encoding(&encInfo);   
            printf("INFO : successfully encoded the data\n ");  
        }
        else
        {
            printf("read and vaildate argrumnet is failed\n");
            return e_failure;
        }

    }

    else if (res == e_decode)
    {
        printf("Decoding\n");
        if((read_and_validate_decode_args(argc,argv,&decInfo)) == e_success)
        {
            printf("read and vaildate argumnet is success\n");
            do_decoding(&decInfo);
            printf("INFO : successfully decoded the data ,stored in %s file\n ",decInfo.secret_fname);
        }
        else
        {
            printf("read and vaildate argrumnet is failed\n");
            return e_failure;
        }
    }
    else
    {
        printf("unsupported mode\n");
    }

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    /*
    s1: check the argv[1] is "-e" or not
        yes-> return e_encode
    s2: check the argv[1] is "-d" or not
        yes-> return e_decode
    else : return e_unsupport
    

    */
    if(strcmp(argv[1],"-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return  e_unsupported;
    }
}
