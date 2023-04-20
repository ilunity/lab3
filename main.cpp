#include <string.h>
#include <mpi.h>
#include <iostream>
#include <cmath>
#include "sha256.h"

using namespace std;

int DEFAULT_DIFFICULTY = 7;


bool isValid(string hash) {
    for (int i = 0; i < DEFAULT_DIFFICULTY; i++) {
        if (hash.at(i) != '0')
            return false;
    }
    return true;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    SHA256 sha256;

    string result_string, hash, input_string = "heyhfd";

    unsigned int counter = pow(10, 16) / size;
    counter *= rank;


    bool status = false;
    bool need_rest = false;
    int result_count;

    while (!need_rest) {
        result_string = input_string + to_string(counter);
        hash = sha256.hash(result_string);

        counter++;
        status = isValid(hash);

        if (counter % 1000000 == 0 || status) {
            MPI_Allreduce(&status, &need_rest, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);

            if (rank == 0) {
                cout << counter << endl;
            }
        }
    }

    if (status) {
        counter--;
        MPI_Send(&counter, 1, MPI_INT, 0, 8, MPI_COMM_WORLD);
    }

    MPI_Status mpi_status;
    if (rank == 0) {
        MPI_Recv(&result_count, 1, MPI_INT, MPI_ANY_SOURCE, 8, MPI_COMM_WORLD, &mpi_status);
        cout << "Counter:" << result_count << endl;
        cout << sha256.hash(input_string + to_string(result_count)) << endl;
    }


    MPI_Finalize();
}
