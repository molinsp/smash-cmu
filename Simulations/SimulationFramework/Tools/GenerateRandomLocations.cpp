/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

#include <cstdlib>
#include <cstdio>
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <iomanip>
using std::setprecision;
#include <time.h>
#include <string>
using std::string;

void printProgramSummary(const char* const progName)
{
    cerr << "use output redirection to direct to desired file" << endl;
    cerr << " -m <matrix_name>" << endl;
    cerr << " -i <num_data>" << endl;
    cerr << " -w <western_longitude>" << endl;
    cerr << " -e <eastern_longitude>" << endl;
    cerr << " -n <northern_latitude>" << endl;
    cerr << " -s <souther_latitude>" << endl;
}

void handleArgs(const int& argc, const char* const argv[],
    int& numBills, double& minX, double& maxX, double& minY, double& maxY,
    string& name)
{
    if(argc != 13)
    {
        printProgramSummary(argv[0]);
        exit(-1);
    }

    for (int i = 1; i < argc; ++i)
    {
        std::string arg1 (argv[i]);
    
        if (arg1 == "-i")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%d", &numBills);
        }
        else if (arg1 == "-m")
        {
            if (i + 1 < argc)
                name = string(argv[++i]);
        }
        else if (arg1 == "-w")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%lf", &minX);
        }
        else if (arg1 == "-e")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%lf", &maxX);
        }
        else if (arg1 == "-s")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%lf", &minY);
        }
        else if (arg1 == "-n")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%lf", &maxY);
        }
        else
        {
            printProgramSummary(argv[0]);
            exit(-1);
        }
    }
}

double frand(const double& min, const double& max)
{
    return ((double)rand()) / RAND_MAX * (max - min) + min;
}

int main(int argc, char* argv[])
{
    int numBills;
    double minX, maxX, minY, maxY;
    string name;
    handleArgs(argc, argv, numBills, minX, maxX, minY, maxY, name);

    srand(time(NULL));

    cout << setprecision(15);
    cout << name << " = {}" << endl;
    for(int i = 1; i <= numBills; ++i)
    {
        cout << name << "[" << i << "] = {}" << endl;
        double x = frand(minX, maxX);
        double y = frand(minY, maxY);
        cout << name << "[" << i << "][1] = " << x << endl;
        cout << name << "[" << i << "][2] = " << y << endl;
    }

    return 0;
}
