#include "StringUtils.h"

// Includes
//------------------------------------------------------------------------------
// System
#include <sstream>
#include <vector>

std::string SplitAndGetElement(const std::string& input, char delimiter, int index)
{
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(iss, token, delimiter))
    {
        tokens.push_back(token);
    }

    if (index >= 0 && index < tokens.size())
    {
        return tokens[index];
    }
    else
    {
        throw std::out_of_range("Index is out of bounds.");
    }
}