#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
#include <cstdlib>
#include <string>

/**
 * @brief for data that the mouse needs
 * 
 */
struct MouseData {
    std::vector<std::vector<int>>* matrix;
    int cell_size;
    cv::Mat* img;
    cv::Mat* origImg;
    int counter = 0;
    int checkpoint_counter = -5;
    std::vector<std::pair<int, int>> route;
    int robotX = 10;
    int robotY = 10;
    int mode = 0;
};

struct GridData {
    int beginX = 0;
    int beginY = 0;
    int endX = 0;
    int endY = 0;
};

struct storeData {
    GridData gData;
    MouseData MData;
};


/**
 * @brief for the grid making and updating
 * 
 * @param img 
 * @param cell_size 
 * @param matrix 
 */
void UpdateGridImage(int cell_size, std::vector<std::vector<int>>& matrix, storeData* data) {
    cv::Scalar grid_color = cv::Scalar(0, 0, 255);
    cv::Scalar grid_color_green = cv::Scalar(0, 255, 0);
    cv::Scalar grid_color_checkpoint1 = cv::Scalar(255, 0, 255);
    cv::Mat grid_img = *data->MData.origImg;
    std::cout << data->gData.beginX << " , " << data->gData.beginY << std::endl;
    for (int i = 0; i < matrix.size(); i++){
        int y = data->gData.beginY + i * cell_size;
        for (int j = 0; j < matrix.at(0).size(); j++){
            int x = data->gData.beginX + j * cell_size;
            if(matrix[i][j] == 0){
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color, 1);
            }
            else if(matrix[i][j] == 9999){
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), cv::Scalar(100, 100, 100), -1);
            }
            else if(matrix[i][j] < 0){
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color_checkpoint1, -1);
            }
            else{
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color_green, -1);
            }
        }
    }
    cv::imshow("Grid Image", grid_img);
}

int CheckSurroundings(storeData* data){
    bool next_found = false;
    for(int y = data->MData.robotY -1; y <= data->MData.robotY + 1; ++y){
        for(int x = data->MData.robotX - 1; x <= data->MData.robotX + 1; ++x){
            if (x < 0 || x >= 173 || y < 0 || y >= 491) {
                continue;
            } // out of bounds check
            if(data->MData.route.size() <= data->MData.counter){
                return 0; // error voorkomen
            }else if(x == data->MData.route[data->MData.counter].first && y == data->MData.route[data->MData.counter].second){
                int value = data->MData.matrix->at(x).at(y);
                data->MData.counter++;
                std::cout << "COORDINATE FOUND" << std::endl;
                if(value < 0){
                    std::cout << "CHECKPOINT FOUND, CHECKPOINT FUNCTIE UITVOEREN" << std::endl;
                }
                if(!(data->MData.route.size() > data->MData.counter)){
                    std::cout << "RESET" <<std::endl;
                    data->MData.counter = 0;
                }
                std::cout << "Next coordinate:" << data->MData.route[data->MData.counter].first << "/" << data->MData.route[data->MData.counter].second << std::endl;
                return 0;
            }
        }
    }
    
    return 0;
}


/**
 * @brief For mouse movement and events
 * 
 * @param event 
 * @param x 
 * @param y 
 * @param flags 
 * @param user_data 
 */
void OnMouse(int event, int x, int y, int flags, void* user_data) {
    storeData* data = (storeData*)user_data;
    int cell_size = data->MData.cell_size;
    std::vector<std::vector<int>>* matrix = data->MData.matrix;
    
    int j = (x - data->gData.beginX) / cell_size;
    int i = (y - data->gData.beginY) / cell_size;
    
    if (event == cv::EVENT_MOUSEMOVE) {
        if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
            if(data->MData.mode == 0){
                if (flags & cv::EVENT_FLAG_LBUTTON) {
                    if (matrix->at(i).at(j) == 0 && matrix->at(i).at(j) != 1) {
                        // data->MData.counter++;
                        matrix->at(i).at(j) = data->MData.counter;
                        data->MData.route.push_back(std::make_pair(i, j));
                        std::cout << j << "/" << i << ": " << data->MData.counter << std::endl;
                        std::cout << "route: ";
                        for(int routeDoornemer = 0; routeDoornemer < data->MData.route.size(); routeDoornemer++){
                            std::cout << data->MData.route[routeDoornemer].first << "/" << data->MData.route[routeDoornemer].second << ",";
                        }
                        std::cout << std::endl;
                        UpdateGridImage(cell_size, *matrix, data);
                    }
                    // else if(matrix->at(i).at(j) != counter){
                    //     checkpoint_counter--;
                    //     matrix->at(i).at(j) = checkpoint_counter;
                    //     std::cout << j << "/" << i << ": " << " Verandering into checkpoint" << std::endl;
                    // }
                } else if (flags & cv::EVENT_FLAG_RBUTTON) {
                    matrix->at(i).at(j) = 0;
                    UpdateGridImage(cell_size, *matrix, data);
                }
                CheckSurroundings(data);
            }
            else{
                if (flags & cv::EVENT_FLAG_LBUTTON) {
                    matrix->at(i).at(j) = 9999;
                    data->MData.robotX = i;
                    data->MData.robotY = j;
                    UpdateGridImage(cell_size, *matrix, data);
            
                } else if (flags & cv::EVENT_FLAG_RBUTTON) {
                    matrix->at(i).at(j) = 0;
                    UpdateGridImage(cell_size, *matrix, data);
                }
                CheckSurroundings(data);
            }
        }
    } else if (event == cv::EVENT_MOUSEWHEEL) {
        if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
            std::cout << data->MData.mode << std::endl;
            if(data->MData.mode == 0){
                std::cout << "mode: " << data->MData.mode << std::endl;
                data->MData.mode = 1;
            }
            else{
                std::cout << "mode: " << data->MData.mode << std::endl;
                data->MData.mode = 0;
            }
        }
    }
}

/**
 * @brief Create a Grid Image object
 * 
 * @param img 
 * @param cell_size 
 * @param matrix 
 * @return std::vector<std::vector<int>> 
 */
std::vector<std::vector<int>> CreateGridImage(cv::Mat& img, cv::Mat& origImg, cv::Point begin, cv::Point end){
    cv::namedWindow("Grid Image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Grid Image", img.cols, img.rows);

    int longest_dim = std::max(img.cols, img.rows);
    int cell_size = std::ceil((double)longest_dim / 120);
    // int size = std::ceil((double)longest_dim / cell_size);

    storeData uData;
    uData.gData.beginX = begin.x;
    uData.gData.beginY = begin.y;
    uData.gData.endX = end.x;
    uData.gData.endY = end.y;

    // std::cout << begin.x <<  " , "  << begin.y << std::endl;
    // std::cout << uData.gData.beginX <<  " , "  << uData.gData.beginY << std::endl;

    cv::Rect roi(begin.x, begin.y, end.x - begin.x, end.y - begin.y);
    cv::Mat roiImage = img(roi);


    int width = std::ceil((double)roiImage.cols / cell_size);
    int height = std::ceil((double)roiImage.rows / cell_size);


    std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 0));

    uData.MData.matrix = &matrix;
    uData.MData.cell_size = cell_size;
    uData.MData.img = &img;
    uData.MData.origImg = &origImg;

    cv::setMouseCallback("Grid Image", OnMouse, &uData);
    UpdateGridImage(cell_size, matrix, &uData);

    cv::waitKey(0);

    return matrix;
}


int main(){
    cv::Mat img = cv::imread("floor4.png", cv::IMREAD_COLOR);
    if (img.empty()){
        std::cout << "Could not read the image" << std::endl;
        return -1;
    }

    cv::Mat hsv_img; 
    cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);
    // Define the lower and upper bounds of the green color range
    // cv::Scalar lower(30, 40, 40);
    // cv::Scalar upper(90, 255, 255);

    // black color
    cv::Scalar lower(0, 0, 0);
    cv::Scalar upper(180, 255, 30);

    // Create a mask to filter out non-green pixels
    cv::Mat mask;
    cv::inRange(hsv_img, lower, upper, mask);
    // Apply the mask to the original image
    cv::Mat maskImg;
    img.copyTo(maskImg, mask);


    // Find the coordinates of green pixels
    std::vector<cv::Point> greenCoords;
    for (int y = 0; y < mask.rows; ++y) {
        for (int x = 0; x < mask.cols; ++x) {
            if (mask.at<uchar>(y, x) == 255) {
                greenCoords.emplace_back(x, y);
            }
        }
    }

    // Find the highest, lowest, leftmost, and rightmost points
    cv::Point highestPoint = greenCoords[0];
    cv::Point lowestPoint = greenCoords[0];
    cv::Point leftmostPoint = greenCoords[0];
    cv::Point rightmostPoint = greenCoords[0];

    for (const auto& coord : greenCoords) {
        if (coord.y < highestPoint.y) {
            highestPoint = coord;
        }
        if (coord.y > lowestPoint.y) {
            lowestPoint = coord;
        }
        if (coord.x < leftmostPoint.x) {
            leftmostPoint = coord;
        }
        if (coord.x > rightmostPoint.x) {
            rightmostPoint = coord;
        }
    }

    cv::Point startPoint(leftmostPoint.x, highestPoint.y);  // Starting coordinate of the grid
    cv::Point endPoint(rightmostPoint.x, lowestPoint.y);  // Ending coordinate of the grid

    // // Select a pixel position (x, y)
    // int x = 100;
    // int y = 200;
    // // Get the RGB values of the pixel
    // for(int y=0;y<hsv_img.rows;y++){
    //     for(int x=0;x<hsv_img.cols;x++){
    //         cv::Vec3b pixel = hsv_img.at<cv::Vec3b>(y, x);
    //         int red = pixel[2];
    //         int blue = pixel[0];
    //         int green = pixel[1];

    //         if(red >= 0 && green >= 111 && blue >= 0 && red <= 165  && blue <= 165){
    //             // Output the RGB values
    //             std::cout << "Pixel (" << x << ", " << y << ") RGB: "
    //             << "R=" << red << ", G=" << green << ", B=" << blue << std::endl; 
    //         }
    //     }
    // }
    

    std::vector<std::vector<int>> matrix = CreateGridImage(maskImg, img, startPoint, endPoint);

    return 0;
}
