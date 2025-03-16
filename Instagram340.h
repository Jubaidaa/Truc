#ifndef INSTAGRAM340_H
#define INSTAGRAM340_H

#include <memory>
#include <iostream>
#include "User.h"

class Instagram340 {
private:
    // Stockage du currentUser en smart pointer
    std::shared_ptr<User> currentUser;

public:
    // Big 3
    Instagram340();
    ~Instagram340();                                     // destructeur
    Instagram340(const Instagram340& other);             // constructeur de copie
    Instagram340& operator=(const Instagram340& other);  // opérateur d’affectation

    // Fonction principale
    void runInstagram(); 

    // Surcharge de l’opérateur << (Partie 2 : “Welcome to Instagram 340!”)
    friend std::ostream& operator<<(std::ostream& os, const Instagram340& insta);
};

#endif
