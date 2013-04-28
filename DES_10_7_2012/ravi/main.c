#include <stdio.h>
#include <malloc.h>
#include <time.h>

#include "my_image_lib.h"

void put_white(
                Image *image,
                int i,
                int j
              )
{
    image->c_matrix[i][j].r=255;
    image->c_matrix[i][j].g=255;
    image->c_matrix[i][j].b=255;
}

void put_black(
                Image *image,
                int i,
                int j
              )
{
    image->c_matrix[i][j].r=0;
    image->c_matrix[i][j].g=0;
    image->c_matrix[i][j].b=0;
}


void create_overlay(
                        Image *img1,
                        Image *img2
                   )
{
    
    Image *overlay;

    bmp_image *bitmap;
    
    int     i,
            sum,
            j;
    
    overlay = (Image *)malloc(sizeof(Image));
    
    overlay->width = img1->width;
    overlay->height = img1->height;
    
    overlay->c_matrix = (color_pixel **)malloc(sizeof(color_pixel *)*overlay->height);
        
    for(i = 0; i < overlay->height; i++){
        overlay->c_matrix[i] = (color_pixel *)malloc(sizeof(color_pixel)*overlay->width);
        for(j = 0; j < overlay->width; j++){
            sum = img1->c_matrix[i][j].r + img2->c_matrix[i][j].r;
            if(sum > 255) put_white(overlay, i, j);
            else put_black(overlay, i, j);
        }
    }
            
    bitmap = bmp_from_image(overlay);
    write_bmp_image(bitmap, "overlayed_image.bmp");
}    


void create_crypted_image(
                            Image *image
                         )
{
    
    int     i,
            j,
            random;
    
    Image   *img1,
            *img2; 
    
    bmp_image *bitmap;
               

    srand(time(NULL));
    
    img1 = (Image *)malloc(sizeof(Image));
    img2 = (Image *)malloc(sizeof(Image));
    
    img1->width = 2*image->width;
    img1->height = 2*image->height;
    
    img2->width = 2*image->width;
    img2->height = 2*image->height;
    
    color_to_grayscale(image);
    
    for(i = 0; i < image->height; i++){
        for(j = 0; j < image->width; j++){
            
            if(image->c_matrix[i][j].r > 127)
                put_white(image, i, j);
            
            else
                put_black(image, i, j);
                
        }
    }
   
    img1->c_matrix = (color_pixel **)malloc(sizeof(color_pixel*)*img1->height);
    img2->c_matrix = (color_pixel **)malloc(sizeof(color_pixel*)*img1->height);
    
    for(i = 0; i < img1->height; i++){  
        img1->c_matrix[i] = (color_pixel *)malloc(sizeof(color_pixel)*img1->width);
        img2->c_matrix[i] = (color_pixel *)malloc(sizeof(color_pixel)*img1->width);
    }
    for(i = 0; i < image->height; i++){
             
        for(j = 0; j < image->width; j++){

                        
            if(image->c_matrix[i][j].r==255){
                
                if(rand()%2 == 0){
                    
                    put_black(img1, 2*i, 2*j);
                    put_white(img1, 2*i, (2*j)+1);
                    put_white(img1, (2*i)+1, 2*j);
                    put_black(img1, (2*i)+1, (2*j)+1);
                    
                    put_black(img2, 2*i, 2*j);
                    put_white(img2, 2*i, (2*j)+1);
                    put_white(img2, (2*i)+1, 2*j);
                    put_black(img2, (2*i)+1, (2*j)+1);
                
                }
                
                else{
                
                
                    put_white(img1, 2*i, 2*j);
                    put_black(img1, 2*i, (2*j)+1);
                    put_black(img1, (2*i)+1, 2*j);
                    put_white(img1, (2*i)+1, (2*j)+1);
                    
                    put_white(img2, 2*i, 2*j);
                    put_black(img2, 2*i, (2*j)+1);
                    put_black(img2, (2*i)+1, 2*j);
                    put_white(img2, (2*i)+1, (2*j)+1);
                                
                }
            }        
            
            
            else{
                
                if(rand()%2 == 0){
                    
                    put_black(img1, 2*i, 2*j);
                    put_white(img1, 2*i, (2*j)+1);
                    put_white(img1, (2*i)+1, 2*j);
                    put_black(img1, (2*i)+1, (2*j)+1);
                    
                    put_white(img2, 2*i, 2*j);
                    put_black(img2, 2*i, (2*j)+1);
                    put_black(img2, (2*i)+1, 2*j);
                    put_white(img2, (2*i)+1, (2*j)+1);
                
                }
                
                else{
                
                
                    put_white(img1, 2*i, 2*j);
                    put_black(img1, 2*i, (2*j)+1);
                    put_black(img1, (2*i)+1, 2*j);
                    put_white(img1, (2*i)+1, (2*j)+1);
                    
                    put_black(img2, 2*i, 2*j);
                    put_white(img2, 2*i, (2*j)+1);
                    put_white(img2, (2*i)+1, 2*j);
                    put_black(img2, (2*i)+1, (2*j)+1);
                                
                }
            }
        }
    }
    
    
    
    bitmap = bmp_from_image(img1);
    write_bmp_image(bitmap, "cyphered1.bmp");
    
    bitmap = bmp_from_image(img2);
    write_bmp_image(bitmap, "cyphered2.bmp");
    
    create_overlay(img1, img2);
}
    

    
           
                   
int main    (
                int argc,
                char **argv
            )
{
    bmp_image   *bitmap_image;
    
    Image       *plain_img;
   
     
    
    bitmap_image = read_bmp("plain_image.bmp");
    plain_img = image_from_bmp(bitmap_image);
    
    create_crypted_image(plain_img);
    
    return 0;
    
}
