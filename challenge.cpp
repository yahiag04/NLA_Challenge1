#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <Eigen/Dense>

#include <cstdio>

int main() {
    int width, height, channels;
    unsigned char *data = stbi_load("deer.jpeg", &width, &height, &channels, 1);

    if(data == nullptr) {
        printf("Error loading image: %s\n", stbi_failure_reason());
        return 1;
    }

    printf("Image loaded successfully: %dx%d, %d channels\n", width, height, channels); 

    Eigen::MatrixXd image(height, width);


    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            image(i, j) = data[i * width + j];
        }
    }

    printf("Eigen Matrix size: %ld x %ld\n", image.rows(), image.cols());

    stbi_image_free(data);
    return 0;
}