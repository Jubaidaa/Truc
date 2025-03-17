#ifndef INSTAGRAM340_H
#define INSTAGRAM340_H

#include <memory>
#include <vector>
#include <iostream>
#include "User.h"

/**
 * Instagram340:
 *  - Gère désormais une collection de User (allUsers) pour "sign up / log in".
 *  - Stocke l'utilisateur courant (currentUser).
 */
class Instagram340 {
private:
    // L'utilisateur actuellement connecté (ou nullptr si aucun)
    std::shared_ptr<User> currentUser;

    // Liste globale de tous les comptes créés (Sign Up)
    std::vector<std::shared_ptr<User>> allUsers;

    // Affiche le menu principal si quelqu'un est connecté
    void showMainMenu();

public:
    // Big 3 (déjà exigé dans le devoir)
    Instagram340();
    ~Instagram340();
    Instagram340(const Instagram340& other);
    Instagram340& operator=(const Instagram340& other);

    // Point d'entrée de l'application
    // Désormais: propose un "landing page" sign up / log in, 
    // puis (si on se log) le menu principal
    void runInstagram();

    // Surcharge de l'opérateur <<
    // pour afficher "Welcome to Instagram 340!"
    friend std::ostream& operator<<(std::ostream& os, const Instagram340& insta);
};

#endif
