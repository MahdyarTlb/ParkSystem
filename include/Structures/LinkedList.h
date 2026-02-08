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
            std::cout<< temp->visitor->id << (temp->visitor->type == VIP ? "\tVIP" : "\t   ") << temp->visitor->name << "\t p: " << temp->visitor->patience;
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
};
#endif