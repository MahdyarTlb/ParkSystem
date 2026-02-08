#ifndef RIDE_H
#define RIDE_H

#include <string>
#include "../structures/LinkedList.h"

class Ride {
public:
    std::string name;
    int capacity;
    int duration;
    int processedCount;
    int currentlyServing;
    RideQueue queue;
    RideQueue servingNow;

    Ride() : capacity(0), duration(0), processedCount(0), currentlyServing(0) {}
    
    Ride(std::string n, int cap, int dur) 
        : name(n), capacity(cap), duration(dur), processedCount(0), currentlyServing(0) {}
};

#endif