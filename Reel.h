#ifndef REEL_H
#define REEL_H

#include "Post.h"

class Reel : public Post {
public:
    Reel(const std::string &title, const std::string &url, int duration);
    virtual ~Reel() = default;

    // Override de la méthode edit()
    void edit() override;

    // Affichage
    void displayPost() const override;
};

#endif
