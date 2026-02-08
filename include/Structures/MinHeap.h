#ifndef MINHEAP_H
#define MINHEAP_H

#include "../Models/Event.h"
using namespace std;

class MinHeap{
    private:

    Event* heap;
    int capacity;
    int size;

    void resize() { // manual management array (instesd of vector)
        capacity *= 2;
        Event* newHeap = new Event[capacity];
        for (int i = 0; i < size; i++) {
            newHeap[i] = heap[i];
        }
        delete[] heap; // delete last array
        heap = newHeap;
    }
    void heapUp(int index){
        while (index > 0 && heap[index].timestamp < heap[(index - 1) / 2].timestamp) {
            swap(heap[index], heap[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    void heapDown(int index){
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heap[left].timestamp < heap[smallest].timestamp) smallest = left;
        if (right < size && heap[right].timestamp < heap[smallest].timestamp) smallest = right;

        if (smallest != index) {
            swap(heap[index], heap[smallest]);
            heapDown(smallest);
        }
    }
    public:
    MinHeap(int initCap = 10) {
        size = 0;
        capacity = initCap;
        heap = new Event[capacity];
    }

    ~MinHeap() { delete[] heap; } // Destructor (memmory leak)

    void push(Event e) {
        if (size == capacity) resize();
        heap[size] = e;
        heapUp(size);
        size++;
    }

    Event top() { return heap[0]; }

    void pop() {
        if (size == 0) return;
        heap[0] = heap[size - 1];
        size--;
        heapDown(0);
    }

    bool empty() { return size == 0; }

};

#endif