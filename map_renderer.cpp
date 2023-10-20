#include "map_renderer.h"

using namespace std;
using namespace svg;

namespace transport {

    namespace map {

        void RenderMap(const TransportCatalogue& tc, ostream& out, const SphereProjector& projector, double line_width,
            double stop_radius, int bus_label_font_size, Point bus_label_offset, int stop_label_font_size, Point stop_label_offset,
            Color underlayer_color, double underlayer_width, vector<Color> color_palette)
        {
            svg::Document document;
            vector<Text> bus_texts;
            std::map<string, Text> stop_texts;
            std::map<string, Circle> stops;
            int i = 0;
            for (const auto& [name, bus] : tc.GetBuses()) {
                if (bus.stops.empty()) continue;

                Polyline bus_route = Polyline().SetStrokeColor(color_palette[i % color_palette.size()]).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND).SetStrokeWidth(line_width);
                for (const Stop* stop : bus.stops) {
                    Point center = projector(stop->coordinates);
                    bus_route.AddPoint(center);
                    stops.insert({ stop->name, Circle().SetCenter(center).SetRadius(stop_radius).SetFillColor(Color{"white"}) });
                    stop_texts.insert({ stop->name, Text().SetFontFamily("Verdana").SetData(stop->name).SetFontSize(stop_label_font_size).SetOffset(stop_label_offset)
                                                          .SetPosition(center).SetStrokeColor(underlayer_color).SetStrokeWidth(underlayer_width)
                                                          .SetFillColor(underlayer_color).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND) });
                    stop_texts.insert({ stop->name + "_", Text().SetFontFamily("Verdana").SetData(stop->name).SetFontSize(stop_label_font_size).SetOffset(stop_label_offset).SetPosition(center).SetFillColor(Color{"black"})});
                }
                document.Add(move(bus_route));

                Text bus_text = Text().SetFontWeight("bold").SetFontFamily("Verdana").SetData(name).SetOffset(bus_label_offset).SetFontSize(bus_label_font_size).SetFillColor(color_palette[i % color_palette.size()]).SetPosition(projector(bus.stops[0]->coordinates));
                Text under_bus_text = Text().SetFontWeight("bold").SetFontFamily("Verdana").SetData(name).SetOffset(bus_label_offset).SetFontSize(bus_label_font_size)
                                            .SetFillColor(underlayer_color).SetPosition(projector(bus.stops[0]->coordinates)).SetStrokeColor(underlayer_color)
                                            .SetFillColor(underlayer_color).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND).SetStrokeWidth(underlayer_width);
                bus_texts.push_back(under_bus_text);
                bus_texts.push_back(bus_text);
                if (!bus.is_roundtrip) {
                    int middle = bus.stops.size() / 2;
                    bus_texts.push_back(under_bus_text.SetPosition(projector(bus.stops[middle]->coordinates)));
                    bus_texts.push_back(bus_text.SetPosition(projector(bus.stops[middle]->coordinates)));
                }
                ++i;
            }

            for (auto& text : bus_texts) document.Add(move(text));
            for (auto& [_, stop] : stops) document.Add(move(stop));
            for (auto& [_, text] : stop_texts) document.Add(move(text));
            document.Render(out);
        }

    }

}