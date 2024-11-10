// Singleton.h
#ifndef SINGLETON_H
#define SINGLETON_H
#include <common.h>

class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }

    string getTestDate() const { return testDate; }
    void setTestDate(string value) { testDate = value; }

private:
    Singleton() : testDate("") {}  // Private constructor

    string testDate;

    // Delete copy constructor and assignment operator to prevent copies
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

#endif // SINGLETON_H
