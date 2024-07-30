#include "personParser.h"
#include <iostream>
#include <fstream>

std::map<GroupSorter::GroupID, GroupSorter::Group> PersonParser::ParsePeople(std::string fileName)
{
    std::map<std::string, Person> people;
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        std::cout << "Datei '" << fileName << "' konnte nicht geöffnet werden\n";
        exit(EXIT_FAILURE);
    }

    std::string line;
    std::string currentPersonName = "";
    for (int lineIndex = 1; std::getline(file, line); lineIndex++)
    {
        if (line[0] == '&') // Line contains mustBeWith data
        {
            if (currentPersonName == "")
            {
                std::cerr << "Fehlender Personenname in Zeile " << lineIndex << std::endl;
                exit(EXIT_FAILURE);
            }

            auto &mustBeWith = people[currentPersonName].mustBeWith;

            std::string name = "";
            for (int i = 1; i < line.size(); i++) // i = 1 so that '&' is skipped
            {
                char c = line[i];

                if (c == ' ')
                    continue;

                if (c == ',' || c == '\n')
                {
                    mustBeWith.push_back(name);
                    name = "";
                    continue;
                }
                name.push_back(c);
            }
            mustBeWith.push_back(name);
        }
        else if (line[0] == '!') // Line contains cantBeWith data
        {
            if (currentPersonName == "")
            {
                std::cerr << "Fehlender Personenname in Zeile " << lineIndex << std::endl;
                exit(EXIT_FAILURE);
            }

            auto &cantBeWith = people[currentPersonName].cantBeWith;

            std::string name = "";
            for (int i = 1; i < line.size(); i++) // i = 1 so that '&' is skipped
            {
                char c = line[i];

                if (c == ' ')
                    continue;

                if (c == ',')
                {
                    cantBeWith.push_back(name);
                    name = "";
                    continue;
                }

                name.push_back(c);
            }
            cantBeWith.push_back(name);
        }
        else if (line == "") // Skip empty lines
            continue;
        else // Line contains person
        {
            if (line.back() != ':')
            {
                std::cout << "!" << line << "!\n";
                std::cout << "Fehlender Doppelpunkt in Zeile " << lineIndex << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string name = line.substr(0, line.size() - 1);
            currentPersonName = name;
            people[name] = Person();
        }
    }
    file.close();

    // PrintPeople(people, false, false);

    std::map<GroupSorter::GroupID, GroupSorter::Group> groups;

    // Initialize groups & fill the idToName vector
    {
        size_t index = 0;
        for (auto &person : people)
        {
            idToName.push_back(person.first);
            groups[index] = GroupSorter::Group(index);

            index++;
        }
    }

    // Add mustBeWith & cantBeWith
    {
        size_t personIndex = 0;
        for (auto &person : people)
        {
            for (auto &name : person.second.mustBeWith)
            {
                auto it = std::find(idToName.begin(), idToName.end(), name);
                if (it == idToName.end())
                {
                    std::cout << "Erwähnter Name '" << name << "' wurde nicht definiert\n";
                    exit(EXIT_FAILURE);
                }
                size_t index = it - idToName.begin();
                groups[personIndex].mustBeWith.push_back(index);
            }
            for (auto &name : person.second.cantBeWith)
            {
                auto it = std::find(idToName.begin(), idToName.end(), name);
                if (it == idToName.end())
                {
                    std::cout << "Erwähnter Name '" << name << "' wurde nicht definiert\n";
                    exit(EXIT_FAILURE);
                }
                size_t index = it - idToName.begin();
                groups[personIndex].cantBeWith.push_back(index);
            }
            personIndex++;
        }
    }
    // GroupSorter::PrintGroups(groups, true, true);
    return groups;
}
void PersonParser::PrintSolutions(std::vector<GroupSorter::Solution> &solutions, std::map<GroupSorter::GroupID, GroupSorter::Group> &groups)
{
    for (int i = 0; i < solutions.size(); i++)
    {
        std::cout << "\nSolution " << i << ":\n";
        GroupSorter::Solution &solution = solutions[i];
        for (auto &container : solution.containers)
        {
            for (auto &group : container.groups)
            {
                for (auto &person : groups[group].objs)
                {
                    std::cout << idToName[person] << ", ";
                }
            }
            std::cout << "\n";
        }
    }
}
void PersonParser::PrintPeople(std::map<std::string, Person> &people, bool printMustBeWith, bool printCantBeWith)
{
    std::cout << "\nPrinting people:\n";
    for (auto &person : people)
    {
        std::cout << person.first << ":\n";
        if (printMustBeWith && person.second.mustBeWith.size() != 0)
        {
            std::cout << "&    ";
            for (std::string otherPerson : person.second.mustBeWith)
                std::cout << otherPerson << ", ";
            std::cout << std::endl;
        }
        if (printCantBeWith && person.second.cantBeWith.size() != 0)
        {
            std::cout << "!    ";
            for (std::string otherPerson : person.second.cantBeWith)
                std::cout << otherPerson << ", ";
            std::cout << std::endl;
        }
    }
}