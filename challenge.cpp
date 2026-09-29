#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>
#include <unsupported/Eigen/SparseExtra>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>

using namespace Eigen;


SparseMatrix<double> create_convolution_matrix(const MatrixXd &filter, const int height, const int width);
MatrixXd create_filter(const std::string &filter_name);



int main() {

    //task1: Image loading and conversion to Eigen Matrix
    int width, height, channels;
    unsigned char *data = stbi_load("deer.jpg", &width, &height, &channels, 1);

    if(data == nullptr) {
        printf("Error loading image: %s\n", stbi_failure_reason());
        return 1;
    }

    printf("Image loaded successfully: %dx%d, %d channels\n", width, height, channels); 

    MatrixXd image(height, width);


    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            image(i, j) = data[i * width + j];
        }
    }

    printf("Eigen Matrix size: %ld x %ld\n", image.rows(), image.cols());

    //task2

    MatrixXd noisyImage = image; // Create a copy of the original image

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

    VectorXd v(width * height);
    VectorXd w(width * height);

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
    
    /*TASK4:  Write the convolution operation corresponding to the smoothing kernel Hav1 as a matrix
        vector multiplication between a matrix A1 having size mnxmn and the image vector.
        Report the number of non-zero entries in A1*/

    SparseMatrix A1 = create_convolution_matrix(create_filter("av1"), height, width);
    
    printf("Non-zero entries in A1: %ld\n", A1.nonZeros());

    //Task 5
    VectorXd smoothed_noisy_deer = A1 * w;
    unsigned char *tmp = new unsigned char[width * height];

    for(int i=0; i<height * width; i++) {
            tmp[i] = static_cast<unsigned char>(smoothed_noisy_deer[i]);
    }

    if(stbi_write_png("smoothed_noisy_deer.png", width, height, 1, tmp, width) == 0) {
        printf("Error writing image");
        stbi_image_free(data);
        return 1;
    }else {
        printf("Noisy image saved successfully as smoothed_noisy_deer.png\n");
    }
    



    //Task 6
    SparseMatrix A2 = create_convolution_matrix(create_filter("sh1"), height, width);

    printf("Non-zero entries in A2: %ld\n", A2.nonZeros());

    if(A2.isApprox(A2.transpose())){
        printf("A2 is symmetric\n");
    } else {
        printf("A2 is not symmetric\n");
    }

    //Task 7
    VectorXd sharpened_image = A2 * v;

    for(int i=0; i<height * width; i++) {
        if(sharpened_image[i] < 0) sharpened_image[i] = 0;
        if(sharpened_image[i] > 255) sharpened_image[i] = 255;

        tmp[i] = static_cast<unsigned char>(sharpened_image[i]);
    }

    if(stbi_write_png("sharpened_deer.png", width, height, 1, tmp, width) == 0) {
        printf("Error writing image");
        stbi_image_free(data);
        return 1;
    }else {
        printf("Sharpened image saved successfully as sharpened_deer.png\n");
    }



    // Task 8: export A2 and w in Matrix Market format.
    if(saveMarketVector(w, "w.mtx") && saveMarket(A2, "A2.mtx")){
        printf("Files saved successfully: w.mtx and A2.mtx\n");
    } else {
        printf("Error saving files\n"); 
    }

    // BiCGSTAB + ILU(0): 18 iterations
    // Final residual: 2.426823e-13 (tolerance 1e-12)


    //TASK9: solution x to PNG 

    std::ifstream xFile("x.mtx");
    std::string banner;
    std::getline(xFile, banner);

    int n;
    if (xFile >> n && n == width * height) {
        std::vector<unsigned char> pixels(n, 0);
        int index;
        double value;

        while (xFile >> index >> value) {
            if (index >= 1 && index <= n) {
                if (value < 0) value = 0;
                if (value > 255) value = 255;
                pixels[index - 1] = static_cast<unsigned char>(std::lround(value));
            }
        }

        stbi_write_png("solution_deer.png", width, height, 1,
                    pixels.data(), width);
        printf("Solution image saved as solution_deer.png\n");
    } else {
        printf("Cannot read x.mtx or its size is incorrect\n");
    }



    //Task 10
    SparseMatrix A3 = create_convolution_matrix(create_filter("ed2"), height, width);

    if(A3.isApprox(A3.transpose())){
        printf("A3 is symmetric\n");
    } else {
        printf("A3 is not symmetric\n");
    }



    stbi_image_free(data);
    return 0;
}


SparseMatrix<double> create_convolution_matrix(const MatrixXd &filter, const int height, const int width){
    int n = height * width;
    SparseMatrix<double> A(n, n);
    std::vector<Triplet<double>> triplets;
    triplets.reserve(n * 9);

    for(int i = 0; i < n; i++){
        int row = i / width;
        int col = i % width;

        for(int k = -1; k <= 1; k++){
            for(int h = -1; h <= 1; h++){
                int image_r = row + k;
                int image_c = col + h;

                if((image_r >= 0 && image_r < height) && (image_c >= 0 && image_c < width && filter(k + 1, h + 1) != 0.0)){
                    int col_A = image_r * width + image_c;
                    triplets.push_back(Triplet<double>(i, col_A, filter(k+1, h+1)));
                }
            }
        }
    }

    A.setFromTriplets(triplets.begin(), triplets.end());
    return A;


}




MatrixXd create_filter(const std::string &filter_name){
    MatrixXd filter = MatrixXd::Zero(3, 3);

    if(filter_name == "av1"){
        filter(0, 0) = 1.0;
        filter(0, 1) = 1.0;
        filter(0, 2) = 1.0;
        filter(1, 0) = 1.0;
        filter(1, 1) = 4.0;
        filter(1, 2) = 1.0;
        filter(2, 0) = 1.0;
        filter(2, 1) = 1.0;
        filter(2, 2) = 1.0;
        filter = filter / 12.0;
        return filter;
    }else if(filter_name == "sh1"){
        filter(0, 1) = -3.0;
        filter(1, 0) = -1.0;
        filter(1, 1) = 9.0;
        filter(1, 2) = -3.0;
        filter(2, 1) = -1.0;
        return filter;
    }else if(filter_name == "ed2"){
        filter(0, 0) = -1;
        filter(0, 2) = 1;
        filter(1, 0) = -2;
        filter(1, 2) = 2;
        filter(2, 0) = -1;
        filter(2, 2) = 1;
        return filter;
    }

    throw std::invalid_argument("Filtro sconosciuto: " + filter_name);



}
