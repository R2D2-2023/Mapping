#include "CYdLidar.h"
#include <string>
#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include <raymath.h>

#include <stdlib.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()


/**
 * @file scanExporter.cpp
 * @brief renders lidar scans with raylib and applies rotation from magnetometer and can save this as png's.
 *
 * Lidar type: ydlidar x2l
 * magnetometer sensor read via serial
 * by pressing space you can save 1 scan as png
 * renders using raylib (gpu accelerated)
 */


/**
 * @brief StrContains returns true if character c is in string str.
 *
 * go's through whole string until it finds the character c and returns true otherwise returns false
 */
bool strContains(char * str, char c)
{
    int i = 0;
    while(str[i] != '\0')
    {
        if(str[i] == c)
            return true;
        i++;
    }
    return false;
}

/**
 * @brief Wraps angles inbetween 0 and 360.
 *
 * limits angles (int) inbetween the range 0 and 360
 */
int wrapAround(int n)
{
    while(n < 0)
    {
        n += 360;
    }

    while(n > 360)
    {
        n -= 360;
    }

    return n;
}

/**
 * @brief rotation averaging, takes in array
 *
 * example array {0, 360} gets averaged to 0, not 180
 * averaging done by converting angles to vectors averaging them.
 * Afterwords using atan2 to get an angle.
 * Angles returned are often under 0, so be warned
 */
float averageRotation(float * angles, int amount)
{
  Vector2 averageVector = {0,0};

  for(int i = 0; i < amount; i++)
  {
      float angle = angles[i];
      float x = sin(angle);
      float y = cos(angle);

      averageVector.x += x / amount;
      averageVector.y += y / amount;
  }

  float averageAngle = atan2(averageVector.x, averageVector.y);
  return averageAngle;
}

/**
 * @brief returns true if there are numbers before the dot.
 *
 */
bool numberBeforeDecimals(char * str)
{
  int i = 0;
  while(str[i] != '\0' && str[i] != '\n' && str[i] != '.')
  {
    if(str[i] >= '0' || str[i] <= '9')
      return true;
  }
  return false;
}

#define AMOUNT_ROTATIONS 15

using namespace std;
using namespace ydlidar;

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

  // manual port not found, manually overwitten
  // with: /dev/ttyUSB0
  // recommended udev rules for linux https://github.com/YDLIDAR/YDLidar-SDK/blob/master/doc/howto/how_to_create_a_udev_rules.md
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

  //settings for x2l, lower baudrate
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

  //x2l doesn't do two-way communication
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




  //read out magnetometer
  // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
  int serial_port = open("/dev/ttyACM0", O_RDWR);

  // Create new termios struct, we call it 'tty' for convention
  struct termios tty;

  // Read in existing settings, and handle any error
  if(tcgetattr(serial_port, &tty) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
  // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

  tty.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate to be 9600
  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 1;
  }
  // Allocate memory for read buffer, set size according to your needs
  char read_buf [256*4];

  // Normally you wouldn't do this memset() call, but since we will just receive
  // ASCII data for this example, we'll set everything to 0 so we can
  // call printf() easily.

  // Read bytes. The behaviour of read() (e.g. does it block?,
  // how long does it block for?) depends on the configuration
  // settings above, specifically VMIN and VTIME
  int num_bytes = 1;

  char line [256*4] = {0}; //uhh very good code, dont go above it :P

  RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  RenderTexture2D blurTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  RenderTexture2D map = LoadRenderTexture(GetScreenWidth()*4, GetScreenHeight()*4);

  Shader blurShader = LoadShader(0, "../blurShader.fs");

  // Turn On success and loop  
  while (ret && ydlidar::os_isOk() && !WindowShouldClose()) {

    //do scan
    LaserScan scan;
    if (laser.doProcessSimple(scan)) {
      ///...
    } else {
      fprintf(stderr, "Failed to get Lidar Data\n");
      fflush(stderr);
    }

    //rotation applied to scan in radians
    static float rotation = 0;
    //past AMOUNT_ROTATIONS for smooting
    static float rotations[AMOUNT_ROTATIONS] = {0};
    static int rotationsIndex = 0;

    num_bytes = 1;

    int rotationCount = 0;

    float angles [30] = {0};
    int anglesCount = 0;

    //reading from tty, some devices only send half of a print so it's buffered until a '\n' is found
    //or if rotationCount is above 1 (should be changed depending on amount of messages per second)

    while(num_bytes > 0 && rotationCount < 1)
    {
      memset(&read_buf, '\0', sizeof(read_buf));
      num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
      rotationCount++;

      if(num_bytes > 0)
      {
        printf(read_buf);
        // printf("num_bytes %i | read_buf %s  | line %s\n", num_bytes, read_buf, line);
        strcat(line, read_buf);


        //if line contains '\n' read it out and empty
        if(strContains(read_buf, '\n') || strContains(read_buf, '\r') || strContains(read_buf, 10))
        {
            // printf("%.0f | %s\n('v')", atof(line), line);

            //if there are no numbers before the decimals it should be considered broken data and skipped
            if(numberBeforeDecimals(line))
            {
              angles[anglesCount] = atof(line);
              printf(" < %.2f >\n", angles[anglesCount]);
              angles[anglesCount] *= DEG2RAD;
              anglesCount++;
              rotation = atof(line) * DEG2RAD;
            }

            //empty line
            line[0] = '\0';
        }
      }else
      {
        // printf("Error reading: %s", strerror(errno));
      }
    }

    //rotation smoothing

    // if(anglesCount > 1)
    // {
    //   float newRotation = averageRotation(angles, anglesCount);
    //   rotations[rotationsIndex] = newRotation;

    //   rotationsIndex++;
    //   rotationsIndex = rotationsIndex % AMOUNT_ROTATIONS;

    //   rotation = averageRotation(rotations, AMOUNT_ROTATIONS);
    // }


    //begin render texture mode
    BeginTextureMode(target);

    //clear render texture
    ClearBackground((Color){0,0,0,0});
    //half clear render texture
    // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 125});


    //simple point filter
    const int pointsCount = 5;
    Vector2 lastPoints [pointsCount] = {0};
    int lastPointsIndex = -1;

    for(int i = 0; i < scan.points.size(); i++)
    {
      lastPointsIndex++;
      LaserPoint point = scan.points[i];

      //add rotation from magnetometer to point angle
      float angle = -point.angle + rotation;
      //calculate position
      float x = sin(angle);
      float y = cos(angle);

      x *= point.range;
      y *= point.range;

      // if(point.intensity < 0.1)
      //   continue;

      float maxDist = .8;
      int distCounter = 0;

      //simple filter

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

      lastPoints[lastPointsIndex%pointsCount] = (Vector2){x, y};

      // if(tooFar)
      //   continue;

      

      //transform onto screen

      float maxDistance = 10;

      if(point.range > maxDistance)
        continue;

      x /= maxDistance * 2;
      y /= maxDistance * 2;

      x *= GetScreenWidth();
      y *= GetScreenHeight();

      x += GetScreenWidth()/2;
      y += GetScreenHeight()/2;

      //point intensity ranges between 1008 and 1016 (????)
      point.intensity -= 1000;
      point.intensity /= 16.0;

      //strength is determined by point.intensity and point range
      // the further a point is the less accurate and thus shouldn't be valued as much
      float strength = point.intensity * (1- point.range/maxDistance);

      //draw line for clear space (anything inbetween lidar and point is considered empty space)
      DrawLineEx((Vector2){GetScreenWidth()/2, GetScreenHeight()/2}, (Vector2){x,y}, 4, (Color){255, 255, 255, strength*255});
      //draw point
      DrawCircle(x, y, 2, (Color){0, 255, 0, strength *255});
    }

    //draw middle
    // DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 8, GREEN);

    EndTextureMode();


    //is space or left shift is pressed save scan as png
    if(IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_LEFT_SHIFT))
    {
      static int imageCount = 0;
      Image img = LoadImageFromTexture(target.texture);

      bool result = ExportImage(img, TextFormat("./exportedImage%i.png", imageCount));
      imageCount++;
    }

    //show scan to screen
    BeginDrawing();
      ClearBackground(BLACK);
      DrawText(TextFormat("%0f", rotation*RAD2DEG), 0,0,20, WHITE);
      DrawTexture(target.texture, 0, 0, WHITE);
    EndDrawing();
  }
  // Stop the device scanning thread and disable motor.
  laser.turnOff();
  // Uninitialize the SDK and Disconnect the LiDAR.
  laser.disconnecting();
  return 0;
}