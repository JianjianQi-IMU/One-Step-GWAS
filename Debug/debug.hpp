#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <QWidget>
#include <QDebug>

#include <fstream>
#include <string>
#include <vector>
#include <map>



inline void mySplit(std::vector<std::string>& list, const std::string& line, char sep)
{
    list.clear();
    int i = 0, pi = 0, len = line.length();
    bool f = false;
    for (i = 0; i < len; i++) {
        if (f && line[i] == sep) {
            f = false;
            list.emplace_back(line.substr(pi, i - pi));
        } else if ((!f) && line[i] != sep) {
            f = true;
            pi = i;
        }
    }
    if (f) {
        list.emplace_back(line.substr(pi, i - pi));
    }
}

inline void myStrip(std::string& line)
{
    while (line.back() == '\r' || line.back() == '\t' || line.back() == ' ' || line.back() == '\n') {
        line.pop_back();
    }
}

class Debug
{
public:
    Debug();
};

#endif // DEBUG_HPP
