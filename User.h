#ifndef USER_H
#define USER_H

#include <string>
#include <memory>
#include <iostream>
#include "LinkedBag.h"
#include "Post.h"

class User {
private:
    std::string username;
    std::string email;
    std::string password;
    std::string bio;
    std::string profilePic;

    // Stockage sous forme de LinkedBag de shared_ptr<Post>
    LinkedBag<std::shared_ptr<Post>> posts;

public:
    // Big 3
    User();                                 // constructeur par défaut
    ~User();                                // destructeur
    User(const User& other);               // constructeur de copie
    User& operator=(const User& other);    // opérateur=

    // Autre constructeur
    User(const std::string &username,
         const std::string &email,
         const std::string &password,
         const std::string &bio,
         const std::string &profilePic);

    // Set / Get
    void setPassword(const std::string &pw);
    std::string getUsername() const;

    // Affichage
    void displayUserInfo() const;

    // Gestion de posts
    void createPost(std::shared_ptr<Post> newPost);
    void displayAllPosts() const;
    void displayKthPost(int k) const;
    bool modifyPostTitle(int k, const std::string &newTitle);
    bool editPost(int k);
    bool deletePost(int k);

    // Surcharge de <<
    friend std::ostream& operator<<(std::ostream& os, const User& u);
};

#endif
