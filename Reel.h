#ifndef REEL_H
#define REEL_H

#include "Post.h"

class Reel : public Post {
public:
    Reel(const std::string &title, const std::string &url, int duration);
    virtual ~Reel() = default;

    // Override the edit operation
    void edit() override;

    // Display
    void displayPost() const override;
};

#endif
