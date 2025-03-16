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

   // Big 3
   LinkedBag(const LinkedBag<ItemType>& aBag); 
   LinkedBag<ItemType>& operator=(const LinkedBag<ItemType>& aBag);

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

#include "LinkedBag.cpp" // Inclusion du .cpp pour les templates

#endif
