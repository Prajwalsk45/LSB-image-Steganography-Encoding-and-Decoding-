#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>

Status open_file(DecodeInfo *decInfo)
{
    

    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status valid(char* substring,char* mainstring)   //function to check string it present at end or not
{
    int flag=0;
    int j=0;
    int k=0;
    for(int i=0;mainstring[i] != '\0' ;i++)
    {
        flag=0;
        j=0;
        for(int k=i;mainstring[k] != '\0';k++){
            if(substring[j]==mainstring[k]){
                j++;
                flag=1;
            }
            else
            {
                flag=0;
                break;
            }
        }
        if (substring[j] == '\0')
        {
            return e_success;
        }
        
    }

    return e_failure;
}

Status validate_no_dot(char* mainstring)
{
    for(int i=0;mainstring[i] != '\0' ;i++)
    {
        if(mainstring[i]=='.')
        {
            printf("dont give complete file name\n");
            return e_failure;
        }
    }
    return e_success;
}

Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo)
{
    if(argc < 3 && argc > 4)
    {
        return e_failure;
    }

    //.bmp 
    if((valid(".bmp",argv[2]))==e_success)
    {
        decInfo->stego_image_fname = malloc(strlen(argv[2]) + 1);           
        strcpy(decInfo->stego_image_fname, argv[2]);
        
    }
    else
    {
        return e_failure;
    }

    //secret data file
    if(argv[3]==NULL)
    {
        decInfo->secret_fname = malloc(20);
        strcpy(decInfo->secret_fname,"output");
    }
    else 
    {
        if(validate_no_dot(argv[3])==e_success)
        {
            decInfo->secret_fname = malloc(20);
            strcpy(decInfo->secret_fname,argv[3]);
        }
        else{
            return e_failure;
        }
    }

    return e_success;
}

Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image,54, SEEK_SET);
    if(ftell(fptr_stego_image)==54)
    {
        return e_success;
    }

    return e_failure;
}

char decode_lsb_to_byte(char *image_buffer)
{
    char ch=0;
    for(int i=0;i<8;i++)
    {
        char get_bit=image_buffer[i] & 1;
        get_bit=(get_bit<<(7-i));
        ch=ch|get_bit;
    }

    return ch;
}

Status decode_image_to_data(char *data,int size,FILE *fptr_stego_image)
{
    char buffer[8];
    int i=0;
    // printf("%d\n",size);
    for(i=0;i<size;i++)
    {
        fread(buffer,1,8,fptr_stego_image);
        data[i]=decode_lsb_to_byte(buffer);
        
        
    }

    return e_success;
    
}

int decode_lsb_to_size(char *image_buffer)
{
    int n=0;
    for(int i=0;i<32;i++)
    {
        char get_bit=image_buffer[i] & 1;
        get_bit=(get_bit<<(31-i));
        n=n|get_bit;
    }

    return n;

}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    int size=strlen(magic_string);

    char get_magic_string[size+1];
    get_magic_string[size] = '\0';            //a string length is determined by the null character ('\0'), not by how many characters it contains.
    // printf("%ld\n",strlen(get_magic_string));
    
    decode_image_to_data(get_magic_string,size,decInfo->fptr_stego_image);
    // printf("%s\n",get_magic_string);

    if((strcmp(get_magic_string,magic_string)) == 0)
    {
        return e_success;
    }

    return e_failure;
}

Status decode_secret_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,1,32,decInfo->fptr_stego_image);
    decInfo->extn_size=decode_lsb_to_size(buffer);
    //printf("%ld\n",decInfo->extn_size);
    return e_success;
}

Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo)
{
    long size=strlen(file_extn);
    

    decode_image_to_data(file_extn,size,decInfo->fptr_stego_image);

    strcat(decInfo->secret_fname,file_extn);
    // printf("%s\n",file_extn);
    // printf("%s\n",decInfo->secret_fname);
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    
    if(decInfo->fptr_secret == NULL){
        return e_failure;
    }
    else
    {
        printf("secret file opened\n");
    }
    return e_success;

}


Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,1,32,decInfo->fptr_stego_image);
    decInfo->size_secret_file=decode_lsb_to_size(buffer);
    //printf("%ld\n",decInfo->size_secret_file);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char final_data[decInfo->size_secret_file];
    // printf("%s\n",final_data);
    // printf("%ld\n",strlen(final_data));
    decode_image_to_data(final_data,decInfo->size_secret_file,decInfo->fptr_stego_image);
    // printf("%s\n",final_data);
    fwrite(final_data,1,decInfo->size_secret_file,decInfo->fptr_secret);
    
    return e_success;

}






Status do_decoding(DecodeInfo *decInfo)
{
    if(open_file(decInfo) == e_success)
    {
        printf("open file is success\n");
    }
    else
    {
        printf("Open file is failed\n");
        return e_failure;
    }



    if((skip_bmp_header(decInfo->fptr_stego_image))==e_success)
    {
        printf("header skiped successfully\n");
    }
    else
    {
        printf("failed to skip header\n");
        return e_failure;
    }

    

    if((decode_magic_string("#*",decInfo)) == e_success)
    {
        printf("magic string decoded\n");
    }
    else
    {
        printf("failed to read magic string\n");
        return e_failure;
    }

    

    
    if((decode_secret_extn_size(decInfo)) == e_success)
    {
        printf("secret file ext size read sucessfully\n");
    }

    

    //printf("%ld\n",decInfo->extn_size);
    char extension_secret_file[decInfo->extn_size+1];
    extension_secret_file[decInfo->extn_size]='\0';
    if((decode_secret_file_extn(extension_secret_file,decInfo)) == e_success)
    {
        printf("extension file name done\n");
    }

    

    if((decode_secret_file_size(decInfo)) == e_success)
    {
        printf("secret file size done\n");
    }
    
    
    if((decode_secret_file_data(decInfo)) == e_success)
    {
        printf("secret message successfully stored in file\n");
    }

    // printf("%ld\n",ftell(decInfo->fptr_secret));
    // printf("%ld\n",ftell(decInfo->fptr_stego_image));

    

}

