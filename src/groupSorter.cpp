#include "groupSorter.h"
#include <stdio.h>
#include <cassert>
#include <iostream>

void GroupSorter::FindAllSolutions()
{
    VerifyInput();
    // Merge groups
    {
        for (int i = 0; i < groups.size(); i++)
        {
            // Skip merged and therefor deleted groups
            if (groups.count(i) == 0)
                continue;

            Group &group = groups[i];
            while (group.mustBeWith.size() > 0)
            {
                GroupID groupToAddID = group.mustBeWith[0];
                group.mustBeWith.erase(group.mustBeWith.begin()); // delete first element

                if (groups.count(groupToAddID) == 0)
                    continue;

                Group &groupToAdd = groups[groupToAddID];

                // GroupToAdd only contains 1 element because it would be evaluated after the current group
                group.objs.push_back(groupToAdd.objs[0]);

                // Merge the mustBeWith & cantBeWith arrays
                for (GroupID groupID : groupToAdd.mustBeWith)
                {
                    if (groupID != i) // Prevent adding its new group
                        group.mustBeWith.push_back(groupID);
                }
                for (GroupID groupID : groupToAdd.cantBeWith)
                {
                    if (groupID != i) // Prevent adding its new group
                        group.cantBeWith.push_back(groupID);
                }

                // Delete the now merged group
                groups.erase(groupToAddID);
            }
        }
        // PrintGroups(groups, false, true);
    }

    std::vector<GroupID> ids;
    for (auto pair : groups)
    {
        ids.push_back(pair.first);
    }

    RecursiveTree(ids);
}
void GroupSorter::VerifyInput()
{
    size_t totalObjCount = 0;
    size_t totalContainerSlots = 0;

    for (auto &container : containers)
    {
        if (container.capacity == 0)
        {
            std::cerr << "Container mit Kapazitaet 0 gefunden\n";
            exit(EXIT_FAILURE);
        }
        totalContainerSlots += container.capacity;
    }

    for (int i = 0; i < groups.size(); i++)
    {
        auto &group = groups[i];
        for (ObjID otherID : group.mustBeWith)
        {
            auto &other = groups[otherID];
            if (std::find(other.cantBeWith.begin(), other.cantBeWith.end(), i) != other.cantBeWith.end())
            {
                std::cerr << "Wiederspruch in Beziehung gefunden\n";
                exit(EXIT_FAILURE);
            }
            if (std::find(other.mustBeWith.begin(), other.mustBeWith.end(), i) == other.mustBeWith.end())
            {
                std::cerr << "Einseitige Beziehung gefunden\n";
                exit(EXIT_FAILURE);
            }
        }
        totalObjCount++;
    }

    if (totalObjCount > totalContainerSlots)
    {
        std::cerr << "Es gibt mehr Objekte als Plaetze\n";
        exit(EXIT_FAILURE);
    }
}
void GroupSorter::RecursiveTree(std::vector<GroupID> ids)
{
    // If all groups have been successfully added, save this as a solution
    if (ids.size() == 0)
    {
        AddSolution(containers);
        return;
    }

    for (GroupID id : ids)
    {
        if (!TryAdd(id))
            continue;
        ids.erase(std::find(ids.begin(), ids.end(), id));
        RecursiveTree(ids);
        ids.push_back(id);

        RemoveGroup(id);
    }
}
bool GroupSorter::TryAdd(GroupID id)
{
    if (currentContainerIndex >= containers.size())
    {
        std::cerr << currentContainerIndex << "; " << id << "\n";
        PrintContainers(containers);
        exit(EXIT_FAILURE);
    }

    Container &ctr = containers[currentContainerIndex];
    Group &group = groups[id];

    // Check if the group can be added, return false otherwise
    {
        // Return false if the group doesn't fit into the container
        if (ctr.size + group.objs.size() > ctr.capacity)
            return false;

        // Return false if the group can't be with any of the already present groups
        for (GroupID groupID : ctr.groups)
        {
            if (CONTAINS(groups[groupID].cantBeWith, id) || CONTAINS(group.cantBeWith, groupID))
                return false;
        }
    }

    // Add the group to the container
    {
        ctr.groups.push_back(id);
        ctr.size += group.objs.size();

        // If the container is now full, switch to the next container
        if (ctr.size == ctr.capacity)
            currentContainerIndex++;
    }
    // printf("%d\n", currentContainerIndex);

    return true;
}
void GroupSorter::RemoveGroup(GroupID id)
{
    // std::cout << currentContainerIndex << "\n";

    if (containers[currentContainerIndex].size == 0 || currentContainerIndex == containers.size())
        currentContainerIndex--;

    // if (currentContainerIndex >= containers.size())
    //     std::cerr << "ERROR: " << currentContainerIndex << "\n";
    Container &ctr = containers[currentContainerIndex];
    ctr.groups.pop_back();
    ctr.size -= groups[id].objs.size();

    assert(std::find(ctr.groups.begin(), ctr.groups.end(), id) == ctr.groups.end());
}

bool GroupSorter::Container::operator<(const Container &other) const
{
    assert(groups.size() != 0);
    assert(other.groups.size() != 0);
    return groups[0] < other.groups[0];
}
bool GroupSorter::Container::operator==(const Container &other) const
{
    assert(groups.size() != 0);
    assert(other.groups.size() != 0);
    return groups[0] == other.groups[0];
}
void GroupSorter::AddSolution(std::vector<Container> containers)
{
    // Sort the containers and the container contents (needed by next step)
    std::sort(containers.begin(), containers.end());
    for (auto &container : containers)
    {
        std::sort(container.groups.begin(), container.groups.end());
    }

    // Check if this solution already exists
    for (auto &solution : solutions)
    {
        for (int i = 0; i < solution.containers.size(); i++)
        {
            if (solution.containers[i] == containers[i])
                return;
        }
    }

    solutions.push_back(Solution(containers));
}

void GroupSorter::PrintGroups(std::map<GroupID, Group> groups, bool printMustBeWith, bool printCantBeWith)
{
    std::cout << "\nPrinting groups\n";

    for (auto pair : groups)
    {
        printf("%d: ", pair.first);
        for (ObjID obj : pair.second.objs)
            printf("%d, ", obj);
        printf("\n");

        if (printMustBeWith && pair.second.mustBeWith.size() != 0)
        {
            std::cout << "&   ";
            for (ObjID id : pair.second.mustBeWith)
            {
                std::cout << id << ", ";
            }
            std::cout << std::endl;
        }

        if (printCantBeWith && pair.second.cantBeWith.size() != 0)
        {
            std::cout << "!   ";
            for (ObjID id : pair.second.cantBeWith)
            {
                std::cout << id << ", ";
            }
            std::cout << "\n";
        }
    }
}
void GroupSorter::PrintContainers(std::vector<Container> containers)
{
    std::cout << "\nPrinting containers\n";
    for (auto &container : containers)
    {
        for (auto id : container.groups)
        {
            std::cout << id << ", ";
        }
        std::cout << "\n";
    }
}
