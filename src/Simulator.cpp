#include "../include/Simulator.h"
#include <iostream>

using namespace std;

// 1. سازنده (Constructor)
Simulator::Simulator() {
    currentTime = 0;
    totalRides = 0;
}

// 2. پیدا کردن دستگاه
Ride* Simulator::findRide(string name) {
    for (int i = 0; i < totalRides; i++) {
        if (rides[i].name == name) return &rides[i];
    }
    return nullptr;
}

// 3. نمایش افراد در حال بازی
void Simulator::displayCurrentlyServing(string rideName) {
    Ride* r = findRide(rideName);
    if (r) {
        r->servingNow.display(); 
    }
}

// 4. اضافه کردن دستگاه
void Simulator::addRide(string name, int capacity, int duration) {
    if (totalRides < 10) {
        rides[totalRides] = Ride(name, capacity, duration);
        totalRides++;
    }
}

// 5. اضافه کردن بازدیدکننده
void Simulator::addVisitor(int id, string name, string type) {
    VisitorType vType = (type == "VIP") ? VIP : NORMAL;
    Visitor* newVisitor = new Visitor(id, name, vType, currentTime);
    visitors.addVisitor(newVisitor);
    cout << "Visitor " << name << " (ID: " << id << ") added to the park." << endl;
}

// 6. پیوستن به صف
void Simulator::joinQueue(int id, string rideName) {
    Visitor* v = visitors.findVisitor(id);
    Ride* r = findRide(rideName);
    if (v && r) {
        r->queue.enqueue(v);
        cout << v->name << " joined " << r->name << " queue." << endl;
    }
}

// 7. موتور اصلی شبیه‌ساز
void Simulator::tick(int minutes) {
    for (int m = 0; m < minutes; m++) {
        currentTime++;
        
        while (!eventHeap.empty() && eventHeap.top().timestamp <= currentTime) {
            Event currentEvent = eventHeap.top();
            eventHeap.pop();

            if (currentEvent.type == RIDE_FINISHED) {
                Ride* r = findRide(currentEvent.rideName);
                if (r) {
                    r->currentlyServing--;
                    r->processedCount++;
                    r->servingNow.leaveQueue(currentEvent.visitorId);
                    cout << "At time " << currentTime << ": Visitor " << currentEvent.visitorId 
                         << " finished " << r->name << endl;
                }
            }
        }

        for (int i = 0; i < totalRides; i++) {
            Ride& r = rides[i];
            while (r.currentlyServing < r.capacity) {
                Visitor* nextV = r.queue.dequeue();
                if (!nextV) break;
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

// 8. گزارش وضعیت
void Simulator::status() {
    cout << "--- STATUS REPORT ---" << endl;
    for (int i = 0; i < totalRides; i++) {
        Ride& r = rides[i];
        cout << " * Queue " << r.name << ": [";
        r.queue.display(); // لینک‌لیست باید ID و اولویت را چاپ کند
        cout << "]" << endl;
        
        cout << " * Ride " << r.name << ": Serving [";
        r.servingNow.display(); 
        cout << "]" << endl;
    }
    cout << " * Time: " << currentTime << endl;
    cout << "---------------------" << endl;
}

// 9. گزارش نهایی
void Simulator::report() {
    cout << "--- FINAL REPORT ---" << endl;
    for (int i = 0; i < totalRides; i++) {
        cout << " * " << rides[i].name << " Total Served: " << rides[i].processedCount << endl;
    }
}

// ۱. نمایش اطلاعات بازدیدکننده
void Simulator::visitorInfo(int id) {
    Visitor* v = visitors.findVisitor(id);
    if (!v) { cout << "Visitor not found!" << endl; return; }

    string location = "Roaming in Park";
    for (int i = 0; i < totalRides; i++) {
        if (rides[i].queue.exists(id)) location = "In Queue: " + rides[i].name;
        if (rides[i].servingNow.exists(id)) location = "In Ride: " + rides[i].name;
    }

    cout << "Name: " << v->name << " | Type: " << (v->type == VIP ? "VIP" : "Normal") 
         << " | Location: " << location << endl; // [cite: 129, 130]
}

// ۲. تبدیل به VIP
void Simulator::makeVip(int id) {
    Visitor* v = visitors.findVisitor(id);
    if (!v) return;

    v->type = VIP; // تغییر لیبل [cite: 131]

    // چک کردن تمام صف‌ها برای جابجایی جایگاه فرد [cite: 132]
    for (int i = 0; i < totalRides; i++) {
        if (rides[i].queue.exists(id)) { // اگر در این صف بود
            rides[i].queue.leaveQueue(id); // حذف از جایگاه قبلی
            rides[i].queue.enqueue(v);    // درج مجدد در جایگاه جدید (طلایی)
        }
    }
    cout << v->name << " is now VIP and moved forward in queues." << endl;
}

void Simulator::leaveQueue(int id, string rideName) {
    Ride* r = findRide(rideName);
    if (r) {
        r->queue.leaveQueue(id); // حذف از لیست پیوندی 
        cout << "Visitor " << id << " left the queue of " << rideName << "." << endl;
    }
}

// ۳. حذف بازدیدکننده (نیاز به حذف از AVL و صف‌ها دارد)
void Simulator::deleteVisitor(int id) {
    // ۱. حذف از صف انتظار تمام دستگاه‌ها
    for (int i = 0; i < totalRides; i++) {
        rides[i].queue.leaveQueue(id);
        rides[i].servingNow.leaveQueue(id); // اگر داخل دستگاه بود هم حذف شود
    }
    
    // ۲. حذف از درخت AVL (آپدیت ریشه درخت)
    // فرض می‌کنیم ریشه درخت در کلاس AVLTree خصوصی است، پس یک متد عمومی بساز
    visitors.removeFromTree(id); 

    cout << "Visitor " << id << " has been completely removed from the system." << endl;
}