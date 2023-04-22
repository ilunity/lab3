#include <sstream>
#include <mpi.h>
#include <iostream>
#include <cmath>
#include "sha256.h"

using namespace std;

int DEFAULT_DIFFICULTY = 5;


bool isValid(string hash) {
    for (int i = 0; i < DEFAULT_DIFFICULTY; i++) {
        if (hash.at(i) != '0')
            return false;
    }
    return true;
}

string my_to_string(unsigned int v) {
    stringstream ss;
    ss << v;
    string r;
    ss >> r;
    return r;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int size, rank, root = 0, tag =8;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    SHA256 sha256;

    string result_string, hash, input_string = "heyhfd";

    unsigned int counter = pow(10, 16) / size;
    counter *= rank;


    bool status;
    int result_count, result_rank = size;

    while (result_rank == size) {
        result_string = input_string + my_to_string(++counter);
        hash = sha256.hash(result_string);

        status = isValid(hash);

        if (counter % 1000000 == 0 || status) {
            int send_rank = status ? rank : size;
            MPI_Allreduce(&send_rank, &result_rank, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        }
    }

    if (rank == result_rank) {
        if (rank != root) {
            MPI_Send(&counter, 1, MPI_INT, root, tag, MPI_COMM_WORLD);
        } else {
            result_count = counter;
        }
    }

    if (rank == root) {
        if (rank != result_rank) {
            MPI_Recv(&result_count, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        cout << "Counter:" << result_count << endl;
        cout << "Hash: " << sha256.hash(input_string + my_to_string(result_count)) << endl;
        cout << "Rank: " << result_rank << endl;
    }


    MPI_Finalize();
}
