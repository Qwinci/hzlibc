#include "net_utils.hpp"
#include <hz/string_utils.hpp>

hz::optional<in_addr> parse_ipv4(hz::string_view ip) {
	size_t offset = 0;
	in_addr addr {};
	int i = 0;
	while (true) {
		auto segment_end = ip.find('.', offset);
		auto segment = ip.substr_abs(offset, segment_end);

		size_t count;
		auto num = hz::to_integer<uint8_t>(segment, 10, &count);
		if (count != segment.size() || !count) {
			return hz::nullopt;
		}
		addr.s_addr |= num << (i++ * 8);

		if (segment_end == hz::string_view::npos) {
			if (i == 4) {
				return addr;
			}
			else {
				return hz::nullopt;
			}
		}
		else {
			offset = segment_end + 1;
		}
	}
}

hz::optional<in6_addr> parse_ipv6(hz::string_view ip) {
	auto compressed_pos = ip.find("::");

	in6_addr addr {};
	int i = 0;
	int zeros = 0;

	if (compressed_pos != hz::string_view::npos) {
		size_t segments = ip.count(':') - 1;
		if (segments > 7) {
			return hz::nullopt;
		}
		if (compressed_pos != 0 && compressed_pos != ip.size() - 2) {
			++segments;
		}

		zeros = static_cast<int>(8 - segments);
	}

	size_t offset = 0;
	while (true) {
		if (offset == compressed_pos) {
			i += zeros;
			offset += 2;
			if (i == 8) {
				return addr;
			}
			else {
				continue;
			}
		}

		auto segment_end = ip.find(':', offset);
		auto segment = ip.substr_abs(offset, segment_end);

		size_t count;
		auto num = hz::to_integer<uint16_t>(segment, 16, &count);
		if (count != segment.size() || !count) {
			return hz::nullopt;
		}
		addr.s6_addr16[i++] = hz::to_ne_from_be(num);

		if (segment_end == hz::string_view::npos) {
			if (i != 8) {
				return hz::nullopt;
			}
			break;
		}
		else {
			if (i == 8) {
				return hz::nullopt;
			}
			if (segment_end == compressed_pos) {
				offset = segment_end;
			}
			else {
				offset = segment_end + 1;
			}
		}
	}

	return addr;
}
