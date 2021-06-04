#include <iostream>
#include <unordered_map>
#include "circular_buffer.h"

using hash_map = std::unordered_map<std::string, std::string>;

int main()
{
    CircularBuffer<hash_map> cb(3);

    hash_map colors = {
            {"RED","#FF0000"},
            {"GREEN","#00FF00"},
            {"BLUE","#0000FF"}
    };

    hash_map surnames = {
            {"ANDREW", "STANKEVICH" },
            {"VLADIMIR", "PUTIN"}
    };

    hash_map capitals = {
            {"Russia", "Moscow"},
            {"Finland", "Helsinki"}
    };

    cb.push_back(colors);
    cb.push_back(surnames);
    cb.push_back(capitals);

    cb[0]["BLACK"] = "#000000"; // хочу черный в табличке
    cb[0]["WHITE"] = "#FFFFFF"; // и белый тоже

    for (hash_map &map: cb)
    {
        std::cout << "same hash map:\n";
        auto iter = map.begin();
        while (iter != map.end())
        {
            if (iter->first == "VLADIMIR") iter->second = "VASILIEV";
            std::cout << iter->first << " -> " << iter->second << "\n";
            ++iter;
        }
        std::cout << "\n";
    }

    return 0;
}