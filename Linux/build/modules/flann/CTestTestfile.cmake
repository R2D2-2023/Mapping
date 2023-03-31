# CMake generated Testfile for 
# Source directory: /home/r2d2/Desktop/opencvTest/opencv/modules/flann
# Build directory: /home/r2d2/Desktop/opencvTest/build/modules/flann
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_flann "/home/r2d2/Desktop/opencvTest/build/bin/opencv_test_flann" "--gtest_output=xml:opencv_test_flann.xml")
set_tests_properties(opencv_test_flann PROPERTIES  LABELS "Main;opencv_flann;Accuracy" WORKING_DIRECTORY "/home/r2d2/Desktop/opencvTest/build/test-reports/accuracy" _BACKTRACE_TRIPLES "/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVUtils.cmake;1763;add_test;/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVModule.cmake;1375;ocv_add_test_from_target;/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVModule.cmake;1133;ocv_add_accuracy_tests;/home/r2d2/Desktop/opencvTest/opencv/modules/flann/CMakeLists.txt;2;ocv_define_module;/home/r2d2/Desktop/opencvTest/opencv/modules/flann/CMakeLists.txt;0;")
