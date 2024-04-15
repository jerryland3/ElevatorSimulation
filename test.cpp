#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>

using namespace std;

// Constants
const int NUM_ELEVATORS = 4;    // Number of elevators in the building
const int NUM_FLOORS = 100;     // Number of floors in the building
const int MAX_PASSENGERS = 8;   // Maximum number of passengers allowed in an elevator
const int INITIAL_FLOOR_TRAVEL_TIME = 10; // Initial time (in seconds) for an elevator to move between floors
const int IMPROVED_FLOOR_TRAVEL_TIME = 5; // Improved time (in seconds) for an elevator to move between floors
const int STOPPING_TIME = 2; // Time (in seconds) for an elevator to stop

// Enums
enum ElevatorState { STOPPED, STOPPING, MOVING_UP, MOVING_DOWN };  // Possible states of an elevator

// Classes
class Passenger {
public:
	int startTime, startFloor, endFloor;  // Passenger's start time, start floor, and destination floor
	int waitTime, travelTime;  // Time spent waiting for an elevator and time spent traveling in an elevator

	Passenger(int st, int sf, int ef) {
		startTime = st;
		startFloor = sf;
		endFloor = ef;
		waitTime = 0;
		travelTime = 0;
	}
};

class Floor {
public:
	int floorNumber;  // Floor number
	queue<Passenger*> waitingPassengers;  // Queue of passengers waiting on the floor

	Floor(int n) {
		floorNumber = n;
	}
};

class Building {
public:
	vector<Floor> floors;  // Vector of floors in the building
	vector<Elevator> elevators;  // Vector of elevators in the building

	Building() {
		floors.reserve(NUM_FLOORS);  // Reserve space for floors
		for (int i = 0; i < NUM_FLOORS; i++) {
			floors.emplace_back(i);  // Create and add a floor object for each floor
		}
		elevators.reserve(NUM_ELEVATORS);  // Reserve space for elevators
		for (int i = 0; i < NUM_ELEVATORS; i++) {
			elevators.emplace_back();  // Create and add an elevator object for each elevator
		}
	}

	void addPassengerToFloor(Passenger* passenger) {
		floors[passenger->startFloor].waitingPassengers.push(passenger);  // Add a passenger to the queue for their start floor
	}

	void runSimulation(vector<Passenger*>& passengers, int floorTravelTime) {
		// Initialize passenger queues
		for (Passenger* p : passengers) {
			addPassengerToFloor(p);
		}

		int currentTime = 0;
		while (!allPassengersArrived(passengers)) {
			// Update elevators
			for (Elevator& elevator : elevators) {
				elevator.update(currentTime, floors);
			}

			currentTime++;
		}
	}

	bool allPassengersArrived(const vector<Passenger*>& passengers) {
		for (Passenger* p : passengers) {
			if (p->travelTime == 0) {
				return false;  // If any passenger hasn't arrived yet, return false
			}
		}
		return true;  // All passengers have arrived
	}
};

class Elevator {
public:
	int currentFloor;  // Current floor of the elevator
	int nextStopTime;  // Time when the elevator will next stop
	ElevatorState state;  // Current state of the elevator
	vector<Passenger*> passengers;  // Vector of passengers currently in the elevator

	Elevator() {
		currentFloor = 0;  // Elevators start at the ground floor
		nextStopTime = 0;
		state = STOPPED;
	}

	void update(int time, vector<Floor>& floors) {
		switch (state) {
		case STOPPED:
			if (time >= nextStopTime) {
				pickUpPassengers(floors[currentFloor].waitingPassengers);  // Pick up passengers from the current floor
				if (!passengers.empty()) {
					int nextFloor = passengers[0]->endFloor;  // Get the destination floor of the first passenger
					if (nextFloor > currentFloor) {
						state = MOVING_UP;
					}
					else {
						state = MOVING_DOWN;
					}
					nextStopTime = time + INITIAL_FLOOR_TRAVEL_TIME;  // Set the next stop time based on the travel time
				}
			}
			break;
		case STOPPING:
			if (time >= nextStopTime) {
				state = STOPPED;
				dischargePassengers(floors[currentFloor].waitingPassengers);  // Discharge passengers at the current floor
				pickUpPassengers(floors[currentFloor].waitingPassengers);  // Pick up new passengers
			}
			break;
		case MOVING_UP:
			if (time >= nextStopTime) {
				currentFloor++;  // Move up one floor
				if (shouldStop(currentFloor, floors)) {
					state = STOPPING;
					nextStopTime = time + STOPPING_TIME;
				}
				else {
					nextStopTime = time + INITIAL_FLOOR_TRAVEL_TIME;
				}
			}
			break;
		case MOVING_DOWN:
			if (time >= nextStopTime) {
				currentFloor--;  // Move down one floor
				if (shouldStop(currentFloor, floors)) {
					state = STOPPING;
					nextStopTime = time + STOPPING_TIME;
				}
				else {
					nextStopTime = time + INITIAL_FLOOR_TRAVEL_TIME;
				}
			}
			break;
		}
	}

private:
	bool shouldStop(int floor, const vector<Floor>& floors) {
		for (Passenger* p : passengers) {
			if (p->endFloor == floor) {
				return true;  // Stop if any passenger's destination is the current floor
			}
		}
		return !floors[floor].waitingPassengers.empty();  // Stop if there are passengers waiting on the current floor
	}

	void dischargePassengers(queue<Passenger*>& floorQueue) {
		vector<Passenger*> toRemove;
		for (Passenger* p : passengers) {
			if (p->endFloor == currentFloor) {
				p->travelTime = nextStopTime - p->startTime - p->waitTime;  // Calculate the passenger's travel time
				toRemove.push_back(p);  // Mark the passenger for removal from the elevator
			}
		}
		for (Passenger* p : toRemove) {
			passengers.erase(remove(passengers.begin(), passengers.end(), p), passengers.end());  // Remove the passenger from the elevator
		}
	}

	void pickUpPassengers(queue<Passenger*>& floorQueue) {
		while (!floorQueue.empty() && passengers.size() < MAX_PASSENGERS) {
			Passenger* p = floorQueue.front();
			floorQueue.pop();
			p->waitTime = nextStopTime - p->startTime;  // Calculate the passenger's wait time
			passengers.push_back(p);  // Add the passenger to the elevator
		}
	}
};

// Function prototypes
vector<Passenger*> readPassengersFromFile(const string& filename);
double calculateAverageWaitTime(const vector<Passenger*>& passengers);
double calculateAverageTravelTime(const vector<Passenger*>& passengers);

int main() {
	// Read passenger data from file
	vector<Passenger*> passengers = readPassengersFromFile("Elevator.csv");

	// Create a building
	Building building;

	// Run simulation with initial floor travel time
	building.runSimulation(passengers, INITIAL_FLOOR_TRAVEL_TIME);
	double initialAverageWaitTime = calculateAverageWaitTime(passengers);
	double initialAverageTravelTime = calculateAverageTravelTime(passengers);

	cout << "Initial average wait time: " << initialAverageWaitTime << " seconds" << endl;
	cout << "Initial average travel time: " << initialAverageTravelTime << " seconds" << endl;

	// Reset passenger data
	for (Passenger* p : passengers) {
		p->waitTime = 0;
		p->travelTime = 0;
	}

	// Run simulation with improved floor travel time
	building.runSimulation(passengers, IMPROVED_FLOOR_TRAVEL_TIME);
	double improvedAverageWaitTime = calculateAverageWaitTime(passengers);
	double improvedAverageTravelTime = calculateAverageTravelTime(passengers);

	cout << "Improved average wait time: " << improvedAverageWaitTime << " seconds" << endl;
	cout << "Improved average travel time: " << improvedAverageTravelTime << " seconds" << endl;

	double waitTimeReduction = (1 - improvedAverageWaitTime / initialAverageWaitTime) * 100;
	double travelTimeReduction = (1 - improvedAverageTravelTime / initialAverageTravelTime) * 100;

	cout << "Percentage reduction in average wait time: " << waitTimeReduction << "%" << endl;
	cout << "Percentage reduction in average travel time: " << travelTimeReduction << "%" << endl;
}