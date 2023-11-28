#include "json_reader.h"
#include "transport_router.h"
#include "json_builder.h"

using namespace std;
using namespace json;

svg::Color SetColor(const json::Node& color);

string GetSvg(const transport::TransportCatalogue& tc, const Dict& settings);

void ReadJson(transport::TransportCatalogue& tc, istream& input, ostream& output) {
	Dict all = Load(input).GetRoot().AsDict();

	Array read_array = all.at("base_requests").AsArray();
	for (const Node& query : read_array) {
		Dict cur_query = query.AsDict();
		string name = cur_query.at("name").AsString();

		if (cur_query.at("type") == "Stop") {
			tc.AddStop(name, cur_query.at("latitude").AsDouble(), cur_query.at("longitude").AsDouble());

			for (const auto& [stop_name, distance] : cur_query.at("road_distances").AsDict()) {
				tc.AddDistance(name, stop_name, distance.AsInt());
			}
		}
		else if (cur_query.at("type") == "Bus") {
			Array stops_arr = cur_query.at("stops").AsArray();

			vector<string> stops;
			for (const Node& stop : stops_arr) stops.push_back(stop.AsString());

			tc.AddBus(name, stops, cur_query.at("is_roundtrip").AsBool());
		}
	}

	Dict settings_dict = all.at("routing_settings").AsDict();
	double bus_wait_time = settings_dict.at("bus_wait_time").AsDouble();
	double bus_velocity = settings_dict.at("bus_velocity").AsDouble();

	transport::Router router{ static_cast<int>(bus_wait_time), bus_velocity };
	router.BuildGraph(tc);

	Array stat_array = all.at("stat_requests").AsArray();
	if (!stat_array.empty()) {
		Array out_all;

		for (const Node& query : stat_array) {
			Dict cur_query = query.AsDict();
			int id = cur_query.at("id").AsInt();

			if (cur_query.at("type") == "Stop") {
				string name = cur_query.at("name").AsString();
				Dict dict;
				dict["request_id"] = { id };

				if (tc.GetStopInfoString(name).first == "not found")
					dict["error_message"] = { "not found" };
				else if (tc.GetStopInfoString(name).first == "no buses")
					dict["error_message"] = { "no buses" };
				else {
					Array arr;
					for (const string& bus : tc.GetStopInfoString(name).second)
						arr.push_back({ bus });
					dict["buses"] = { move(arr) };
				}
				out_all.push_back({ move(dict) });
			}
			else if (cur_query.at("type") == "Bus") {
				string name = cur_query.at("name").AsString();
				Dict dict;
				dict["request_id"] = { id };

				if (tc.GetBusInfoString(name).first == "not found")
					dict["error_message"] = { "not found" };
				else {
					auto map = tc.GetBusInfoString(name).second;
					dict["curvature"] = { map.at("curvature") };
					dict["route_length"] = { map.at("route_length") };
					dict["stop_count"] = { map.at("stop_count") };
					dict["unique_stop_count"] = { map.at("unique_stop_count") };
				}
				out_all.push_back({ move(dict) });
			}
			else if (cur_query.at("type") == "Map") {
				Dict dict;
				dict["map"] = GetSvg(tc, all.at("render_settings").AsDict());
				dict["request_id"] = id;
				out_all.push_back({ move(dict) });
			}
			else if (cur_query.at("type") == "Route") {
				string from{ cur_query.at("from").AsString() };
				string to{ cur_query.at("to").AsString() };
				json::Node result;
				const auto& routing = router.FindRoute(from, to);

				if (!routing) {
					result = json::Builder{}
						.StartDict()
						.Key("request_id"s).Value(id)
						.Key("error_message"s).Value("not found"s)
						.EndDict()
						.Build();
				}
				else {
					json::Array items;
					double total_time = 0.0;
					items.reserve(routing.value().edges.size());
					for (auto& edge_id : routing.value().edges) {
						const graph::Edge<double> edge = router.GetGraph().GetEdge(edge_id);
						if (edge.quality == 0) {
							items.emplace_back(json::Node(json::Builder{}
								.StartDict()
								.Key("stop_name"s).Value(edge.name)
								.Key("time"s).Value(edge.weight)
								.Key("type"s).Value("Wait"s)
								.EndDict()
								.Build()));

							total_time += edge.weight;
						}
						else {
							items.emplace_back(json::Node(json::Builder{}
								.StartDict()
								.Key("bus"s).Value(edge.name)
								.Key("span_count"s).Value(static_cast<int>(edge.quality))
								.Key("time"s).Value(edge.weight)
								.Key("type"s).Value("Bus"s)
								.EndDict()
								.Build()));

							total_time += edge.weight;
						}
					}

					result = json::Builder{}
						.StartDict()
						.Key("request_id"s).Value(id)
						.Key("total_time"s).Value(total_time)
						.Key("items"s).Value(items)
						.EndDict()
						.Build();
				}

				out_all.push_back(move(result));
			}

		}
		
		json::Print(Document{out_all}, { cout });
	}



}

svg::Color SetColor(const json::Node& color) {
	if (color.IsString()) return color.AsString();
	else if (color.IsArray()) {
		json::Array color_array = color.AsArray();
		if (color_array.size() == 3) return svg::Rgb{ static_cast<uint8_t>(color_array[0].AsInt()), static_cast<uint8_t>(color_array[1].AsInt()), static_cast<uint8_t>(color_array[2].AsInt()) };
		else if (color_array.size() == 4) return svg::Rgba{ static_cast<uint8_t>(color_array[0].AsInt()), static_cast<uint8_t>(color_array[1].AsInt()), static_cast<uint8_t>(color_array[2].AsInt()), color_array[3].AsDouble()};
	}
}

string GetSvg(const transport::TransportCatalogue& tc, const Dict& settings) {
	ostringstream blank;

	vector<geo::Coordinates> coordinates;
	for (const auto& [name, stop] : tc.GetStops())
		coordinates.push_back(stop.coordinates);

	transport::map::SphereProjector projector(coordinates.begin(), coordinates.end(), settings.at("width").AsDouble(), settings.at("height").AsDouble(), settings.at("padding").AsDouble());

	double line_width = settings.at("line_width").AsDouble();
	double stop_radius = settings.at("stop_radius").AsDouble();

	int bus_label_font_size = settings.at("bus_label_font_size").AsInt();

	Array offset = settings.at("bus_label_offset").AsArray();
	svg::Point bus_label_offset{ offset[0].AsDouble(), offset[1].AsDouble() };

	int stop_label_font_size = settings.at("stop_label_font_size").AsInt();

	offset = settings.at("stop_label_offset").AsArray();
	svg::Point stop_label_offset{ offset[0].AsDouble(), offset[1].AsDouble() };

	svg::Color underlayer_color = SetColor(settings.at("underlayer_color"));

	double underlayer_width = settings.at("underlayer_width").AsDouble();

	vector<svg::Color> color_palette;
	for (const Node& node : settings.at("color_palette").AsArray())
		color_palette.push_back(SetColor(node));

	transport::map::RenderMap(tc, blank, projector, line_width, stop_radius, bus_label_font_size, bus_label_offset, stop_label_font_size, stop_label_offset, underlayer_color, underlayer_width, color_palette);
	return blank.str();
}
