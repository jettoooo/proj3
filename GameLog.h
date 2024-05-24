#ifndef GAMELOG_H
#define GAMELOG_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class GameLog
{
public:
    void record(string& message) { mLog.push_back(message); }
    void display()
    {
        while (!mLog.empty())
        {
            cout << mLog.front() << endl;
            mLog.erase(mLog.begin());
        }
    }
private:
    vector<string> mLog;
};

#endif