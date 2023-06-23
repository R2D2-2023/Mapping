#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
#include <cstdlib>
#include <string>
#include <windows.h>    

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
    std::vector<std::pair<int, int>> meetPunten;
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
void UpdateGridImage(const std::vector<std::vector<int>>& matrix, storeData* data) {
    cv::Scalar grid_color = cv::Scalar(0, 0, 255);
    cv::Scalar grid_color_green = cv::Scalar(0, 255, 0);
    cv::Scalar grid_color_checkpoint = cv::Scalar(255, 0, 255);
    cv::Mat grid_img = data->MData.origImg->clone();
    int cell_size = data->MData.cell_size;

    for (size_t i = 0; i < matrix.size(); i++) {
        int y = data->gData.beginY + i * cell_size;
        for (size_t j = 0; j < matrix[i].size(); j++) {
            int x = data->gData.beginX + j * cell_size;
            int value = matrix[i][j];
            cv::Scalar rect_color;
            switch (value) {
                case 0:
                    rect_color = grid_color;
                    break;
                case 9999:
                    rect_color = cv::Scalar(100, 100, 100);
                    break;
                default:
                    if (value < 0) {
                        rect_color = grid_color_checkpoint;
                    } else {
                        rect_color = grid_color_green;
                    }
                    break;
            }
            cv::rectangle(grid_img, cv::Point(x, y), cv::Point(x + cell_size, y + cell_size), rect_color, (value == 0) ? 1 : -1);
        }
    }
    cv::imshow("Grid Image", grid_img);
}

cv::Mat editImage(){
    // Retrieve the screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    cv::Mat img = cv::imread("floor4.png", cv::IMREAD_COLOR);
    if (img.empty()){
        std::cout << "Could not read the image" << std::endl;
    }

    // Calculate the aspect ratio of the image
    double imageAspectRatio = static_cast<double>(img.cols) / img.rows;

    // Calculate the maximum width and height based on the screen resolution
    int maxWidth = screenWidth;
    int maxHeight = screenHeight;

    // Calculate the scaled width and height while maintaining the aspect ratio
    int scaledWidth = maxWidth;
    int scaledHeight = static_cast<int>(scaledWidth / imageAspectRatio);

    if (scaledHeight > maxHeight) {
        scaledHeight = maxHeight;
        scaledWidth = static_cast<int>(scaledHeight * imageAspectRatio);
    }

    // Calculate the position to center the image
    int posX = (screenWidth - scaledWidth) / 2;
    int posY = (screenHeight - scaledHeight) / 2;

    // Create a window for displaying the image
    // cv::namedWindow("Image", cv::WINDOW_NORMAL);
    cv::moveWindow("Image", posX, posY);

    // Resize the image
    cv::resize(img, img, cv::Size(scaledWidth, scaledHeight));

    return img;
}


int CheckSurroundings(storeData* data){
    bool next_found = false;
    for(int y = data->MData.robotY; y <= data->MData.robotY; y++){
        for(int x = data->MData.robotX; x <= data->MData.robotX; x++){
            // std::cout << "x: " << x << " y: " << y << std::endl;
            if (x < 0 || x >= 65 || y < 0 || y >= 232) {
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
    storeData* data = static_cast<storeData*>(user_data);
    int cell_size = data->MData.cell_size;
    std::vector<std::vector<int>>& matrix = *(data->MData.matrix);
    int matrix_width = matrix[0].size();

    int j = (x - data->gData.beginX) / cell_size;
    int i = (y - data->gData.beginY) / cell_size;

    if (event == cv::EVENT_MOUSEMOVE) {
        if (i >= 0 && i < matrix.size() && j >= 0 && j < matrix_width) {
            auto& current_cell = matrix[i][j];
            if (data->MData.mode == 0) {
                if (flags & cv::EVENT_FLAG_LBUTTON) {
                    if (current_cell == 0 && current_cell != 1) {
                        data->MData.counter++;
                        current_cell = data->MData.counter;
                        data->MData.route.push_back(std::make_pair(i, j));
                        std::cout << j << "/" << i << ": " << data->MData.counter << std::endl;
                        std::cout << "route: ";
                        std::stringstream route_ss;
                        for (const auto& route_coord : data->MData.route) {
                            route_ss << route_coord.first << "/" << route_coord.second << ",";
                        }
                        std::cout << route_ss.str() << std::endl;
                        UpdateGridImage(matrix, data);
                    }
                } else if (flags & cv::EVENT_FLAG_RBUTTON) {
                    if (current_cell != 0) {
                        bool is_in_route_or_meetpunten = false;
                        auto route_it = std::find(data->MData.route.begin(), data->MData.route.end(), std::make_pair(i, j));
                        auto meetpunten_it = std::find(data->MData.meetPunten.begin(), data->MData.meetPunten.end(), std::make_pair(i, j));
                        if (route_it != data->MData.route.end() || meetpunten_it != data->MData.meetPunten.end()) {
                            is_in_route_or_meetpunten = true;
                            if (route_it != data->MData.route.end()) {
                                data->MData.route.erase(route_it);
                            }
                            if (meetpunten_it != data->MData.meetPunten.end()) {
                                data->MData.meetPunten.erase(meetpunten_it);
                            }
                        }
                        if (!is_in_route_or_meetpunten) {
                            current_cell = 0;
                            UpdateGridImage(matrix, data);
                        }
                    }
                }
                CheckSurroundings(data);
            } else {
                if (flags & cv::EVENT_FLAG_LBUTTON) {
                    current_cell = 9999;
                    data->MData.robotX = i;
                    data->MData.robotY = j;
                    UpdateGridImage(matrix, data);
                } else if (flags & cv::EVENT_FLAG_RBUTTON) {
                    if (current_cell == 0) {
                        data->MData.checkpoint_counter--;
                        current_cell = data->MData.checkpoint_counter;
                        data->MData.meetPunten.push_back(std::make_pair(i, j));
                        std::cout << "meetPunten: ";
                        std::stringstream meetpunten_ss;
                        for (const auto& meetpunten_coord : data->MData.meetPunten) {
                            meetpunten_ss << meetpunten_coord.first << "/" << meetpunten_coord.second << ",";
                        }
                        std::cout << meetpunten_ss.str() << std::endl;
                        UpdateGridImage(matrix, data);
                    }
                }
                CheckSurroundings(data);
            }
        }
    } else if (event == cv::EVENT_MOUSEWHEEL) {
        if (i >= 0 && i < matrix.size() && j >= 0 && j < matrix_width) {
            std::cout << data->MData.mode << std::endl;
            data->MData.mode = (data->MData.mode == 0) ? 1 : 0;
            std::cout << "mode: " << data->MData.mode << std::endl;
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
    int longest_dim = std::max(img.cols, img.rows);
    int cell_size = std::ceil((double)longest_dim / 300);

    storeData uData;
    uData.gData.beginX = begin.x;
    uData.gData.beginY = begin.y;
    uData.gData.endX = end.x;
    uData.gData.endY = end.y;

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
    std::cout << "" << std::endl;
    UpdateGridImage(matrix, &uData);

    cv::waitKey(0); 

    return matrix;
}


int main(){
    cv::Mat img = editImage();

    cv::Mat hsv_img; 
    cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);

    // Define the lower and upper bounds of the black color range
    cv::Scalar lower(0, 0, 0);
    cv::Scalar upper(180, 255, 30);

    // Create a mask to filter out non-black pixels
    cv::Mat mask;
    cv::inRange(hsv_img, lower, upper, mask);
    // Apply the mask to the original image
    cv::Mat maskImg;
    img.copyTo(maskImg, mask);


    // Find the coordinates of black pixels
    std::vector<cv::Point> coords;
    for (int y = 0; y < mask.rows; ++y) {
        for (int x = 0; x < mask.cols; ++x) {
            if (mask.at<uchar>(y, x) == 255) {
                coords.emplace_back(x, y);
            }
        }
    }

    // Find the highest, lowest, leftmost, and rightmost points
    cv::Point highestPoint = coords[0];
    cv::Point lowestPoint = coords[0];
    cv::Point leftmostPoint = coords[0];
    cv::Point rightmostPoint = coords[0];

    for (const auto& coord : coords) {
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
    
    std::vector<std::vector<int>> matrix = CreateGridImage(maskImg, img, startPoint, endPoint);

    return 0;
}
