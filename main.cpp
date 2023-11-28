#include "json_builder.h"
#include "json_reader.h"
#include <iostream>


using namespace std;

int main() {
	transport::TransportCatalogue tc;
	ReadJson(tc, cin, cout);
}