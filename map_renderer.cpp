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
    for (auto [name, vec] : routes_with_coords) {//����������� ���������� � ������� �� ���� ���������
        for (auto coords : vec) {
            route_with_points[name].push_back(sp(coords));
        }
    }
    size_t color_index = 0;
    for (auto [name, stops] : route_with_points) {//����� ���� ����������� ����� ������ � ������������ � �������� ������
        svg::Polyline polyline;
        polyline.SetFillColor(svg::NoneColor);
        polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        polyline.SetStrokeWidth(line_width);
        for (auto point : stops) {
            polyline.AddPoint(point);
        }
        if (color_palette.size() > color_index && stops.size() > 0) {//���� ���� ��������� � ����� �� ���������
            polyline.SetStrokeColor(color_palette[color_index]);//����������� ��������� ���� ���������� ��������
            ++color_index;
        }
        else if (stops.size() == 0) {//���� ��������� ���, ��������� � ���������� ��������, ����� �� ���� �� ��������
            continue;
        }
        else {//���� ��������� ����, �� ����� �����������, ����������� ����� ������ ����, � ������ ����� ������������� �� ������
            polyline.SetStrokeColor(color_palette[0]);
            color_index = 1;
        }

        result.Add(polyline);//��������� � ��������� ������
    }
    color_index = 0;
    for (auto& [bus_name_, points] : route_with_points) {//���� ���� �������� ��������� �������� � ����
        if (color_index >= color_palette.size()) {
            color_index = 0;
        }
        auto bus = *tc.FindBus(bus_name_);
        const auto under_bus_text = RenderBusText(points[0], underlayer_color, bus_name_, true);
        result.Add(under_bus_text);
        const auto main_bus_text = RenderBusText(points[0], color_palette[color_index], bus_name_, false);
        //���� ����� ����������� � ������ color_palette, a ���� - c underlayer_color
        result.Add(main_bus_text);
        if (!bus.IsRoundtrip && !IsOneStop(points[0], points[points.size() / 2])) {
            const auto bus_text_second_under = RenderBusText(points[points.size() / 2], underlayer_color, bus_name_, true);
            result.Add(bus_text_second_under);
            const auto bus_text_second = RenderBusText(points[points.size() / 2], color_palette[color_index], bus_name_, false);
            //���� ����� ����������� � ������ color_palette, � ���� - c underlayer_color
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
    }//��� �� �������� � �������� �������� ��� ����������
    for (auto [name, point] : points) {//����� ����� �������� ��������, ��������� �� �������, ��������, �������� ���� - ������, 
        //���� �������� - ������������, ������ �������� - ���� ����, ����� - �������, ������� ������ (font_weight) �� �/� ������
        //size_(stop_label_font_size) - ����, ���� - ��� �������� ��������� ������
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
    auto bus_list = tc.GetAllBuses();//�������� ������ ���� ���������
    std::map<std::string, std::vector<geo::Coordinates>> routes_with_coords;
    std::vector<geo::Coordinates> coords_list;
    for (auto bus : bus_list) {//�������� ���������� ���� ��������� � ���������
        for (auto stop_name : bus.road) {
            project::Stop* stop_ptr = tc.FindStop(stop_name);
            double lat = stop_ptr->latitude;
            double lng = stop_ptr->longtitude;
            geo::Coordinates coords = { lat, lng };
            coords_list.push_back(coords);//��������� ���������� ���� ��������� � ���� ����� ������
            //����� �� �� ������ ������� ��������������� � ������� SphereProjector
            routes_with_coords[bus.name].push_back(coords);//����� ��� �������� ����� ������������� � ������������������ �������
        }
    }
    renderer::SphereProjector sp{ coords_list.begin(), coords_list.end(), width, height, padding }; //������� ������ SphereProjector
    //����� �������� � ��� ���������� ������������� (������ ����� �������� �������� ������� ������)
    svg::Document result = RenderBuses(tc, sp, routes_with_coords);

    //--------------������ ����� ���������� ��������� � �� ��������--------------
    //1. ������� �� ���� ��������� ��������� � ��� - �� �������� ����� ������������� � ������������������ �������, � ��������� �������
    //��� �� �����������, ��� ����� ������ ��������� �������� ���� �� ���� �������
    //2. ���� ��� ����� �������������� ������: ���� ��� ��� ������ �������, ������ - ��� ������ ��������
    //3. �������� void-������� ��������� ��������� � ��������, ������� ����� ��������� �� ������ ������ Document
    std::map<std::string, svg::Point> stops_with_points;
    for (auto bus : bus_list) {
        for (auto stop_name : bus.road) {
            project::Stop stop = *(tc.FindStop(stop_name));
            stops_with_points.emplace(stop.name, sp({ stop.latitude, stop.longtitude }));
        }
    }//��� �� ������� ��� ������� ��������� � ���������, ��� ��������� ������������� �� �������� � �� �������� ����������

    RenderStops(result, stops_with_points);

    return result;//���������� ����� �����
}

