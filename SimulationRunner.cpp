/**
 * @file main.cpp
 * @brief Contains the main function for simulating elevator behavior in two buildings.
 *
 * This file defines the main function which simulates elevator behavior in two buildings with different configurations.
 * Test sets are organized to validate different aspects of elevator behavior and input handling.
 *
 * @date 4/20/2024
 * @version 1.0
 * @author Jerry Wang
 */

#pragma once

#include "Building.h"
#include "spdlog/spdlog.h"
#include <spdlog/sinks/basic_file_sink.h>

using namespace std;

/**
 * @brief Main function to simulate elevator behavior in two buildings.
 *
 * This function creates two buildings with different elevator speed settings
 * and simulates elevator behavior in each building.
 *
 * @return Integer indicating the exit status of the program.
 */
int main() {
	// Initialize variables for building configurations
	int numOfFloors = 100;
	int numOfElevators = 4;
	int elevatorSpeedTime1 = 10; // Time taken for the elevator to move between floors in Building 1 (in seconds).
	int elevatorSpeedTime2 = 5;  // Time taken for the elevator to move between floors in Building 2 (in seconds).
	int elevatorStoppingTime = 2;

	// Create Building 1
	cout << "Building 1: 10 seconds for elevator to move between floors" << endl;
	Building myBuilding1(numOfFloors, numOfElevators, elevatorSpeedTime1, elevatorStoppingTime, "status_10sec_speed");
	myBuilding1.simulate(); // Simulate elevator behavior in Building 1.

	cout << "\n\n\nBuilding 2: 5 seconds for elevator to move between floors" << endl;
	// Create Building 2
	Building myBuilding2(numOfFloors, numOfElevators, elevatorSpeedTime2, elevatorStoppingTime, "status_5sec_speed");
	myBuilding2.simulate(); // Simulate elevator behavior in Building 2.

	return 0;
}