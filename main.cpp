#include <string>
#include <iostream>
#include <cmath>
#include "sha256.h"

using namespace std;

int DEFAULT_DIFFICULTY = 6;


bool isValid(string hash) {
    for (int i = 0; i < DEFAULT_DIFFICULTY; i++) {
        if (hash.at(i) != '0')
            return false;
    }
    return true;
}

int main(int argc, char **argv) {
    SHA256 sha256;

    string result, hash, input_string = "heyhfd";

    long random;
    int counter = 1534524;
    do {
//        random = ((rand() % RAND_MAX) << 32) + (rand() % RAND_MAX);
        random = (rand() % RAND_MAX) + counter;
//        result = input_string + to_string(counter);
        result = input_string + to_string(random);
        hash = sha256.hash(result);

        counter++;
        if (counter % 100000 == 0) {
            cout << counter << endl;
        }
    } while (!isValid(hash));

    counter--;
    cout << "Random:" << random << endl;
    cout << "Counter:" << counter << endl;
    cout << sha256.hash(input_string + to_string(counter)) << endl;
}
