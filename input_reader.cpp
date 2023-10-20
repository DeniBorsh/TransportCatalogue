/*
#include "input_reader.h"

namespace transport {

	namespace input {

		using namespace std;

		void MakeAddRequests(TransportCatalogue& tc) {
			int n;
			cin >> n;
			string dummy;
			getline(cin, dummy);
			for (int i = 0; i < n; ++i) {
				string full_query;
				getline(cin, full_query);
				detail::QueryProcessor query_procesor(full_query);
				string operation = query_procesor.GetOperation();

				if (operation == "Stop") {
					string name = query_procesor.GetName();
					tc.AddStop(name, query_procesor.GetCoordinates());
					while (!query_procesor.IsOver()) {
						auto dis = query_procesor.GetDistance();
						tc.AddDistance(name, dis.first, dis.second);
					}
				}
				else if (operation == "Bus") {
					string name = query_procesor.GetName();
					vector<string> stops = query_procesor.AddStops();
					tc.AddBus(name, stops);
				}
				else {
					--i;
				}
			}
		}

	}

}

*/

