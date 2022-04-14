#include "Akinator.h"


int main ()
{
    Akinator Akinator ("AkinatorData.txt");

    Akinator.guess();

    Akinator.getGraphOfTree();

    return 0;
}
