#include "Post.h"
#include <iostream>
#include <ctime>

Post::Post(const std::string &title, const std::string &url, int duration)
    : title(title), url(url), duration(duration), numLikes(0)
{
    timestamp = std::time(nullptr);
}

// Constructeur de copie
Post::Post(const Post& other)
  : title(other.title),
    timestamp(std::time(nullptr)), // on peut décider de conserver le timestamp d'origine
    numLikes(other.numLikes),
    url(other.url),
    duration(other.duration)
{
}

// Opérateur=
Post& Post::operator=(const Post& other)
{
    if (this != &other) {
        title    = other.title;
        url      = other.url;
        duration = other.duration;
        numLikes = other.numLikes;
        // timestamp = other.timestamp; // ou pas
    }
    return *this;
}

// Set / Get
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

// Affichage
void Post::displayPost() const {
    std::cout << "Title: " << title << "\n"
              << "URL: " << url << "\n"
              << "Likes: " << numLikes << "\n"
              << "Duration: " << duration << "\n";
}

// Surcharge <<
std::ostream& operator<<(std::ostream& os, const Post& p) {
    os << "[Post] Title: " << p.title << "\n"
       << "URL: " << p.url << "\n"
       << "Likes: " << p.numLikes << "\n"
       << "Duration: " << p.duration << "\n"
       << "Timestamp: " << p.getTimestamp() << "\n";
    return os;
}
