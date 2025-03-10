#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <emscripten.h>


EMSCRIPTEN_KEEPALIVE
void change_brightness(uint8_t *imageData, int height, int width, int brightnessValue)
{
    for(int i=0; i< width * height * 4; i += 4){
        imageData[i] = (uint8_t) fmax(0, fmin(255, imageData[i] + brightnessValue));
        imageData[i+1] = (uint8_t) fmax(0, fmin(255, imageData[i+1] + brightnessValue));
        imageData[i+2] = (uint8_t) fmax(0, fmin(255, imageData[i+2] + brightnessValue));
    }
}

EMSCRIPTEN_KEEPALIVE
void mirror_image(uint8_t *imageData, int width, int height) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width / 2; col++) {
            int left_index = (row * width + col)*4;
            int right_index = (row * width + (width - col - 1))*4;
            
            for (int c = 0; c < 4; c++) {
                uint8_t temp = imageData[left_index + c];
                imageData[left_index + c] = imageData[right_index + c];
                imageData[right_index + c] = temp;
            }
        }
    }
}

EMSCRIPTEN_KEEPALIVE
void zoom_image(uint8_t *imageData, int width, int height, float zoomFactor) {
    if (zoomFactor < 1.0f) zoomFactor = 1.0f;  // Prevent zooming out below 1x

    int new_width = (int)(width / zoomFactor);
    int new_height = (int)(height / zoomFactor);

    int start_x = (width - new_width) / 2;
    int start_y = (height - new_height) / 2;

    uint8_t *zoomedData = (uint8_t *)malloc(width * height * 4);
    if (!zoomedData) return;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_x = start_x + (x * new_width) / width;
            int src_y = start_y + (y * new_height) / height;

            if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
                int src_index = (src_y * width + src_x) * 4;
                int dest_index = (y * width + x) * 4;

                zoomedData[dest_index] = imageData[src_index];         // Red
                zoomedData[dest_index + 1] = imageData[src_index + 1]; // Green
                zoomedData[dest_index + 2] = imageData[src_index + 2]; // Blue
                zoomedData[dest_index + 3] = imageData[src_index + 3]; // Alpha
            }
        }
    }

    // Copy zoomed data back to imageData
    for (int i = 0; i < width * height * 4; i++) {
        imageData[i] = zoomedData[i];
    }

    free(zoomedData);
}

EMSCRIPTEN_KEEPALIVE
void highlight_text(uint8_t* imgData, int width, int height) {
    int numPixels = width * height;
    
    for (int i = 0; i < numPixels; i++) {
        int idx = i * 4;

        uint8_t r = imgData[idx];     
        uint8_t g = imgData[idx + 1];  
        uint8_t b = imgData[idx + 2]; 
        uint8_t a = imgData[idx + 3]; 

        float luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;

        if (luminance < 128) {
            imgData[idx] = 0;       
            imgData[idx + 1] = 0;   
            imgData[idx + 2] = 0;   
            imgData[idx + 3] = 255; 
        } else {
            imgData[idx] = 255;  
            imgData[idx + 1] = 255; 
            imgData[idx + 2] = 255; 
            imgData[idx + 3] = 255;  
        }
    }
}

