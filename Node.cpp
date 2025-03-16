#include <iostream>
#include <memory>
#include <string>
#include "Node.h"
#include "Post.h"  // Pour pouvoir instancier Node<std::shared_ptr<Post>>

// ===================== DÉFINITIONS DES MÉTHODES TEMPLATES =====================

template<class ItemType>
Node<ItemType>::Node()
    : next(nullptr)
{
    std::cout << "[Node] default constructor\n";
}

template<class ItemType>
Node<ItemType>::Node(const ItemType& anItem)
    : item(anItem), next(nullptr)
{
    std::cout << "[Node] constructor with anItem\n";
}

template<class ItemType>
Node<ItemType>::Node(const ItemType& anItem, Node<ItemType>* nextNodePtr)
    : item(anItem), next(nextNodePtr)
{
    std::cout << "[Node] constructor with anItem + next\n";
}

template<class ItemType>
Node<ItemType>::~Node()
{
    std::cout << "[Node] destructor\n";
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

// ===================== INSTANTIATIONS EXPLICITES =====================
//
// Ici, tu dois lister TOUTES les versions dont tu as besoin.
// Par exemple:
//
//  - Node<std::string>
//  - Node<std::shared_ptr<Post>>
//  - Node<int> (si tu en as besoin ailleurs)
// 
// Si tu oublies un type que tu utilises, tu auras un undefined reference.
template class Node<std::string>;
template class Node<std::shared_ptr<Post>>;
// template class Node<int>;  // Décommente si tu utilises Node<int>
