#include "Instagram340.h"
#include "Reel.h"
#include "Story.h"
#include <iostream>

Instagram340::Instagram340() : currentUser(nullptr) {}

Instagram340::~Instagram340() {
    delete currentUser;
}

void Instagram340::runInstagram() {
    bool done = false;

    while (!done) {
        std::cout << "\nMenu:\n"
                  << "1. Create Profile\n"
                  << "2. Display Profile\n"
                  << "3. Modify Password\n"
                  << "4. Create Post\n"
                  << "5. Display All Posts\n"
                  << "6. Display K-th Post\n"
                  << "7. Modify Post Title\n"
                  << "8. Edit Post\n"
                  << "9. Delete Post\n"
                  << "10. Quit\n"
                  << "Enter choice: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1: {
            // Create Profile
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

            // If already had a user, delete first
            if (currentUser) {
                delete currentUser;
                currentUser = nullptr;
            }
            currentUser = new User(uName, em, pw, b, pic);
            break;
        }
        case 2: {
            // Display Profile
            if (currentUser) {
                currentUser->displayUserInfo();
            } else {
                std::cout << "No user profile created yet.\n";
            }
            break;
        }
        case 3: {
            // Modify Password
            if (currentUser) {
                std::string newPw;
                std::cout << "Enter new password: ";
                std::cin >> newPw;
                currentUser->setPassword(newPw);
            } else {
                std::cout << "No user profile created yet.\n";
            }
            break;
        }
        case 4: {
            // Create Post
            if (!currentUser) {
                std::cout << "No user profile created yet.\n";
                break;
            }
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

            Post* newPost = nullptr;
            if (postType == 1) {
                newPost = new Reel(title, url, duration);
            } else {
                newPost = new Story(title, url, duration);
            }
            currentUser->createPost(newPost);
            break;
        }
        case 5: {
            // Display All Posts
            if (currentUser) {
                currentUser->displayAllPosts();
            } else {
                std::cout << "No user profile created yet.\n";
            }
            break;
        }
        case 6: {
            // Display K-th Post
            if (currentUser) {
                int k;
                std::cout << "Enter K: ";
                std::cin >> k;
                currentUser->displayKthPost(k);
            } else {
                std::cout << "No user profile created yet.\n";
            }
            break;
        }
        case 7: {
            if (currentUser) {
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
            } else {
                std::cout << "No user profile created yet.\n";
            }
            break;
        }
        case 8: {
            if (currentUser) {
                int k;
                std::cout << "Enter Post index to edit: ";
                std::cin >> k;
                if (!currentUser->editPost(k)) {
                    std::cout << "Unable to edit post at index " << k << "\n";
                }
            } else {
                std::cout << "No user profile created yet.\n";
            }
            break;
        }
        case 9: {
            // Delete Post
            if (currentUser) {
                int k;
                std::cout << "Enter Post index to delete: ";
                std::cin >> k;
                if (!currentUser->deletePost(k)) {
                    std::cout << "Unable to delete post at index " << k << "\n";
                }
            } else {
                std::cout << "No user profile created yet.\n";
            }
            break;
        }
        case 10:
            done = true;
            break;
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }

    std::cout << "Exiting Instagram340.\n";
}
