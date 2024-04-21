/**
 * @file Elevator.h
 * @brief Declaration and implementation of the Elevator class.
 *
 * The Elevator class represents an elevator in the building.
 * It manages the movement of the elevator, including picking up and dropping off passengers,
 * and logging relevant information.
 *
 * @date 4/20/2024
 * @version 1.0
 * @author Jerry Wang
 */

#pragma once
#include "Passenger.h"
#include "ElevatorState.h"
#include "Floor.h"
#include "spdlog/spdlog.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <deque>
#include <memory>
#include <array>

class Elevator {
public:
	/**
	 * @brief Constructs an Elevator object with the specified parameters.
	 *
	 * @param elevatorNum The unique identifier for the elevator.
	 * @param speed The speed of the elevator in floors per second.
	 * @param elevatorStoppingTime The time it takes for the elevator to stop at each floor.
	 * @param logFileName The file name for logging elevator activities.
	 */
	Elevator(int elevatorNum, int speed, int elevatorStoppingTime, std::string logFileName)
		: elevatorID(elevatorNum), ELEVATOR_SPEED(speed), ELEVATOR_STOP_TIME{ elevatorStoppingTime },
		direction(ElevatorDirection::UP), state(ElevatorState::STOPPED),
		logFileName{ logFileName + "_elevator_" + std::to_string(elevatorNum) },
		logFileLocation{ "logs/" + this->logFileName + ".txt" } {
		log = spdlog::basic_logger_mt(this->logFileName, this->logFileLocation);
	}

	/**
	 * @brief Checks if the elevator has passengers.
	 *
	 * @return True if the elevator has passengers, false otherwise.
	 */
	bool hasPassengers() const {
		return !passengers.empty();
	}

	/**
	 * @brief Updates the state of the elevator.
	 *
	 * This method handles the logic for moving the elevator, picking up and dropping off passengers,
	 * and updating the elevator state.
	 *
	 * @param currentTime The current simulation time in seconds.
	 * @param NUM_OF_FLOORS The total number of floors in the building.
	 * @param floors A vector containing references to all the floors in the building.
	 */
	void update(int currentTime, const int NUM_OF_FLOORS, std::vector<Floor>& floors) {
		switch (state) {
		case ElevatorState::STOPPED: // Stopped State
			// Discharge passengers if there are any that get off at this floor
			if (!passengers.empty()) {
				dropOffPassengers(floors.at(currentFloor - 1), currentTime);
			}

			// if we are at the top or bottom floor, change direction so we can pick the right passengers
			if (currentFloor == 1) {
				direction = ElevatorDirection::UP;
			}
			else if (currentFloor == NUM_OF_FLOORS) {
				direction = ElevatorDirection::DOWN;
			}

			// If there are passengers waiting on this floor and going in the same direction, pick them up
			// if the elevator is not at capacity
			if (!floors.at(currentFloor - 1).getWaitingPassengers().empty()) {
				pickUpPassengers(floors.at(currentFloor - 1), currentTime);
			}

			// Keep moving in the same direction until we reach the top floor
			if (ElevatorDirection::UP == direction && currentFloor != NUM_OF_FLOORS) {
				state = ElevatorState::MOVING_UP;
				nextActionTime = currentTime + ELEVATOR_SPEED;
			}
			// reach top floor, change direction and start moving down
			else if (ElevatorDirection::UP == direction && currentFloor == NUM_OF_FLOORS) {
				state = ElevatorState::MOVING_DOWN;
				nextActionTime = currentTime + ELEVATOR_SPEED;
				direction = ElevatorDirection::DOWN;
			}
			// keep moving down until we reach the bottom floor
			else if (ElevatorDirection::DOWN == direction && currentFloor != 1) {
				state = ElevatorState::MOVING_DOWN;
				nextActionTime = currentTime + ELEVATOR_SPEED;
			}
			// reach bottom floor, change direction and start moving up
			else if (ElevatorDirection::DOWN == direction && currentFloor == 1) {
				state = ElevatorState::MOVING_UP;
				nextActionTime = currentTime + ELEVATOR_SPEED;
				direction = ElevatorDirection::UP;
			}
			break;

		case ElevatorState::STOPPING: // Stopping State
			// If the elevator is currently stopping, check if the stopping time has elapsed
			if (nextActionTime == currentTime) {
				state = ElevatorState::STOPPED;
			}
			break;

		case ElevatorState::MOVING_UP: // Moving Up State
			// If the elevator has reached its next action time, move up a floor
			if (nextActionTime == currentTime) {
				++currentFloor;

				// Check if the elevator should stop at this floor
				if (shouldStopAtFloor(floors.at(currentFloor - 1))) {
					state = ElevatorState::STOPPING;
					nextActionTime = currentTime + ELEVATOR_STOP_TIME - 1; // -1 second to account for stopped state which takes 1 second to execute
				}
				// if not and we are not at the top floor, keep moving up
				else if (currentFloor != NUM_OF_FLOORS) {
					nextActionTime = currentTime + ELEVATOR_SPEED;
				}
				// if we are at the top floor, change direction to down and start moving down
				else {
					state = ElevatorState::MOVING_DOWN;
					nextActionTime = currentTime + ELEVATOR_SPEED;
					direction = ElevatorDirection::DOWN;
				}
			}
			break;

		case ElevatorState::MOVING_DOWN: // Moving Down State
			// If the elevator has reached its next action time, move down a floor
			if (nextActionTime == currentTime) {
				--currentFloor;

				// Check if the elevator should stop at this floor
				if (shouldStopAtFloor(floors.at(currentFloor - 1))) {
					state = ElevatorState::STOPPING;
					nextActionTime = currentTime + ELEVATOR_STOP_TIME - 1; // -1 second to account for stopped state which takes 1 second to execute
				}
				// if not and we are not at the bottom floor, keep moving down
				else if (currentFloor != 1) {
					nextActionTime = currentTime + ELEVATOR_SPEED;
				}
				// if we are at the bottom floor, change direction to up and start moving up
				else {
					state = ElevatorState::MOVING_UP;
					nextActionTime = currentTime + ELEVATOR_SPEED;
					direction = ElevatorDirection::UP;
				}
			}
			break;
		} // end of switch
	}

private:
	int elevatorID; /**< The unique identifier for the elevator. */
	int currentFloor = 1; /**< The current floor where the elevator is located. */
	int nextActionTime = 0; /**< The time for the next action of the elevator. */
	const int ELEVATOR_SPEED; /**< The speed of the elevator in floors per second. */
	const int ELEVATOR_STOP_TIME; /**< The time it takes for the elevator to stop at each floor. */
	const int CAPACITY = 8; /**< The maximum capacity of the elevator. */
	const std::string logFileName; /**< The file name for logging elevator activities. */
	const std::string logFileLocation; /**< The file location for logging elevator activities. */
	ElevatorState state; /**< The current state of the elevator. */
	ElevatorDirection direction; /**< The current direction of the elevator. */
	std::deque<Passenger> passengers; /**< A queue of passengers currently inside the elevator. */
	std::shared_ptr<spdlog::logger> log; /**< A logger for recording elevator activities. */

	/**
	 * @brief Checks if the elevator should stop at the given floor.
	 *
	 * This method determines whether the elevator should stop at a particular floor based on the
	 * destination floors of the passengers inside the elevator and the waiting passengers on the floor.
	 *
	 * @param floor The floor to check.
	 * @return True if the elevator should stop at the floor, false otherwise.
	 */
	bool shouldStopAtFloor(Floor& floor) {
		// check if there are passengers in the elevator that want to get off at this floor
		for (auto& passenger : passengers) {
			if (passenger.getEndFloor() == floor.getFloorNumber()) {
				return true;
			}
		}

		// if we are at capacity, don't stop
		if (passengers.size() > CAPACITY) {
			return false;
		}

		// check if there are passengers on this floor that want to go in the same direction
		for (auto& passenger : floor.getWaitingPassengers()) {
			if (passenger.getDirection() == direction) {
				return true;
			}
		}

		return false;
	}

	/**
	 * @brief Picks up passengers waiting on the given floor.
	 *
	 * This method picks up passengers waiting on the floor who are going in the same direction as the elevator.
	 *
	 * @param floor The floor from which to pick up passengers.
	 * @param currentTime The current simulation time in seconds.
	 */
	void pickUpPassengers(Floor& floor, int currentTime) {
		// pick up passengers that are going in the same direction, up to the capacity of the elevator
		for (auto it = floor.getWaitingPassengers().begin(); it != floor.getWaitingPassengers().end();) {
			// if the elevator is at capacity, stop picking up passengers
			if (passengers.size() == CAPACITY) {
				break;
			}

			// if the passenger is going in the same direction as the elevator, pick them up
			if (it->getDirection() == direction) {
				it->calculateWaitTime(currentTime);
				passengers.push_back(*it);

				logStatusPickup(currentTime, *it);
				it = floor.getWaitingPassengers().erase(it);
			}
			else {
				++it;
			}
		}
	}

	/**
	 * @brief Drops off passengers at the given floor.
	 *
	 * This method drops off passengers inside the elevator whose destination is the given floor.
	 *
	 * @param floor The floor at which to drop off passengers.
	 * @param currentTime The current simulation time in seconds.
	 */
	void dropOffPassengers(Floor& floor, int currentTime) {
		for (auto it = passengers.begin(); it != passengers.end();) {
			if (it->getEndFloor() == floor.getFloorNumber()) {
				it->calculateTravelTime(currentTime);
				floor.getDeliveredPassengers().push_back(*it);

				logStatusDropoff(currentTime, *it);
				it = passengers.erase(it);
			}
			else {
				++it;
			}
		}
	}

	/**
	 * @brief Logs the status of picking up a passenger.
	 *
	 * This method logs information about picking up a passenger, including the current time, floor, direction,
	 * elevator state, number of passengers, and the passenger's ID and start floor.
	 *
	 * @param currentTime The current simulation time in seconds.
	 * @param passenger The passenger being picked up.
	 */
	void logStatusPickup(int currentTime, const Passenger& passenger) {
		log->info("Time: {}", currentTime);
		log->info("Current floor: {}", currentFloor);
		log->info("Direction: {}", direction == ElevatorDirection::UP ? "UP" : "DOWN");
		log->info("State: {}", state == ElevatorState::STOPPED ? "STOPPED" : state == ElevatorState::MOVING_UP ? "MOVING UP" : "MOVING DOWN");
		log->info("Number of passengers: {}", passengers.size());
		log->info("Passengers On Board: ");
		for (auto& passenger : passengers) {
			log->info("\tPassenger {}", passenger.getPassengerID());
		}
		log->info("Passenger {} picked up at floor {} at time {}", passenger.getPassengerID(), passenger.getStartFloor(), currentTime);
		log->info("\n");
	}

	/**
	 * @brief Logs the status of dropping off a passenger.
	 *
	 * This method logs information about dropping off a passenger, including the current time, floor, direction,
	 * elevator state, number of passengers, and the passenger's ID and end floor.
	 *
	 * @param currentTime The current simulation time in seconds.
	 * @param passenger The passenger being dropped off.
	 */
	void logStatusDropoff(int currentTime, const Passenger& passenger) {
		log->info("Time: {}", currentTime);
		log->info("Current floor: {}", currentFloor);
		log->info("Direction: {}", direction == ElevatorDirection::UP ? "UP" : "DOWN");
		log->info("State: {}", state == ElevatorState::STOPPED ? "STOPPED" : state == ElevatorState::MOVING_UP ? "MOVING UP" : "MOVING DOWN");
		log->info("Number of passengers: {}", passengers.size());
		log->info("Passengers On Board: ");
		for (auto& passenger : passengers) {
			log->info("\tPassenger {}", passenger.getPassengerID());
		}
		log->info("Passenger {} dropped off at floor {} at time {}", passenger.getPassengerID(), passenger.getEndFloor(), currentTime);
		log->info("\n");
	}
};
