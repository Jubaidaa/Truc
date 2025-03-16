#include "LinkedBag.h"
#include "Node.h"
#include <string>
#include <vector>

// ======================================================================
// CONSTRUCTEUR PAR DEFAUT
template<class ItemType>
LinkedBag<ItemType>::LinkedBag() : headPtr(nullptr), itemCount(0)
{
}

// ======================================================================
// DESTRUCTEUR
template<class ItemType>
LinkedBag<ItemType>::~LinkedBag()
{
    clear(); // Supprime tous les noeuds
}

// ======================================================================
// CONSTRUCTEUR DE COPIE
template<class ItemType>
LinkedBag<ItemType>::LinkedBag(const LinkedBag<ItemType>& aBag)
    : headPtr(nullptr), itemCount(0)
{
    Node<ItemType>* curPtr = aBag.headPtr;
    while (curPtr != nullptr) {
        add(curPtr->getItem());
        curPtr = curPtr->getNext();
    }
}

// ======================================================================
// OPERATEUR=
template<class ItemType>
LinkedBag<ItemType>& LinkedBag<ItemType>::operator=(const LinkedBag<ItemType>& aBag)
{
    if (this != &aBag) {
        clear();

        Node<ItemType>* curPtr = aBag.headPtr;
        while (curPtr != nullptr) {
            add(curPtr->getItem());
            curPtr = curPtr->getNext();
        }
    }
    return *this;
}

// ======================================================================
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

// ======================================================================
// append
template<class ItemType>
bool LinkedBag<ItemType>::append(const ItemType& newEntry)
{
    Node<ItemType>* newNode = new Node<ItemType>(newEntry);
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

// findKthItem
template<class ItemType>
Node<ItemType>* LinkedBag<ItemType>::findKthItem(const int &k) const
{
    if (k < 1 || k > itemCount) {
        return nullptr;
    }
    Node<ItemType>* current = headPtr;
    for (int i = 1; i < k; i++) {
        current = current->getNext();
    }
    return current;
}

// ======================================================================
// Force l’instanciation de certains types (optionnel)
template class LinkedBag<int>;
template class LinkedBag<double>;
template class LinkedBag<std::string>;
