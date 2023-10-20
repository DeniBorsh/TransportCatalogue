/*
#include "stat_reader.h"

using namespace std;

namespace transport {

	namespace output {

		void WriteBusInfo(const std::string& name, const std::pair<std::string, vector<double>>& infos) {
			cout << "Bus " << name << ": ";
			if (infos.first == "not found") cout << "not found" << endl;
			else cout <<
				(int)infos.second.at(0) << " stops on route, " <<
				(int)infos.second.at(1) << " uniqe stops, " <<
				(int)infos.second.at(2) << " route length, " << setprecision(6) <<
				infos.second.at(3) << " curvature" << endl;
		}

		void WriteStopInfo(const std::string& name, const std::pair<std::string, vector<std::string>>& infos) {
			cout << "Stop " << name << ": ";
			if (infos.first == "no buses") cout << "no buses" << endl;
			else if (infos.first == "not found") cout << "not found" << endl;
			else {
				cout << "buses";
				for (const string& stop : infos.second)
					cout << " " << stop;
				cout << endl;
			}
		}


		void MakeFindRequest(const TransportCatalogue& tc) {
			int n;
			cin >> n;

			string dummy;

			getline(cin, dummy);

			vector<string> names;
			vector<Info> infos;
			for (int i = 0; i < n; ++i) {
				string full_query;
				getline(cin, full_query);
				detail::QueryProcessor query_processor(full_query);
				if (query_processor.GetOperation() == "Bus") {
					string name = query_processor.GetName();
					names.push_back(name);
					Info tmp;
					tmp.as_doubles = tc.GetBusInfoString(name);
					infos.push_back(move(tmp));
				}
				else {
					string name = query_processor.GetName();
					names.push_back(name);
					Info tmp;
					tmp.as_strings = tc.GetStopInfoString(name);
					infos.push_back(move(tmp));
				}
			}
			for (int i = 0; i < n; ++i) {
				if (infos[i].as_doubles.first.empty()) WriteStopInfo(names[i], infos[i].as_strings);
				else WriteBusInfo(names[i], infos[i].as_doubles);
			}
		}

	}

}
*/