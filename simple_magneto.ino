#include <Wire.h>
#include <LSM303.h>


LSM303 compass;

#define STACK_SIZE 10

float stack[STACK_SIZE];
int top = -1;

void push(float data) {
  if (!(top >= STACK_SIZE - 1)) {  // check if the stack is full
    top++;  // increment the top of the stack
    stack[top] = data;  // add the data to the top of the stack
  }
}

int pop() {
  if (top < 0) {  // check if the stack is empty
    return -1;  // return an error code
  }
  else {
    int data = stack[top];  // get the data from the top of the stack
    top--;  // decrement the top of the stack
    return data;  // return the data
  }
}
void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  
  /*
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */
  compass.m_min = (LSM303::vector<int16_t>){-465,   -223,  -1023};
  compass.m_max = (LSM303::vector<int16_t>){+263,   +528,   -412};
}

void loop() {
  /*
  When given no arguments, the heading() function returns the angular
  difference in the horizontal plane between a default vector and
  north, in degrees.
  
  The default vector is chosen by the library to point along the
  surface of the PCB, in the direction of the top of the text on the
  silkscreen. This is the +X axis on the Pololu LSM303D carrier and
  the -Y axis on the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH
  carriers.
  
  To use a different vector as a reference, use the version of heading()
  that takes a vector argument; for example, use
  
    compass.heading((LSM303::vector<int>){0, 0, 1});
  
  to use the +Z axis as a reference.
  */
  compass.read();
  float heading1 = compass.heading((LSM303::vector<int>){0, 0, 1});
  push(heading1);
  delay(10);
  compass.read();
  float heading2 = compass.heading((LSM303::vector<int>){0, 0, 1});
  push(heading2);
  delay(10);
  compass.read();
  float heading3 = compass.heading((LSM303::vector<int>){0, 0, 1});
  push(heading3);
  delay(10);
  compass.read();
  float heading4 = compass.heading((LSM303::vector<int>){0, 0, 1});
  push(heading4);
  delay(10);
  compass.read();
  float heading5 = compass.heading((LSM303::vector<int>){0, 0, 1});
  push(heading5);
  
  int gedeeld_door_nummer = top + 1;
  float verzamelde_data_punten = 0;
  float data_punt = pop();
  while(data_punt != -1){
    verzamelde_data_punten += data_punt;
    data_punt = pop();
  }
  Serial.println(verzamelde_data_punten);
  Serial.println(gedeeld_door_nummer);
  float heading = verzamelde_data_punten / gedeeld_door_nummer;
  Serial.println(heading);
  Serial.println("hier komen de headings");
  Serial.println(heading1);
  Serial.println(heading2);
  Serial.println(heading3);
  Serial.println(heading4);
  Serial.println(heading5);
  delay(500);

}