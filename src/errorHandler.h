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

    // hutParser & personParser
    cant_open_file,

    // hutParser
    invalid_char,

    // personParser
    missing_person_name,
    missing_colon,
    multiple_definitions,
    undefined_name,
};

template <typename... Args>
inline void Error(Errors errorType, Args... args)
{
    std::map<Errors, std::string_view> errorMessages = {
        {Errors::invalid_params, "Fehlende oder zu viele Parameter!\nKorrekte Verwendung:\n./main [Personendatei] [Huettendatei]"},
        {Errors::container_without_capacity, "Huette mit Kapazitaet 0 gefunden"},
        {Errors::invalid_relation, "Wiederspruch in Beziehung gefunden"},
        {Errors::one_way_relation, "Einseitige Beziehung gefunden"},
        {Errors::more_objs_than_space, "Es gibt mehr Objekte als Plaetze"},
        {Errors::cant_open_file, "Datei '{}' konnte nicht geoeffnet werden"},
        {Errors::invalid_char, "Invalides Zeichen '{}' in Zeile {}"},
        {Errors::missing_person_name, "Fehlender Personenname in Zeile {}"},
        {Errors::missing_colon, "Fehlender Doppelpunkt in Zeile {}"},
        {Errors::multiple_definitions, "Zweite Definition von '{}' in Zeile {} gefunden"},
        {Errors::undefined_name, "Erwaehnter Name '{}' wurde nicht definiert"},
    };

    std::cerr << std::vformat(errorMessages[errorType], std::make_format_args(args...));
    exit(EXIT_FAILURE);
}
