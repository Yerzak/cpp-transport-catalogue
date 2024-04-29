#include "request_handler.h"
#include <algorithm>
#include <cstdlib>
#include <map>
#include <vector>

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
RequestHandler::RequestHandler(const project::TransportCatalogue& db, const renderer::MapRenderer& renderer)
    : db_(db), renderer_(std::move(renderer)) {
}

std::optional<project::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    auto bus_ptr = db_.FindBus(bus_name);
    if (bus_ptr) {
        auto result = db_.FindBusInfo(bus_ptr->name);
        if (result.IsExist) {
            return result;
        }
    }
    return std::nullopt;
}

svg::Document RequestHandler::RenderThisMap() const {//ЗДЕСЬ НУЖНО ПРИСВОИТЬ ЦВЕТА И ТОЛЩИНЫ ФИГУРАМ   
    return renderer_.RenderMap(db_);
}

const std::unordered_set<RequestHandler::BusPtr>* RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    return db_.GetBusesForStop(stop_name);
}