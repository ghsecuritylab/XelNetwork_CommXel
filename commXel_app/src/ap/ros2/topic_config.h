/*
 * topic_config.hpp
 *
 *  Created on: May 16, 2018
 *      Author: Kei
 */

#ifndef ROS2_TOPIC_CONFIG_H_
#define ROS2_TOPIC_CONFIG_H_


enum TopicIdNumbering 
{
  /* builtin_interfaces */
  BUILTIN_INTERFACES_TIME_TOPIC = 1,
  BUILTIN_INTERFACES_DURATION_TOPIC,

  /* std_msgs */
  STD_MSGS_BOOL_TOPIC,
  STD_MSGS_STRING_TOPIC,
  STD_MSGS_INT32_TOPIC,
  STD_MSGS_INT64_TOPIC,
  STD_MSGS_EMPTY_TOPIC,
  STD_MSGS_HEADER_TOPIC,
  STD_MSGS_MULTI_ARRAY_DIMENSION_TOPIC,

  /* geometry_msgs */
  GEOMETRY_MSGS_VECTOR3_TOPIC,
  GEOMETRY_MSGS_TWIST_TOPIC,
  GEOMETRY_MSGS_QUATERNION_TOPIC,
  GEOMETRY_MSGS_POINT_TOPIC,
  GEOMETRY_MSGS_POSE_TOPIC,
  GEOMETRY_MSGS_POSE_WITH_CONVARIANCE_TOPIC,
  GEOMETRY_MSGS_TWIST_WITH_CONVARIANCE_TOPIC,
  GEOMETRY_MSGS_TRANSFORM_TOPIC,
  GEOMETRY_MSGS_TRANSFORM_STAMPED_TOPIC,

  /* diagnostic_msgs */
  DIAGNOSTIC_MSGS_KEY_VALUE_TOPIC,

  /* sensor_msgs */
  SENSOR_MSGS_IMU_TOPIC,
  SENSOR_MSGS_LASER_SCAN_TOPIC,
  SENSOR_MSGS_BATTERY_STATE_TOPIC,
  SENSOR_MSGS_MAGNETIC_FIELD_TOPIC,
  SENSOR_MSGS_JOINT_STATE_TOPIC,

  /* nav_msgs */
  NAV_MSGS_ODOMETRY_TOPIC,

  /* turtlebot3_msgs */
  TURTLEBOT3_MSGS_SOUND_TOPIC,
  TURTLEBOT3_MSGS_VERSION_INFO_TOPIC,
  TURTLEBOT3_MSGS_SENSOR_STATE_TOPIC
};

#endif /* ROS2_TOPIC_CONFIG_H_ */
