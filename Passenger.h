#pragma once
#include <stdexcept>
#include"ElevatorState.h"

class Passenger {
public:
	Passenger(int passengerID, int startTime, int startFloor, int endFloor) : passengerID{ passengerID }, startTime{ startTime }, waitTime{ 0 }, travelTime{ 0 }
	{
		// make sure the floor numbers are valid
		if (startFloor < 1 || startFloor > 100 || endFloor < 1 || endFloor > 100) {
			throw std::invalid_argument("Invalid floor number");
		}
		else {
			this->startFloor = startFloor;
			this->endFloor = endFloor;
		}

		// set the direction of the passenger
		if (startFloor < endFloor) {
			this->direction = ElevatorDirection::UP;
		}
		else {
			this->direction = ElevatorDirection::DOWN;
		}
	}

	int getWaitTime() const { return waitTime; }
	int getTravelTime() const { return travelTime; }
	ElevatorDirection getDirection() const { return direction; }
	int getStartFloor() const { return startFloor; }
	int getEndFloor() const { return endFloor; }
	int getStartTime() const { return startTime; }

	void calculateWaitTime(int currentTime) {
		waitTime = currentTime - startTime;
	}

	void calculateTravelTime(int currentTime) {
		travelTime = currentTime - (startTime + waitTime);
	}

private:
	int passengerID;
	int startTime;
	int startFloor;
	int endFloor;
	ElevatorDirection direction;
	int waitTime;
	int travelTime;
};
