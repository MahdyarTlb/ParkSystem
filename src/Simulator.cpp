#include "../include/Simulator.h"
#include <iostream>

using namespace std;

int Simulator::totalWaitTime = 0;
int Simulator::totalVisitorsServed = 0;

// Constructor
Simulator::Simulator() {
    currentTime = 0;
    totalRides = 0;
}

Ride* Simulator::findRide(string name) {
    for (int i = 0; i < totalRides; i++) {
        if (rides[i].name == name) return &rides[i];
    }
    return nullptr;
}

void Simulator::displayCurrentlyServing(string rideName) {
    Ride* r = findRide(rideName);
    if (r) {
        r->servingNow.display(); 
    }
}

void Simulator::addRide(string name, int capacity, int duration) {
    if (totalRides < 10) {
        rides[totalRides] = Ride(name, capacity, duration);
        totalRides++;
    }
}

void Simulator::addVisitor(int id, string name, int patience) {
    Visitor* newVisitor = new Visitor(id, name, NORMAL, currentTime, patience);
    visitors.addVisitor(newVisitor);
    cout << "Visitor " << name << " (ID: " << id << ") added to the park." << endl;
}

void Simulator::joinQueue(int id, string rideName) {
    Visitor* v = visitors.findVisitor(id);
    Ride* r = findRide(rideName);
    if (v->isBusy) {
        cout << "Error: Visitor " << v->name << " is already in a queue or ride!" << endl;
        return;
    }

    if (v && r) {
        r->queue.enqueue(v);
        v->enterQueueTime = currentTime;
        v->isBusy = true;
        cout << v->name << " joined " << r->name << " queue." << endl;
    }
}

void Simulator::tick(int minutes) {
    for (int m = 0; m < minutes; m++) {
        currentTime++;

        for (int i = 0; i < totalRides; i++) {
            Ride& r = rides[i];
            r.queue.removeImpatient(currentTime); //remove impatient
        }

        while (!eventHeap.empty() && eventHeap.top().timestamp <= currentTime) {
            Event currentEvent = eventHeap.top();
            eventHeap.pop();

            if (currentEvent.type == RIDE_FINISHED) {
                Ride* r = findRide(currentEvent.rideName);
                if (r) {
                    r->currentlyServing--;
                    r->processedCount++;
                    r->servingNow.leaveQueue(currentEvent.visitorId);
                    Visitor* v = visitors.findVisitor(currentEvent.visitorId);

                    if (v) v->isBusy = false; // free player

                    cout << "At time " << currentTime << ": Visitor " << currentEvent.visitorId 
                         << " finished " << r->name << endl;
                }
            }
        }

        for(int i = 0; i < totalRides; i++) {
            Ride& r = rides[i];
            while (r.currentlyServing < r.capacity) {
                Visitor* nextV = r.queue.dequeue();
                if (!nextV) break;

                int wait = currentTime - nextV->enterQueueTime;
                Simulator::totalWaitTime += wait;
                Simulator::totalVisitorsServed++;

                r.currentlyServing++;
                r.servingNow.enqueue(nextV);
                
                Event finishEvent;
                finishEvent.timestamp = currentTime + r.duration;
                finishEvent.visitorId = nextV->id;
                finishEvent.rideName = r.name;
                finishEvent.type = RIDE_FINISHED;
                eventHeap.push(finishEvent);
            }
        }
    }
}

void Simulator::status() {
    cout << "--- STATUS REPORT ---" << endl;
    for (int i = 0; i < totalRides; i++) {
        Ride& r = rides[i];
        cout << " * Queue " << r.name << ": [";
        r.queue.display();
        cout << "]" << endl;
        
        cout << " * Ride " << r.name << ": Serving [";
        r.servingNow.display(); 
        cout << "]" << endl;
    }
    cout << " * Time: " << currentTime << endl;
    cout << "---------------------" << endl;
}

void Simulator::report() {
    cout << "--- FINAL REPORT ---" << endl;
    for (int i = 0; i < totalRides; i++) {
        cout << " * " << rides[i].name << " Total Served: " << rides[i].processedCount << endl;
        cout << "   - Remaining in Queue: [";
        rides[i].queue.display(); 
        cout << "]" << endl;
    }

    if (Simulator::totalVisitorsServed > 0) {
        double avgWait = (double)Simulator::totalWaitTime / Simulator::totalVisitorsServed;
        cout << " * Average Wait Time in Park: " << avgWait << " minutes." << endl;
    } else {
        cout << " * Average Wait Time in Park: 0 minutes." << endl;
    }
    cout << "---------------------" << endl;
}

void Simulator::visitorInfo(int id) {
    Visitor* v = visitors.findVisitor(id);
    if (!v) { cout << "Visitor not found!" << endl; return; }

    string location = "Roaming in Park";
    for (int i = 0; i < totalRides; i++) {
        if (rides[i].queue.exists(id)) location = "In Queue: " + rides[i].name;
        if (rides[i].servingNow.exists(id)) location = "In Ride: " + rides[i].name;
    }

    cout << "Name: " << v->name << " | Type: " << (v->type == VIP ? "VIP" : "Normal") 
         << " | Location: " << location << endl;
}

void Simulator::makeVip(int id) {
    Visitor* v = visitors.findVisitor(id);
    if (!v) return;

    v->type = VIP;

    // check all of queues for changing priority
    for (int i = 0; i < totalRides; i++) {
        if (rides[i].queue.exists(id)) {
            rides[i].queue.leaveQueue(id); // left last rank
            rides[i].queue.enqueue(v);    // new rank
        }
    }
    cout << v->name << " is now VIP and moved forward in queues." << endl;
}

void Simulator::leaveQueue(int id, string rideName) {
    Ride* r = findRide(rideName);
    Visitor* v = visitors.findVisitor(id);

    if (!v || !r) {
        cout << "Visitor or Ride not found!" << endl;
        return;
    }

    // ۱is busy
    if (!v->isBusy) {
        cout << "Visitor " << id << " is not in any queue or ride!" << endl;
        return;
    }

    // is serving?
    if (r->servingNow.exists(id)) {
        cout << "Error: Visitor " << id << " is currently on the " << rideName << " and cannot leave!" << endl;
        return;
    }

    // is in the queue
    if (r->queue.exists(id)) {
        r->queue.leaveQueue(id);
        v->isBusy = false; // free
        cout << "Visitor " << id << " left the queue of " << rideName << "." << endl;
    } else {
        cout << "Visitor " << id << " is not in the queue of " << rideName << " (maybe in another ride?)" << endl;
    }
}

void Simulator::deleteVisitor(int id) {
    // delete from rides queue
    for (int i = 0; i < totalRides; i++) {
        rides[i].queue.leaveQueue(id);
        rides[i].servingNow.leaveQueue(id); // remove if riding
    }
    
    visitors.removeFromTree(id); 

    cout << "Visitor " << id << " has been completely removed from the system." << endl;
}