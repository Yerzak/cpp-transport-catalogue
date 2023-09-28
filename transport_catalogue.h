#pragma once
#include <string>
#include<string_view>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<deque>
#include<iostream>
#include<algorithm>
#include<tuple>
#include "input_reader.h"
#include "stat_reader.h"
namespace project {
    struct Bus {
        std::string name;//надо инициализировать из общего дека
        std::vector<std::string_view> road;
        bool operator<(Bus& other) {
            return std::lexicographical_compare(name.begin(), name.end(), other.name.begin(), other.name.end());
        }
    };
    struct Stop {
        std::string name;
        double longtitude;
        double latitude;
        bool operator<(Stop& other) {
            return std::lexicographical_compare(name.begin(), name.end(), other.name.begin(), other.name.end());
        }
    };
    struct BusInfo {
        std::string name;
        std::vector<std::string_view> road;
        std::unordered_set <std::string_view> unique_stops;
        size_t road_length;
    };
    namespace detail {
        struct RouteLengthHash {
            static const size_t koeff_first = 28;
            size_t operator() (const std::pair<Stop*, Stop*>& segment) const {
                size_t from_name = static_cast<size_t>(text_hasher(segment.first->name));
                size_t to_name = static_cast<size_t>(text_hasher(segment.second->name));
                return from_name + to_name * koeff_first;
            }
        private:
            std::hash<std::string> text_hasher;
        };
    }
    class TransportCatalogue {
    public:
        void AddBus(BusBefore& bus_data);
        void AddStop(StopBefore& stop_data);
        BusInfo FindBusInfo(const std::string& name);
        Bus* FindBus(std::string_view bus);
        void SetDistance(Stop* from, Stop* to, size_t distance);
        size_t GetDistance(Stop* from, Stop* to);
        double GetCurvature(size_t fact_distance, const std::vector<std::string_view>& road);
        Stop* FindStop(std::string_view stop);
        std::pair<std::string, std::unordered_set<Bus*>> FindStopInfo(const std::string& name);
        size_t GetBusCount();
        size_t GetStopCount();

    private:
        std::deque<Stop> list_of_stops;
        std::deque <Bus> list_of_buses;
        std::unordered_map<std::string_view, Stop*> stops;
        std::unordered_map <std::string_view, Bus*> buses;
        std::unordered_map<std::pair<Stop*, Stop*>, size_t, detail::RouteLengthHash> route_lengths;
        std::unordered_map<std::string_view, std::unordered_set<Bus*>> stop_with_buses;
    };
}






