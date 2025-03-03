#include "LinkedBag.h"
#include "Node.h"

template<class ItemType>
bool LinkedBag<ItemType>::append(const ItemType& newEntry) {
    Node<ItemType>* newNode = new Node<ItemType>();
    newNode->setItem(newEntry);
    newNode->setNext(nullptr);

    if (headPtr == nullptr) {
        headPtr = newNode;
    } else {
        // Traverse to the end of the list
        Node<ItemType>* curPtr = headPtr;
        while (curPtr->getNext() != nullptr) {
            curPtr = curPtr->getNext();
        }
        //newNode at end
        curPtr->setNext(newNode);
    }
    itemCount++;
    return true;
}

template<class ItemType>
Node<ItemType>* LinkedBag<ItemType>::findKthItem(const int &k) {
    if (k < 1 || k > itemCount) {
        return nullptr;
    }

    Node<ItemType>* curPtr = headPtr;
    int index = 1;
    while (curPtr != nullptr && index < k) {
        curPtr = curPtr->getNext();
        index++;
    }
    return curPtr;
}
