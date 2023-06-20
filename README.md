# Mapping
Git repository voor de mapping groep (positie bepaling 1)

installeer ros-melodict-desktop-full git ros-melodic-cartographer-ros ros-melodic-gazebo-msgs ros-melodic-gazebos-plugins python-rosdep2 pycrypto libsdl1.2-dev libsdl-image1.2 libsdl-image1.2-dev

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

#done with installation
#to run
roslaunch gbot_core gbot.launch #starts cartographer
roslaunch gbot_core visualization.launch #for visualization (requires gui)
rosrun map_server map_saver #saves map
