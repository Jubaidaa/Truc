#ifndef POST_H
#define POST_H

#include <string>
#include <ctime>
#include <iostream>

class Post {
protected:
    std::string title;
    time_t timestamp;
    int numLikes;
    std::string url;
    int duration; 

public:
    // Big 3 (partiels)
    Post(const std::string &title, const std::string &url, int duration);
    virtual ~Post() = default;            // destructeur virtuel
    Post(const Post& other);             // constructeur de copie
    Post& operator=(const Post& other);  // opérateur=

    // Set / Get
    void setTitle(const std::string &newTitle);
    std::string getTitle() const;
    int getNumLikes() const;
    void setNumLikes(int n);
    int getDuration() const;
    void setDuration(int d);
    time_t getTimestamp() const;

    // Affichage
    virtual void displayPost() const;

    // Edit
    virtual void edit() = 0;

    // Surcharges d’opérateurs de flux (optionnel)
    friend std::ostream& operator<<(std::ostream& os, const Post& p);
    // pour la saisie si besoin
};

#endif
