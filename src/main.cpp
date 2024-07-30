#include <string>
#include <iostream>
#include <fstream>

#include "groupSorter.h"
#include "personParser.h"
#include "hutParser.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "Fehlende oder zu viele Parameter!\nKorrekte Verwendung:\n./main [Personendatei] [Huettendatei]";
        return 0;
    }
    PersonParser personParser;
    map<GroupSorter::GroupID, GroupSorter::Group> groups = personParser.ParsePeople(argv[1]);
    vector<GroupSorter::Container> containers = ParseHuts(argv[2]);

    GroupSorter groupSorter;
    groupSorter.groups = groups;
    groupSorter.containers = containers;
    groupSorter.FindAllSolutions();

    cout << "Solution count: " << groupSorter.solutions.size() << endl;
    personParser.PrintSolutions(groupSorter.solutions, groupSorter.groups);

    return 0;
}