#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
#include <cstdlib>
#include <string>

struct MouseData {
    std::vector<std::vector<int>>* matrix;
    int cell_size;
};

void onMouse(int event, int x, int y, int flags, void* userdata) {
    MouseData* data = (MouseData*)userdata;
    int cell_size = data->cell_size;
    std::vector<std::vector<int>>* matrix = data->matrix;
    int j = x / cell_size;
    int i = y / cell_size;
    if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
        matrix->at(i).at(j) = 1;
    }
}

std::vector<std::vector<int>> createGridImage(cv::Mat& img, std::vector<std::vector<int>> matrix){
    int longest_dim = std::max(img.cols, img.rows); 
    int cell_size = std::ceil((double)longest_dim / 120);
    int size = std::ceil((double)longest_dim / cell_size);


    cv::Scalar grid_color = cv::Scalar(0, 0, 255);
    cv::Mat grid_img = img.clone();
    for (int i = 0; i < size; i++){
        int y = i * cell_size;
        for (int j = 0; j < size; j++){
            int x = j * cell_size;
            if(matrix[i][j] == 1){
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color, -1);
            }
            else{
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color, 1);
            }
        }
    }

    MouseData data;
    data.matrix = &matrix;
    data.cell_size = cell_size;
    cv::namedWindow("Grid Image", cv::WINDOW_NORMAL);
    cv::setMouseCallback("Grid Image", onMouse, &data);
    cv::imshow("Grid Image", grid_img);

    if (grid_img.empty() || grid_img.cols <= 0 || grid_img.rows <= 0) {
        std::cerr << "Error: Grid image is empty or has zero dimensions." << std::endl;
        exit(-1);
    }

    cv::waitKey(0);

    return matrix;
}

int main(){
    cv::Mat img = cv::imread("map.png", cv::IMREAD_COLOR);
    if (img.empty()){
        std::cout << "Could not read the image" << std::endl;
        return -1;
    }
    int longest_dim = std::max(img.cols, img.rows); 
    int cell_size = std::ceil((double)longest_dim / 120);
    int size = std::ceil((double)longest_dim / cell_size);

    std::vector<std::vector<int>> matrix( size , std::vector<int> (size, 0));  ;
    matrix = createGridImage(img, matrix);
    for(auto lst : matrix){

        for(auto e : lst){
               std::cout << e << " ";
        }
        std::cout << std::endl;
    }

    matrix = createGridImage(img, matrix);

    return 0;
}