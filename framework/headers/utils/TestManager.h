// TestManager.h
#ifndef TestManager_H
#define TestManager_H
#include <common.h>

class TestManager {
public:
    static TestManager& getInstance() {
        static TestManager instance;
        return instance;
    }

    string getTestDate() const { return testDate; }
    void setTestDate(string value) { testDate = value; }

private:
    TestManager() : testDate("") {}  // Private constructor

    string testDate;

    // Delete copy constructor and assignment operator to prevent copies
    TestManager(const TestManager&) = delete;
    TestManager& operator=(const TestManager&) = delete;
};

#endif // TestManager_H
