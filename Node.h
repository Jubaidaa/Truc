#ifndef NODE_H
#define NODE_H

#include <iostream>

template<class ItemType>
class Node {
private:
    ItemType item;
    Node<ItemType>* next;

public:
    Node();
    Node(const ItemType& anItem);
    Node(const ItemType& anItem, Node<ItemType>* nextNodePtr);
    ~Node();

    void setItem(const ItemType& anItem);
    void setNext(Node<ItemType>* nextNodePtr);

    ItemType getItem() const;
    Node<ItemType>* getNext() const;
};

// ============== Implémentation directe dans le header (template) =============

template<class ItemType>
Node<ItemType>::Node()
  : next(nullptr)
{
}

template<class ItemType>
Node<ItemType>::Node(const ItemType& anItem)
  : item(anItem), next(nullptr)
{
}

template<class ItemType>
Node<ItemType>::Node(const ItemType& anItem, Node<ItemType>* nextNodePtr)
  : item(anItem), next(nextNodePtr)
{
}

template<class ItemType>
Node<ItemType>::~Node()
{
    std::cout << "--- Destroying bag node ... " << std::endl;
}

template<class ItemType>
void Node<ItemType>::setItem(const ItemType& anItem)
{
    item = anItem;
}

template<class ItemType>
void Node<ItemType>::setNext(Node<ItemType>* nextNodePtr)
{
    next = nextNodePtr;
}

template<class ItemType>
ItemType Node<ItemType>::getItem() const
{
    return item;
}

template<class ItemType>
Node<ItemType>* Node<ItemType>::getNext() const
{
    return next;
}

#endif
