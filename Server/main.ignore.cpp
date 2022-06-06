#include <iostream>
//stringstream
#include <sstream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
using namespace std;


int main() {

    stringstream ss;
    // cout << ss.str() << endl;
    fstream ok("main.ignore.cpp", ios::in);
    while (1)
        {
            ss << "Hello World";
            // pause();
        }
    return 0;
}
