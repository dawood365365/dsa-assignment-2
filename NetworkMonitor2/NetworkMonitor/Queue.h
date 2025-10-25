#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <stdexcept>

template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };
    
    Node* frontNode;
    Node* rearNode;
    int count;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr), count(0) {}
    
    // Copy constructor for iteration
    Queue(const Queue& other) : frontNode(nullptr), rearNode(nullptr), count(0) {
        Node* curr = other.frontNode;
        while (curr) {
            enqueue(curr->data);
            curr = curr->next;
        }
    }
    
    // Assignment operator
    Queue& operator=(const Queue& other) {
        if (this != &other) {
            while (!isEmpty()) dequeue();
            Node* curr = other.frontNode;
            while (curr) {
                enqueue(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }
    
    ~Queue() { 
        while (!isEmpty()) dequeue(); 
    }
    
    void enqueue(const T& val) {
        Node* newNode = new Node(val);
        if (isEmpty()) {
            frontNode = rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        count++;
    }
    
    void dequeue() {
        if (isEmpty()) return;
        Node* temp = frontNode;
        frontNode = frontNode->next;
        if (!frontNode) rearNode = nullptr;
        delete temp;
        count--;
    }
    
    T& front() {
        if (isEmpty()) throw std::runtime_error("Queue is empty");
        return frontNode->data;
    }
    
    const T& front() const {
        if (isEmpty()) throw std::runtime_error("Queue is empty");
        return frontNode->data;
    }
    
    bool isEmpty() const { 
        return frontNode == nullptr; 
    }
    
    int size() const { 
        return count; 
    }
    
    void clear() {
        while (!isEmpty()) dequeue();
    }
    
    void display() const {
        Node* curr = frontNode;
        std::cout << "Queue (Front → Rear): ";
        while (curr) {
            std::cout << curr->data.id << " ";
            curr = curr->next;
        }
        std::cout << std::endl;
    }
};

#endif