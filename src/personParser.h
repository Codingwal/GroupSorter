#include <map>
#include <string>

#include "groupSorter.h"

class PersonParser
{
public:
    std::map<GroupSorter::GroupID, GroupSorter::Group> ParsePeople(std::string fileName);
    void PrintSolutions(std::vector<GroupSorter::Solution> &solutions, std::map<GroupSorter::GroupID, GroupSorter::Group> &groups);

private:
    struct Person
    {
        std::vector<std::string> mustBeWith;
        std::vector<std::string> cantBeWith;
    };

private:
    void PrintPeople(std::map<std::string, Person> &people, bool printMustBeWith, bool printCantBeWith);

private:
    std::vector<std::string> idToName;
};