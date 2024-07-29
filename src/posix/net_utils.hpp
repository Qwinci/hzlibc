#pragma once
#include "netinet/in.h"
#include <hz/optional.hpp>
#include <hz/string_view.hpp>

hz::optional<in_addr> parse_ipv4(hz::string_view ip);
hz::optional<in6_addr> parse_ipv6(hz::string_view ip);
