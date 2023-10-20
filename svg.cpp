#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
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
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Polyline ----------------

    Polyline& Polyline::AddPoint(Point point) {
        if (points_.size() > 0) {
            Point last = points_[points_.size() - 1];
            path_length_ += sqrt(pow(point.x - last.x, 2) + pow(point.y - last.y, 2));
        }
        points_.push_back(point);
        return *this;
    }

    Polyline& Polyline::SetFilling(bool filling) {
        filling_ = filling;
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;

        int i = 0;
        for (; i < points_.size() - 1; ++i)
            out << points_[i].x << ","sv << points_[i].y << " "sv;
        out << points_[i].x << ","sv << points_[i].y << "\" "sv;
        if (!filling_) out << "fill=\"none\" "sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Text -----------------

    Text& Text::SetPosition(Point pos) {
        position_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    }

    std::string Text::GetData() const {
        return data_;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text x=\""sv << position_.x << "\" y=\""sv << position_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" "sv << "dy=\""sv << offset_.y << "\" "sv;
        out << "font_size=\"" << font_size_ << "\" "sv;
        if (!font_family_.empty()) out << "font_family=\""sv << font_family_ << "\" "sv;
        if (!font_weight_.empty()) out << "font_weight=\""sv << font_weight_ << "\" "sv;
        RenderAttrs(out);
        out << ">"sv << data_ << "</text>"sv;
    }

    // ------------ Document ---------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(move(obj));
    }

    void Document::Render(std::ostream& out) const {
        RenderContext rout(out, 2, 2);
        rout.out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        rout.out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& obj : objects_) {
            obj->Render(rout);
        }
        rout.out << "</svg>"sv;
    }
}