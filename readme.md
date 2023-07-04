# Mapping
This repository contains code in Python and C++ that allows you to create a grid from a given image, draw on it, and save the drawn values. Additionally, the Python code includes functionality to read the saved route and visualize it. The code utilizes the OpenCV library for image processing and manipulation.



## Prerequisites
- Python 3.x
- C++ compiler
- OpenCV library


## installation
1. Clone the repository:
- git clone https://github.com/R2D2-2023/Mapping.git
- Alternatively, you can download the repository as a ZIP file and extract it.

2. Navigate to the "Ros" directory and follow the instructions in the [README](Ros/README.md) file to download and set up the required dependencies for the Lidar-based mapping module.

## Usage

### Python Code

1. Install the necessary Python packages:
- pip install opencv-python numpy
2. Run the Python script to generate a grid from an image and draw on it
3. Save the drawn values on closing the program
4. run read.py to visualize the saved route
- The script will display the input image with the drawn route highlighted.

### C++ Code
1. Compile the C++ code using your preferred compiler. Make sure OpenCV is properly installed and linked and use this command to compile:
```
  g++ -g main.cpp -o main -I C:\OpenCV-MinGW-Build-OpenCV-4.1.1-x64\include -L C:\OpenCV-MinGW-Build-OpenCV-4.1.1-x64\x64\mingw\bin -llibopencv_calib3d411 -llibopencv_core411 -llibopencv_dnn411 -llibopencv_features2d411 -llibopencv_flann411 -llibopencv_highgui411 -llibopencv_imgcodecs411 -llibopencv_imgproc411 -llibopencv_ml411 -llibopencv_objdetect411 -llibopencv_photo411 -llibopencv_stitching411 -llibopencv_video411 -llibopencv_videoio411
```


2. Run the compiled executable.
The program will display the image with a grid overlaid on it. use your mouse to draw and the scroll wheel to change the drawing modes.
3. Save the drawn values by closing the program

## Lidar-based Mapping (Ros)
For the Lidar-based mapping module, please refer to the [Ros](Ros) directory for detailed instructions on how to download and use it.

## Authors
- [@BasvandenBergh](https://github.com/BasvandenBergh)
- [@jamesonLin](https://www.github.com/jamesonLin)
- [@Jorismaas](https://github.com/Jorismaas)
- [@JSaurusRex](https://github.com/JSaurusRex)
- [@LukkenS-HU](https://github.com/LukkenS-HU)

## License
[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)
[![AGPL License](https://img.shields.io/badge/license-AGPL-blue.svg)](http://www.gnu.org/licenses/agpl-3.0)



## Acknowledgements
- [OpenCV](https://opencv.org/) - Open Source Computer Vision Library
- [cartographer](https://github.com/cartographer-project/cartographer)  - Open Source Lidar Library

Please refer to the individual code files for specific attributions and licenses.

