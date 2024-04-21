/**
 * @file Floor.h
 * @brief Declaration of the Floor class.
 *
 * The Floor class represents a floor in a building.
 * It manages waiting and delivered passengers on the floor.
 *
 * @date 4/20/2024
 * @version 1.0
 * @author Jerry Wang
 */

#pragma once
#include "Passenger.h"
#include <queue>
#include <memory>

class Floor {
public:
	/**
	 * @brief Constructs a Floor object with the specified floor number.
	 *
	 * @param floorNumber The floor number of the floor.
	 * @throw std::invalid_argument if floor number is negative or greater than 100.
	 */
	Floor(int floorNumber) : floorNumber(floorNumber) {
		if (floorNumber < 0 || floorNumber > 100) {
			throw std::invalid_argument("Floor number must be non-negative");
		}
	}

	/**
	 * @brief Adds a waiting passenger to the floor.
	 *
	 * @param passenger The passenger to be added to the floor.
	 */
	void addWaitingPassenger(Passenger& passenger) {
		waitingPassengers.push_back(passenger);
	}

	/**
	 * @brief Gets the floor number of the floor.
	 *
	 * @return The floor number.
	 */
	int getFloorNumber() const {
		return floorNumber;
	}

	/**
	 * @brief Gets the queue of waiting passengers on the floor.
	 *
	 * @return A reference to the deque of waiting passengers.
	 */
	std::deque<Passenger>& getWaitingPassengers() {
		return waitingPassengers;
	}

	/**
	 * @brief Gets the queue of delivered passengers on the floor.
	 *
	 * @return A reference to the deque of delivered passengers.
	 */
	std::deque<Passenger>& getDeliveredPassengers() {
		return deliveredPassengers;
	}

	/**
	 * @brief Checks if there are waiting passengers on the floor.
	 *
	 * @return true if there are waiting passengers, false otherwise.
	 */
	bool hasWaitingPassengers() const {
		return !waitingPassengers.empty();
	}

private:
	int floorNumber; // The floor number of the floor.
	std::deque<Passenger> waitingPassengers; // The deque of waiting passengers on the floor.
	std::deque<Passenger> deliveredPassengers; // The deque of delivered passengers on the floor.
};
