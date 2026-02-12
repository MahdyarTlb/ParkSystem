#include "../include/Simulator.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int Simulator::totalWaitTime = 0;
int Simulator::totalVisitorsServed = 0;

// Constructor
Simulator::Simulator(): undoStack(100) { 
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

    undoStack.push({ADD_V, id, ""}); // push to the stack
}

void Simulator::joinQueue(int id, string rideName, int p) {
    Visitor* v = visitors.findVisitor(id);
    Ride* r = findRide(rideName);
    if (v->isBusy) {
        cout << "Error: Visitor " << v->name << " is already in a queue or ride!" << endl;
        return;
    }

    if (v && r) {
        r->queue.enqueue(v);
        v->patience = p;
        v->enterQueueTime = currentTime;
        v->isBusy = true;
        cout << v->name << " joined " << r->name << " queue." << endl;
    }

    undoStack.push({JOIN_Q, id, rideName});
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

    saveSystemState("final_report.txt"); // save in a file
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

    undoStack.push({MAKE_V_VIP, id, ""});
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

    undoStack.push({LEAVE_Q_ACTION, id, rideName});
}

void Simulator::deleteVisitor(int id) {
    // delete from rides queue
    for (int i = 0; i < totalRides; i++) {
        rides[i].queue.leaveQueue(id);
        rides[i].servingNow.leaveQueue(id); // remove if riding
    }
    
    if (visitors.findVisitor(id)) {

        visitors.removeFromTree(id); 

        cout << "Visitor " << id << " has been completely removed from the system." << endl;

    } else {

        cout << "No one has " << id << " id to remove!" << endl;

    }
}

void Simulator::undo() {
    if (undoStack.isEmpty()) {
        cout << "Nothing to undo!" << endl;
        return;
    }

    UndoAction last = undoStack.pop();
    cout << "Undoing action... ";

    switch (last.type) {
        case ADD_V: {
            // معکوسِ اضافه کردن -> حذف از درخت
            // دقت کن که باید از تمام صف‌ها هم پاک بشه (مثل deleteVisitor)
            deleteVisitor(last.visitorId); 
            cout << "Visitor " << last.visitorId << " removed." << endl;
            break;
        }
        case JOIN_Q: {
            // معکوسِ وارد صف شدن -> خارج شدن از صف
            Ride* r = findRide(last.rideName);
            if (r) {
                r->queue.leaveQueue(last.visitorId);
                Visitor* v = visitors.findVisitor(last.visitorId);
                if (v) v->isBusy = false;
                cout << "Visitor " << last.visitorId << " removed from " << last.rideName << " queue." << endl;
            }
            break;
        }
        case MAKE_V_VIP: {
            // معکوسِ VIP شدن -> برگشت به NORMAL
            Visitor* v = visitors.findVisitor(last.visitorId);
            if (v) {
                v->type = NORMAL;
                // نکته حرفه‌ای: برای برگردوندن جایگاهش در صف، باید یک بار از صف در بیاد و دوباره بره تو
                for (int i = 0; i < totalRides; i++) {
                    if (rides[i].queue.exists(v->id)) {
                        rides[i].queue.leaveQueue(v->id);
                        rides[i].queue.enqueue(v); // چون الان Normal شده، میره آخر صف
                    }
                }
                cout << "Visitor " << last.visitorId << " is now NORMAL again." << endl;
            }
            break;
        }
        case LEAVE_Q_ACTION: {
            // معکوسِ انصراف از صف -> دوباره وارد صف شدن
            Visitor* v = visitors.findVisitor(last.visitorId);
            if (v) {
                joinQueue(v->id, last.rideName); // دوباره میره تو صف
                cout << "Visitor " << last.visitorId << " returned to " << last.rideName << " queue." << endl;
            }
            break;
        }
    }
}

void Simulator::saveSystemState(string fileName) {
    ofstream outFile(fileName);
    if (!outFile) return;

    outFile << "TIME " << currentTime << endl;

    // ۱. ذخیره همه بازدیدکننده‌ها
    visitors.saveToStream(outFile); 

    // ۲. ذخیره وضعیت دستگاه‌ها
    for (int i = 0; i < totalRides; i++) {
        // الف) ذخیره افرادی که سوار هستند
        outFile << "RIDE_SERVING " << rides[i].name << " ";
        rides[i].servingNow.saveToStream(outFile); 
        outFile << endl;

        // ب) ذخیره افرادی که در صف انتظار هستند
        outFile << "RIDE_QUEUE " << rides[i].name << " ";
        rides[i].queue.saveToStream(outFile); 
        outFile << endl;
    }

    outFile.close();
    cout << "Deep Save Completed (Including Serving Visitors)." << endl;
}

void Simulator::loadSystemState(string fileName) {
    ifstream inFile(fileName);
    if (!inFile) {
        cout << "Error: Backup file not found!" << endl;
        return;
    }

    string cmd;
    while (inFile >> cmd) {
        if (cmd == "TIME") {
            inFile >> currentTime;
        } 
        else if (cmd == "VISITOR") {
            int id, p; string name;
            inFile >> id >> name >> p;
            addVisitor(id, name, p);
        }
        else if (cmd == "RIDE_QUEUE" || cmd == "RIDE_SERVING") {
        string type = cmd;
        string rideName;
        inFile >> rideName;
        Ride* r = findRide(rideName);
        
        string line;
        getline(inFile, line);
        stringstream ss(line);
        int vId;
        while (ss >> vId) {
            Visitor* v = visitors.findVisitor(vId);
            if (v && r) {
                v->isBusy = true;
                if (type == "RIDE_QUEUE") {
                    r->queue.enqueue(v);
                } else {
                    r->servingNow.enqueue(v);
                    r->currentlyServing++;
                    // اضافه کردن به Heap
                    Event finishEvent;
                    finishEvent.timestamp = currentTime + r->duration;
                    finishEvent.visitorId = vId;
                    finishEvent.rideName = r->name;
                    finishEvent.type = RIDE_FINISHED;
                    eventHeap.push(finishEvent);
                }
            }
        }
    }
    }
    inFile.close();
    cout << "--- System Restored Successfully at T=" << currentTime << " ---" << endl;
}