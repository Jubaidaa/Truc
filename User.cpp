#include "User.h"
#include <iostream>
#include <vector>

User::User()
    : username(""), email(""), password(""), bio(""), profilePic("") {}

User::User(const std::string &username,
           const std::string &email,
           const std::string &password,
           const std::string &bio,
           const std::string &profilePic)
    : username(username), email(email), password(password),
      bio(bio), profilePic(profilePic) {}

void User::setPassword(const std::string &pw) {
    password = pw;
}

std::string User::getUsername() const {
    return username;
}

void User::displayUserInfo() const {
    std::cout << "Username: " << username << "\n"
              << "Email: " << email << "\n"
              << "Bio: " << bio << "\n"
              << "Profile Pic: " << profilePic << "\n";
}

void User::createPost(Post* newPost) {
    // We append to the LinkedBag
    posts.append(newPost);
}

void User::displayAllPosts() const {
    std::vector<Post*> allPosts = posts.toVector();
    if (allPosts.empty()) {
        std::cout << "No posts available.\n";
        return;
    }
    for (std::size_t i = 0; i < allPosts.size(); i++) {
        std::cout << "===== Post #" << (i+1) << " =====\n";
        allPosts[i]->displayPost();
        std::cout << "====================\n";
    }
}

void User::displayKthPost(int k) const {
    Node<Post*>* nodePtr = posts.findKthItem(k);
    if (!nodePtr) {
        std::cout << "Invalid index: " << k << "\n";
        return;
    }
    Post* thePost = nodePtr->getItem();
    thePost->displayPost();
}

bool User::modifyPostTitle(int k, const std::string &newTitle) {
    Node<Post*>* nodePtr = posts.findKthItem(k);
    if (!nodePtr) {
        return false;
    }
    Post* thePost = nodePtr->getItem();
    thePost->setTitle(newTitle);
    return true;
}

bool User::editPost(int k) {
    Node<Post*>* nodePtr = posts.findKthItem(k);
    if (!nodePtr) {
        return false;
    }
    Post* thePost = nodePtr->getItem();
    thePost->edit(); // Polymorphic call
    return true;
}

bool User::deletePost(int k) {
    Node<Post*>* nodePtr = posts.findKthItem(k);
    if (!nodePtr) {
        return false;
    }
    Post* thePost = nodePtr->getItem();
    // Remove from bag
    bool success = posts.remove(thePost);
    if (success) {
        // also delete from heap
        delete thePost;
    }
    return success;
}
