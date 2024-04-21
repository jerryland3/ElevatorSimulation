/**
 * @file Building.h
 * @brief Defines the Building class, which simulates the behavior of a building with elevators and passengers.
 *
 * The Building class represents a building with multiple floors, elevators, and passengers.
 * It simulates the movement of elevators and the arrival of passengers at different floors.
 *
 * @date 4/20/2024
 * @version 1.0
 * @author Jerry Wang
 */

#pragma once
#include"Passenger.h"
#include"Statistic.h"
#include"Elevator.h"
#include"Floor.h"
#include "spdlog/spdlog.h"
#include<queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>

class Building {
public:
	/**
	 * @brief Constructs a Building object with the specified parameters.
	 *
	 * Initializes the Building object with the given number of floors, elevators, elevator speed,
	 * elevator stopping time, and log file name.
	 *
	 * @param numOfFloors The number of floors in the building.
	 * @param numOfElevators The number of elevators in the building.
	 * @param elevatorSpeed The speed of the elevators (in seconds per floor).
	 * @param elevatorStoppingTime The time taken for the elevator to stop at a floor (in seconds).
	 * @param logFileName The name of the log file to store simulation information.
	 */
	Building(int numOfFloors, int numOfElevators, int elevatorSpeed, int elevatorStoppingTime, std::string logFileName) : NUM_OF_FLOORS{ numOfFloors }, NUM_OF_ELEVATORS{ numOfElevators }, ELEVATOR_SPEED{ elevatorSpeed }, ELEVATOR_STOPPING_TIME{ elevatorStoppingTime }, logFileName{ logFileName }, logFileLocation{ "logs/" + logFileName + "_passenger_log" + ".txt" } {
		// initialize containers
		initalizeFloors();
		initalizeElevators();
		initalizePassengers();

		// for error checking
		totalPassenger = passengers.size();
	}

	/**
	 * @brief Simulates elevator behavior in the building.
	 *
	 * This function simulates the movement of elevators and the arrival of passengers
	 * until all passengers have arrived at their destinations.
	 */
	void simulate() {
		auto file_logger = spdlog::basic_logger_mt(logFileName + "_adding_passengers", logFileLocation);
		auto time_logger = spdlog::basic_logger_mt(logFileName + "_time_log", "logs/" + logFileName + "_time_log" + ".txt");
		auto stat_logger = spdlog::basic_logger_mt(logFileName + "_stat_log", "logs/" + logFileName + "_stat_log" + ".txt");

		// keep updating until all passengers arrived
		while (!allPassengerArrived()) {
			// update passengers
			while (!passengers.empty() && passengers.front().getStartTime() == currentTime) {
				Floor& passengerStartFloor = floors[passengers.front().getStartFloor() - 1];
				Passenger passenger = passengers.front();
				passengerStartFloor.addWaitingPassenger(passenger);
				passengers.pop();

				// log passenger arrival
				file_logger->info("Passenger {} arrived at floor {} at time {}", passenger.getPassengerID(), passengerStartFloor.getFloorNumber(), currentTime);
			}

			// update elevators. Start elevator at different time to improve pickup passenger efficiency

			elevators[0].update(currentTime, NUM_OF_FLOORS, floors);

			if (currentTime >= 100) {
				elevators[1].update(currentTime, NUM_OF_FLOORS, floors);
			}
			if (currentTime >= 500) {
				elevators[2].update(currentTime, NUM_OF_FLOORS, floors);
			}
			if (currentTime >= 700) {
				elevators[3].update(currentTime, NUM_OF_FLOORS, floors);
			}

			// log statistics
			statLog(stat_logger);

			// increment time
			++currentTime;
		} // end while

		// put wait and travel time to statistic
		for (int i = 0; i < NUM_OF_FLOORS; ++i) {
			for (auto& passenger : floors.at(i).getDeliveredPassengers()) {
				travelTimeStat.addNumber(passenger.getTravelTime());
				waitTimeStat.addNumber(passenger.getWaitTime());
				time_logger->info("Passenger {}: wait time {}, travel time {}", passenger.getPassengerID(), passenger.getWaitTime(), passenger.getTravelTime());
				++deliveredPassenger;
			}
		}

		// print statistics
		std::cout << "\nAverage wait time: " << waitTimeStat.getAverage() << std::endl;
		std::cout << "Average travel time: " << travelTimeStat.getAverage() << std::endl;

		// check if all passengers are delivered
		if (totalPassenger != deliveredPassenger) {
			throw std::runtime_error("Not all passengers are delivered");
		}
	}

private:
	const int NUM_OF_FLOORS; ///< Number of floors in the building.
	const int NUM_OF_ELEVATORS; ///< Number of elevators in the building.
	const int ELEVATOR_SPEED; ///< Speed of the elevators (in seconds per floor).
	const int ELEVATOR_STOPPING_TIME; ///< Time taken for the elevator to stop at a floor (in seconds).
	int currentTime = 0; ///< Current simulation time.
	std::vector<Floor> floors; ///< Vector of floors in the building.
	std::vector<Elevator> elevators; ///< Vector of elevators in the building.
	std::queue<Passenger> passengers; ///< Queue of passengers waiting to enter the building.
	Statistic travelTimeStat; ///< Statistic for passenger travel times.
	Statistic waitTimeStat; ///< Statistic for passenger wait times.

	const std::string logFileName; ///< Name of the log file.
	const std::string logFileLocation; ///< Location to save log file

	// For error checking
	size_t totalPassenger = 0; ///< Total number of passengers.
	size_t deliveredPassenger = 0; ///< Number of passengers delivered to their destinations.

	/**
	 * @brief Initializes the floors in the building.
	 *
	 * This function initializes the floors in the building based on the specified number of floors.
	 */
	void initalizeFloors() {
		for (int i = 0; i < NUM_OF_FLOORS; ++i) {
			floors.push_back(Floor(i + 1));
		}
	}

	/**
	 * @brief Initializes the elevators in the building.
	 *
	 * This function initializes the elevators in the building based on the specified number of elevators.
	 */
	void initalizeElevators() {
		for (int i = 0; i < NUM_OF_ELEVATORS; ++i) {
			elevators.push_back(Elevator(i, ELEVATOR_SPEED, ELEVATOR_STOPPING_TIME, logFileName));
		}
	}

	/**
	 * @brief Initializes the passengers waiting to enter the building.
	 *
	 * This function reads passenger data from a CSV file and creates Passenger objects,
	 * which are then added to the queue of waiting passengers.
	 */
	void initalizePassengers() {
		std::ifstream inputFile("Mod10_Assignment_Elevators.csv"); // open csv
		std::string line;
		int startFloor = 1;
		int endFloor = 1;
		int startTime = 1;
		int id = 1;

		std::getline(inputFile, line); // skip first line)

		// read each line and create passenger object and push to queue
		while (std::getline(inputFile, line)) {
			std::stringstream ss(line);
			std::string token;
			while (std::getline(ss, token, ',')) {
				startTime = std::stoi(token);
				std::getline(ss, token, ',');
				startFloor = std::stoi(token);
				std::getline(ss, token, ',');
				endFloor = std::stoi(token);
			}
			Passenger passenger(id, startTime, startFloor, endFloor);
			passengers.push(passenger);
			++id;
		}
		inputFile.close();
	}

	/**
	 * @brief Checks if all passengers have arrived at their destinations.
	 *
	 * This function checks if all passengers have arrived at their destinations
	 * by verifying that there are no waiting passengers on floors and no passengers in elevators.
	 *
	 * @return True if all passengers have arrived, false otherwise.
	 */
	bool allPassengerArrived() {
		// check all floors for waiting passengers
		for (int i = 0; i < NUM_OF_FLOORS; ++i) {
			if (floors[i].hasWaitingPassengers()) {
				return false;
			}
		}
		// check all elevators for passengers
		for (int i = 0; i < NUM_OF_ELEVATORS; ++i) {
			if (elevators[i].hasPassengers()) {
				return false;
			}
		}

		// check queue has passengers
		return passengers.empty();
	}

	/**
	 * @brief Logs statistical information about the elevator simulation.
	 *
	 * This method logs statistical information about the elevator simulation,
	 * such as the time between floors, current simulation time, number of waiting passengers,
	 * and average wait time.
	 *
	 * @param statLogger A shared pointer to the logger where the statistical information will be logged.
	 */
	void statLog(std::shared_ptr<spdlog::logger> statLogger) {
		// Log time between floors and current simulation time
		statLogger->info("Time Between Floors: {} seconds", ELEVATOR_SPEED);
		statLogger->info("Current Simulation Time: {}", currentTime);

		// Initialize counters for waiting and delivered passengers, and average wait time
		int waitingPassengerCount = 0;
		int deliveredPassengerCount = 0;
		int averageWaitTime = 0;

		// Iterate through each floor to count waiting passengers
		for (int i = 0; i < NUM_OF_FLOORS; ++i) {
			if (!floors[i].getWaitingPassengers().empty()) {
				waitingPassengerCount += floors[i].getWaitingPassengers().size();
			}
		}

		// Iterate through each floor to count delivered passengers and calculate average wait time
		for (int i = 0; i < NUM_OF_FLOORS; ++i) {
			deliveredPassengerCount += floors[i].getDeliveredPassengers().size();
			for (auto& passenger : floors.at(i).getDeliveredPassengers()) {
				averageWaitTime += passenger.getWaitTime();
			}
		}

		// Calculate average wait time if there are delivered passengers
		if (deliveredPassengerCount != 0) {
			averageWaitTime /= deliveredPassengerCount;
		}

		// Log number of waiting passengers and average wait time
		statLogger->info("Number of Waiting Passengers: {}", waitingPassengerCount);
		statLogger->info("Average Wait Time: {} seconds", averageWaitTime);
		statLogger->info("\n");
	}
};
