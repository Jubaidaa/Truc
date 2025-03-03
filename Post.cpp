#include "Post.h"
#include <iostream>
#include <ctime>

Post::Post(const std::string &title, const std::string &url, int duration)
    : title(title), url(url), duration(duration), numLikes(0)
{
    timestamp = std::time(nullptr);  
}

void Post::setTitle(const std::string &newTitle) {
    title = newTitle;
}

std::string Post::getTitle() const {
    return title;
}

int Post::getNumLikes() const {
    return numLikes;
}

void Post::setNumLikes(int n) {
    numLikes = n;
}

int Post::getDuration() const {
    return duration;
}

void Post::setDuration(int d) {
    duration = d;
}

time_t Post::getTimestamp() const {
    return timestamp;
}

void Post::displayPost() const {
    // Basic post info
    std::cout << "Title: " << title << "\n"
              << "URL: " << url << "\n"
              << "Likes: " << numLikes << "\n";
}
