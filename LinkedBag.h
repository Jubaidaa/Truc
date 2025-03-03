#ifndef LINKED_BAG_
#define LINKED_BAG_

#include "Node.h"

template<class ItemType>
class LinkedBag {
private:
   Node<ItemType>* headPtr;    // Pointer to the first node
   int itemCount;              // Current count of bag items
   
   // ... other private methods
   
public:
   LinkedBag();
   LinkedBag(const LinkedBag<ItemType>& aBag); // Copy constructor
   virtual ~LinkedBag();                       // Destructor
   
   int getCurrentSize() const;
   bool isEmpty() const;
   bool add(const ItemType& newEntry);
   bool remove(const ItemType& anEntry);
   void clear();
   bool contains(const ItemType& anEntry) const;
   int getFrequencyOf(const ItemType& anEntry) const;
   vector<ItemType> toVector() const;
   
   // New methods:
   bool append(const ItemType& newEntry);  // PART 1
   Node<ItemType>* findKthItem(const int &k);  // PART 1
};
#endif
