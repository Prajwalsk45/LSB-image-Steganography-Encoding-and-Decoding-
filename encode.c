#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>

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

Status validate(char* substring,char* mainstring)   //function to check string it present at end or not
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

Status validatesecrate(char* mainstring,EncodeInfo *encInfo)      //valiadte '.' and store the extension
{
    int flag=0;
    int i=0;
    for(i=0;mainstring[i] != '\0' ;i++)
    {
        if(mainstring[i]=='.')
        {
            flag=1;
            break;
        }
    }

    if(flag==1){
        int k=0;
        for(int j=i;mainstring[j] != '\0';j++)
        {
            encInfo->extn_secret_file[k++]=mainstring[j];
        }
        encInfo->extn_secret_file[k]='\0';
        
        return e_success;
    }
    return e_failure;
}

Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo)
{

    if(argc < 4 && argc > 5)
    {
        return e_failure;
    }
    
    

    //to check .bmp and update structure src_image_fname
    if((validate(".bmp",argv[2]))==e_success)
    {
        encInfo->src_image_fname = malloc(strlen(argv[2]) + 1);           //since encInfo->src_image_fname is pointer , i am allocating memory to store beautiful.bmp
        strcpy(encInfo->src_image_fname, argv[2]);
        
    }
    else
    {
        return e_failure;
    }
    

    //check . for secreate file and store it in encInfo-> secret_fname
    if(validatesecrate(argv[3],encInfo)==e_success)
    {
        encInfo->secret_fname = malloc(strlen(argv[3]) + 1);
        strcpy(encInfo-> secret_fname,argv[3]);
        //strcpy(encInfo->extn_secret_file,".txt");
    }
    else
    {
        return e_failure;
    }

    //argv[4]
    if(argv[4]==NULL)
    {
        encInfo->stego_image_fname = malloc(strlen(argv[2]) + 1);
        strcpy(encInfo->stego_image_fname,"stego.bmp");
    }
    else 
    {
        if(validate(".bmp",argv[4])==e_success)
        {
            encInfo->stego_image_fname = malloc(strlen(argv[4]) + 1);
            strcpy(encInfo->stego_image_fname,argv[4]);
        }
        else{
            return e_failure;
        }
    }

    return e_success;

}



uint get_file_size(FILE *fptr)
{
    //fseek and ftell
    fseek(fptr,0, SEEK_END);

    uint len=ftell(fptr);
    
    //printf("%d\n",len);

    return len;

}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo-> size_secret_file =get_file_size(encInfo->fptr_secret);
    //check comparison

    uint res=(((encInfo-> size_secret_file)+(2)+(sizeof(int))+(4)+(sizeof(int)))*8)+54;

    if(encInfo->image_capacity > res)
    {
        return e_success;
    }

    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    if(fptr_src_image == NULL || fptr_dest_image == NULL )
    {
        printf("falied to open file\n");
        return e_failure;
    }
    rewind(fptr_src_image);                //set  file pointer at begining

    fread(buffer,1,54,fptr_src_image);
    fwrite(buffer,1,54,fptr_dest_image);

    if(ftell(fptr_src_image) != 54)
    {
        return e_failure;
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        char get_bit=((data>>(7-i)))&1;
        image_buffer[i] = image_buffer[i] & ~1;
        image_buffer[i] = image_buffer[i] | get_bit;
    }

    return e_success;
}

Status encode_size_to_lsb(int data, char *buffer)
{
    for(int i=0;i<32;i++)
    {
        char get_bit=((data>>(31-i)))&1;
        buffer[i] = buffer[i] & ~1;
        buffer[i] = buffer[i] | get_bit;
    }
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(buffer,1,8,fptr_src_image);
        encode_byte_to_lsb(data[i],buffer);
        fwrite(buffer,1,8,fptr_stego_image);
    }

    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{

    encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);

    return e_success;

}

Status encode_secret_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char extn_buffer[32];
    fread(extn_buffer,1,32,encInfo->fptr_src_image);
    encode_size_to_lsb(extn_size,extn_buffer);
    fwrite(extn_buffer,1,32,encInfo->fptr_stego_image);

    return e_success;
    
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);

    return e_success;

}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char data_buffer[32];
    fread(data_buffer,1,32,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,data_buffer);
    fwrite(data_buffer,1,32,encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file];
    fseek(encInfo->fptr_secret, 0, SEEK_SET); 
    fread(buffer,1,encInfo->size_secret_file,encInfo->fptr_secret);
    
    encode_data_to_image(buffer,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image);

    return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[8];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        fwrite(buffer, 1, bytes_read, fptr_dest);
    }
}






/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf("open file is success\n");
    }
    else
    {
        printf("Open file is failed\n");
        return e_failure;
    }

    if((check_capacity(encInfo)) == e_success)  //similar to above write success and failure code
    {
        printf("source image file has enough data to store > capacity checked\n");
    }
    else
    {
        printf("failed , no enough capacity\n");
        return e_failure;
    }

    // printf("%ld\n",ftell(encInfo->fptr_src_image));
    // printf("%ld\n",ftell(encInfo->fptr_stego_image));

    if((copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image))==e_success)
    {
        printf("header copied successfully\n");
    }
    else
    {
        printf("failed to copy header\n");
        return e_failure;
    }

    // printf("%ld\n",ftell(encInfo->fptr_src_image));
    // printf("%ld\n",ftell(encInfo->fptr_stego_image));

    if((encode_magic_string("#*",encInfo)) == e_success)
    {
        printf("magic string encoded\n");
    }

    // printf("%ld\n",ftell(encInfo->fptr_src_image));
    // printf("%ld\n",ftell(encInfo->fptr_stego_image));

    //printf("%s,%ld\n",encInfo->extn_secret_file,strlen(encInfo->extn_secret_file));

    if((encode_secret_extn_size(strlen(encInfo->extn_secret_file),encInfo)) == e_success)
    {
        printf("Extension size done\n");
    }

    // printf("%ld\n",ftell(encInfo->fptr_src_image));
    // printf("%ld\n",ftell(encInfo->fptr_stego_image));

    if((encode_secret_file_extn(encInfo->extn_secret_file,encInfo)) == e_success)
    {
        printf("extension file name done\n");
    }

    // printf("%ld\n",ftell(encInfo->fptr_src_image));
    // printf("%ld\n",ftell(encInfo->fptr_stego_image));

    if((encode_secret_file_size(encInfo->size_secret_file,encInfo)) == e_success)
    {
        printf("secret file size done\n");
    }

    // printf("%ld\n",ftell(encInfo->fptr_src_image));
    // printf("%ld\n",ftell(encInfo->fptr_stego_image));

    if((encode_secret_file_data(encInfo)) == e_success)
    {
        printf("secret data encoded\n");
    }

    // printf("%ld\n",ftell(encInfo->fptr_src_image));
    // printf("%ld\n",ftell(encInfo->fptr_stego_image));

    if((copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)) == e_success)
    {
        printf("remaining data copied\n");
    }

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_stego_image);
    fclose(encInfo->fptr_secret);
}
