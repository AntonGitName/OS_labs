#include "commons.h"

namespace
{
    int sum_str(const char* str1)
    {
        std::stringstream ss(str1);
        int sum = 0;
        int tmp;
        while (ss >> tmp)
        {
            sum += tmp;
        }
        return sum;
    }
}

int target_func(const char* str1, const char* str2, const char* str3)
{
    return sum_str(str1) - sum_str(str2) + sum_str(str3);
}

std::string generate_str()
{
    std::string str(rand() % (MAX_STR_LENGTH - MIN_STR_LENGTH + 1) + MIN_STR_LENGTH, ' ');
    int lastSpace = 0;
    bool isFirst = true;
    for (auto&& c : str)
    {
        c = '0' + rand() % 10;
        if ((lastSpace > 0) && ((lastSpace == 2) || (rand() & 1)))
        {
            c = ' ';
            lastSpace = -1;
        }
        ++lastSpace;
        if (isFirst && c == '0')
        {
            c += 1 + rand() % 9;
        }
        isFirst = (c == ' ');
    }
    return str;
}
