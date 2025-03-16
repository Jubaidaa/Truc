#ifndef STORY_H
#define STORY_H

#include "Post.h"

class Story : public Post {
private:
    time_t expirationTime;

public:
    Story(const std::string &title, const std::string &url, int duration);
    virtual ~Story() = default;

    void edit() override;
    void displayPost() const override;
};

#endif
