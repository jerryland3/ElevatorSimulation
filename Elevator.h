#pragma once
#include "Passenger.h"
#include "ElevatorState.h"
#include "Floor.h"
#include <deque>
#include <memory>
#include <array>

class Elevator {
public:
	Elevator(int elevatorNum, int speed) : elevatorID(elevatorNum), ELEVATOR_SPEED(speed), direction(ElevatorDirection::UP), state(ElevatorState::STOPPED), currentFloor{ 1 }, nextActionTime{ 0 } {}

	bool hasPassengers() const {
		return !passengers.empty();
	}

	void update(int currentTime, const int NUM_OF_FLOORS, std::vector<Floor>& floors) {
		switch (state)
		{
		case ElevatorState::STOPPED:
			// discharge passengers if there are any that gets off at this floor
			if (!passengers.empty())
			{
				dropOffPassengers(floors.at(currentFloor - 1), currentTime);
			}

			// if there are passenger waiting on this floor and going in the same direction, pick them up
			if (!floors.at(currentFloor - 1).getWaitingPassengers().empty())
			{
				pickUpPassengers(floors.at(currentFloor - 1), currentTime);
			}

			// keep moving in the same direction until we reach the top or bottom floor
			if (ElevatorDirection::UP == direction && currentFloor != NUM_OF_FLOORS) {
				state = ElevatorState::MOVING_UP;
				nextActionTime = currentTime + ELEVATOR_SPEED;
			}
			else if (ElevatorDirection::DOWN == direction && currentFloor != 1) {
				state = ElevatorState::MOVING_DOWN;
				nextActionTime = currentTime + ELEVATOR_SPEED;
			}
			break;

		case ElevatorState::STOPPING:
			if (nextActionTime == currentTime)
			{
				state = ElevatorState::STOPPED;
			}
			break;

		case ElevatorState::MOVING_UP:
			if (currentFloor == NUM_OF_FLOORS)
			{
				state = ElevatorState::MOVING_DOWN;
				nextActionTime = currentTime + ELEVATOR_SPEED;
				break;
			}

			if (nextActionTime == currentTime)
			{
				++currentFloor;
				if (currentFloor == NUM_OF_FLOORS)
				{
					direction = ElevatorDirection::DOWN;
				}
				if (shouldStopAtFloor(floors.at(currentFloor - 1)))
				{
					state = ElevatorState::STOPPING;
					nextActionTime = currentTime + ELEVATOR_STOP_TIME - 1;
				}
				else
				{
					nextActionTime = currentTime + ELEVATOR_SPEED;
				}
			}
			break;

		case ElevatorState::MOVING_DOWN:
			if (currentFloor == 1)
			{
				state = ElevatorState::MOVING_UP;
				nextActionTime = currentTime + ELEVATOR_SPEED;
				break;
			}
			if (nextActionTime == currentTime)
			{
				--currentFloor;
				if (currentFloor == 1)
				{
					direction = ElevatorDirection::UP;
				}
				if (shouldStopAtFloor(floors.at(currentFloor - 1)))
				{
					state = ElevatorState::STOPPING;
					nextActionTime = currentTime + ELEVATOR_STOP_TIME - 1;
				}
				else
				{
					nextActionTime = currentTime + ELEVATOR_SPEED;
				}
			}
			break;
		} // end of switch
	}
private:
	int elevatorID;
	int currentFloor;
	ElevatorDirection direction;
	int nextActionTime;
	const int ELEVATOR_SPEED;
	const int ELEVATOR_STOP_TIME = 2;
	const int CAPACITY = 8;
	ElevatorState state;
	std::deque<Passenger> passengers;

	bool shouldStopAtFloor(Floor& floor)
	{
		// check if there are passengers in the elevator that want to get off at this floor
		for (auto& passenger : passengers)
		{
			if (passenger.getEndFloor() == floor.getFloorNumber())
			{
				return true;
			}
		}

		// if we are at capacity, don't stop
		if (passengers.size() > CAPACITY)
		{
			return false;
		}

		// check if there are passengers on this floor that want to go in the same direction
		for (auto& passenger : floor.getWaitingPassengers())
		{
			if (passenger.getDirection() == direction)
			{
				return true;
			}
		}

		return false;
	}

	void pickUpPassengers(Floor& floor, int currentTime)
	{
		for (auto it = floor.getWaitingPassengers().begin(); it != floor.getWaitingPassengers().end();)
		{
			if (it->getDirection() == direction)
			{
				it->calculateWaitTime(currentTime);
				passengers.push_back(*it);
				it = floor.getWaitingPassengers().erase(it);
			}
			else
			{
				++it;
			}

			if (passengers.size() == CAPACITY)
			{
				break;
			}
		}
	}

	void dropOffPassengers(Floor& floor, int currentTime)
	{
		for (auto it = passengers.begin(); it != passengers.end();)
		{
			if (it->getEndFloor() == floor.getFloorNumber())
			{
				it->calculateTravelTime(currentTime);
				floor.getDeliveredPassengers().push_back(*it);
				it = passengers.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
};
