#include <drivers/driver.h>
using namespace myos;

drivers::Driver::Driver(){
    
}
drivers::Driver::~Driver(){
    
}

void drivers::Driver::Activate(){
    
}
int drivers::Driver::Reset(){
    return 0;   
}
void drivers::Driver::Deactivate(){
}

drivers::DriverManager::DriverManager(){
    numDrivers = 0;
}

void drivers::DriverManager::AddDriver(Driver* drv){
    drivers[numDrivers] = drv;
    numDrivers++;
}

void drivers::DriverManager::ActivateAll(){
    for(int i = 0; i < numDrivers; i++){
        drivers[i]->Activate();
    }
}

