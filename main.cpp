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
    cv::Mat* img;
};

void updateGridImage(cv::Mat& img, int cell_size, std::vector<std::vector<int>>& matrix) {
    cv::Scalar grid_color = cv::Scalar(0, 0, 255);
    cv::Scalar grid_color_green = cv::Scalar(0, 255, 0);
    cv::Mat grid_img = img.clone();
    for (int i = 0; i < matrix.size(); i++){
        int y = i * cell_size;
        for (int j = 0; j < matrix.at(0).size(); j++){
            int x = j * cell_size;
            if(matrix[i][j] == 1){
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color_green, -1);
            }
            else{
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color, 1);
            }
        }
    }

    cv::imshow("Grid Image", grid_img);
}

void onMouse(int event, int x, int y, int flags, void* userdata) {
    MouseData* data = (MouseData*)userdata;
    int cell_size = data->cell_size;
    std::vector<std::vector<int>>* matrix = data->matrix;
    int j = x / cell_size;
    int i = y / cell_size;
    if(event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON)){
        if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
            matrix->at(i).at(j) = 1;
            updateGridImage(*data->img, cell_size, *matrix);
        }
    } 
    else if(event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_RBUTTON)){
         if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
            matrix->at(i).at(j) = 0;
            updateGridImage(*data->img, cell_size, *matrix);
        }
    }
}

std::vector<std::vector<int>> createGridImage(cv::Mat& img, int longest_dim, int cell_size, int size, std::vector<std::vector<int>> matrix){
    cv::namedWindow("Grid Image", cv::WINDOW_NORMAL);

    MouseData data;
    data.matrix = &matrix;
    data.cell_size = cell_size;
    data.img = &img;

    cv::setMouseCallback("Grid Image", onMouse, &data);
    updateGridImage(img, cell_size, matrix);

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

    std::vector<std::vector<int>> matrix( size , std::vector<int> (size, 0));
    matrix = createGridImage(img, longest_dim, cell_size, size, matrix);

    return 0;
}
