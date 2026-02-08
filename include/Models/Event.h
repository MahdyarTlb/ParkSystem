#ifndef EVENT_H
#define EVENT_H

#include <string>
using namespace std;

enum EventType{
    RIDE_FINISHED, 
    VISITOR_LEAVE
};

struct Event{
    int timestamp;
    int visitorId;
    string rideName;
    EventType type;

    bool operator<(const Event& other) const { // compare by timestamp
        return timestamp < other.timestamp;
    }
};
#endif