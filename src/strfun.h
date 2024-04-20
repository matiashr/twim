#ifndef STR_FUN_H
#define STR_FUN_H
#include <vector>
#include <string>


typedef std::vector<std::string> StringVector;
typedef std::string String;

static inline std::string toUpperCase(const std::string& str) 
{
    std::string result;
    for (char c : str) {
        result += std::toupper(c);
    }
    return result;
}

#endif

/* vim: set foldmethod=syntax: */
