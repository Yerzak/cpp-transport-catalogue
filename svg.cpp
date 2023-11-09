#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }


    void svg::OStreamColorPrinter::operator()(std::monostate) const {
        output << "none"sv;
    }
    void svg::OStreamColorPrinter::operator()(const std::string s) const {
        output << s;
    }
    void svg::OStreamColorPrinter::operator()(Rgb color) const {
        output << "rgb("sv << static_cast<int>(color.red) << ","sv << static_cast<int>(color.green) << ","sv << static_cast<int>(color.blue) << ")"sv;
    }
    void svg::OStreamColorPrinter::operator()(Rgba color) const {
        output << "rgba("sv << static_cast<int>(color.red) << ","sv << static_cast<int>(color.green) << ","sv << static_cast<int>(color.blue) << "," << color.opacity << ")"sv;
    }


    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\""sv;
        out << " r=\""sv << radius_ << "\" "sv;
        RenderAttrs(out);
        out << "/>"sv;
    }
    // -----------Polyline -----------------
    Polyline& Polyline::AddPoint(Point point) {
        points.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        bool IsFirst = true;
        for (auto& ob : points) {
            if (IsFirst) {
                out << ob.x << ","sv << ob.y;
                IsFirst = false;
            }
            else {
                out << " "sv << ob.x << ","sv << ob.y;
            }
        }
        out << "\" ";
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ------------- Text --------------------
    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text ";
        RenderAttrs(out);
        out << "x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" font-size=\""sv << size_ << "\" "sv;
        if (!font_family_.empty()) {
            out << "font-family=\"" << font_family_ << "\" "sv;
        }
        if (!font_weight_.empty()) {
            out << "font-weight=\"" << font_weight_ << "\" "sv;
        }
        out << ">"sv;
        for (char s : data_) {
            if (s == '\"') {
                out << "&qout;"sv;
            }
            else if (s == '<') {
                out << "&lt;"sv;
            }
            else if (s == '>') {
                out << "&gt;"sv;
            }
            else if (s == '\'') {
                out << "&apos;"sv;
            }
            else if (s == '&') {
                out << "&amp;"sv;
            }
            else {
                out << s;
            }
        }
        out << "</text>"sv;
    }
    // ------------ Document ------------------------------


        // Добавляет в svg-документ объект-наследник svg::Object
    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        if (obj != nullptr) {
            objects_.push_back(std::move(obj));
        }
    }

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl << "<svg xmlns="sv << "\"http://www.w3.org/2000/svg\" "sv << "version=\"1.1\">"sv << std::endl;
        for (const auto& obj_ptr : objects_) {
            if (obj_ptr != nullptr) {
                obj_ptr->Render(RenderContext(out));
            }

        }
        out << "</svg>"sv << std::endl;
    }
}  // namespace svg