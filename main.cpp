#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>

int main() {
    // Retrieve the screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // Load the image
    cv::Mat image = cv::imread("Toerk_map.png", cv::IMREAD_COLOR);
    if (image.empty()){
        std::cout << "Could not read the image" << std::endl;
        return -1;
    }

    // Calculate the aspect ratio of the image
    double imageAspectRatio = static_cast<double>(image.cols) / image.rows;

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
    cv::namedWindow("Image", cv::WINDOW_NORMAL);
    cv::moveWindow("Image", posX, posY);

    // Resize the image
    cv::resize(image, image, cv::Size(scaledWidth, scaledHeight));

    // Display the image
    cv::imshow("Image", image);
    cv::waitKey(0);

    return 0;
}