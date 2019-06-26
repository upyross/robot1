/*
 * File:          robot1.c
 * Date:
 * Description:
 * Author:
 * Modifications:
 */

#include <webots/robot.h>
#include <webots/motor.h>
#include <webots/position_sensor.h>
#include <webots/distance_sensor.h>

#include <stdio.h>
#include <math.h>

/*
 * Macros
 */
#define TIME_STEP 64
#define PI 3.14159

#define OBSTACLE_DISTANCE 40.0

enum {
  GO,
  TURN,
  FREEWAY,
  OBSTACLE
};

/*
 * Global variables
 */

double initial_angle_wheel1;
/*
 * Auxiliar functions
 */
int checkForObstacles(WbDeviceTag distance_sensor) {
  double distance = wb_distance_sensor_get_value(distance_sensor);

  if (distance > OBSTACLE_DISTANCE)
    return FREEWAY;
  else
    return OBSTACLE;
}

void goRobot(WbDeviceTag *wheels, double velocity) {
  wb_motor_set_velocity(wheels[0], -velocity);
  wb_motor_set_velocity(wheels[1], -velocity);
}

void stopRobot(WbDeviceTag *wheels) {
  wb_motor_set_velocity(wheels[0], 0);
  wb_motor_set_velocity(wheels[1], 0);
}

void turnRight(WbDeviceTag *wheels) {
  //double initial_position = wb_position_sensor_get_value()
  wb_motor_set_velocity(wheels[0], 0.3);
  wb_motor_set_velocity(wheels[1], -0.3);
}

double getAngleRobot(WbDeviceTag pos_sensor) {
  printf("Calculando angulo\n");
  double angle, angle_wheel1, angle_wheel2;

  angle_wheel1 = wb_position_sensor_get_value(pos_sensor);
  printf("Angle Wheel1: %lf\n", angle_wheel1);
  angle = fabs(angle_wheel1 - initial_angle_wheel1);
  printf("Angle: %lf\n", angle);

  return angle;
}

float clearAngleRobot() {
  printf("Clearing angle\n");
}

/*
 * This is the main program.
 * The arguments of the main function can be specified by the
 * "controllerArgs" field of the Robot node
 */
int main(int argc, char **argv)
{
  /* necessary to initialize webots stuff */
  wb_robot_init();

  //robot1_init()

  /*
   * You should declare here WbDeviceTag variables for storing
   * robot devices like this:
   *  WbDeviceTag my_sensor = wb_robot_get_device("my_sensor");
   *  WbDeviceTag my_actuator = wb_robot_get_device("my_actuator");
   */
  // Motor devices
  WbDeviceTag wheel_right = wb_robot_get_device("motor_right");
  WbDeviceTag wheel_left = wb_robot_get_device("motor_left");

  WbDeviceTag wheels[2];
  wheels[0] = wheel_right;
  wheels[1] = wheel_left;

  wb_motor_set_position(wheel_right, INFINITY);
  wb_motor_set_position(wheel_left, INFINITY);

  // Encoder devices
  WbDeviceTag encoder = wb_robot_get_device("encoder1");
  wb_position_sensor_enable(encoder, TIME_STEP);

  // Distance sensor devices
  WbDeviceTag dist_sensor = wb_robot_get_device("distance_sensor");
  wb_distance_sensor_enable(dist_sensor, TIME_STEP);

  //double ds_value;
  double ps_value;
  short int ds_state, robot_state = GO;
  float velocity = 0.5;
  float angle;

  /* main loop
   * Perform simulation steps of TIME_STEP milliseconds
   * and leave the loop when the simulation is over
   */
  while (wb_robot_step(TIME_STEP) != -1) {
    if (robot_state == GO) {
      ds_state = checkForObstacles(dist_sensor);

      if (ds_state == FREEWAY) {
        goRobot(wheels, velocity);
        angle = wb_position_sensor_get_value(encoder);
        printf("Angle: %lf\n", angle);
      } else if (ds_state == OBSTACLE) {
        robot_state = TURN;
        stopRobot(wheels);
        initial_angle_wheel1 = wb_position_sensor_get_value(encoder);
      }
    } else if (robot_state == TURN) {
      turnRight(wheels);
      angle = getAngleRobot(encoder);

      if (angle >= 0.4*PI) {
        robot_state = GO;
        stopRobot(wheels);
        clearAngleRobot();
      }
    }
  }

  /* Enter your cleanup code here */

  /* This is necessary to cleanup webots resources */
  wb_robot_cleanup();

  return 0;
}
