#include "../include/Simulator.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    Simulator sim;
    string command;

    sim.addRide("Jet", 2, 5);
    sim.addRide("Wheel", 5, 10);

    while (cin >> command) {
        if (command == "ADD_VISITOR") {
            int id, patience; string name;
            cin >> id >> name >> patience;
            sim.addVisitor(id, name, patience);
        } else if (command == "JOIN_QUEUE") {
            int id; string ride;
            cin >> id >> ride;
            sim.joinQueue(id, ride);
        } else if (command == "TICK") {
            int m; cin >> m;
            sim.tick(m);
        } else if (command == "STATUS") {
            sim.status();
        } else if (command == "REPORT") {
            sim.report();
        } else if (command == "VISITOR_INFO") {
            int id; cin >> id;
            sim.visitorInfo(id);
        } else if (command == "MAKE_VIP") {
            int id; cin >> id;
            sim.makeVip(id);
        } else if (command == "DELETE_VISITOR") {
            int id; cin >> id;
            sim.deleteVisitor(id);
        } else if (command == "LEAVE_QUEUE") {
            int id; string rideName;
            cin >> id >> rideName;
            sim.leaveQueue(id, rideName);
        }
    }
    return 0;
}