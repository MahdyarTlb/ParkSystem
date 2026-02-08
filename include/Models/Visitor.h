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

    Visitor(int _id, string _name, VisitorType _type, int _time){
        id = _id;
        name = _name;
        type = _type;
        arrivalTime = _time;
        patience = 100; // default patience
    }
};

#endif