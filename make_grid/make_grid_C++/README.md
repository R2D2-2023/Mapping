# Mapping
This C++ program allows you to create a grid from a given image and then draw a route on it. The program utilizes the OpenCV library to process images and display the graphical interface.

## Vereisten
1. C++17 or higher
2. OpenCV library

## Installatie
1. In the "install" folder, there is a zip file that needs to be installed on your C:\ disk.
2. Then, you need to download and install mingw64 from the following link: https://www.mingw-w64.org/downloads/. Install it on your C:\ disk.
3. To build the code, use the following command in the terminal:
```
 g++ -g main.cpp -o main -I C:\OpenCV-MinGW-Build-OpenCV-4.1.1-x64\include -L C:\OpenCV-MinGW-Build-OpenCV-4.1.1-x64\x64\mingw\bin -llibopencv_calib3d411 -llibopencv_core411 -llibopencv_dnn411 -llibopencv_features2d411 -llibopencv_flann411 -llibopencv_highgui411 -llibopencv_imgcodecs411 -llibopencv_imgproc411 -llibopencv_ml411 -llibopencv_objdetect411 -llibopencv_photo411 -llibopencv_stitching411 -llibopencv_video411 -llibopencv_videoio411
 ```
