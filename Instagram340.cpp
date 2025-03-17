#include "Instagram340.h"
#include "Reel.h"
#include "Story.h"
#include <iostream>
#include <memory>

// ============= BIG 3 =============

// Constructeur par défaut
Instagram340::Instagram340()
    : currentUser(nullptr)
{
}

// Destructeur
Instagram340::~Instagram340() {
    // Rien de particulier, les smart pointers se gèrent seuls
}

// Constructeur de copie
Instagram340::Instagram340(const Instagram340& other) {
    // Copie superficielle des shared_ptr
    // (si tu veux un clonage profond, il faudrait dupliquer les Users)
    currentUser = other.currentUser;
    allUsers = other.allUsers;
}

// Opérateur d'affectation
Instagram340& Instagram340::operator=(const Instagram340& other) {
    if (this != &other) {
        currentUser = other.currentUser;
        allUsers = other.allUsers;
    }
    return *this;
}

// ============= Surcharge << =============
std::ostream& operator<<(std::ostream& os, const Instagram340& insta) {
    os << "Welcome to Instagram 340!\n";
    return os;
}

// ============= runInstagram() =============
//
// Atterrit d'abord sur un "landing page" : 
// (1) Sign Up, (2) Log In, (0) Exit
// Quand un user est loggé, on appelle showMainMenu().
void Instagram340::runInstagram() {
    // Affiche le message via la surcharge <<
    std::cout << *this;

    bool done = false;
    while (!done) {
        // Landing Page menu
        std::cout << "\nLanding Page:\n"
                  << "1. Sign Up (create account)\n"
                  << "2. Log In\n"
                  << "0. Exit\n"
                  << "Enter choice: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1: {
            // Sign Up
            std::string uName, em, pw, b, pic;
            std::cout << "Enter username: ";
            std::cin >> uName;
            std::cout << "Enter email: ";
            std::cin >> em;
            std::cout << "Enter password: ";
            std::cin >> pw;
            std::cout << "Enter bio: ";
            std::cin.ignore();
            std::getline(std::cin, b);
            std::cout << "Enter profile pic: ";
            std::cin >> pic;

            // On crée un nouvel User
            auto newUser = std::make_shared<User>(uName, em, pw, b, pic);
            // On l'ajoute à la liste globale
            allUsers.push_back(newUser);
            // On se logge automatiquement dessus
            currentUser = newUser;

            // On affiche le menu principal
            showMainMenu();
            // Quand showMainMenu() se termine, on revient ici
            break;
        }
        case 2: {
            // Log In
            if (allUsers.empty()) {
                std::cout << "No accounts exist yet. Please sign up first.\n";
                break;
            }
            // Affiche la liste des comptes
            std::cout << "Accounts:\n";
            for (std::size_t i = 0; i < allUsers.size(); i++) {
                std::cout << (i + 1) << ". " 
                          << allUsers[i]->getUsername() 
                          << "\n";
            }
            int idx;
            std::cout << "Enter the account number: ";
            std::cin >> idx;
            if (idx < 1 || (unsigned)idx > allUsers.size()) {
                std::cout << "Invalid choice.\n";
                break;
            }

            // On récupère le user choisi
            currentUser = allUsers[idx - 1];
            std::cout << "Logged in as " << currentUser->getUsername() << "\n";

            // On affiche le menu principal
            showMainMenu();
            break;
        }
        case 0:
            // Exit program
            done = true;
            break;
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }

    std::cout << "Exiting Instagram340.\n";
}

// ============= showMainMenu() =============
//
// N'est appelé que s'il y a un currentUser.
// Boucle sur les options 1..9 + "0" pour LOG OUT
void Instagram340::showMainMenu() {
    if (!currentUser) {
        std::cout << "Error: no user is logged in.\n";
        return;
    }

    bool exitMenu = false;
    while (!exitMenu) {
        std::cout << "\nMain Menu (User: " << currentUser->getUsername() << ")\n"
                  << "1. Display Profile\n"
                  << "2. Modify Password\n"
                  << "3. Create Post\n"
                  << "4. Display All Posts\n"
                  << "5. Display K-th Post\n"
                  << "6. Modify Post Title\n"
                  << "7. Edit Post\n"
                  << "8. Delete Post\n"
                  << "0. Log Out\n"
                  << "Enter choice: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1: {
            currentUser->displayUserInfo();
            break;
        }
        case 2: {
            std::string newPw;
            std::cout << "Enter new password: ";
            std::cin >> newPw;
            currentUser->setPassword(newPw);
            break;
        }
        case 3: {
            // Create Post
            std::cout << "Enter 1 for Reel, 2 for Story: ";
            int postType;
            std::cin >> postType;

            std::string title, url;
            int duration;
            std::cout << "Title: ";
            std::cin.ignore();
            std::getline(std::cin, title);
            std::cout << "URL: ";
            std::cin >> url;
            std::cout << "Duration (sec): ";
            std::cin >> duration;

            std::shared_ptr<Post> newPost;
            if (postType == 1) {
                newPost = std::make_shared<Reel>(title, url, duration);
            } else {
                newPost = std::make_shared<Story>(title, url, duration);
            }
            currentUser->createPost(newPost);
            break;
        }
        case 4:
            // Display All Posts
            currentUser->displayAllPosts();
            break;
        case 5: {
            // Display K-th Post
            int k;
            std::cout << "Enter K: ";
            std::cin >> k;
            currentUser->displayKthPost(k);
            break;
        }
        case 6: {
            // Modify Post Title
            int k;
            std::string newT;
            std::cout << "Enter Post index: ";
            std::cin >> k;
            std::cout << "Enter new title: ";
            std::cin.ignore();
            std::getline(std::cin, newT);
            if (!currentUser->modifyPostTitle(k, newT)) {
                std::cout << "Unable to modify post at index " << k << "\n";
            }
            break;
        }
        case 7: {
            // Edit Post
            int k;
            std::cout << "Enter Post index to edit: ";
            std::cin >> k;
            if (!currentUser->editPost(k)) {
                std::cout << "Unable to edit post at index " << k << "\n";
            }
            break;
        }
        case 8: {
            // Delete Post
            int k;
            std::cout << "Enter Post index to delete: ";
            std::cin >> k;
            if (!currentUser->deletePost(k)) {
                std::cout << "Unable to delete post at index " << k << "\n";
            }
            break;
        }
        case 0:
            // Log Out
            currentUser = nullptr;
            exitMenu = true; // on quitte le menu -> retour au landing page
            break;
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}
