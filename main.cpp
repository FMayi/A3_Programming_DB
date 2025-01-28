/*
Skeleton code for External storage management
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    // Initialize the Storage Manager Class with the Binary .dat file name we want to create
    StorageManager manager("EmployeeRelation.dat");

    // Assuming the Employee.CSV file is in the same directory, 
    // we want to read from the Employee.csv and write into the new data_file
    manager.createFromFile("Employee.csv");

    // Searching for Employee IDs Using [manager.findAndPrintEmployee(id)]
    /***TO_DO***/
    for (int i = 1; i < argc; ++i) {
        int searchId = stoi(argv[i]);
        cout << "\nSearching Employee ID " << searchId << ": ";
        manager.findAndPrintEmployee(searchId);
    }

    /* Search the employee by ID one by one as an infinite loop uncomment if needed*/
    // int searchId;
    // while (true) {
    //     cout << "Enter Employee ID to search (Input 0 to exit): ";
    //     cin >> searchId;
    //     if (!searchId) break;
    //     manager.findAndPrintEmployee(searchId);
    // }

    return 0;
}
