#include "transport_catalogue.h"

using namespace std;

namespace transport {

	void TransportCatalogue::AddStop(const string& name, double lat, double lng) {
		auto it = stops_.find(name);
		if (it == stops_.end()) {
			Stop stop{ name, {lat, lng}, stops_count_++ };
			stops_.emplace(name, move(stop));
		}
		else {
			it->second.name = name;
			it->second.coordinates.lat = lat;
			it->second.coordinates.lng = lng;
		}
	}

	void TransportCatalogue::AddStop(const Stop& stop) {
		AddStop(stop.name, stop.coordinates.lat, stop.coordinates.lng);
	}

	void TransportCatalogue::AddStop(const string& name, geo::Coordinates coordinates) {
		AddStop(name, coordinates.lat, coordinates.lng);
	}


	void TransportCatalogue::AddDistance(const string& first, const string& second, int distance) {
		pair<Stop*, Stop*> stops_pair(&stops_[first], &stops_[second]);
		pair<Stop*, Stop*> stops_pair_rev(stops_pair.second, stops_pair.first);
		distances_[stops_pair] = distance;
		if (!distances_.count(stops_pair_rev)) distances_[stops_pair_rev] = distance;
	}


	void TransportCatalogue::AddBus(const string& name, const string& stop, bool is_roundtrip) {
		if (!stops_.count(stop)) AddStop(stop, 0.0, 0.0);
		BusOrig bus;
		bus.name = name;
		bus.id = buses_count_++;
		bus.is_roundtrip = is_roundtrip;
		bus.stops.push_back(&((*stops_.find(stop)).second));
		buses_[name] = move(bus);
		stops_buses_[stop].insert(&buses_[name]);
	}

	void TransportCatalogue::SetParams(Settings settings) {
		settings_ = move(settings);
	}

	Stop TransportCatalogue::FindStop(const string& name) const {
		auto it = stops_.find(name);
		if (it != stops_.end()) return (*it).second;
		else return Stop{ "", 0.0, 0.0, 0 };
	}

	Bus TransportCatalogue::FindBus(const string& name) const {
		auto it = buses_.find(name);
		if (it != buses_.end()) {
			vector<Stop> stops;
			for (const Stop* stop : it->second.stops) {
				stops.push_back(*stop);
			}
			return Bus{ name, move(stops), it->second.is_roundtrip, it->second.id };
		}
		else {
			return Bus{ name, vector<Stop>{}, 0};
		}
	}

	unordered_map<string, double> TransportCatalogue::GetBusInfo(const string& name) const {
		unordered_map<string, double> output;
		BusOrig bus = FindBusOrig(name);
		if (bus.stops.empty()) return output;

		output["stop_count"] = bus.stops.size();

		set<Stop*, StopPtrComparator> tmp(bus.stops.begin(), bus.stops.end());
		output["unique_stop_count"] = tmp.size();

		double route_length = 0;
		double route_length_straight = 0;
		for (int i = 0; i < bus.stops.size() - 1; ++i) {
			auto it = distances_.find({ bus.stops[i], bus.stops[i + 1] });
			if (it == distances_.end())
				it = distances_.find({ bus.stops[i + 1], bus.stops[i] });
			route_length += it->second;
			route_length_straight += geo::ComputeDistance(bus.stops[i]->coordinates, bus.stops[i + 1]->coordinates);
		}
		output["route_length"] = route_length;

		output["curvature"] = (1.0 * route_length / route_length_straight);
		return output;
	}

	pair<string, unordered_map<string, double>> TransportCatalogue::GetBusInfoString(const string& name) const {
		unordered_map<string, double> dbls = GetBusInfo(name);
		if (dbls.empty()) {
			return { "not found", dbls };
		}
		else {
			return { "bus", dbls };
		}
	}

	pair<string, vector<string>> TransportCatalogue::GetStopInfoString(const string& name) const {
		vector<string> output;
		if (!stops_.count(name)) return { "not found", output };
		else if (!stops_buses_.count(name)) return { "no buses", output };
		for (const BusOrig* bus : stops_buses_.at(name)) {
			output.push_back(bus->name);
		}
		return { "stop", output };
	}

	BusOrig TransportCatalogue::FindBusOrig(const string& name) const {
		auto it = buses_.find(name);
		if (it == buses_.end()) return BusOrig{};
		return it->second;
	}

	const unordered_map<string, Stop, hash<string>>& TransportCatalogue::GetStops() const {
		return stops_;
	}

	const map<string, BusOrig>& TransportCatalogue::GetBuses() const {
		return buses_;
	}

	int TransportCatalogue::GetDistance(Stop* first, Stop* second) const {
		return distances_.at(make_pair( first, second ));
	}

	Settings TransportCatalogue::GetSettings() const {
		return settings_;
	}

	size_t TransportCatalogue::GetStopsCount() const {
		return stops_count_;
	}

}