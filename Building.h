#pragma once
#include"Passenger.h"
#include"Floor.h"
#include"Elevator.h"
#include"Statistic.h"
#include<array>
#include<queue>
#include <iostream>
#include <fstream>
#include <sstream>

class Building {
public:
	Building(int numOfFloors, int numOfElevators, int elevatorSpeed, int elevatorStoppingTime) : NUM_OF_FLOORS{ numOfFloors }, NUM_OF_ELEVATORS{ numOfElevators }, ELEVATOR_SPEED{ elevatorSpeed }, ELEVATOR_STOPPING_TIME{ elevatorStoppingTime } {
		// initalize containers
		initalizeFloors();
		initalizeElevators();
		initalizePassengers();

		// for error checking
		totalPassenger = passengers.size();
	}

	void simulate() {
		// keep updating until all passengers arrived
		while (!allPassengerArrived()) {
			// update passengers
			while (!passengers.empty() && passengers.front().getStartTime() == currentTime) {
				Floor passengerStartfloor = floors[passengers.front().getStartFloor() - 1];
				passengerStartfloor.addWaitingPassenger(passengers.front());
				passengers.pop();
			}

			// update elevator1
			elevators[0].update(currentTime, NUM_OF_FLOORS, floors);

			// wait for 100 seconds, then update elevator2
			if (currentTime >= 100) {
				elevators[1].update(currentTime, NUM_OF_FLOORS, floors);
			}

			// wait for 1000 seconds then update elevator3
			if (currentTime >= 500) {
				elevators[2].update(currentTime, NUM_OF_FLOORS, floors);
			}

			// wait for 1100 seconds then update elevator4
			if (currentTime >= 700) {
				elevators[3].update(currentTime, NUM_OF_FLOORS, floors);
			}

			// update time
			++currentTime;
			//std::cout << "Current time: " << currentTime << std::endl;
		}

		// calculate statistics
		for (int i = 0; i < NUM_OF_FLOORS; ++i) {
			for (auto& passenger : floors.at(i).getDeliveredPassengers()) {
				travelTimeStat.addNumber(passenger.getTravelTime());
				waitTimeStat.addNumber(passenger.getWaitTime());
				++deliveredPassenger;
			}
		}

		// print statistics
		std::cout << "\nAverage wait time: " << waitTimeStat.getAverage() << std::endl;
		std::cout << "Average travel time: " << travelTimeStat.getAverage() << std::endl;

		// print total and delivered passengers
		std::cout << "\nTotal passengers: " << totalPassenger << std::endl;
		std::cout << "Delivered passengers: " << deliveredPassenger << std::endl;
	}

private:
	const int NUM_OF_FLOORS;
	const int NUM_OF_ELEVATORS;
	const int ELEVATOR_SPEED;
	const int ELEVATOR_STOPPING_TIME;
	int currentTime = 0;
	std::vector<Floor> floors;
	std::vector<Elevator> elevators;
	std::queue<Passenger> passengers;
	Statistic travelTimeStat;
	Statistic waitTimeStat;

	// for error checking
	size_t totalPassenger = 0;
	size_t deliveredPassenger = 0;

	void initalizeFloors() {
		for (int i = 0; i < NUM_OF_FLOORS; ++i) {
			floors.push_back(Floor(i + 1));
		}
	}

	void initalizeElevators() {
		for (int i = 0; i < NUM_OF_ELEVATORS; ++i) {
			elevators.push_back(Elevator(i, ELEVATOR_SPEED, ELEVATOR_STOPPING_TIME));
		}
	}

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
};
