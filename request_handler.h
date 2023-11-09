#pragma once
#include "transport_catalogue.h"
#include "svg.h"
#include "map_renderer.h"
#include <optional>
#include <string>
#include <string_view>
#include <iostream>

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)

class RequestHandler {
public:
    using BusPtr = project::Bus*;
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const project::TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<project::BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;
    svg::Text RenderBusText(svg::Point pos, svg::Color color, const std::string& data, bool IsUnder) const;
    svg::Document RenderBuses(renderer::SphereProjector& sp, std::map<std::string, std::vector<geo::Coordinates>>& routes_with_coords) const;
    void RenderStops(svg::Document& res, std::map<std::string, svg::Point>& points) const;
    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const project::TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};