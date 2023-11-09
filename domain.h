#pragma once
#include <map>
#include <vector>
#include <string>
#include <unordered_set>
#include <string_view>
struct BusBefore {
    std::string name;
    std::vector<std::string> road;
    bool IsRoundtrip;
};
struct StopBefore {
    std::string name;
    double lat, lng;
    std::map<std::string, size_t> road_lengths;
};
namespace project {
    struct Bus {
        std::string name;//надо инициализировать из общего дека
        std::vector<std::string_view> road;
        bool operator<(Bus& other) {
            return std::lexicographical_compare(name.begin(), name.end(), other.name.begin(), other.name.end());
        }
        bool IsRoundtrip;
    };
    struct Stop {
        std::string name;
        double longtitude;
        double latitude;
        bool operator<(Stop& other) {
            return std::lexicographical_compare(name.begin(), name.end(), other.name.begin(), other.name.end());
        }
    };
    struct BusStat {
        std::string name;
        std::vector<std::string_view> road;
        std::unordered_set <std::string_view> unique_stops;
        size_t road_length;
        double curvature;
        bool IsExist;
    };
}//namespace project