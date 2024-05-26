#include "essentials.h"



listMutex_t* pseudocodeOfProcesses;




pseudocodeInfo* currentPseudocode;



pseudocodeInfo* getCurrentPseudocode()
{
    return currentPseudocode;
}



void setCurrentPseudocode(pseudocodeInfo* value)
{
    currentPseudocode = value;
}






int auxPID;

bool closurePIDsAreEqual(void* element)
{
    return ((pseudocodeInfo*)element)->PID == auxPID;
}