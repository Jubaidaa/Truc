#include "Story.h"
#include <iostream>
#include <ctime>

Story::Story(const std::string &title, const std::string &url, int duration)
    : Post(title, url, duration)
{
    //enforce duration <= 60
    if (duration > 60) 
    {
        setDuration(60);
    }
    expirationTime = getTimestamp() + (24 * 3600);  
}

void Story::edit() {
    std::cout << "Editing Story: added filter, music, stickers, and effects.\n";
}

void Story::displayPost() const {
    std::cout << "[STORY POST]\n";
    Post::displayPost();
    time_t now = std::time(nullptr);
    double hoursLeft = difftime(expirationTime, now) / 3600;
    if (hoursLeft <= 0) {
        std::cout << "This story has expired.\n";
    } else {
        std::cout << "Expires in ~" << hoursLeft << " hours.\n";
    }
    std::cout << "Duration: " << duration << " seconds (limit 60)\n";
}
