#ifndef USER_H
#define USER_H

#include <string>
#include "LinkedBag.h"
#include "Post.h"

class User {
private:
    std::string username;
    std::string email;
    std::string password;
    std::string bio;
    std::string profilePic;

    // Using a LinkedBag of Post* for polymorphism:
    LinkedBag<Post*> posts;

public:
    // Constructors
    User();
    User(const std::string &username,
         const std::string &email,
         const std::string &password,
         const std::string &bio,
         const std::string &profilePic);

    // Setters and getters
    void setPassword(const std::string &pw);
    std::string getUsername() const;

    // Display user info 
    void displayUserInfo() const;

    void createPost(Post* newPost);
    void displayAllPosts() const;
    void displayKthPost(int k) const;
    bool modifyPostTitle(int k, const std::string &newTitle);
    bool editPost(int k);
    bool deletePost(int k);

};

#endif
