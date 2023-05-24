#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
#include <cstdlib>
#include <string>
#include <Windows.h>

/**
 * @brief for data that the mouse needs
 * 
 */
struct MouseData {
    std::vector<std::vector<int>>* matrix;
    int cell_size;
    cv::Mat* img;
};

struct GridData {
    int beginX;
    int beginY;
    int endX;
    int endY;
};
int counter = 1;


/**
 * @brief for the grid making and updating
 * 
 * @param img 
 * @param cell_size 
 * @param matrix 
 */
void UpdateGridImage(cv::Mat& img, int cell_size, std::vector<std::vector<int>>& matrix) {
    cv::Scalar grid_color = cv::Scalar(0, 0, 255);
    cv::Scalar grid_color_green = cv::Scalar(0, 255, 0);
    cv::Mat grid_img = img.clone();
    for (int i = 0; i < matrix.size(); i++){
        int y = i * cell_size;
        for (int j = 0; j < matrix.at(0).size(); j++){
            int x = j * cell_size;
            
            if(matrix[i][j] == 0){
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color, 1);
                
            }else if(matrix[i][j] == 10){
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), cv::Scalar(255, 0, 0), -1);
            }
            else{
                cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color_green, -1);
            }
        }
    }

    cv::imshow("Grid Image", grid_img);
}

int CheckSurroundings(std::vector<std::vector<int>>* matrix){
    int markerX = 20;
    int markerY = 20;
    int highest_number = -1;
    int nextXcoordinate = 0;
    int nextYcoordinate = 0;
    if(matrix->at(markerX).at(markerY) == 555){
    // Check the surrounding blocks
    for (int y = markerY - 1; y <= markerY + 1; ++y) {
        for (int x = markerX - 1; x <= markerX + 1; ++x) {
            // Skip if the current position is out of bounds
            if (x < 0 || x >= 173 || y < 0 || y >= 491) {
                continue;
            }

            // Check the value at the current position
            int value = matrix->at(x).at(y);

            // Perform actions based on the value
            if (value == 0) {
                continue;
            } else if (value != 555) {
                if(highest_number < value){
                    highest_number = value;
                    nextXcoordinate = y;
                    nextYcoordinate = x;

                }
                // std::cout << "Block at (" << x << ", " << y << ") contains a marker with the value: " << value << std::endl;
            }
        }
    }
    }
    
    if(highest_number != -1){
        std::cout << "The highest number is: " << highest_number << " at the coordinates x/y: " << nextXcoordinate << "/" << nextYcoordinate << std::endl;
    }
    return 0;
}

bool IsKeyPressed(int key)
{
    return (GetAsyncKeyState(key) & 0x8000) != 0;
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
    MouseData* data = (MouseData*)user_data;
    int cell_size = data->cell_size;
    std::vector<std::vector<int>>* matrix = data->matrix;
    int j = x / cell_size;
    int i = y / cell_size;
    if(event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON)){
        if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
            if(matrix->at(i).at(j) == 0){
                counter++;
                matrix->at(i).at(j) = counter;
                std::cout << j << "/" << i << ": " << counter << std::endl;
                
                if(counter == 20){
                    matrix->at(20).at(20) = 555;
                }
                UpdateGridImage(*data->img, cell_size, *matrix);
            }
        CheckSurroundings(matrix);
        }
    } 
    else if(event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_RBUTTON)){
         if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
            matrix->at(i).at(j) = 0;
            UpdateGridImage(*data->img, cell_size, *matrix);
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
std::vector<std::vector<int>> CreateGridImage(cv::Mat& img, cv::Point begin, cv::Point end){
    cv::namedWindow("Grid Image", cv::WINDOW_NORMAL);

    int longest_dim = std::max(img.cols, img.rows);
    int cell_size = std::ceil((double)longest_dim / 120);
    // int size = std::ceil((double)longest_dim / cell_size);

    GridData gData;
    gData.beginX = begin.x;
    gData.beginY = begin.y;
    gData.endX = end.x;
    gData.endY = end.y; 


    // std::cout << begin.y << std::endl;
    // std::cout << begin.x << std::endl;
    // std::cout << begin.y << std::endl;
    // std::cout << end.x << std::endl;
    // std::cout << end.y << std::endl;
    // std::cout << cell_size << std::endl;
    // std::cout << (end.x - begin.x) << std::endl;
    // std::cout << (end.y - begin.y) << std::endl;


    cv::Rect roi(begin.x, begin.y, end.x - begin.x, end.y - begin.y);
    cv::Mat roiImage = img(roi);

    int longest_dim_roi = std::max(roiImage.cols, roiImage.rows);
    int size = std::ceil((double)longest_dim_roi / cell_size);


    std::vector<std::vector<int>> matrix( size , std::vector<int> (size, 0));

    MouseData data;
    data.matrix = &matrix;
    data.cell_size = cell_size;
    data.img = &img;

    cv::setMouseCallback("Grid Image", OnMouse, &data);
    UpdateGridImage(img, cell_size, matrix);

    cv::waitKey(0);

    return matrix;
}


int main(){
    cv::Mat img = cv::imread("image.png", cv::IMREAD_COLOR);
    if (img.empty()){
        std::cout << "Could not read the image" << std::endl;
        return -1;
    }

    cv::Mat hsv_img; 
    cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);
    // Define the lower and upper bounds of the green color range
    cv::Scalar lowerGreen(30, 40, 40);
    cv::Scalar upperGreen(90, 255, 255);
    // Create a mask to filter out non-green pixels
    cv::Mat greenMask;
    cv::inRange(hsv_img, lowerGreen, upperGreen, greenMask);
    // Apply the mask to the original image
    cv::Mat greenImg;
    img.copyTo(greenImg, greenMask);


    // Find the coordinates of green pixels
    std::vector<cv::Point> greenCoords;
    for (int y = 0; y < greenMask.rows; ++y) {
        for (int x = 0; x < greenMask.cols; ++x) {
            if (greenMask.at<uchar>(y, x) == 255) {
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
    if (IsKeyPressed('z'))
        {
            std::cout << "Key Z is pressed" << std::endl;
        }

    std::vector<std::vector<int>> matrix = CreateGridImage(greenImg, startPoint, endPoint);

    return 0;
}

// /**
//  * @brief for data that the mouse needs
//  * 
//  */
// struct MouseData {
//     std::vector<std::vector<int>>* matrix;
//     int cell_size;
//     cv::Mat* img;
// };

// /**
//  * @brief for the grid making and updating
//  * 
//  * @param img 
//  * @param cell_size 
//  * @param matrix 
//  */
// void UpdateGridImage(cv::Mat& img, int cell_size, std::vector<std::vector<int>>& matrix) {
//     cv::Scalar grid_color = cv::Scalar(0, 0, 255);
//     cv::Scalar grid_color_green = cv::Scalar(0, 255, 0);
//     cv::Mat grid_img = img.clone();
//     for (int i = 0; i < matrix.size(); i++){
//         int y = i * cell_size;
//         for (int j = 0; j < matrix.at(0).size(); j++){
//             int x = j * cell_size;
//             if(matrix[i][j] == 1){
//                 cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color_green, -1);
//             }
//             else{
//                 cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), grid_color, 1);
//             }
//         }
//     }

//     cv::imshow("Grid Image", grid_img);
// }

// /**
//  * @brief For mouse movement and events
//  * 
//  * @param event 
//  * @param x 
//  * @param y 
//  * @param flags 
//  * @param user_data 
//  */
// void OnMouse(int event, int x, int y, int flags, void* user_data) {
//     MouseData* data = (MouseData*)user_data;
//     int cell_size = data->cell_size;
//     std::vector<std::vector<int>>* matrix = data->matrix;
//     int j = x / cell_size;
//     int i = y / cell_size;
//     if(event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON)){
//         if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
//             matrix->at(i).at(j) = 1;
//             UpdateGridImage(*data->img, cell_size, *matrix);
//         }
//     } 
//     else if(event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_RBUTTON)){
//          if (i >= 0 && i < matrix->size() && j >= 0 && j < matrix->at(0).size()) {
//             matrix->at(i).at(j) = 0;
//             UpdateGridImage(*data->img, cell_size, *matrix);
//         }
//     }
// }

// /**
//  * @brief Create a Grid Image object
//  * 
//  * @param img 
//  * @param cell_size 
//  * @param matrix 
//  * @return std::vector<std::vector<int>> 
//  */
// std::vector<std::vector<int>> CreateGridImage(cv::Mat& img, int cell_size, std::vector<std::vector<int>> matrix){
//     cv::namedWindow("Grid Image", cv::WINDOW_NORMAL);

//     MouseData data;
//     data.matrix = &matrix;
//     data.cell_size = cell_size;
//     data.img = &img;

//     cv::setMouseCallback("Grid Image", OnMouse, &data);
//     UpdateGridImage(img, cell_size, matrix);

//     cv::waitKey(0);

//     return matrix;
// }


// int main(){
//     // Load the image
//     cv::Mat image = cv::imread("image.png");

//     if (image.empty()) {
//         std::cout << "Failed to load the image." << std::endl;
//         return -1;
//     }

//     // Convert the image from BGR to HSV color space
//     cv::Mat hsvImage;
//     cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

//     // Define the lower and upper bounds for green in HSV color space
//     cv::Scalar lowerGreen(40, 50, 50);    // Lower hue, saturation, and value thresholds for green
//     cv::Scalar upperGreen(80, 255, 255);  // Upper hue, saturation, and value thresholds for green

//     // Threshold the image to obtain only green pixels
//     cv::Mat greenMask;
//     cv::inRange(hsvImage, lowerGreen, upperGreen, greenMask);

//     std::vector<cv::Point> greenPixelPositions;
//     cv::findNonZero(greenMask, greenPixelPositions);

//     // Print the pixel positions of the detected green pixels
//     for (const cv::Point& position : greenPixelPositions) {
//         std::cout << "Green pixel position: (" << positio   n.x << ", " << position.y << ")" << std::endl;
//     }

//     // Show the original image and the green mask
//     cv::imshow("Original Image", image);
//     cv::imshow("Green Mask", greenMask);
//     cv::waitKey(0);

//     return 0;
// }
