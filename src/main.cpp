#include <string>
#include <iostream>
#include <fstream>
#include <format>

#include "groupSorter.h"
#include "personParser.h"
#include "hutParser.h"
#include "errorHandler.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Error(Errors::invalid_params);
    }
    PersonParser personParser;
    map<GroupSorter::GroupID, GroupSorter::Group> groups = personParser.ParsePeople(argv[1]);
    vector<GroupSorter::Container> containers = ParseHuts(argv[2]);

    GroupSorter groupSorter;
    groupSorter.groups = groups;
    groupSorter.containers = containers;
    groupSorter.FindAllSolutions();

    personParser.PrintSolutions(groupSorter.solutions, groupSorter.groups);

    return 0;
}