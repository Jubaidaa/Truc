#include "LinkedBag.h"
#include "Node.h"
// #include "Node.cpp"
#include "vector"
#include <string>

template<class ItemType>
bool LinkedBag<ItemType>::append(const ItemType& newEntry) {
    Node<ItemType>* newNode = new Node<ItemType>();
    newNode->setItem(newEntry);
    newNode->setNext(nullptr);

    if (headPtr == nullptr) {
        headPtr = newNode;
    } else {
        Node<ItemType>* current = headPtr;
        while (current->getNext() != nullptr) {
            current = current->getNext();
        }
        current->setNext(newNode);
    }
    itemCount++;
    return true;
}

template<class ItemType>
Node<ItemType>* LinkedBag<ItemType>::findKthItem(const int &k) const {
    if (k < 1 || k > itemCount) {
        return nullptr;
    }
    Node<ItemType>* current = headPtr;
    for (int i = 1; i < k; i++) {
        current = current->getNext();
    }
    return current;
}

// Explicit template instantiation
template class LinkedBag<int>;
template class LinkedBag<double>;
template class LinkedBag<std::string>;
