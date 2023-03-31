# CMake generated Testfile for 
# Source directory: /home/r2d2/Desktop/opencvTest/opencv/modules/core
# Build directory: /home/r2d2/Desktop/opencvTest/build/modules/core
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_core "/home/r2d2/Desktop/opencvTest/build/bin/opencv_test_core" "--gtest_output=xml:opencv_test_core.xml")
set_tests_properties(opencv_test_core PROPERTIES  LABELS "Main;opencv_core;Accuracy" WORKING_DIRECTORY "/home/r2d2/Desktop/opencvTest/build/test-reports/accuracy" _BACKTRACE_TRIPLES "/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVUtils.cmake;1763;add_test;/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVModule.cmake;1375;ocv_add_test_from_target;/home/r2d2/Desktop/opencvTest/opencv/modules/core/CMakeLists.txt;175;ocv_add_accuracy_tests;/home/r2d2/Desktop/opencvTest/opencv/modules/core/CMakeLists.txt;0;")
add_test(opencv_perf_core "/home/r2d2/Desktop/opencvTest/build/bin/opencv_perf_core" "--gtest_output=xml:opencv_perf_core.xml")
set_tests_properties(opencv_perf_core PROPERTIES  LABELS "Main;opencv_core;Performance" WORKING_DIRECTORY "/home/r2d2/Desktop/opencvTest/build/test-reports/performance" _BACKTRACE_TRIPLES "/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVUtils.cmake;1763;add_test;/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVModule.cmake;1274;ocv_add_test_from_target;/home/r2d2/Desktop/opencvTest/opencv/modules/core/CMakeLists.txt;176;ocv_add_perf_tests;/home/r2d2/Desktop/opencvTest/opencv/modules/core/CMakeLists.txt;0;")
add_test(opencv_sanity_core "/home/r2d2/Desktop/opencvTest/build/bin/opencv_perf_core" "--gtest_output=xml:opencv_perf_core.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_core PROPERTIES  LABELS "Main;opencv_core;Sanity" WORKING_DIRECTORY "/home/r2d2/Desktop/opencvTest/build/test-reports/sanity" _BACKTRACE_TRIPLES "/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVUtils.cmake;1763;add_test;/home/r2d2/Desktop/opencvTest/opencv/cmake/OpenCVModule.cmake;1275;ocv_add_test_from_target;/home/r2d2/Desktop/opencvTest/opencv/modules/core/CMakeLists.txt;176;ocv_add_perf_tests;/home/r2d2/Desktop/opencvTest/opencv/modules/core/CMakeLists.txt;0;")
