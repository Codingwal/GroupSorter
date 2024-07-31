#include "groupSorter.h"

#include <stdio.h>
#include <cassert>
#include <iostream>

#include "errorHandler.h"

void GroupSorter::FindAllSolutions()
{
    VerifyInput();

    // Merge groups
    {
        for (int i = 0; i < groups.size(); i++)
        {
            // Skip merged and therefore deleted groups
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
    }

    std::vector<GroupID> ids;
    for (auto pair : groups)
    {
        ids.push_back(pair.first);
    }

    ids.push_back(-1); // Add the placeholder id

    RecursiveTree(ids);
}
// Verifies the input & sets the placeholder count
void GroupSorter::VerifyInput()
{
    size_t totalObjCount = 0;
    size_t totalContainerSlots = 0;

    for (auto &container : containers)
    {
        if (container.capacity == 0)
            Error(Errors::container_without_capacity);

        totalContainerSlots += container.capacity;
    }

    for (int i = 0; i < groups.size(); i++)
    {
        auto &group = groups[i];
        for (ObjID otherID : group.mustBeWith)
        {
            auto &other = groups[otherID];
            if (std::find(other.cantBeWith.begin(), other.cantBeWith.end(), i) != other.cantBeWith.end())
                Error(Errors::invalid_relation);

            if (std::find(other.mustBeWith.begin(), other.mustBeWith.end(), i) == other.mustBeWith.end())
                Error(Errors::one_way_relation);
        }
        for (ObjID otherID : group.cantBeWith)
        {
            auto &other = groups[otherID];
            if (std::find(other.cantBeWith.begin(), other.cantBeWith.end(), i) == other.cantBeWith.end())
                Error(Errors::one_way_relation);
        }
        totalObjCount++;
    }

    if (totalObjCount > totalContainerSlots)
        Error(Errors::more_objs_than_space);

    placeholderCount = totalContainerSlots - totalObjCount;
}
void GroupSorter::RecursiveTree(std::vector<GroupID> ids)
{
    assert(CONTAINS(ids, -1));

    // If all groups have been successfully added, save this as a solution
    if (ids.size() == 1 && placeholderCount == 0)
    {
        AddSolution(containers);
        return;
    }

    for (GroupID id : ids)
    {
        if (id == -1)
        {
            if (placeholderCount == 0)
                continue;

            assert(currentContainerIndex < containers.size());

            // Add placeholder to container
            Container &ctr = containers[currentContainerIndex];
            ctr.groups.push_back(-1);
            ctr.size++;
            if (ctr.size == ctr.capacity) // If the container is now full, switch to the next container
                currentContainerIndex++;

            placeholderCount--;
            RecursiveTree(ids);
            placeholderCount++;

            RemoveGroup(id);
            continue;
        }

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
    assert(currentContainerIndex < containers.size());

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
    if (containers[currentContainerIndex].size == 0 || currentContainerIndex == containers.size())
        currentContainerIndex--;

    Container &ctr = containers[currentContainerIndex];
    ctr.groups.pop_back();

    if (id != -1)
        ctr.size -= groups[id].objs.size();
    else
        ctr.size--;

    assert(id == -1 || !CONTAINS(ctr.groups, id)); // Only throws if id != -1 and ctr.groups.contains(id)
}

bool GroupSorter::Container::operator<(const Container &other) const
{
    assert(groups.size() != 0);
    assert(other.groups.size() != 0);

    // if (groups.size() != other.groups.size())
    //     return groups.size() > other.groups.size(); // Print bigger containers first

    return groups[0] < other.groups[0];
}
bool GroupSorter::Container::operator==(const Container &other) const
{
    assert(groups.size() != 0);
    assert(other.groups.size() != 0);

    if (other.groups.size() != groups.size())
        return false;

    for (int i = 0; i < groups.size(); i++)
        if (other.groups[i] != groups[i])
            return false;

    return true;
}
void GroupSorter::AddSolution(std::vector<Container> containers)
{
    // Sort the containers and the container contents (needed by next step)
    for (auto &container : containers)
    {
        std::sort(container.groups.begin(), container.groups.end());
    }
    std::sort(containers.begin(), containers.end());

    // if (containers[0].groups[0] == 0 && containers[1].groups[0] == 1 && containers[1].groups[1] == 4 && containers[1].groups[2] == 6 && containers[2].groups[0] == 2)
    // {
    //     PrintContainers(containers);
    //     for (auto &solution : solutions)
    //     {
    //         for (int i = 0; i < solution.containers.size(); i++)
    //         {
    //             if (solution.containers[i] == containers[i])
    //             {
    //                 std::cout << containers[i].groups[0] << "; " << i;
    //                 exit(EXIT_FAILURE);
    //             }
    //         }
    //     }
    // }

    // Check if this solution already exists
    for (auto &solution : solutions)
    {
        for (int i = 0; i < solution.containers.size(); i++)
        {
            if (solution.containers[i] != containers[i])
                break;

            if (i == solution.containers.size() - 1)
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
