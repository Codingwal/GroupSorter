#pragma once

#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>
#include <cassert>

#define CONTAINS(vector, element) (std::find(vector.begin(), vector.end(), element) != vector.end())

class GroupSorter
{
public:
    using GroupID = uint32_t;
    using ObjID = uint32_t;
    struct Group
    {
        std::vector<ObjID> objs;
        std::vector<GroupID> mustBeWith;
        std::vector<GroupID> cantBeWith;
        Group(ObjID obj)
        {
            objs.push_back(obj);
        }
        Group() = default;
    };
    struct Container
    {
        size_t capacity = 0;
        std::vector<GroupID> groups;
        size_t size = 0;
        Container(size_t _capacity)
        {
            capacity = _capacity;
            size = 0;
        }
        bool operator<(const Container &other) const;
        bool operator==(const Container &other) const;
    };
    struct Solution
    {
        std::vector<Container> containers;
        Solution(std::vector<Container> _containers)
        {
            containers = _containers;
        }
    };

public:
    void FindAllSolutions();
    static void PrintGroups(std::map<GroupID, Group> groups, bool printMustBeWith, bool printCantBeWith);
    static void PrintContainers(std::vector<Container> containers);

public:
    std::map<GroupID, Group> groups;
    std::vector<Container> containers;
    std::vector<Solution> solutions;

private:
    void RecursiveTree(std::vector<GroupID> ids);
    bool TryAdd(GroupID id);
    void RemoveGroup(GroupID id);
    void AddSolution(std::vector<Container> containers);

private:
    size_t currentContainerIndex = 0;
};