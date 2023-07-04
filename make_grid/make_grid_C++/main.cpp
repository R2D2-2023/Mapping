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
    cv::Mat* orig_Img;
    int counter = 0;
    int checkpoint_counter = -5;
    std::vector<std::pair<int, int>> route;
    std::vector<std::pair<int, int>> meetPunten;
    int robotX = 10;
    int robotY = 10;
    int mode = 0;
};

struct GridData {
    int begin_X = 0;
    int begin_Y = 0;
    int end_X = 0;
    int end_Y = 0;
};

struct StoreData {
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
void updateGridImage(const std::vector<std::vector<int>>& matrix, StoreData* data) {
    cv::Scalar grid_color = cv::Scalar(0, 0, 255);
    cv::Scalar grid_color_green = cv::Scalar(0, 255, 0);
    cv::Scalar grid_color_checkpoint = cv::Scalar(255, 0, 255);
    cv::Mat grid_img = data->MData.orig_Img->clone();
    int cell_size = data->MData.cell_size;

    for (size_t i = 0; i < matrix.size(); i++) {
        int y = data->gData.begin_Y + i * cell_size;
        for (size_t j = 0; j < matrix[i].size(); j++) {
            int x = data->gData.begin_X + j * cell_size;
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

/**
 * @brief for editing the image size
 * 
 * @return img scaled with screen size 
 */
cv::Mat editImage(){
    // Retrieve the screen resolution
    int screen_Width = GetSystemMetrics(SM_CXSCREEN);
    int screen_Height = GetSystemMetrics(SM_CYSCREEN);

    cv::Mat img = cv::imread("floor4.png", cv::IMREAD_COLOR);
    if (img.empty()){
        std::cout << "Could not read the image" << std::endl;
    }

    // Calculate the aspect ratio of the image
    double image_Aspect_Ratio = static_cast<double>(img.cols) / img.rows;

    // Calculate the maximum width and height based on the screen resolution
    int max_Width = screen_Width;
    int max_Height = screen_Height;

    // Calculate the scaled width and height while maintaining the aspect ratio
    int scaled_Width = max_Width;
    int scaled_Height = static_cast<int>(scaled_Width / image_Aspect_Ratio);

    if (scaled_Height > max_Height) {
        scaled_Height = max_Height;
        scaled_Width = static_cast<int>(scaled_Height * image_Aspect_Ratio);
    }

    // Calculate the position to center the image
    int pos_X = (screen_Width - scaled_Width) / 2;
    int pos_Y = (screen_Height - scaled_Height) / 2;

    // Create a window for displaying the image
    // cv::namedWindow("Image", cv::WINDOW_NORMAL);
    cv::moveWindow("Image", pos_X, pos_Y);

    // Resize the image
    cv::resize(img, img, cv::Size(scaled_Width, scaled_Height));

    return img;
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
void onMouse(int event, int x, int y, int flags, void* user_data) {
    StoreData* data = static_cast<StoreData*>(user_data);
    int cell_size = data->MData.cell_size;
    std::vector<std::vector<int>>& matrix = *(data->MData.matrix);
    int matrix_width = matrix[0].size();

    int j = (x - data->gData.begin_X) / cell_size;
    int i = (y - data->gData.begin_Y) / cell_size;

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
                        updateGridImage(matrix, data);
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
                            updateGridImage(matrix, data);
                        }
                    }
                }
            } else {
                if (flags & cv::EVENT_FLAG_LBUTTON) {
                    current_cell = 9999;
                    data->MData.robotX = i;
                    data->MData.robotY = j;
                    updateGridImage(matrix, data);
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
                        updateGridImage(matrix, data);
                    }
                }
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
std::vector<std::vector<int>> createGridImage(cv::Mat& img, cv::Mat& orig_Img, cv::Point begin, cv::Point end){
    cv::namedWindow("Grid Image");

    int longest_dim = std::max(img.cols, img.rows);
    int cell_size = std::ceil((double)longest_dim / 300);

    StoreData uData;
    uData.gData.begin_X = begin.x;
    uData.gData.begin_Y = begin.y;
    uData.gData.end_X = end.x;
    uData.gData.end_Y = end.y;

    cv::Rect roi(begin.x, begin.y, end.x - begin.x, end.y - begin.y);
    cv::Mat roiImage = img(roi);

    int width = std::ceil((double)roiImage.cols / cell_size);
    int height = std::ceil((double)roiImage.rows / cell_size);

    std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 0));

    uData.MData.matrix = &matrix;
    uData.MData.cell_size = cell_size;
    uData.MData.img = &img;
    uData.MData.orig_Img = &orig_Img;

    cv::setMouseCallback("Grid Image", onMouse, &uData);
    updateGridImage(matrix, &uData);

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
    cv::Point highest_Point = coords[0];
    cv::Point lowest_Point = coords[0];
    cv::Point left_most_Point = coords[0];
    cv::Point right_most_Point = coords[0];

    for (const auto& coord : coords) {
        if (coord.y < highest_Point.y) {
            highest_Point = coord;
        }
        if (coord.y > lowest_Point.y) {
            lowest_Point = coord;
        }
        if (coord.x < left_most_Point.x) {
            left_most_Point = coord;
        }
        if (coord.x > right_most_Point.x) {
            right_most_Point = coord;
        }
    }

    cv::Point start_Point(left_most_Point.x, highest_Point.y);  // Starting coordinate of the grid
    cv::Point end_Point(right_most_Point.x, lowest_Point.y);  // Ending coordinate of the grid
    
    std::vector<std::vector<int>> matrix = createGridImage(maskImg, img, start_Point, end_Point);

    return 0;
}
