#pragma once
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "geo.h"
#include <map>
#include <set>

namespace transport {

	struct Settings {
		int bus_wait_time{};
		int bus_velocity{};
		Settings& SetWaitTime(int a) { bus_wait_time = a; return *this; }
		Settings& SetVelocity(int a) { bus_velocity = a; return *this; }
	};

	struct Stop {
		std::string name;
		geo::Coordinates coordinates;
	};

	struct Bus {
		std::string name;
		std::vector<Stop> stops;
		bool is_roundtrip = true;
	};

	struct BusOrig {
		std::string name;
		std::vector<Stop*> stops;
		bool is_roundtrip = true;
	};

	struct StopComparator {
		bool operator()(const Stop& p1, const Stop& p2) const {
			return p1.name < p2.name;
		}
	};

	struct StopPtrComparator {
		bool operator()(const Stop* p1, const Stop* p2) const {
			return p1->name < p2->name;
		}
	};

	struct BusOrigComparator {
		bool operator()(const BusOrig* p1, const BusOrig* p2) const {
			return p1->name < p2->name;
		}
	};

	struct PairHasher {
		size_t operator()(const std::pair<Stop*, Stop*>& pair_) const {
			return ptr_hasher(pair_.first) + ptr_hasher(pair_.second) * 43;
		}
		std::hash<const void*> ptr_hasher;
	};

	class TransportCatalogue {
	public:
		void AddStop(const Stop& stop);

		void AddStop(const std::string& name, double lat, double lng);

		void AddStop(const std::string& name, geo::Coordinates coor);

		void AddDistance(const std::string& first, const std::string& second, int distance);

		void AddBus(const std::string& name, const std::string& stop, bool is_roundtrip);

		void SetParams(Settings settings);

		template<typename Container>
		void AddBus(const std::string& name, const Container& stops, bool is_roundtrip);

		Stop FindStop(const std::string& name) const;

		Bus FindBus(const std::string& name) const;

		std::unordered_map<std::string, double> GetBusInfo(const std::string& name) const;

		std::pair<std::string, std::unordered_map<std::string, double>> GetBusInfoString(const std::string& name) const;

		std::pair<std::string, std::vector<std::string>> GetStopInfoString(const std::string& name) const;

		const std::unordered_map<std::string, Stop, std::hash<std::string>>& GetStops() const;

		const std::map<std::string, BusOrig>& GetBuses() const;

	private:
		std::unordered_map<std::string, Stop, std::hash<std::string>> stops_;
		std::map<std::string, BusOrig> buses_;
		std::unordered_map<std::string, std::set<BusOrig*, BusOrigComparator>, std::hash<std::string>> stops_buses_;
		std::unordered_map<std::pair<Stop*, Stop*>, int, PairHasher> distances_;
		Settings settings_;

		BusOrig FindBusOrig(const std::string& name) const;
	};

	template<typename Container>
	void TransportCatalogue::AddBus(const std::string& name, const Container& stops, bool is_roundtrip) {
		BusOrig bus;
		bus.name = name;
		bus.is_roundtrip = is_roundtrip;
		for (const auto& stop : stops) {
			if (!stops_.count(stop)) AddStop(stop, 0.0, 0.0);
			bus.stops.push_back(&(stops_.find(stop)->second));
		}
		if (!is_roundtrip) {
			auto it = stops.rbegin();
			++it;
			for (; it != stops.rend(); ++it)
				bus.stops.push_back(&(stops_.find(*it)->second));
		}
		buses_[name] = std::move(bus);
		for (const auto& stop : stops) {
			stops_buses_[stop].insert(&buses_[name]);
		}
	}

}