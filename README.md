# Mapping
Git repository voor de mapping groep (positie bepaling 1)

installeer ros-melodic-desktop-full git ros-melodic-cartographer-ros ros-melodic-gazebo-msgs ros-melodic-gazebos-plugins python-rosdep2 pycrypto libsdl1.2-dev libsdl-image1.2 libsdl-image1.2-dev

installeer op x86/x86_64 rosmelodic-desktop-full docker rosbridge_suite

```bash
source /opt/ros/melodic/setup.bash

git clone https://github.com/YDLIDAR/YDLidar-SDK
cd YDLidar-SDK
mkdir build
cmake ..
make
sudo make install
cd ~

mkdir ydlidar_ws
cd ydlidar_ws
git clone https://github.com/YDLIDAR/ydlidar_ros_driver src
catkin_make
catkin_make install
source devel/setup.bash
cd ~

mkdir gbot_core_ws
cd gbot_core_ws
https://github.com/JSaurusRex/gbot_core_ydlidar src
catkin_make
catkin_make install
source devel/setup.bash
cd ~

mkdir map_server
cd map_server
git clone https://github.com/ros-planning/navigation
mv navigation/map_server src
rm -rf navigation
catkin_make
catkin_make install
source devel/setup.bash
cd ~
```

#done with installation
#to run
```bash
ROS_IP=(ip of x86/x86_64) ROS_MASTER_URI=(ip of rosserver/machine) roslaunch gbot_core gbot.launch #starts cartographer
roslaunch gbot_core visualization.launch #for visualization (requires gui)
rosrun map_server map_saver #saves map

#for webserver
#on x86/x86_64
ROS_IP=(ip of rosserver) roslaunch rosbridge_server rosbridge_websocket.launch
docker run -p 8080:8080 cruise/webviz
```
(ip of x86/x86_64):8080/?rosbridge-websocket-url=ws://(ip of rosserver)
