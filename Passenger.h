/**
 * @file Passenger.h
 * @brief Declaration of the Passenger class.
 *
 * The Passenger class represents a passenger in the building.
 * It manages information about the passenger, including their ID, start time, start and end floors, direction,
 * waiting time, and travel time.
 *
 * @date 4/20/2024
 * @version 1.0
 * @author Jerry Wang
 */

#pragma once
#include <stdexcept>
#include "ElevatorState.h"

class Passenger {
public:
	/**
	 * @brief Constructs a Passenger object with the specified parameters.
	 *
	 * @param passengerID The unique identifier for the passenger.
	 * @param startTime The time at which the passenger arrives.
	 * @param startFloor The floor from which the passenger starts.
	 * @param endFloor The floor to which the passenger wants to go.
	 * @throw std::invalid_argument if the startFloor or endFloor is invalid.
	 */
	Passenger(int passengerID, int startTime, int startFloor, int endFloor)
		: passengerID(passengerID), startTime(startTime), waitTime(0), travelTime(0) {
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

	/**
	 * @brief Gets the waiting time of the passenger.
	 *
	 * @return The waiting time in seconds.
	 */
	int getWaitTime() const { return waitTime; }

	/**
	 * @brief Gets the travel time of the passenger.
	 *
	 * @return The travel time in seconds.
	 */
	int getTravelTime() const { return travelTime; }

	/**
	 * @brief Gets the direction in which the passenger wants to travel.
	 *
	 * @return The direction of travel.
	 */
	ElevatorDirection getDirection() const { return direction; }

	/**
	 * @brief Gets the starting floor of the passenger.
	 *
	 * @return The starting floor number.
	 */
	int getStartFloor() const { return startFloor; }

	/**
	 * @brief Gets the destination floor of the passenger.
	 *
	 * @return The destination floor number.
	 */
	int getEndFloor() const { return endFloor; }

	/**
	 * @brief Gets the start time of the passenger.
	 *
	 * @return The start time in seconds.
	 */
	int getStartTime() const { return startTime; }

	/**
	 * @brief Gets the unique identifier of the passenger.
	 *
	 * @return The passenger ID.
	 */
	int getPassengerID() const { return passengerID; }

	/**
	 * @brief Calculates the waiting time of the passenger.
	 *
	 * @param currentTime The current time in seconds.
	 */
	void calculateWaitTime(int currentTime) { waitTime = currentTime - startTime; }

	/**
	 * @brief Calculates the travel time of the passenger.
	 *
	 * @param currentTime The current time in seconds.
	 */
	void calculateTravelTime(int currentTime) { travelTime = currentTime - (startTime + waitTime); }

private:
	int passengerID; // The unique identifier for the passenger.
	int startTime; // The time at which the passenger arrives.
	int startFloor; // The floor from which the passenger starts.
	int endFloor; // The floor to which the passenger wants to go.
	ElevatorDirection direction; // The direction in which the passenger wants to travel.
	int waitTime; // The amout of time passenger waits for elevator.
	int travelTime; // The travel time of the passenger. Time when passenger gets on elevator - time when passenger arrives at destination
};
