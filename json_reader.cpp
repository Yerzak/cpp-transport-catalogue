#include "json_reader.h"
#include <unordered_set>

StopBefore JSONReader::ParseStop(std::map<std::string, json::Node> stop) {
    std::map<std::string, size_t> length_to;
    for (auto& [to, len] : stop.at("road_distances").AsMap()) {
        length_to.emplace(std::move(to), static_cast<size_t>(len.AsInt()));
    }
    StopBefore sb;
    sb.name = std::move(stop.at("name").AsString());
    sb.lat = stop.at("latitude").AsDouble();
    sb.lng = stop.at("longitude").AsDouble();
    sb.road_lengths = std::move(length_to);
    return sb;
}

BusBefore JSONReader::ParseBus(std::map<std::string, json::Node> bus) {
    std::vector<std::string> road;
    for (auto& stop : bus.at("stops").AsArray()) {
        road.push_back(std::move(stop.AsString()));
    }
    BusBefore bb;
    bb.name = std::move(bus.at("name").AsString());
    bb.road = std::move(road);
    bb.IsRoundtrip = bus.at("is_roundtrip").AsBool();
    return bb;
}

void JSONReader::MakeBaseRequests(std::vector<json::Node>& base) {
    std::vector<StopBefore> stop_before;
    std::vector<BusBefore> bus_before;
    for (auto& object : base) {//преобразуем информацию об остановках и маршрутах в удобный формат
        //для последующей обработки в транспортном справочнике
        if (object.AsMap().at("type").AsString() == "Stop") {
            stop_before.push_back(std::move(ParseStop(object.AsMap())));
        }
        else if (object.AsMap().at("type").AsString() == "Bus") {
            bus_before.push_back(std::move(ParseBus(object.AsMap())));
        }
    }
    for (auto& b4 : stop_before) {
        tc.AddStop(b4);
    }
    for (auto& b4 : bus_before) {
        tc.AddBus(b4);
    }
}

renderer::MapRenderer JSONReader::MakeRenderSettings(std::map<std::string, json::Node> settings) {
    renderer::MapRenderer mp;
    if (settings.count("width") && !settings.at("width").IsNull()) {
        mp.width = settings.at("width").AsDouble();
    }
    if (settings.count("height") && !settings.at("height").IsNull()) {
        mp.height = settings.at("height").AsDouble();
    }
    if (settings.count("padding") && !settings.at("padding").IsNull()) {
        mp.padding = settings.at("padding").AsDouble();
    }
    if (settings.count("line_width") && !settings.at("line_width").IsNull()) {
        mp.line_width = settings.at("line_width").AsDouble();
    }
    if (settings.count("stop_radius") && !settings.at("stop_radius").IsNull()) {
        mp.stop_radius = settings.at("stop_radius").AsDouble();
    }
    if (settings.count("bus_label_font_size") && !settings.at("bus_label_font_size").IsNull()) {
        mp.bus_label_font_size = static_cast<size_t>(settings.at("bus_label_font_size").AsInt());
    }
    if (settings.count("bus_label_offset") && !settings.at("bus_label_offset").IsNull()) {
        mp.bus_label_offset.clear();
        for (auto elem : settings.at("bus_label_offset").AsArray()) {
            mp.bus_label_offset.push_back(elem.AsDouble());
        }
    }
    if (settings.count("stop_label_font_size") && !settings.at("stop_label_font_size").IsNull()) {
        mp.stop_label_font_size = static_cast<size_t>(settings.at("stop_label_font_size").AsInt());
    }
    if (settings.count("stop_label_offset") && !settings.at("stop_label_offset").IsNull()) {
        mp.stop_label_offset.clear();
        for (auto elem : settings.at("stop_label_offset").AsArray()) {
            mp.stop_label_offset.push_back(elem.AsDouble());
        }
    }
    if (settings.count("underlayer_color") && !settings.at("underlayer_color").IsNull()) {
        if (settings.at("underlayer_color").IsString()) {
            mp.underlayer_color = settings.at("underlayer_color").AsString();
        }
        else if (settings.at("underlayer_color").IsArray()) {
            int rgba_size = 4;
            auto arr = settings.at("underlayer_color").AsArray();
            if (arr.size() < rgba_size) {
                mp.underlayer_color = svg::Rgb(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
            }
            else {
                mp.underlayer_color = svg::Rgba(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
            }
        }
    }
    if (settings.count("underlayer_width") && !settings.at("underlayer_width").IsNull()) {
        mp.underlayer_width = settings.at("underlayer_width").AsDouble();
    }
    if (settings.count("color_palette") && !settings.at("color_palette").IsNull()) {
        auto palette = settings.at("color_palette").AsArray();
        if (palette.size() > 0) {
            mp.color_palette.clear();
        }
        for (auto elem : palette) {
            if (elem.IsString()) {
                mp.color_palette.push_back(elem.AsString());
            }
            else if (elem.IsArray()) {
                int rgba_size = 4;
                auto arr = elem.AsArray();
                if (arr.size() < rgba_size) {
                    svg::Rgb rgb;
                    rgb.red = arr[0].AsInt();
                    rgb.green = arr[1].AsInt();
                    rgb.blue = arr[2].AsInt();
                    mp.color_palette.push_back(std::move(rgb));
                }
                else {
                    svg::Rgba rgba;
                    rgba.red = arr[0].AsInt();
                    rgba.green = arr[1].AsInt();
                    rgba.blue = arr[2].AsInt();
                    rgba.opacity = arr[3].AsDouble();
                    mp.color_palette.push_back(std::move(rgba));
                }
            }
        }
    }
    return mp;
}

std::vector<std::map<std::string, json::Node>> JSONReader::MakeStatRequests(std::vector<json::Node>& stat, RequestHandler& rh) {
    std::vector <std::map<std::string, json::Node>> result;
    result.reserve(stat.size());
    for (auto& req_node : stat) {//идем по массиву запросов
        std::map<std::string, json::Node> answer;
        auto request_ = std::move(req_node.AsMap());//берем один запрос и превращаем его в словарь
        answer.emplace("request_id", request_.at("id"));
        if (request_.at("type").AsString() == "Stop") {//если запрос об остановке
            if (auto buses_set_ptr = rh.GetBusesByStop(request_.at("name").AsString())) {//ищем список ее автобусов
                // - возвращается указатель на сет указателей
                std::vector<json::Node> bus_list;//т.к. Node не может принять в конструктор сет, создаем вектор
                std::for_each(buses_set_ptr->begin(), buses_set_ptr->end(), [&bus_list](RequestHandler::BusPtr elem) {
                    bus_list.push_back(json::Node(elem->name));
                    });

                std::sort(bus_list.begin(), bus_list.end(), [](auto prev, auto post) {
                    return prev.AsString() < post.AsString();
                    });//не факт, что они отсортированы, т.к. сет содержал указатели, а не имена
                //поэтому сортируем имена уже в векторе
                answer.emplace("buses", json::Node(json::Array(bus_list.begin(), bus_list.end())));
            }
            else {
                answer.emplace("error_message", json::Node(std::string("not found")));
            }

        }
        else if (request_.at("type").AsString() == "Bus") {//если запрос об автобусе
            if (auto bus_information = rh.GetBusStat(request_.at("name").AsString())) {// вернется optional BusStat
                answer.emplace("curvature", json::Node(bus_information->curvature));
                answer.emplace("route_length", json::Node(static_cast<int>(bus_information->road_length)));
                answer.emplace("stop_count", json::Node(static_cast<int>(bus_information->road.size())));
                answer.emplace("unique_stop_count", json::Node(static_cast<int>(bus_information->unique_stops.size())));
            }
            else {
                answer.emplace("error_message", json::Node(std::string("not found")));
            }
        }
        else if (request_.at("type").AsString() == "Map") {
            //СЮДА МЫ ПЕРЕНОСИМ ОБРАБОТКУ RENDER-sETTINGS И ГРАФИЧЕСКИЙ ВЫВОД ИЗ MAKE-REQUESTS
            auto map_result = rh.RenderThisMap();
            std::ostringstream output;
            map_result.Render(output);
            std::string out_info = std::move(output.str());
            answer.emplace("map", json::Node(out_info));
        }
        result.push_back(answer);
    }
    return result;
}

std::vector<std::map<std::string, json::Node>> JSONReader::MakeRequests(std::istream& input) {
    auto requests = std::move(json::LoadNode(input));//превратили входные данные в мапу
    auto base = std::move(requests.AsMap().at("base_requests").AsArray());//выделили запросы на добавление - теперь вектор
    MakeBaseRequests(base);
    auto mp = MakeRenderSettings(requests.AsMap().at("render_settings").AsMap());
    RequestHandler rh(tc, mp);
    auto stat = std::move(requests.AsMap().at("stat_requests").AsArray());//выделили запросы на поиск - теперь вектор
    return std::move(MakeStatRequests(stat, rh));
}