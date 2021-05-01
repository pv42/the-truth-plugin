// SheetsApiTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SheetsAPI.h"
#include "logger.h"
#include <vector>
#include <string>
#include <memory>


using std::shared_ptr;

int main()
{
    std::cout << "Hello World!\n";
    string key = "AIzaSyDXE1rxSTrA_6g0T8ax9goY4MUB4UO5MSg";
    string id = "19AEHyOVnXCzTlVmmKROu7AHLn9NYN7JzjLCNm7KE9Tc";
    Logger::init("");
    SheetsAPI api(id, key);
    api.downloadNames();
    shared_ptr<vector<string>> names = api.getNames();

    std::cout << names->size() << std::endl;
    for(string s: *names) {
        std::cout << s << std::endl;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
