cmake_minimum_required(VERSION 3.8)
project(ros_msgs)

# 查找 ROS 2 必需的包
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# 使用 IDL 生成自定义消息类型
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/MasterHandleData.idl"
)

# 生成 ROS 2 包
ament_package()