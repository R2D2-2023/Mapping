# Mapping
Dit C++ programma stelt je in staat om een raster te maken van een opgegeven afbeelding, waarop je vervolgens een route kunt tekenen. Het programma maakt gebruik van OpenCV-bibliotheek om afbeeldingen te verwerken en de grafische interface te tonen.

## Vereisten
1. C++17 of hoger
2. OpenCV-bibliotheek

## Installatie
1. In de map install staat zip bestand die eerst ge installeerd moeten worden in je C:\ disk
2. daarna moet je https://www.mingw-w64.org/downloads/ via die link mingw64 dowloaden en installeren op je C:\ disk
3. om de code the builden met de commando in de terminal: g++ -g main.cpp -o main -I C:\OpenCV-MinGW-Build-OpenCV-4.1.1-x64\include -L C:\OpenCV-MinGW-Build-OpenCV-4.1.1-x64\x64\mingw\bin -llibopencv_calib3d411 -llibopencv_core411 -llibopencv_dnn411 -llibopencv_features2d411 -llibopencv_flann411 -llibopencv_highgui411 -llibopencv_imgcodecs411 -llibopencv_imgproc411 -llibopencv_ml411 -llibopencv_objdetect411 -llibopencv_photo411 -llibopencv_stitching411 -llibopencv_video411 -llibopencv_videoio411
