#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "my_bmp_lib.h"

#define TRUE 1
#define FALSE 0



typedef struct color_pixel
{
    unsigned char b, g, r;
}
color_pixel;

typedef struct alpha_channel
{
    unsigned char alpha;
}
alpha_channel;


typedef struct Image
{
    unsigned int    height, width;
    char            is_grayscale;
    color_pixel     **c_matrix;
    alpha_channel   **alpha;    
}
Image;


/***********************************************************

input:
    *   Image *
    
output:
    *   void

        Converts a colored Image to grayscale Image
                  
************************************************************/          
void color_to_grayscale(
                             Image *colored_Image
                       )
{
    int     i,
            j;

    float   grayscale;
    
    
    if(colored_Image->is_grayscale){
        return ;
    }
    
    colored_Image->is_grayscale = TRUE;
    
    
    
    for(i = 0; i < colored_Image->height; i++){
        
        for(j = 0; j < colored_Image->width; j++){
            
            grayscale = (colored_Image->c_matrix[i][j].r * 0.2989 + colored_Image->c_matrix[i][j].g * 0.587 + colored_Image->c_matrix[i][j].b * 0.1140);
            
            colored_Image->c_matrix[i][j].r = grayscale;
            colored_Image->c_matrix[i][j].g = grayscale;
            colored_Image->c_matrix[i][j].b = grayscale;
            
        }
        
    }
}

/********************************************************

input:
    *   bmp_image *
   
output:
    *   Image *
    
        Makes a new instance of Image from bmp_image 
        and returns its address
        
********************************************************/
Image* image_from_bmp  (
                              bmp_image *bmp
                       )
{
    Image *img;
    
    int i,
        j;
        
    img = (Image *)malloc(sizeof(Image));
    
    if(bmp->f_header.offset != 54)  
        indexed_to_true(bmp);
      
    img->is_grayscale = FALSE;
    img->height = bmp->i_header.height;
    img->width  = bmp->i_header.width;
    
    img->c_matrix = (color_pixel **)malloc(sizeof(color_pixel *)*img->height);
           
    for(i = 0; i < bmp->i_header.height; i++){
        img->c_matrix[i] = (color_pixel *)malloc(sizeof(color_pixel)*img->width);
        for(j = 0; j < bmp->i_header.width; j++){
            img->c_matrix[i][j].r = bmp->t_pixel[i][j].r;
            img->c_matrix[i][j].g = bmp->t_pixel[i][j].g;
            img->c_matrix[i][j].b = bmp->t_pixel[i][j].b;
        }
    }
    
    return img;
}


/******************************************************

input:
    *   Image *

output:
    *   bmp_image *
    
        Returns a true type bmp_Image file from a 
             Image type pointer variable
             
*******************************************************/
bmp_image*  bmp_from_image(
                                Image *img
                          )
{
    int i,
        j;
    
    bmp_image *bmp;
    
    if(img->c_matrix == NULL)
        err("In bmp_from_img in \"my_Image_lib.h\"\nThe input Image is not valid");
    
    bmp = (bmp_image *)malloc(sizeof(bmp_image));
    
    
    /* Setting the values for file header */
    bmp->f_header.m_number[0]='B'; bmp->f_header.m_number[1]='M';
    bmp->f_header.size = img->height * four_multiple(img->width) + 54;
    bmp->f_header.reserved1 = bmp->f_header.reserved2 = 0;
    bmp->f_header.offset = 54;
    
    
    /*  Setting the values for Image header */
    bmp->i_header.height = img->height;
    bmp->i_header.width  = img->width;
    bmp->i_header.h_size = 40;
    bmp->i_header.color_planes = 1;
    bmp->i_header.b_pixel = 24;
    bmp->i_header.compression = 0;
    bmp->i_header.size_bitmap = img->width*img->height;
    bmp->i_header.h_resolution = 0;
    bmp->i_header.v_resolution = 0;
    bmp->i_header.colors = 0;
    bmp->i_header.important_colors = 0;
    
 
    
    /* Setting the values of pixels */
    bmp->t_pixel = (bmp_color_pixel **)malloc(sizeof(bmp_color_pixel *)*(img->height));
    for(i = 0; i < img->height; i++){
        
        bmp->t_pixel[i] = (bmp_color_pixel *)malloc(sizeof(bmp_color_pixel )*(four_multiple(img->width)));
        
        for(j=0; j < img->width; j++){
            bmp->t_pixel[i][j].r = img->c_matrix[i][j].r;
            bmp->t_pixel[i][j].g = img->c_matrix[i][j].g;
            bmp->t_pixel[i][j].b = img->c_matrix[i][j].b;
        }
    }
    
    return bmp;
}



/******************************************************

input:
    *   Image *

output:
    *   Image *
    
        Returns address of new instance of Image exactly 
            same as the input Image

*******************************************************/        
Image* copy_image(
                    Image *img
                 )
{
    Image *image;
    
    int i,
        j;
        
    image = (Image *)malloc(sizeof(Image));
    
    image->height = img->height;
    image->width = img->width;
    image->is_grayscale = img->is_grayscale;
    
    image->c_matrix = (color_pixel **)malloc(sizeof(color_pixel *)*image->height);
   
    for(i = 0; i < img->height; i++){
        image->c_matrix[i] = (color_pixel *)malloc(sizeof(color_pixel)*image->width);
        for(j = 0; j < img->width; j++){
            image->c_matrix[i][j].r = img->c_matrix[i][j].r;
            image->c_matrix[i][j].g = img->c_matrix[i][j].g;
            image->c_matrix[i][j].b = img->c_matrix[i][j].b;
        }
    }
     
    return image;
}


    
/***************************************************

input:
    *   Image *
    *   char *
    
output:
    *   void
    
    Creates text file using red values of pixel
      Image could be both indexed or true type
    
***************************************************/
void create_text_from_image (
                                Image *image,
                                char *output_file_name
                            )                                
{
    int   i,
          j;

    FILE   *fp;
    
    Image *grayscale_image;
    
    char    ch1='0', 
            ch2='_', 
            ch3='\n';
    
    fp=fopen(output_file_name, "w");
    
    grayscale_image = copy_image(image);
    color_to_grayscale(grayscale_image);
    image = grayscale_image;
    for(i = 0; i < image->height; i++){
        for(j = 0; j < image->width; j++){
            if(image->c_matrix[i][j].r < 128)
                fwrite(&ch1, 1, 1, fp);
            else fwrite(&ch2, 1, 1, fp);
        }
        fwrite(&ch3, 1, 1, fp);
    }
    
    fclose(fp);
}



