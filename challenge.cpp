#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <Eigen/Dense>

#include <cstdio>
#include <cstdlib>

int main() {

    //task1: Image loading and conversion to Eigen Matrix
    int width, height, channels;
    unsigned char *data = stbi_load("deer.jpg", &width, &height, &channels, 1);

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

    //task2

    Eigen::MatrixXd noisyImage = image; // Create a copy of the original image

    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            double noise = ((double)rand() / RAND_MAX) * 100 - 50; // Random noise between -50 and 50
            noisyImage(i, j) += noise;
            if(noisyImage(i, j) < 0) noisyImage(i, j) = 0; // Clamp to [0, 255]
            if(noisyImage(i, j) > 255) noisyImage(i, j) = 255;
        }
    }

    unsigned char *noisyData = new unsigned char[width * height];
    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            noisyData[i * width + j] = static_cast<unsigned char>(noisyImage(i, j));
        }
    }


    if(stbi_write_png("noisy_deer.png", width, height, 1, noisyData, width) == 0) {
        printf("Error writing image");
        stbi_image_free(data);
        return 1;
    }else {
        printf("Noisy image saved successfully as noisy_deer.png\n");
    }



    //TASK3: Image remodelling and euclidean norm calculation

    Eigen::VectorXd v(width * height);
    Eigen::VectorXd w(width * height);

    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            v(i * width + j) = image(i, j);
            w(i * width + j) = noisyImage(i, j);
        }
    }

    printf("v size: %ld\n", v.size());
    printf("w size: %ld\n", w.size());

    if(v.size() != w.size() || v.size() != width * height || w.size() != width * height) {
        printf("Error: v and w must be of the same size for norm calculation\n");
        stbi_image_free(data);
        return 1;
    }

    //norm calculation
    double euclideanNorm = v.norm();
    printf("Euclidean norm of v: %f\n", euclideanNorm);

    


    stbi_image_free(data);
    return 0;
}
