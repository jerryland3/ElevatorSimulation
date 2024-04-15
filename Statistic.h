#pragma once
/*
 * Class: Statistic
 * Contain information on statistics. Can add numbers to ths list, calculate
 * standard deviation and mean.
 *
 * @date: 1/30/2024
 * @version: New
 * @author: Jerry Wang
 */
#include <string>
#include <list>
#include <limits>
#include <iostream>

class Statistic
{
public:

	/**
	* Constructor: Statistic
	*
	* Set the initial list of numbers.
	*
	* @param numberList - list of numbers. Type: std::list<double>
	*
	*/
	explicit Statistic() {}

	/**
	  * Method: getAverage
	  *
	  * Calculate the mean of the list of numbers
	  *
	  * @return - mean of the list of numbers. Type: double
	  */
	double getAverage() const {
		int sum = 0;
		for (auto element : this->numberList) {
			sum += element;
		}

		if (this->numberList.size() == 0) {
			return std::numeric_limits<double>::quiet_NaN();
		}
		else {
			return static_cast<double>(sum) / this->numberList.size();
		}
	}

	/**
	  * Method: addNumber
	  *
	  * Add a number to the list
	  *
	  * @param number - number to be added. Type: double
	  * @return - void
	  */
	void addNumber(int number) {
		this->numberList.push_back(number);
	}

	/**
	  * Method: printList
	  *
	  * Print the list of numbers
	  *
	  * @return - void
	  */
	void printList() const {
		std::cout << "Number List: ";

		for (auto element : numberList) {
			std::cout << element << " ";
		}
		std::cout << std::endl;
	}

private:
	std::list<int> numberList;
};
