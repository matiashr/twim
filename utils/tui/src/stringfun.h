#ifndef STRINGFUN_H
#define STRINGFUN_H
#include <string>
#include <vector>
#include  <algorithm>

typedef std::vector<std::string> StringVector;

inline std::string unquote( std::string a_str )
{
	std::string res=a_str; 
	try {
		res.erase(std::remove(res.begin(), res.end(), '"'), res.end());
		return res;
	}catch( ...) {
		printf("erase failed\n");
	}
	return a_str;
}

inline std::string padRight(std::string const& str, size_t s, char ch)
{
	if ( str.size() < s )
		return str + std::string(s-str.size(), ch);
	else
		return str;
}

inline std::string padLeft(std::string const& str, size_t s,char ch)
{
	if ( str.size() < s )
		return std::string(s-str.size(), ch) + str;
	else
		return str;
}

#endif
