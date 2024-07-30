#include <string>
#include <iostream>
#include <fstream>
#include <format>

#include "groupSorter.h"
#include "personParser.h"
#include "hutParser.h"
#include "errorHandler.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Error(Errors::invalid_params);
    }

    PersonParser personParser;
    std::map<GroupSorter::GroupID, GroupSorter::Group> groups = personParser.ParsePeople(argv[1]);

    std::vector<GroupSorter::Container> containers = ParseHuts(argv[2]);

    GroupSorter groupSorter(groups, containers);
    groupSorter.FindAllSolutions();

    personParser.PrintSolutions(groupSorter.solutions, groupSorter.groups);
}