#include "Reel.h"
#include <iostream>

Reel::Reel(const std::string &title, const std::string &url, int duration)
    : Post(title, url, duration)
{
    //enforce a duration <= 90
    if (duration > 90) 
    {
        setDuration(90);
    }
}

void Reel::edit() {
    std::cout << "Editing Reel: added filter, AR effects, and music.\n";
}

void Reel::displayPost() const {
    std::cout << "[REEL POST]\n";
    Post::displayPost();
    std::cout << "Duration: " << duration << " seconds (limit 90)\n";
}
