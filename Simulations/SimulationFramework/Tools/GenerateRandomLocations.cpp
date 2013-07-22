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
#include <time.h>

void printProgramSummary(const char* const progName)
{
    cerr << "use output redirection to direct to desired file" << endl;
    cerr << " -n <num_bodies>" << endl;
    cerr << " -x <min_x_coord>" << endl;
    cerr << " -X <max_x_coord>" << endl;
    cerr << " -y <min_y_coord>" << endl;
    cerr << " -Y <max_y_coord>" << endl;
}

void handleArgs(const int& argc, const char* const argv[],
    int& numBills, double& minX, double& maxX, double& minY, double& maxY)
{
    if(argc != 11)
    {
        printProgramSummary(argv[0]);
        exit(-1);
    }

    for (int i = 1; i < argc; ++i)
    {
        std::string arg1 (argv[i]);
    
        if (arg1 == "-n")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%d", &numBills);
        }
        else if (arg1 == "-x")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%lf", &minX);
        }
        else if (arg1 == "-X")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%lf", &maxX);
        }
        else if (arg1 == "-y")
        {
            if (i + 1 < argc)
                sscanf(argv[++i], "%lf", &minY);
        }
        else if (arg1 == "-Y")
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
    handleArgs(argc, argv, numBills, minX, maxX, minY, maxY);

    srand(time(NULL));

    cout << "g_bill_index = 1" << endl;
    cout << "g_bill_locs = {}" << endl;
    for(int i = 1; i <= numBills; ++i)
    {
        cout << "g_bill_locs[" << i << "] = {}" << endl;
        double x = frand(minX, maxX);
        double y = frand(minY, maxY);
        double z = 0.08 - (x - 10) / 10 * 0.2 - (y - 10) / 10 * 0.1;
        cout << "g_bill_locs[" << i << "][1] = " << x << endl;
        cout << "g_bill_locs[" << i << "][2] = " << y << endl;
        cout << "g_bill_locs[" << i << "][3] = " << z << endl;
    }

    return 0;
}
