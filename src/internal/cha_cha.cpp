#include "cha_cha.hpp"

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

	constexpr void state_init(ChaCha20State& state, uint32_t (&key)[8], uint32_t block_counter, uint32_t (&nonce)[3]) {
		state.state[0] = 0x61707865;
		state.state[1] = 0x3320646E;
		state.state[2] = 0x79622D32;
		state.state[3] = 0x6B206574;
		state.state[4] = key[0];
		state.state[5] = key[1];
		state.state[6] = key[2];
		state.state[7] = key[3];
		state.state[8] = key[4];
		state.state[9] = key[5];
		state.state[10] = key[6];
		state.state[11] = key[7];
		state.state[12] = block_counter;
		state.state[13] = nonce[0];
		state.state[14] = nonce[1];
		state.state[15] = nonce[2];
	}

	constexpr void quarter_round(ChaCha20State& state, uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
		chacha20_quarter_round(state.state[x], state.state[y], state.state[z], state.state[w]);
	}

	constexpr void inner_block(ChaCha20State& state) {
		quarter_round(state, 0, 4, 8, 12);
		quarter_round(state, 1, 5, 9, 13);
		quarter_round(state, 2, 6, 10, 14);
		quarter_round(state, 3, 7, 11, 15);
		quarter_round(state, 0, 5, 10, 15);
		quarter_round(state, 1, 6, 11, 12);
		quarter_round(state, 2, 7, 8, 13);
		quarter_round(state, 3, 4, 9, 14);
	}
}

ChaCha20State cha_cha20_block(uint32_t (&key)[8], uint32_t block_counter, uint32_t (&nonce)[3]) {
	ChaCha20State initial_state {};
	state_init(initial_state, key, block_counter, nonce);
	auto state = initial_state;
	for (int i = 0; i < 10; ++i) {
		inner_block(state);
	}

	for (int i = 0; i < 16; ++i) {
		state.state[i] += initial_state.state[i];
	}
	return state;
}
