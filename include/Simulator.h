#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "models/Ride.h"
#include "structures/AVLTree.h"
#include "structures/MinHeap.h"
#include <string>

class Simulator {
private:
    AVLTree visitors;
    Ride rides[10]; // مثلاً آرایه‌ای از دستگاه‌ها
    int totalRides;
    MinHeap eventHeap;
    int currentTime;

    Ride* findRide(std::string name);
    void displayCurrentlyServing(std::string rideName);

public:
    Simulator(); // سازنده
    void addVisitor(int id, std::string name, std::string type);
    void joinQueue(int id, std::string rideName);
    void addRide(std::string name, int capacity, int duration);
    void visitorInfo(int id);
    void makeVip(int id);
    void leaveQueue(int id, string rideName);
    void deleteVisitor(int id);
    void tick(int minutes);
    void status();
    void report();
};

#endif