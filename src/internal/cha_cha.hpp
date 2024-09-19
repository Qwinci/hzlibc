#pragma once
#include <stdint.h>

struct ChaCha20State {
	uint32_t state[16];
};

struct Nonce {
	inline void use() {
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

ChaCha20State cha_cha20_block(uint32_t (&key)[8], uint32_t block_counter, uint32_t (&nonce)[3]);
