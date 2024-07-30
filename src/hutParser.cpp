#include <iostream>
#include <fstream>

#include "hutParser.h"
#include "errorHandler.h"

std::vector<GroupSorter::Container> ParseHuts(std::string fileName)
{
    std::vector<GroupSorter::Container> huts;
    std::ifstream file(fileName);

    if (!file.is_open())
        Error(Errors::cant_open_file, fileName);

    std::string line;
    std::string num = "";
    for (int lineIndex = 1; std::getline(file, line); lineIndex++)
    {
        for (char c : line)
        {
            if (c == ',')
            {
                int capacity = std::stoi(num);
                huts.push_back(GroupSorter::Container(capacity));
                num = "";
            }
            else if (std::isspace(c))
                continue;
            else if (std::isdigit(c))
                num.push_back(c);
            else
                Error(Errors::invalid_char, c, lineIndex);
        }
        if (num != "")
        {
            int capacity = std::stoi(num);
            huts.push_back(GroupSorter::Container(capacity));
            num = "";
        }
    }

    return huts;
}

void PrintHuts(std::vector<GroupSorter::Container> &huts)
{
    std::cout << "\nPrinting huts:\n";
    for (auto &hut : huts)
    {
        std::cout << hut.capacity << ", ";
    }
    std::cout << "\n";
}