#ifndef STACK_H
#define STACK_H

#include <string>
using namespace std;

// انواع دستوراتی که قابلیت برگشت دارن
enum ActionType { ADD_V, JOIN_Q, MAKE_V_VIP, LEAVE_Q_ACTION };

struct UndoAction {
    ActionType type;
    int visitorId;
    string rideName; // برای JOIN_Q و LEAVE_Q
    // نکته: اگر بخوایم MAKE_VIP رو Undo کنیم، باید بدونیم قبلا NORMAL بوده (که همیشه هست)
};

// کلاس پشته به صورت آرایه پویا (چون Vector ممنوعه)
class UndoStack {
private:
    UndoAction* array;
    int capacity;
    int topIndex;

    void resize() {
        capacity *= 2;
        UndoAction* newArray = new UndoAction[capacity];
        for (int i = 0; i <= topIndex; i++) newArray[i] = array[i];
        delete[] array;
        array = newArray;
    }

public:
    UndoStack(int cap = 10) : capacity(cap), topIndex(-1) {
        array = new UndoAction[capacity];
    }
    ~UndoStack() { delete[] array; }

    void push(UndoAction action) {
        if (topIndex == capacity - 1) resize();
        array[++topIndex] = action;
    }

    UndoAction pop() {
        if (isEmpty()) return {ADD_V, -1, ""}; // پشته خالی
        return array[topIndex--];
    }

    bool isEmpty() { return topIndex == -1; }
};

#endif