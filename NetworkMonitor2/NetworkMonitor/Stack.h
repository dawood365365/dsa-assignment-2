#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <stdexcept>

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };
    
    Node* topNode;
    int count;

public:
    Stack() : topNode(nullptr), count(0) {}
    
    ~Stack() { 
        while (!isEmpty()) pop(); 
    }
    
    void push(const T& val) {
        Node* newNode = new Node(val);
        newNode->next = topNode;
        topNode = newNode;
        count++;
    }
    
    void pop() {
        if (isEmpty()) return;
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
        count--;
    }
    
    T& top() {
        if (isEmpty()) throw std::runtime_error("Stack is empty");
        return topNode->data;
    }
    
    const T& top() const {
        if (isEmpty()) throw std::runtime_error("Stack is empty");
        return topNode->data;
    }
    
    bool isEmpty() const { 
        return topNode == nullptr; 
    }
    
    int size() const {
        return count;
    }
    
    void display() const {
        Node* current = topNode;
        std::cout << "Stack (Top → Bottom): ";
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};

#endif