#ifndef LINKED_BAG_
#define LINKED_BAG_

#include <vector>
#include "Node.h"

template<class ItemType>
class LinkedBag {
private:
   Node<ItemType>* headPtr; // Pointer to first node
   int itemCount;           // Number of items

public:
   LinkedBag();
   ~LinkedBag();
   
   bool isEmpty() const;
   int getCurrentSize() const;
   
   bool add(const ItemType& newEntry);
   bool remove(const ItemType& anEntry);
   void clear();
   
   bool contains(const ItemType& anEntry) const;
   int getFrequencyOf(const ItemType& anEntry) const;
   std::vector<ItemType> toVector() const;

   // The new methods:
   bool append(const ItemType& newEntry);
   Node<ItemType>* findKthItem(const int& k) const;
};

// ─────────────────────────────────────────────────────────────────────────────
// Template Definitions Inline
// ─────────────────────────────────────────────────────────────────────────────

template<class ItemType>
LinkedBag<ItemType>::LinkedBag() : headPtr(nullptr), itemCount(0)
{
}

template<class ItemType>
LinkedBag<ItemType>::~LinkedBag()
{
    clear();
}

template<class ItemType>
bool LinkedBag<ItemType>::isEmpty() const
{
    return (itemCount == 0);
}

template<class ItemType>
int LinkedBag<ItemType>::getCurrentSize() const
{
    return itemCount;
}

template<class ItemType>
bool LinkedBag<ItemType>::add(const ItemType& newEntry)
{
    Node<ItemType>* newNode = new Node<ItemType>(newEntry, headPtr);
    headPtr = newNode;
    itemCount++;
    return true;
}

template<class ItemType>
bool LinkedBag<ItemType>::remove(const ItemType& anEntry)
{
    Node<ItemType>* prevPtr = nullptr;
    Node<ItemType>* curPtr = headPtr;
    while (curPtr != nullptr) {
        if (curPtr->getItem() == anEntry) {
            // Found match
            if (prevPtr == nullptr) {
                // removing head
                headPtr = curPtr->getNext();
            } else {
                prevPtr->setNext(curPtr->getNext());
            }
            delete curPtr;
            curPtr = nullptr;
            itemCount--;
            return true;
        }
        prevPtr = curPtr;
        curPtr = curPtr->getNext();
    }
    return false;
}

template<class ItemType>
void LinkedBag<ItemType>::clear()
{
    Node<ItemType>* curPtr = headPtr;
    while (curPtr != nullptr) {
        Node<ItemType>* toDelete = curPtr;
        curPtr = curPtr->getNext();
        delete toDelete;
    }
    headPtr = nullptr;
    itemCount = 0;
}

template<class ItemType>
bool LinkedBag<ItemType>::contains(const ItemType& anEntry) const
{
    Node<ItemType>* curPtr = headPtr;
    while (curPtr != nullptr) {
        if (curPtr->getItem() == anEntry) {
            return true;
        }
        curPtr = curPtr->getNext();
    }
    return false;
}

template<class ItemType>
int LinkedBag<ItemType>::getFrequencyOf(const ItemType& anEntry) const
{
    int frequency = 0;
    Node<ItemType>* curPtr = headPtr;
    while (curPtr != nullptr) {
        if (curPtr->getItem() == anEntry) {
            frequency++;
        }
        curPtr = curPtr->getNext();
    }
    return frequency;
}

template<class ItemType>
std::vector<ItemType> LinkedBag<ItemType>::toVector() const
{
    std::vector<ItemType> bagContents;
    Node<ItemType>* curPtr = headPtr;
    while (curPtr != nullptr) {
        bagContents.push_back(curPtr->getItem());
        curPtr = curPtr->getNext();
    }
    return bagContents;
}

// ────── The newly added functions:

template<class ItemType>
bool LinkedBag<ItemType>::append(const ItemType& newEntry)
{
    Node<ItemType>* newNode = new Node<ItemType>();
    newNode->setItem(newEntry);
    newNode->setNext(nullptr);

    // If bag is empty
    if (headPtr == nullptr) {
        headPtr = newNode;
    }
    else {
        // Find last node
        Node<ItemType>* curPtr = headPtr;
        while (curPtr->getNext() != nullptr) {
            curPtr = curPtr->getNext();
        }
        curPtr->setNext(newNode);
    }

    itemCount++;
    return true;
}

template<class ItemType>
Node<ItemType>* LinkedBag<ItemType>::findKthItem(const int& k) const
{
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

#endif
