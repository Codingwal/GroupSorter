#pragma once

#include <string>
#include <map>
#include <iostream>
#include <format>

enum class Errors
{
    // main
    invalid_params,

    // groupSorter
    container_without_capacity,
    invalid_relation,
    one_way_relation,
    more_objs_than_space,

    // hutParse
    cant_open_file,
};

template <typename... Args>
inline void Error(Errors errorType, Args... args)
{
    std::map<Errors, std::string> errorMessages = {
        {Errors::invalid_params, "Fehlende oder zu viele Parameter!\nKorrekte Verwendung:\n./main [Personendatei] [Huettendatei]"},
        {Errors::container_without_capacity, "Container mit Kapazitaet 0 gefunden"},
        {Errors::invalid_relation, "Wiederspruch in Beziehung gefunden"},
        {Errors::one_way_relation, "Einseitige Beziehung gefunden"},
        {Errors::more_objs_than_space, "Es gibt mehr Objekte als Plaetze"},
        {Errors::cant_open_file, "Datei {} konnte nicht geoeffnet werden"},
    };

    std::cerr << std::vformat(errorMessages[errorType], args...);
    exit(EXIT_FAILURE);
}
