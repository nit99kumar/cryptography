#pragma pack(1)

#define TRUE 1
#define FALSE 0

typedef struct file_header
{
    unsigned char m_number[2];
    unsigned int size;
    short reserved1;
    short reserved2;
    unsigned int offset;
}
file_header;



typedef struct image_header
{
    unsigned int h_size;
    unsigned int width;
    unsigned int height;
    short color_planes;
    short b_pixel;
    unsigned int compression;
    unsigned int size_bitmap;
    unsigned int h_resolution;
    unsigned int v_resolution;
    unsigned int colors;
    unsigned int important_colors;
}
image_header;



typedef struct color_palette 
{
    unsigned char b, g, r, junk;
}
 color_palette;



typedef struct bmp_color_pixel
{
    unsigned char b, g, r;
}
bmp_color_pixel;


typedef struct bmp_image
{ 
    file_header      f_header;
    image_header     i_header;
    color_palette    *c_table;
    bmp_color_pixel  **t_pixel;
    unsigned char    **i_pixel;
}
bmp_image;



void err (
            char *string
         )
{
    printf("\n%s\n\nNow program will now exit.\n", string);
    exit(-1);
}


int four_multiple(int num)
{
    if((num%4)==0)
       return(num);
    return ((num/4)+1)*4 ;   
}


/************************************************************

         Retuns address of true type image from indexed image
         
         
         If input image is true type itself then the 
         address of input itself is returned. 
         
         Else a new instance of bmp_image is created 
         and its address is returned
         
*************************************************************/
bmp_image* indexed_to_true  (
                                bmp_image *image
                            )
{
    
    bmp_image   *t_image;
    
    int         i,
                j;
    
   
    if (image->f_header.offset == 54){   
        return image;
    }
    t_image = (bmp_image *)malloc(sizeof(bmp_image)); 
    t_image->f_header = image->f_header;
    t_image->f_header.size = 54 + 3 * (image->i_header.height) * (four_multiple(image->i_header.width));
    t_image->f_header.offset = 54;
    
    t_image->i_header = image->i_header;
    t_image->i_header.b_pixel = 24;
    
    t_image->t_pixel = (bmp_color_pixel **)malloc(sizeof(bmp_color_pixel *)*image->i_header.height);
    for(i = 0; i < image->i_header.height; i++){
        t_image->t_pixel[i] = (bmp_color_pixel *)malloc(sizeof(bmp_color_pixel)*four_multiple(image->i_header.width));
        for(j = 0; j < image->i_header.width; j++){
            t_image->t_pixel[i][j].r = image->c_table[image->i_pixel[i][j]].r;
            t_image->t_pixel[i][j].g = image->c_table[image->i_pixel[i][j]].g;
            t_image->t_pixel[i][j].b = image->c_table[image->i_pixel[i][j]].b;
        }
        for( ; j < four_multiple(image->i_header.width); j++){
            t_image->t_pixel[i][j].r = 0;
            t_image->t_pixel[i][j].g = 0;
            t_image->t_pixel[i][j].b = 0;
        }
    }
    
    return t_image;
}
    



/***************************************************************

        Reads a bmp image file (indexed or true color) and 
        returns the address of bmp_image variable address
         
***************************************************************/
bmp_image* read_bmp (
                        char *filename
                    )
{
    FILE         *fp;
        
    int         i,  
                new_width;
        
    bmp_image    *image;
                
    
    
        
    /*    Opening image file     */
    fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("In read_bmp\n\tCan't Open the file\n");
        return 0;
    }
    
    image = (bmp_image*)malloc(sizeof(bmp_image));
    
    /*    Reading file header and checking if file is valid */
    fread(&image->f_header, sizeof(file_header), 1, fp);
    if(image->f_header.m_number[0] != 'B' || image->f_header.m_number[1] != 'M')
        err("In read_bmp\n\tThe file is not a valid Bit Map file");
    
        
    /*     Reading image header     */
     fread(&image->i_header, sizeof(image_header), 1, fp);
    
     /*     Reading the image table if available */
    if(image->f_header.offset == 1078) {
        image->c_table=(color_palette *)malloc(sizeof(color_palette)*256);
        fread(image->c_table, 1024, 1, fp);
    }
  
	else if(image->f_header.offset == 108) {
        image->c_table=(color_palette *)malloc(sizeof(color_palette)*256);
        fread(image->c_table, 64, 1, fp);
    }
    
    /*  Allocating space in which to read pixels */
    if(image->f_header.offset != 54 ){
        image->i_pixel=(unsigned char **)malloc(sizeof(char*)*image->i_header.height);
    }
    
	else image->t_pixel=(bmp_color_pixel **)malloc(sizeof(bmp_color_pixel *)*image->i_header.height);
    
    /*  Calculating the padding to be done  */
    new_width=four_multiple(image->i_header.width);
    fseek(fp, image->f_header.offset, SEEK_SET);   
    
    
    /* Reading image data  */
    if(image->f_header.offset != 54) {
        for(i=0; i<image->i_header.height; i++){
            image->i_pixel[image->i_header.height-1-i]=(unsigned char *)malloc(sizeof(unsigned char)*new_width);
            fread(image->i_pixel[image->i_header.height-1-i], new_width, 1, fp);
        }
    }
    
    
    else{
        for(i=0; i<image->i_header.height; i++){
            image->t_pixel[image->i_header.height-1-i]=(bmp_color_pixel *)malloc(sizeof(bmp_color_pixel)*new_width);
            fread(image->t_pixel[image->i_header.height-1-i], new_width*3, 1, fp);
        }
    } 
    fclose(fp);
    return image;
}



/*********************************************************

  Writes a 16, 256 or true color Bit Map image to a file
                   
**********************************************************/
void write_bmp_image(
                       bmp_image *image,
                       char *output_file_name
                    )
{
    FILE     *fp;

    int     i,
            width,
            indexed = FALSE;


    if(image->f_header.offset != 54)
        indexed = TRUE;
        
        
    fp=fopen(output_file_name, "wb");
    if(fp == NULL)
        err("In write_image\n\tCould not open output file for writing."); 
    
    if( ! fwrite(&image->f_header, sizeof(file_header), 1, fp))
        err("In write_image\n\tCould not write file_header to output file.");
        
    if( ! fwrite(&image->i_header, sizeof(image_header), 1, fp))
        err("In write_image\n\tCould not write image_header to output file.");
    
    
    if(image->f_header.offset == 1078) fwrite(image->c_table, 1024, 1, fp);
    else if(image->f_header.offset == 108) fwrite(image->c_table, 64, 1, fp);
    
    width=four_multiple(image->i_header.width);
   
    if(indexed){
        for(i = 0; i < image->i_header.height; i++)
            fwrite(image->i_pixel[image->i_header.height-1-i], width, 1, fp);
    }
    else{
        for(i=0; i < image->i_header.height; i++)
            fwrite(image->t_pixel[image->i_header.height-1-i], width*3, 1, fp);
    }
        
    fclose(fp);
}

