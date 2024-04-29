#pragma once
#include <cstdint>
#include<deque>
#include <iostream>
#include <memory>
#include <optional>
#include<stdexcept>
#include <sstream>
#include <string>
#include <variant>

namespace svg {

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */


    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */

    class ObjectContainer {
    public:
        template <typename ObjectType>
        void Add(ObjectType obj) {
            AddPtr(std::make_unique<ObjectType>(std::move(obj)));
        }
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
        virtual ~ObjectContainer() = default;
    };

    class Drawable {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };

    struct Rgb {
        Rgb() : red(0), green(0), blue(0) {
        }
        Rgb(uint8_t r, uint8_t g, uint8_t b)
            : red(r), green(g), blue(b) {
        }
        uint8_t red, green, blue;
    };
    struct Rgba {
        Rgba()
            : red(0), green(0), blue(0), opacity(1.0) {
        }
        Rgba(uint8_t r, uint8_t g, uint8_t b, double o)
            : red(r), green(g), blue(b), opacity(o) {
        }
        uint8_t red, green, blue;
        double opacity;
    };
    inline const std::string NoneColor{ "none" };
    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;//NoneColor сюда включать наверное не надо

    // Объявив в заголовочном файле константу со спецификатором inline,
    // мы сделаем так, что она будет одной на все единицы трансляции,
    // которые подключают этот заголовок.
    // В противном случае каждая единица трансляции будет использовать свою копию этой константы

    struct OStreamColorPrinter {
        std::ostream& output;
        void operator()(std::monostate) const;
        void operator()(const std::string s) const;
        void operator()(Rgb color) const;
        void operator()(Rgba color) const;
    };
    inline std::ostream& operator << (std::ostream& out, Color color) {
        std::visit(OStreamColorPrinter{ out }, color);
        return out;
    }
    enum class StrokeLineCap {
        NONE,
        BUTT,
        ROUND,
        SQUARE,
        OVER,
    };

    inline std::string LineCapToString(const StrokeLineCap& cap) {
        switch (cap) {
        case StrokeLineCap::BUTT:
            return "butt";
        case StrokeLineCap::ROUND:
            return "round";
        case StrokeLineCap::SQUARE:
            return "square";
        default:
            throw std::invalid_argument("Invalid line join");
        }
    }
    inline std::ostream& operator<<(std::ostream& out, const StrokeLineCap& cap) {
        out << LineCapToString(cap);
        return out;
    }


    enum class StrokeLineJoin {
        NONE,
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
        OVER,
    };


    inline std::string LineJoinToString(const StrokeLineJoin& join) {
        switch (join) {
        case StrokeLineJoin::ARCS:
            return "arcs";
        case StrokeLineJoin::BEVEL:
            return "bevel";
        case StrokeLineJoin::MITER:
            return "miter";
        case StrokeLineJoin::MITER_CLIP:
            return "miter-clip";
        case StrokeLineJoin::ROUND:
            return "round";
        default:
            throw std::invalid_argument("Invalid line join");
        }
    }
    inline std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& join) {
        out << LineJoinToString(join);
        return out;
    }

    template <typename Owner>
    class PathProps {
    public:

        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeWidth(double width) {
            width_ = width;
            return AsOwner();
        }
        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = line_cap;
            return AsOwner();
        }
        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;
            if (fill_color_) {
                out << "fill=\""sv << *fill_color_ << "\" "sv;
            }
            if (stroke_color_) {
                out << "stroke=\""sv << *stroke_color_ << "\" "sv;
            }
            if (width_) {
                out << "stroke-width=\""sv << *width_ << "\" "sv;
            }
            if (static_cast<int>(line_cap_) > 0 && static_cast<int>(line_cap_) < 4) {
                out << "stroke-linecap=\""sv << line_cap_ << "\" "sv;
            }
            if (static_cast<int>(line_join_) > 0 && static_cast<int>(line_join_) < 6) {
                out << "stroke-linejoin=\""sv << line_join_ << "\" "sv;
            }
        }

    private:
        Owner& AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> width_;
        StrokeLineCap line_cap_ = StrokeLineCap::NONE;
        StrokeLineJoin line_join_ = StrokeLineJoin::NONE;
    };

    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };


    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);
    private:
        void RenderObject(const RenderContext& context) const override;
        std::deque<Point> points;
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text> {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

    private:
        void RenderObject(const RenderContext& context) const override;
        Point pos_ = { 0.0, 0.0 };
        Point offset_ = { 0.0, 0.0 };
        uint32_t size_ = 1;
        std::string font_family_;//по умолчанию не выводится
        std::string font_weight_;//по умолчанию не выводится
        std::string data_ = "";
    };

    class Document final : public ObjectContainer {
    public:
        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;
    private:
        std::deque<std::unique_ptr<Object>> objects_;
        // Прочие методы и данные, необходимые для реализации класса Document
    };

}  // namespace svg