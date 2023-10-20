#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include <sstream>

void ReadJson(transport::TransportCatalogue& tc, std::istream& input, std::ostream& output);