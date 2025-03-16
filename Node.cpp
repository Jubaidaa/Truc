#include "Node.h"
#include <iostream>

template<class ItemType>
Node<ItemType>::Node()
  : next(nullptr)
{
    std::cout << "--- Node() default constructor ---" << std::endl;
}

template<class ItemType>
Node<ItemType>::Node(const ItemType& anItem)
  : item(anItem), next(nullptr)
{
    std::cout << "--- Node(anItem) constructor ---" << std::endl;
}

template<class ItemType>
Node<ItemType>::Node(const ItemType& anItem, Node<ItemType>* nextNodePtr)
  : item(anItem), next(nextNodePtr)
{
    std::cout << "--- Node(anItem, nextPtr) constructor ---" << std::endl;
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

// Instanciation explicite (exemple)
template class Node<int>;
template class Node<double>;
// etc. Pour chaque type de Node<ItemType> que tu utilises
