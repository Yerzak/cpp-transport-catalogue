#pragma once
/*Создайте в модуле map_renderer структуру, которая хранит настройки визуализации карты,
и инициализируйте её значениями прямо в коде.
Так вы сосредоточитесь на рисовании карты и не будете отвлекаться на чтение настроек из JSON.
Вы сможете сделать это позже, когда реализуете визуализацию линий маршрутов.

Воспользуйтесь классом SphereProjector из теории, чтобы корректно проецировать координаты земной поверхности в координаты на карте.

Соблюдайте указанный в условии порядок рисования линий маршрутов и вершин ломаных линий.
Иначе тренажёр будет считать результат визуализации некорректным.

{ "width": 1200.0,
  "height": 1200.0,
  "padding": 50.0,
  "line_width": 14.0,
  "stop_radius": 5.0,
  "bus_label_font_size": 20,
  "bus_label_offset": [7.0, 15.0],
  "stop_label_font_size": 20,
  "stop_label_offset": [7.0, -3.0],
  "underlayer_color": [255, 255, 255, 0.85],
  "underlayer_width": 3.0,
  "color_palette": [ "green", [255, 160, 0], "red" ] S}
*/

#include "svg.h"
#include "geo.h"
#include "transport_catalogue.h"
#include<string>
#include<vector>
#include<cstdlib>
#include<optional>
#include <algorithm>
#include <map>
namespace renderer {
    inline const double EPSILON = 1e-6;
    bool IsZero(double value);

    class SphereProjector {
    public:
        // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end,
            double max_width, double max_height, double padding)
            : padding_(padding) //
        {
            // Если точки поверхности сферы не заданы, вычислять нечего
            if (points_begin == points_end) {
                return;
            }

            // Находим точки с минимальной и максимальной долготой
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // Находим точки с минимальной и максимальной широтой
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // Вычисляем коэффициент масштабирования вдоль координаты x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // Вычисляем коэффициент масштабирования вдоль координаты y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                // Коэффициенты масштабирования по ширине и высоте ненулевые,
                // берём минимальный из них
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom) {
                // Коэффициент масштабирования по ширине ненулевой, используем его
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom) {
                // Коэффициент масштабирования по высоте ненулевой, используем его
                zoom_coeff_ = *height_zoom;
            }
        }

        // Проецирует широту и долготу в координаты внутри SVG-изображения
        svg::Point operator()(geo::Coordinates coords) const {
            return {
                (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_
            };
        }
        double GetZoom();

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };
    class MapRenderer {
    public:
        double width = 1200.0;
        double height = 1200.0;
        double padding = 50.0;
        double line_width = 10.0;
        double stop_radius = 10.0;
        size_t bus_label_font_size = 20;
        std::vector<double> bus_label_offset = { 1.0, 1.0 };
        size_t stop_label_font_size = 20;
        std::vector<double> stop_label_offset = { 7.0, -3.0 };
        svg::Color underlayer_color = svg::Rgba{ 255, 255, 255, 0.85 };
        double underlayer_width = 3.0;
        std::vector<svg::Color> color_palette = { std::string("green"), svg::Rgb{255, 160, 0}, std::string("red") };


        svg::Text RenderBusText(svg::Point pos, svg::Color color, const std::string& data, bool IsUnder) const;
        svg::Document RenderBuses(const project::TransportCatalogue& tc, renderer::SphereProjector& sp, std::map<std::string, std::vector<geo::Coordinates>>& routes_with_coords) const;
        void RenderStops(svg::Document& res, std::map<std::string, svg::Point>& points) const;
        // Этот метод будет нужен в следующей части итогового проекта
        svg::Document RenderMap(const project::TransportCatalogue& tc) const;
    };
}//namespace renderer