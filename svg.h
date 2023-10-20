#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

using namespace std::literals;

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

    struct Rgb {
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba {
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double alpha = 1.0;
    };

    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;

    struct ColorConverter {
        std::ostream& out;

        void operator()(std::monostate) {
            out << "none"s;
        }

        void operator()(std::string str) {
            out << str;
        }

        void operator()(svg::Rgb rgb) {
            out << "rgb("s << unsigned(rgb.red) << ", "s << unsigned(rgb.green) << ", "s << unsigned(rgb.blue) << ")";
        }

        void operator()(svg::Rgba rgba) {
            out << "rgba("s << unsigned(rgba.red) << ", "s << unsigned(rgba.green) << ", "s << unsigned(rgba.blue) << ", "s << rgba.alpha << ")"s;
        }
    };

    inline std::ostream& operator<<(std::ostream& out, Color color) {
        std::visit(ColorConverter{out}, color);
        return out;
    }


    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    inline const std::string ToString(StrokeLineCap v)
    {
        switch (v)
        {
        case StrokeLineCap::BUTT:   return "rutt";
        case StrokeLineCap::ROUND:   return "round";
        case StrokeLineCap::SQUARE: return "square";
        default:      return "";
        }
    }

    inline const std::string ToString(StrokeLineJoin v)
    {
        switch (v)
        {
        case StrokeLineJoin::ARCS: return "arcs";
        case StrokeLineJoin::BEVEL: return "bevel";
        case StrokeLineJoin::MITER: return "miter";
        case StrokeLineJoin::MITER_CLIP: return "miter_clip";
        case StrokeLineJoin::ROUND: return "round";
        default:      return "";
        }
    }

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
            stroke_width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            stroke_line_cap_ = line_cap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            stroke_line_join_ = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_) out << " fill=\""sv << *fill_color_ << "\""sv;
            if (stroke_color_) out << " stroke=\""sv << *stroke_color_ << "\""sv;
            if (stroke_width_) out << " stroke-width=\""sv << stroke_width_.value() << "\""sv;
            if (stroke_line_cap_) out << " stroke-linecap=\""sv << ToString(stroke_line_cap_.value()) << "\""sv;
            if (stroke_line_join_) out << " stroke-linejoin=\""sv << ToString(stroke_line_join_.value()) << "\""sv;
        }

    private:
        Owner& AsOwner() {
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;
    };

    class Object {
    public:
        void Render(const RenderContext& context) const;
        ~Object() = default;
    private:

        virtual void RenderObject(const RenderContext& context) const = 0;
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


    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        Polyline& AddPoint(Point point);

        Polyline& SetFilling(bool filling);

    private:
        void RenderObject(const RenderContext& context) const override;

        std::vector<Point> points_;
        bool filling_ = false;
        double path_length_ = 0.0;
    };


    class Text final : public Object, public PathProps<Text> {
    public:
        Text& SetPosition(Point pos);

        Text& SetOffset(Point offset);

        Text& SetFontSize(uint32_t size);

        Text& SetFontFamily(std::string font_family);

        Text& SetFontWeight(std::string font_weight);

        Text& SetData(std::string data);

        std::string GetData() const;

    private:
        void RenderObject(const RenderContext& context) const override;

        Point position_;
        Point offset_;
        uint32_t font_size_ = 1;
        std::string font_weight_;
        std::string font_family_;
        std::string data_;
    };

    class ObjectContainer {
    public:
        template<typename Obj>
        void Add(const Obj& object) {
            AddPtr(std::make_unique<Obj>(object));
        }

        virtual void AddPtr(std::unique_ptr<Object>&& object) = 0;

        ~ObjectContainer() = default;
    };

    class Document : public ObjectContainer {
    public:
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        void Render(std::ostream& out) const;

    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };

    class Drawable {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;

        virtual ~Drawable() = default;
    };


}  