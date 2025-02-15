// Mars lander simulator
// Version 1.11
// Mechanical simulation functions
// Gabor Csanyi and Andrew Gee, August 2019

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation, to make use of it
// for non-commercial purposes, provided that (a) its original authorship
// is acknowledged and (b) no modified versions of the source code are
// published. Restriction (b) is designed to protect the integrity of the
// exercise for future generations of students. The authors would be happy
// to receive any suggested modifications by private correspondence to
// ahg@eng.cam.ac.uk and gc121@eng.cam.ac.uk.

#include "lander.h"
#include <iostream>
#include <fstream>
#include <vector>

// using namespace std;
//std::ofstream fout;
//fout.open("results.txt", std::ios::app);

//double kh, kp;
//kh = 0.017;
//kp = 1;

void autopilot (void)
  // Autopilot to adjust the engine throttle, parachute and attitude control
{
  // INSERT YOUR CODE HERE
    double kh, kp, delta, altitude, epsilon, pout;
    altitude = position.abs() - MARS_RADIUS;
    kh = KH;
    kp = KP;
    // delta = (UNLOADED_LANDER_MASS + FUEL_CAPACITY * FUEL_DENSITY * fuel);
    // delta = (GRAVITY * MARS_MASS * (UNLOADED_LANDER_MASS + fuel * FUEL_CAPACITY * FUEL_DENSITY) / position.abs2())/MAX_THRUST;
    delta = fuel / 1.5;
    epsilon = -(0.5 + kh * altitude + velocity * position.norm());
    pout = kp * epsilon;
    // cout << (pout + delta) << ',';

    if (pout <= - delta) {
        throttle = 0;
    }
    else if (pout < (1 - delta)) {
        throttle = delta + pout;
    }
    else {
        throttle = 1;
    }

}

void numerical_dynamics(void)
// This is the function that performs the numerical integration to update the
// lander's pose. The time step is delta_t (global variable).
{
    // INSERT YOUR CODE HERE

        double density;
        density = atmospheric_density(position);
        vector3d thrust;
        thrust = thrust_wrt_world();
        vector3d drag_chute, drag_lander, drag_total, grav_force, total_force;
        drag_lander = -0.5 * density * DRAG_COEF_LANDER * M_PI * LANDER_SIZE * LANDER_SIZE * velocity.abs2() * velocity.norm();
        if (parachute_status == DEPLOYED) {
            drag_chute = -0.5 * density * DRAG_COEF_LANDER * 20 * LANDER_SIZE * LANDER_SIZE * velocity.abs2() * velocity.norm();
        }

        else {
            drag_chute = velocity * 0;
        }
        drag_total = drag_chute + drag_lander;
        grav_force = - position.norm() * GRAVITY * MARS_MASS * (UNLOADED_LANDER_MASS + fuel * FUEL_CAPACITY * FUEL_DENSITY) / position.abs2();
        total_force = thrust + drag_total + grav_force;
        vector3d acceleration;
        acceleration = total_force / (UNLOADED_LANDER_MASS + fuel * FUEL_CAPACITY * FUEL_DENSITY);

        static vector3d previous_position;
        vector3d new_position;
        if (simulation_time == 0.0) {
            new_position = position + delta_t * velocity;
            velocity = velocity + delta_t * acceleration;

        }

        else {
            new_position = 2 * position - previous_position + delta_t * delta_t * acceleration;
            velocity = (new_position - position) / delta_t;
        }
        previous_position = position;
        position = new_position;
        

        // Here we can apply an autopilot to adjust the thrust, parachute and attitude
        if (autopilot_enabled) {
            autopilot();
            double height, radialSpeed, targetSpeed;
            height = position.abs() - MARS_RADIUS;
            radialSpeed = velocity * position.norm();
            targetSpeed = -(0.5 + KH * height);
            cout << height << " , " << radialSpeed << " , " << targetSpeed << " , " << throttle << "\n";

            ofstream outputFile;
            outputFile.open("results.txt", ios::app);
            // Append data to the file
            if (outputFile.is_open()) {
                outputFile << height << ' ' << radialSpeed << ' ' << targetSpeed << '\n';
                // cout << height << radialSpeed << targetSpeed ;

                // Close the file
                outputFile.close();

                cout << "Data appended to the file successfully." << endl;
            }

            else {
                cout << "Error opening file." << endl;
            }
        }

        // Here we can apply 3-axis stabilization to ensure the base is always pointing downwards
        if (stabilized_attitude) attitude_stabilization();

    }

void initialize_simulation (void)
  // Lander pose initialization - selects one of 10 possible scenarios
{
  // The parameters to set are:
  // position - in Cartesian planetary coordinate system (m)
  // velocity - in Cartesian planetary coordinate system (m/s)
  // orientation - in lander coordinate system (xyz Euler angles, degrees)
  // delta_t - the simulation time step
  // boolean state variables - parachute_status, stabilized_attitude, autopilot_enabled
  // scenario_description - a descriptive string for the help screen

  scenario_description[0] = "circular orbit";
  scenario_description[1] = "descent from 10km";
  scenario_description[2] = "elliptical orbit, thrust changes orbital plane";
  scenario_description[3] = "polar launch at escape velocity (but drag prevents escape)";
  scenario_description[4] = "elliptical orbit that clips the atmosphere and decays";
  scenario_description[5] = "descent from 200km";
  scenario_description[6] = "";
  scenario_description[7] = "";
  scenario_description[8] = "";
  scenario_description[9] = "";

  switch (scenario) {

  case 0:
    // a circular equatorial orbit
    position = vector3d(1.2*MARS_RADIUS, 0.0, 0.0);
    velocity = vector3d(0.0, -3247.087385863725, 0.0);
    orientation = vector3d(0.0, 90.0, 0.0);
    delta_t = 0.1;
    parachute_status = NOT_DEPLOYED;
    stabilized_attitude = false;
    autopilot_enabled = false;
    break;

  case 1:
    // a descent from rest at 10km altitude
    position = vector3d(0.0, -(MARS_RADIUS + 10000.0), 0.0);
    velocity = vector3d(0.0, 0.0, 0.0);
    orientation = vector3d(0.0, 0.0, 90.0);
    delta_t = 0.1;
    parachute_status = NOT_DEPLOYED;
    stabilized_attitude = true;
    autopilot_enabled = false;
    break;

  case 2:
    // an elliptical polar orbit
    position = vector3d(0.0, 0.0, 1.2*MARS_RADIUS);
    velocity = vector3d(3500.0, 0.0, 0.0);
    orientation = vector3d(0.0, 0.0, 90.0);
    delta_t = 0.1;
    parachute_status = NOT_DEPLOYED;
    stabilized_attitude = false;
    autopilot_enabled = false;
    break;

  case 3:
    // polar surface launch at escape velocity (but drag prevents escape)
    position = vector3d(0.0, 0.0, MARS_RADIUS + LANDER_SIZE/2.0);
    velocity = vector3d(0.0, 0.0, 5027.0);
    orientation = vector3d(0.0, 0.0, 0.0);
    delta_t = 0.1;
    parachute_status = NOT_DEPLOYED;
    stabilized_attitude = false;
    autopilot_enabled = false;
    break;

  case 4:
    // an elliptical orbit that clips the atmosphere each time round, losing energy
    position = vector3d(0.0, 0.0, MARS_RADIUS + 100000.0);
    velocity = vector3d(4000.0, 0.0, 0.0);
    orientation = vector3d(0.0, 90.0, 0.0);
    delta_t = 0.1;
    parachute_status = NOT_DEPLOYED;
    stabilized_attitude = false;
    autopilot_enabled = false;
    break;

  case 5:
    // a descent from rest at the edge of the exosphere
    position = vector3d(0.0, -(MARS_RADIUS + EXOSPHERE), 0.0);
    velocity = vector3d(0.0, 0.0, 0.0);
    orientation = vector3d(0.0, 0.0, 90.0);
    delta_t = 0.1;
    parachute_status = NOT_DEPLOYED;
    stabilized_attitude = true;
    autopilot_enabled = false;
    break;

  case 6:
    break;

  case 7:
    break;

  case 8:
    break;

  case 9:
    break;

  }
}
