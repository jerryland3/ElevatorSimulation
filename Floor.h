#pragma once
#include"Passenger.h"
#include <queue>
#include <memory>

class Floor {
public:
	Floor(int floorNumber) {
		if (floorNumber < 0 || floorNumber > 100) {
			throw std::invalid_argument("Floor number must be non-negative");
		}
		this->floorNumber = floorNumber;
	};

	void addWaitingPassenger(Passenger& passenger) {
		waitingPassengers.push_back(std::move(passenger));
	}

	int getFloorNumber() const {
		return floorNumber;
	}

	std::deque<Passenger>& getWaitingPassengers() {
		return waitingPassengers;
	}

	std::deque<Passenger>& getDeliveredPassengers() {
		return deliveredPassengers;
	}

	bool hasWaitingPassengers() const {
		return !waitingPassengers.empty();
	}

private:
	int floorNumber;
	std::deque<Passenger> waitingPassengers;
	std::deque<Passenger> deliveredPassengers;
};
