#include "CYdLidar.h"
#include <string>
#include <stdio.h>
#include <raylib.h>
#include <math.h>
using namespace std;
using namespace ydlidar;

/**
 * @file visualDemo.cpp
 * @brief visual demo, allows you to make a map but not save it
 *
 * Lidar type: ydlidar x2l
 * everything is rendered using raylib (gpu accelerated) and renders to a render texture until the final step
 * A blur filter is included for image similiarty detection, which meant to be used for automated mapping
 * WASD can be used to move and space to add it to the map render texture
 */


#if defined(_MSC_VER)
#pragma comment(lib, "ydlidar_sdk.lib")
#endif

int main(int argc, char *argv[]) {
  // init system signal
  ydlidar::os_init();

  InitWindow(800, 800, "visual lidar demo");
  // SetTargetFPS(60);

  CYdLidar laser;
  //////////////////////string property/////////////////
  /// Lidar ports
  std::map<std::string, std::string> ports = ydlidar::lidarPortList();
  std::string port = "/dev/ydlidar";
  if(!ports.empty()) {
      port = ports.begin()->second;
  }

  //manually put the port here because it couldn't be found
  // /dev/ttyUSB0
  // these udev rules are recommended for linux https://github.com/YDLIDAR/YDLidar-SDK/blob/master/doc/howto/how_to_create_a_udev_rules.md
  port = "/dev/ttyUSB0";
  /// lidar port
  laser.setlidaropt(LidarPropSerialPort, port.c_str(), port.size());
  /// ignore array
  std::string ignore_array;
  ignore_array.clear();
  laser.setlidaropt(LidarPropIgnoreArray, ignore_array.c_str(),
                    ignore_array.size());

  //////////////////////int property/////////////////
  /// lidar baudrate
  int optval = 230400;

  //settings for x2, lower baudrate
  optval = 115200;
  laser.setlidaropt(LidarPropSerialBaudrate, &optval, sizeof(int));
  /// tof lidar
  optval = TYPE_TRIANGLE;
  laser.setlidaropt(LidarPropLidarType, &optval, sizeof(int));
  /// device type
  optval = YDLIDAR_TYPE_SERIAL;
  laser.setlidaropt(LidarPropDeviceType, &optval, sizeof(int));
  /// sample rate
  optval = 9;
  laser.setlidaropt(LidarPropSampleRate, &optval, sizeof(int));
  /// abnormal count
  optval = 4;
  laser.setlidaropt(LidarPropAbnormalCheckCount, &optval, sizeof(int));

  //////////////////////bool property/////////////////
  /// fixed angle resolution
  bool b_optvalue = false;
  laser.setlidaropt(LidarPropFixedResolution, &b_optvalue, sizeof(bool));
  /// rotate 180
  laser.setlidaropt(LidarPropReversion, &b_optvalue, sizeof(bool));
  /// Counterclockwise
  laser.setlidaropt(LidarPropInverted, &b_optvalue, sizeof(bool));
  b_optvalue = true;
  laser.setlidaropt(LidarPropAutoReconnect, &b_optvalue, sizeof(bool));
  /// one-way communication
  b_optvalue = false;

  //x2 doesn't do two-way communication
  b_optvalue = true;
  laser.setlidaropt(LidarPropSingleChannel, &b_optvalue, sizeof(bool));
  /// intensity
  b_optvalue = false;
  laser.setlidaropt(LidarPropIntenstiy, &b_optvalue, sizeof(bool));
  /// Motor DTR
  b_optvalue = false;
  laser.setlidaropt(LidarPropSupportMotorDtrCtrl, &b_optvalue, sizeof(bool));
  /// HeartBeat
  b_optvalue = false;
  laser.setlidaropt(LidarPropSupportHeartBeat, &b_optvalue, sizeof(bool));


  //////////////////////float property/////////////////
  /// unit: °
  float f_optvalue = 180.0f;
  laser.setlidaropt(LidarPropMaxAngle, &f_optvalue, sizeof(float));
  f_optvalue = -180.0f;
  laser.setlidaropt(LidarPropMinAngle, &f_optvalue, sizeof(float));
  /// unit: m
  f_optvalue = 16.f;
  laser.setlidaropt(LidarPropMaxRange, &f_optvalue, sizeof(float));
  f_optvalue = 0.1f;
  laser.setlidaropt(LidarPropMinRange, &f_optvalue, sizeof(float));
  /// unit: Hz
  f_optvalue = 10.f;
  laser.setlidaropt(LidarPropScanFrequency, &f_optvalue, sizeof(float));

  // initialize SDK and LiDAR
  bool ret = laser.initialize();
  if (ret) {//success
    //Start the device scanning routine which runs on a separate thread and enable motor.
    ret = laser.turnOn();
  } else {
    fprintf(stderr, "%s\n", laser.DescribeError());
    fflush(stderr);
  }

  RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  RenderTexture2D blurTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  RenderTexture2D map = LoadRenderTexture(GetScreenWidth()*4, GetScreenHeight()*4);

  Shader blurShader = LoadShader(0, "../blurShader.fs");

  // Turn On success and loop  
  while (ret && ydlidar::os_isOk() && !WindowShouldClose()) {

    //do lidar scan
    LaserScan scan;
    if (laser.doProcessSimple(scan)) {
      // ...
    } else {
      fprintf(stderr, "Failed to get Lidar Data\n");
      fflush(stderr);
    }

    //begin render texture mode (only being drawn to said texture)
    BeginTextureMode(target);

    //clear render texture to black
    ClearBackground((Color){0,0,0,0});

    //half clear render texture
    // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 125});

    //variables used for simple point filtering
    const int pointsCount = 5;
    Vector2 lastPoints [pointsCount] = {0};
    int lastPointsIndex = -1;

    //go through every point from scan
    for(int i = 0; i < scan.points.size(); i++)
    {
      lastPointsIndex++;
      LaserPoint point = scan.points[i];


      //if space is pressed print all angles
      if(IsKeyPressed(KEY_SPACE))
        printf("point %i: {angle: %.2f, range: %.2f, intensity %.2f}\n", i, point.angle, point.range, point.intensity);
      
      //calculate position
      float x = sin(-point.angle);
      float y = cos(-point.angle);

      x *= point.range;
      y *= point.range;


      //if given intensity is too low skip point

      if(point.intensity < 0.1)
        continue;

      float maxDist = .8;
      int distCounter = 0;

      //simple filter
      //if any point isn't close enough for the last 5 points
      //skip

      for(int j = 0; j < pointsCount; j++)
      {
        if(abs(lastPoints[j].x - x) < maxDist)
        {
          if(abs(lastPoints[j].y - y) < maxDist)
          {
            distCounter++;
          }
        } 
      }

      bool tooFar = distCounter > 1 ? false : true;


      //replace point in array
      lastPoints[lastPointsIndex%pointsCount] = (Vector2){x, y};

      //skip when filter applies
      if(tooFar)
        continue;

      

      //transform onto screen

      float maxDistance = 7;

      x /= maxDistance * 2;
      y /= maxDistance * 2;

      x *= GetScreenWidth();
      y *= GetScreenHeight();

      x += GetScreenWidth()/2;
      y += GetScreenHeight()/2;


      //draw point
      DrawCircle(x, y, 2, (Color){255, 255, 255, sqrt(point.intensity) *255});
    }

    //draw middle
    // DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 8, GREEN);

    EndTextureMode();

    //blur target render texture to be used in image comparison

    BeginTextureMode(blurTarget);
      ClearBackground(BLACK);
      BeginShaderMode(blurShader);
        DrawTexture(target.texture, 0, 0, WHITE);
      EndShaderMode();
    EndTextureMode();


    //load gpu textures from gpu to cpu (very slow, only for demo)

    Image blurImage = LoadImageFromTexture(blurTarget.texture);
    Image mapImage = LoadImageFromTexture(map.texture);

    int pixelCounter = 0;
    int value = 0;

    //get all pixels from image

    Color * blurPixels = LoadImageColors(blurImage);
    Color * mapPixels = LoadImageColors(mapImage);

    //image transforms

    static Vector2 imageOffset = {map.texture.width/2-GetScreenWidth()/2, map.texture.height/2-GetScreenHeight()/2};
    static float imageRotation = 0;

    int speed = 10;
    int rotationSpeed = 3;

    Vector2 addedOffset = {0};

    //manual user movement and rotation of the image

    if(IsKeyDown(KEY_W))
      addedOffset.y += speed;

    if(IsKeyDown(KEY_S))
      addedOffset.y -= speed;

    if(IsKeyDown(KEY_A))
      addedOffset.x += speed;

    if(IsKeyDown(KEY_D))
      addedOffset.x -= speed;

    if(IsKeyDown(KEY_Z))
      imageRotation -= rotationSpeed;

    if(IsKeyDown(KEY_C))
      imageRotation += rotationSpeed;

    float radians = imageRotation*DEG2RAD;

    
    imageOffset.x += cos(radians)*addedOffset.x - sin(radians)*addedOffset.y;
    imageOffset.y += cos(radians)*addedOffset.y + sin(radians)*addedOffset.x;

    //image comparison
    //go through every pixel of scan and compare to map

    for(int x = 0; x < 800; x++)
    {
      for(int y = 0; y < 800; y++)
      {
        Color pixelBlur = blurPixels[x+y*800];

        int x2 = x + imageOffset.x;
        int y2 = y + imageOffset.y;
        Color pixelMap = mapPixels[x2+y2*800];


        //skip black pixels on map for comparison
        if(pixelMap.r < 20) //threshhold
          continue;

        //check distance
        value += abs(pixelMap.r - pixelBlur.r);
        pixelCounter++;
      }
    }

    //unload images and colors (fixes memory leak and out of memory crash)

    UnloadImageColors(blurPixels);
    UnloadImageColors(mapPixels);

    UnloadImage(blurImage);
    UnloadImage(mapImage);

    //get average difference for pixels

    float average = (float)value / (float)pixelCounter;

    printf("average distance: %f\n", average);

    static int counter = 999;

    counter++;

    //if space is pressed add the next 5 scans to the map

    if(IsKeyPressed(KEY_SPACE))
      counter = 0;


    //add to map
    if(counter < 5)
    {
      BeginTextureMode(map);
      // DrawTextureEx(target.texture, imageOffset, imageRotation, 1, WHITE);

      DrawTexturePro(target.texture, (Rectangle){0,0, target.texture.width, target.texture.height},
          (Rectangle){imageOffset.x+GetScreenWidth()/2,map.texture.height-imageOffset.y-GetScreenHeight()/2,GetScreenWidth(),GetScreenHeight()},
          (Vector2){GetScreenWidth()/2,GetScreenHeight()/2}, imageRotation, WHITE);

      // DrawTexture(target.texture, 0, 0, WHITE);
      EndTextureMode();
    }

    //show on screen

    BeginDrawing();
      ClearBackground(BLACK);
      // DrawTextureEx(blurTarget.texture, imageOffset, imageRotation, 1, RED);
      DrawTexture(blurTarget.texture, 0, 0, WHITE);
      DrawTexturePro(map.texture, (Rectangle){0,0, map.texture.width, map.texture.height},
            (Rectangle){GetScreenWidth()/2,GetScreenHeight()/2,GetScreenWidth(),GetScreenHeight()},
            (Vector2){GetScreenWidth()/2,GetScreenHeight()/2}, imageRotation, WHITE);
      // DrawTexture(map.texture, 0, 0, WHITE);

      
      DrawText(TextFormat("average dist: %.2f", average), 0, 0, 20, WHITE);
      DrawText(TextFormat("%i,%i %.1f", (int)imageOffset.x, (int)imageOffset.y, imageRotation), 0, 50, 20, WHITE);
    EndDrawing();
  }
  // Stop the device scanning thread and disable motor.
  laser.turnOff();
  // Uninitialize the SDK and Disconnect the LiDAR.
  laser.disconnecting();
  return 0;
}