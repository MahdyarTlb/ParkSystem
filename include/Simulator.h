#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "models/Ride.h"
#include "structures/AVLTree.h"
#include "structures/MinHeap.h"
#include "structures/Stack.h"
#include <string>

class Simulator {
private:
    UndoStack undoStack;
    AVLTree visitors;
    Ride rides[10]; // array of rides
    int totalRides;
    MinHeap eventHeap;
    int currentTime;
    static int totalWaitTime;
    static int totalVisitorsServed;

    Ride* findRide(std::string name);
    void displayCurrentlyServing(std::string rideName);

public:
    Simulator(); // constructor
    void addVisitor(int id, std::string name, int patience);
    void joinQueue(int id, std::string rideName);
    void addRide(std::string name, int capacity, int duration);
    void visitorInfo(int id);
    void makeVip(int id);
    void leaveQueue(int id, string rideName);
    void deleteVisitor(int id);
    void tick(int minutes);
    void status();
    void report();
    void undo();
};

#endif