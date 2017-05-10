/*
 * proof_of_work.cpp
 *
 *  Created on: 10/may/2017
 *      Author: Federico Lombardi
 */

#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "sha256.h"
#include "proof_of_work.h"

using namespace std;

int DEFAULT_DIFFICULTY = 5;
int DEFAULT_THREAD_NUM = 4;
int difficulty;
int threadNum;
int id = 0;
int winner = -1;

class Miner {

private:
	int minerId;

public:
	Miner() {
		this->minerId = id++;
	}

	int getMinerId() {
		return this->minerId;
	}
};

bool isValid(string hash) {
	for (int i = 0; i< DEFAULT_DIFFICULTY; i++) {
		if (hash.at(i) != '0')
			return false;
	}
	return true;
}

void minerThread(Miner* miner) {
	srand (time(NULL));

	string textBase("base_" + miner->getMinerId());
	string result;
	string hash;
	int random;
	std::stringstream sstm;

	int counter = 0;
	do {
		// compute a step of the PoW
		random = (rand() % RAND_MAX + counter);
		sstm << textBase << random;
		result = sstm.str();
		hash = sha256(result);

		// increment counter and rest sstm
		counter++;
		sstm.str("");
		sstm.clear();
	} while (!isValid(hash));

	cout << "Miner " << miner->getMinerId() << " found a result after " << counter << " iteration! Random number = " << random << "; hash = " << hash << endl;
	if (winner == -1) winner = miner->getMinerId();
}

int main(int argc, char* argv[]) {

	if (argc > 1) {
		try {
			difficulty = atoi(argv[1]);
			if (argc > 2)
				threadNum = atoi(argv[2]);
		}
		catch(const std::exception &e) {
			cout << "Something wrong with input parameters. Setting default values." << endl;
			difficulty = DEFAULT_DIFFICULTY;
			threadNum = DEFAULT_THREAD_NUM;
		}
	}
	else {
		cout << "No input difficulty and threadNum provided. Setting default values." << endl;
		difficulty = DEFAULT_DIFFICULTY;
		threadNum = DEFAULT_THREAD_NUM;
	}

	Miner miners[threadNum];
	thread minerThreads[threadNum];

	for (int i = 0; i < threadNum; i++) {
		cout << "Starting Miner i=" << i << " id=" << miners[i].getMinerId() << endl;
		minerThreads[i] = thread(minerThread, &miners[i]);
	}

	for (int i = 0; i < threadNum; i++) {
		minerThreads[i].join();
	}

	cout << "The winner is Miner " << winner << endl;
	return 0;
}
