#ifndef LINKEDLIST_H // Header Guard (redefinition guard)
#define LINKEDLIST_H

#include "../Models/Visitor.h"
#include <iostream>


struct Node{
    Visitor* visitor;
    Node* next;

    Node(Visitor* v) : visitor(v), next(nullptr) {} // initializer list - constructor (real time making and append)
};

class RideQueue{
    private:
    Node* head;
    Node* tail; //use tail for dont need loop to the end

    public:
    RideQueue() : head(nullptr), tail(nullptr) {}

    void enqueue(Visitor* v){ // priority enqueue

        Node* newNode = new Node(v);

        if(!head){
            head = tail = newNode;
            return;
        }
        if(v->type == VIP){
            if(head->visitor->type == NORMAL){
                newNode->next = head; //first people in the queue
                head = newNode;
            }
            else{ // iterate to the last VIP
                Node* temp = head;
                while(temp->next && temp->next->visitor->type == VIP){
                    temp = temp->next;
                }
                newNode->next = temp->next;
                temp->next = newNode;
                if(newNode->next == nullptr) tail = newNode;
            }
        } else{
            tail->next = newNode;
            tail = newNode;
        }
    }

    Visitor* dequeue() {
        if(!head) return nullptr;
        Node* temp = head;
        Visitor* v = temp->visitor;
        head = head->next;
        if(!head) tail = nullptr;

        delete temp; // free memory but save visitor details - for optimize function
        return v;
    }

    bool leaveQueue(int visitor_id){

        if(!head) return false;

        if(head->visitor->id == visitor_id){
            dequeue();
            return true;
        }

        Node* current = head;
        while(current->next && current->next->visitor->id != visitor_id){
            current = current->next;
        }
        if(current->next){
            Node* toDelete = current->next;
            current->next = current->next->next;

            if(toDelete == tail) tail = current; // the last node

            delete toDelete;
            return true;
        }
        return false;
    }

    void display(){
        Node* temp = head;
        
        while(temp){
            std::cout<< temp->visitor->id << (temp->visitor->type == VIP ? "   (VIP)" : "  ") << temp->visitor->name << "  p: " << temp->visitor->patience << ", ";
            temp = temp->next;
        }
    }

    bool exists(int id) {
        Node* temp = head;
        while (temp != nullptr) {
            if (temp->visitor->id == id) return true;
            temp = temp->next;
        }
        return false;
    }

    void removeImpatient(int now) {
        Node* curr = head;
        while (curr) {
            Node* nextNode = curr->next;
            if (now - curr->visitor->enterQueueTime > curr->visitor->patience) {
                cout << "Visitor " << curr->visitor->name << " lost patience and left!" << endl;
                int idToLeave = curr->visitor->id;
                curr->visitor->isBusy = false; // free
                leaveQueue(idToLeave); // remove from list
            }
            curr = nextNode;
        }
    }

    void saveToStream(ostream& out) {
        Node* temp = head;
        while (temp) {
            out << temp->visitor->id << " ";
            temp = temp->next;
        }
    }
};

// for save in a file
struct LogNode {
    string message;
    LogNode* next;
};

class LogManager {
private:
    LogNode* head;
    LogNode* tail;
public:
    LogManager() : head(nullptr), tail(nullptr) {}

    void addLog(string msg, int time) {
        LogNode* newNode = new LogNode{ "T=" + to_string(time) + ": " + msg, nullptr };
        if (!head) head = tail = newNode;
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    void printAll(ostream& out) {
    LogNode* temp = head;
    while (temp) {
        out << "  - " << temp->message << endl;
        temp = temp->next;
    }
}
};

#endif