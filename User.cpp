#include "User.h"
#include <iostream>
#include <vector>

// =========== Big 3 ===========

User::User()
    : username(""), email(""), password(""), bio(""), profilePic("")
{
}

// Destructeur
User::~User()
{
    // Rien de particulier : la LinkedBag<std::shared_ptr<Post>> se détruit tout seul,
    // et chaque std::shared_ptr<Post> gère la destruction de son Post
}

// Constructeur de copie
User::User(const User& other)
    : username(other.username),
      email(other.email),
      password(other.password),
      bio(other.bio),
      profilePic(other.profilePic)
{
    // On recopie la liste de posts
    std::vector<std::shared_ptr<Post>> allPosts = other.posts.toVector();
    for (auto& p : allPosts) {
        posts.append(p); // Copie la shared_ptr
    }
}

// Opérateur=
User& User::operator=(const User& other)
{
    if (this != &other) {
        username   = other.username;
        email      = other.email;
        password   = other.password;
        bio        = other.bio;
        profilePic = other.profilePic;

        // On vide la liste actuelle
        posts.clear();

        // On recopie les posts
        std::vector<std::shared_ptr<Post>> allPosts = other.posts.toVector();
        for (auto& p : allPosts) {
            posts.append(p);
        }
    }
    return *this;
}

// =========== Autre constructeur ===========
User::User(const std::string &username,
           const std::string &email,
           const std::string &password,
           const std::string &bio,
           const std::string &profilePic)
    : username(username), email(email), password(password),
      bio(bio), profilePic(profilePic)
{
}

// =========== Méthodes ===========

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

void User::createPost(std::shared_ptr<Post> newPost) {
    posts.append(newPost);
}

void User::displayAllPosts() const {
    std::vector<std::shared_ptr<Post>> allPosts = posts.toVector();
    if (allPosts.empty()) {
        std::cout << "No posts available.\n";
        return;
    }
    for (std::size_t i = 0; i < allPosts.size(); i++) {
        std::cout << "===== Post #" << (i + 1) << " =====\n";
        allPosts[i]->displayPost();
        std::cout << "====================\n";
    }
}

void User::displayKthPost(int k) const {
    Node<std::shared_ptr<Post>>* nodePtr = posts.findKthItem(k);
    if (!nodePtr) {
        std::cout << "Invalid index: " << k << "\n";
        return;
    }
    nodePtr->getItem()->displayPost();
}

bool User::modifyPostTitle(int k, const std::string &newTitle) {
    Node<std::shared_ptr<Post>>* nodePtr = posts.findKthItem(k);
    if (!nodePtr) {
        return false;
    }
    nodePtr->getItem()->setTitle(newTitle);
    return true;
}

bool User::editPost(int k) {
    Node<std::shared_ptr<Post>>* nodePtr = posts.findKthItem(k);
    if (!nodePtr) {
        return false;
    }
    nodePtr->getItem()->edit(); // Polymorphique
    return true;
}

bool User::deletePost(int k) {
    Node<std::shared_ptr<Post>>* nodePtr = posts.findKthItem(k);
    if (!nodePtr) {
        return false;
    }
    std::shared_ptr<Post> thePost = nodePtr->getItem();
    // La shared_ptr sort du LinkedBag
    bool success = posts.remove(thePost);
    // Si plus aucune référence ne pointe sur ce Post, il est détruit
    return success;
}

// Surcharge de <<
std::ostream& operator<<(std::ostream& os, const User& u)
{
    os << "User: " << u.username << " | Email: " << u.email << "\n";
    return os;
}
