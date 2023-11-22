#include "map_renderer.h"
bool renderer::IsZero(double value) {
    return std::abs(value) < EPSILON;
}
double renderer::SphereProjector::GetZoom() {
    return zoom_coeff_;
}

bool IsOneStop(svg::Point lhs, svg::Point rhs) {
    bool IsOneLat = (std::abs(std::abs(lhs.x) - std::abs(rhs.x)) < renderer::EPSILON);
    bool IsOneLng = (std::abs(std::abs(lhs.y) - std::abs(rhs.y)) < renderer::EPSILON);
    if (IsOneLat && IsOneLng) {
        return true;
    }
    return false;
}
svg::Text renderer::MapRenderer::RenderBusText(svg::Point pos, svg::Color color, const std::string& data, bool IsUnder) const {
    svg::Text text;
    text.SetPosition(pos);
    text.SetData(data);
    svg::Point offset(bus_label_offset[0], bus_label_offset[1]);
    text.SetOffset(offset);
    text.SetFontSize(bus_label_font_size);
    text.SetFontFamily(std::string("Verdana"));
    text.SetFontWeight(std::string("bold"));
    text.SetFillColor(color);
    if (IsUnder) {
        text.SetStrokeColor(color);
        text.SetStrokeWidth(underlayer_width);
        text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);//must be only in under
        text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);//must be only in under
    }
    return text;
}

svg::Document renderer::MapRenderer::RenderBuses(const project::TransportCatalogue& tc, renderer::SphereProjector& sp, std::map<std::string, std::vector<geo::Coordinates>>& routes_with_coords) const {
    svg::Document result;
    std::map<std::string, std::vector<svg::Point>> route_with_points;
    for (auto [name, vec] : routes_with_coords) {//преобразуем координаты в пиксели во всех маршрутах
        for (auto coords : vec) {
            route_with_points[name].push_back(sp(coords));
        }
    }
    size_t color_index = 0;
    for (auto [name, stops] : route_with_points) {//здесь надо реализовать смену цветов в соответствии с условием задачи
        svg::Polyline polyline;
        polyline.SetFillColor(svg::NoneColor);
        polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        polyline.SetStrokeWidth(line_width);
        for (auto point : stops) {
            polyline.AddPoint(point);
        }
        if (color_palette.size() > color_index && stops.size() > 0) {//если есть остановки и цвета не кончились
            polyline.SetStrokeColor(color_palette[color_index]);//присваиваем очередной цвет очередному маршруту
            ++color_index;
        }
        else if (stops.size() == 0) {//если остановок нет, переходим к следующему маршруту, цвета на него не тратятся
            continue;
        }
        else {//если остановки есть, но цвета закончились, присваиваем самый первый цвет, а индекс цвета устанавливаем на втором
            polyline.SetStrokeColor(color_palette[0]);
            color_index = 1;
        }

        result.Add(polyline);//добавляем в хранилище фигуру
    }
    color_index = 0;
    for (auto& [bus_name_, points] : route_with_points) {//сюда надо добавить параметры подложки и цвет
        if (color_index >= color_palette.size()) {
            color_index = 0;
        }
        auto bus = *tc.FindBus(bus_name_);
        const auto under_bus_text = RenderBusText(points[0], underlayer_color, bus_name_, true);
        result.Add(under_bus_text);
        const auto main_bus_text = RenderBusText(points[0], color_palette[color_index], bus_name_, false);
        //один текст добавляется с цветом color_palette, a один - c underlayer_color
        result.Add(main_bus_text);
        if (!bus.IsRoundtrip && !IsOneStop(points[0], points[points.size() / 2])) {
            const auto bus_text_second_under = RenderBusText(points[points.size() / 2], underlayer_color, bus_name_, true);
            result.Add(bus_text_second_under);
            const auto bus_text_second = RenderBusText(points[points.size() / 2], color_palette[color_index], bus_name_, false);
            //один текст добавляется с цветом color_palette, а один - c underlayer_color
            result.Add(bus_text_second);
        }
        ++color_index;
    }
    return result;
}

void renderer::MapRenderer::RenderStops(svg::Document& res, std::map<std::string, svg::Point>& points) const {
    for (auto [name, point] : points) {
        svg::Circle circle;
        circle.SetCenter(point);
        circle.SetRadius(stop_radius);
        circle.SetFillColor(std::string("white"));
        res.Add(circle);
    }//так мы добавили в итоговый документ все окружности
    for (auto [name, point] : points) {//здесь нужно добавить названия, установив им позицию, смещение, основной цвет - черный, 
        //цвет подложки - единственный, размер подложки - тоже один, шрифт - Вердана, толщина шрифта (font_weight) не д/б задана
        //size_(stop_label_font_size) - один, дата - это название остановки всегда
        svg::Text under_text;
        under_text.SetData(name);
        under_text.SetFillColor(underlayer_color);
        under_text.SetStrokeColor(underlayer_color);
        under_text.SetFontFamily(std::string("Verdana"));
        under_text.SetFontSize(stop_label_font_size);
        under_text.SetOffset({ stop_label_offset[0], stop_label_offset[1] });
        under_text.SetPosition(point);
        under_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        under_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        under_text.SetStrokeWidth(underlayer_width);
        res.Add(under_text);
        svg::Text main_text;
        main_text.SetData(name);
        main_text.SetFillColor(std::string("black"));
        main_text.SetFontFamily(std::string("Verdana"));
        main_text.SetFontSize(stop_label_font_size);
        main_text.SetOffset({ stop_label_offset[0], stop_label_offset[1] });
        main_text.SetPosition(point);
        res.Add(main_text);
    }
}

svg::Document renderer::MapRenderer::RenderMap(const project::TransportCatalogue& tc) const {
    auto bus_list = tc.GetAllBuses();//получаем список всех маршрутов
    std::map<std::string, std::vector<geo::Coordinates>> routes_with_coords;
    std::vector<geo::Coordinates> coords_list;
    for (auto bus : bus_list) {//получаем координаты всех остановок в маршрутах
        for (auto stop_name : bus.road) {
            project::Stop* stop_ptr = tc.FindStop(stop_name);
            double lat = stop_ptr->latitude;
            double lng = stop_ptr->longtitude;
            geo::Coordinates coords = { lat, lng };
            coords_list.push_back(coords);//добавляем координаты всех остановок в один общий вектор
            //чтобы на их основе создать масштабирование с помощью SphereProjector
            routes_with_coords[bus.name].push_back(coords);//здесь все маршруты будут отсортированы в лексикографическом порядке
        }
    }
    renderer::SphereProjector sp{ coords_list.begin(), coords_list.end(), width, height, padding }; //создали объект SphereProjector
    //чтобы получить в нем коэффицент проецирования (теперь будет работать оператор круглые скобки)
    svg::Document result = RenderBuses(tc, sp, routes_with_coords);

    //--------------ТЕПЕРЬ НУЖНО ОБРАБОТАТЬ ОСТАНОВКИ И ИХ НАЗВАНИЯ--------------
    //1. Собрать со всех маршрутов остановки в мэп - их названия будут отсортированы в лексикографическом порядке, а дубликаты удалены
    //Так мы гарантируем, что через каждую остановку проходит хотя бы один маршрут
    //2. Этот мэп будет использоваться дважды: один раз при выводе кружков, второй - при выводе названий
    //3. Написать void-функцию отрисовки остановок и названий, которая будет принимать по ссылке объект Document
    std::map<std::string, svg::Point> stops_with_points;
    for (auto bus : bus_list) {
        for (auto stop_name : bus.road) {
            project::Stop stop = *(tc.FindStop(stop_name));
            stops_with_points.emplace(stop.name, sp({ stop.latitude, stop.longtitude }));
        }
    }//так мы создали наш словарь остановок с пикселями, где остановки отсортированы по алфавиту и не содержат дубликатов

    RenderStops(result, stops_with_points);

    return result;//возвращаем набор фигур
}

