#ifndef POST_H
#define POST_H

#include <string>
#include <ctime>

class Post {
protected:
    std::string title;
    time_t timestamp;
    int numLikes;
    std::string url;
    int duration; 

public:
    
    Post(const std::string &title, const std::string &url, int duration);
    virtual ~Post() = default;

    
    void setTitle(const std::string &newTitle);
    std::string getTitle() const;
    int getNumLikes() const;
    void setNumLikes(int n);
    int getDuration() const;
    void setDuration(int d);
    time_t getTimestamp() const;

    // Display 
    virtual void displayPost() const;

    
    virtual void edit() = 0;  
};

#endif
