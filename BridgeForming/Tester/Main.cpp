/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "MadaraController.h"


int main (int argc, char** argv)
{
    MadaraController* madaraController = new MadaraController(100, 2.0);
    
    getc(stdin);
    return 0;
}
