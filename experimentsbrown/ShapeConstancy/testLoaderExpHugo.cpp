#include <iostream>
#include <fstream>
#include <string>
#include "ParametersLoader.h"

using namespace std;

int main()
{
    std::string filename("parametersExpHugo.txt");

    ifstream paramfile;
    paramfile.open(filename.c_str());
    paramfile.close();
    
    ParametersLoader params;
    params.loadParameterFile(filename);
    params.print();

    return 0;
}
