#ifndef VISITOR_H
#define VISITOR_H

#include <string>
using namespace std;

enum VisitorType {
    NORMAL,
     VIP
    };

    // class for visitors
class Visitor{
    public:

    int id;
    string name;
    VisitorType type;
    int arrivalTime;
    int patience;
    int enterQueueTime;
    static int totalWaitTime; 
    static int totalVisitorsServed;
    bool isBusy;

    Visitor(int _id, string _name, VisitorType _type, int _time, int _patience = 10){ // default patience is 10 min
        id = _id;
        name = _name;
        type = _type;
        arrivalTime = _time;
        patience = _patience;
        isBusy = false; //default isnot busy
    }
};

#endif