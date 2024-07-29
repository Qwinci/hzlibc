#include "bsd/stdlib.h"
#include "utils.hpp"
#include "mutex.hpp"
#include "sys.hpp"
#include "errno.h"
#include <hz/algorithm.hpp>

#define memcpy __builtin_memcpy

namespace {
	constexpr uint32_t rotate_left(uint32_t value, uint8_t num) {
		return (value << num) | (value >> (32 - num));
	}

	constexpr void chacha20_quarter_round(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
		a += b;
		d ^= a;
		d = rotate_left(d, 16);
		c += d;
		b ^= c;
		b = rotate_left(b, 12);
		a += b;
		d ^= a;
		d = rotate_left(d, 8);
		c += d;
		b ^= c;
		b = rotate_left(b, 7);
	}

	struct ChaCha20State {
		uint32_t state[16];

		constexpr void init(uint32_t (&key)[8], uint32_t block_counter, uint32_t (&nonce)[3]) {
			state[0] = 0x61707865;
			state[1] = 0x3320646E;
			state[2] = 0x79622D32;
			state[3] = 0x6B206574;
			state[4] = key[0];
			state[5] = key[1];
			state[6] = key[2];
			state[7] = key[3];
			state[8] = key[4];
			state[9] = key[5];
			state[10] = key[6];
			state[11] = key[7];
			state[12] = block_counter;
			state[13] = nonce[0];
			state[14] = nonce[1];
			state[15] = nonce[2];
		}

		constexpr void inner_block() {
			quarter_round(0, 4, 8, 12);
			quarter_round(1, 5, 9, 13);
			quarter_round(2, 6, 10, 14);
			quarter_round(3, 7, 11, 15);
			quarter_round(0, 5, 10, 15);
			quarter_round(1, 6, 11, 12);
			quarter_round(2, 7, 8, 13);
			quarter_round(3, 4, 9, 14);
		}

		constexpr void quarter_round(uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
			chacha20_quarter_round(state[x], state[y], state[z], state[w]);
		}
	};

	constexpr ChaCha20State cha_cha20_block(uint32_t (&key)[8], uint32_t block_counter, uint32_t (&nonce)[3]) {
		ChaCha20State initial_state {};
		initial_state.init(key, block_counter, nonce);
		auto state = initial_state;
		for (int i = 0; i < 10; ++i) {
			state.inner_block();
		}

		for (int i = 0; i < 16; ++i) {
			state.state[i] += initial_state.state[i];
		}
		return state;
	}

	struct Nonce {
		void use() {
			inner[0] += 1;
			if (inner[0] == 0) {
				inner[1] += 1;
				if (inner[1] == 0) {
					inner[2] += 1;
				}
			}
		}

		uint32_t inner[3];
	};

	Nonce NONCE {};
	uint32_t KEY[8] {};
	bool INITIALIZED {};
	Mutex LOCK {};
}

EXPORT uint32_t arc4random() {
	uint32_t value;
	arc4random_buf(&value, 4);
	return value;
}

EXPORT void arc4random_buf(void* buf, size_t num_bytes) {
	auto guard = LOCK.lock();
	if (!INITIALIZED) {
		ssize_t ret;
		while (auto err = sys_getrandom(KEY, 32, 0, &ret)) {
			if (err == EINTR) {
				continue;
			}
			panic("arc4random_buf: sys_getrandom failed");
		}
	}

	uint32_t counter = 0;
	for (size_t i = 0; i < num_bytes; i += 64) {
		auto state = cha_cha20_block(KEY, counter++, NONCE.inner);
		size_t to_copy = hz::min(num_bytes - i, size_t {64});
		memcpy(static_cast<char*>(buf) + i, state.state, to_copy);
	}
	NONCE.use();
}
