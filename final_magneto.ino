/*   
Compass Calibration
by Hanie Kiani 
https://electropeak.com/learn/ 
*/ 
#include <Wire.h>
#include <LSM303.h>

#define STACK_SIZE 5

float stack[STACK_SIZE];
int top = -1;

void push(float data) {
  float gemiddelde = 0.00;
  if (top >= STACK_SIZE - 1) {  // check if the stack is full
    
    
    Serial.println("Stack overflow!");  // print an error message
    for(int i = top; i > 0; i--){
      stack[i-1] = stack[i];
      
    }
    stack[top] = data;
    for(int i = top; i > 0; i--){
      gemiddelde += stack[i];
    }
    gemiddelde = gemiddelde/top;
  }
  else {
    top++;  // increment the top of the stack
    stack[top] = data;  // add the data to the top of the stack

  }

}

float gemiddeld(int data)
int pop() {
  if (top < 0) {  // check if the stack is empty
    Serial.println("Stack underflow!");  // print an error message
    return -1;  // return an error code
  }
  else {
    int data = stack[top];  // get the data from the top of the stack
    top--;  // decrement the top of the stack
    return data;  // return the data
  }
}

LSM303 compass;
LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32768, -32768, -32768};

char report[80];
bool calibration_done = false; // een bool om te zien of het compass al correct gecalibreerd is.
int i = 0;

float gemiddelde(){
  

}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
}

void loop() {
  compass.read();
  if(calibration_done){
    float heading = compass.heading((LSM303::vector<int>){0, 0, 1});

  
    // Serial.println(heading);
  }
  else if(!calibration_done){
  
  running_min.x = min(running_min.x, compass.m.x);
  running_min.y = min(running_min.y, compass.m.y);
  running_min.z = min(running_min.z, compass.m.z);

  running_max.x = max(running_max.x, compass.m.x);
  running_max.y = max(running_max.y, compass.m.y);
  running_max.z = max(running_max.z, compass.m.z);
  
  snprintf(report, sizeof(report), "min: {%+6d, %+6d, %+6d}    max: {%+6d, %+6d, %+6d}",
    running_min.x, running_min.y, running_min.z,
    running_max.x, running_max.y, running_max.z);
  // Serial.println(report);
  
  i++;
  if(i >= 300){
    compass.m_min = (LSM303::vector<int16_t>){running_min.x,   running_min.y,  running_min.z};
    compass.m_max = (LSM303::vector<int16_t>){running_max.x,   running_max.y,   running_max.z};
    // Serial.println("done calibrating");
    calibration_done = true;
  }
  }
  delay(100);
  
}