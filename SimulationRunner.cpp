#include "Building.h"
using namespace std;
int main() {
	int numOfFloors = 100;
	int numOfElevators = 4;
	int elevatorSpeedTime1 = 10;
	int elevatorSpeedTime2 = 5;
	int elevatorStoppingTime = 2;

	// create a building with 100 floors, 4 elevators, and 10 seconds for elevator to move between floors
	cout << "Building 1: 10 seconds for elevator to move between floors" << endl;
	Building myBuilding1(numOfFloors, numOfElevators, elevatorSpeedTime1, elevatorStoppingTime);
	myBuilding1.simulate();

	cout << "\n\n\nBuilding 2: 5 seconds for elevator to move between floors" << endl;
	// create a building with 100 floors, 4 elevators, and 5 seconds for elevator to move between floors
	Building myBuilding2(numOfFloors, numOfElevators, elevatorSpeedTime2, elevatorStoppingTime);
	myBuilding2.simulate();
}