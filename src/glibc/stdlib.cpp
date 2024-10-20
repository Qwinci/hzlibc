#include "stdlib.h"
#include "utils.hpp"
#include "errno.h"
#include "stdio.h"
#include "fcntl.h"
#include "errno.h"
#include "unistd.h"
#include "cha_cha.hpp"


EXPORT void* reallocarray(void* old, size_t num_blocks, size_t size) {
	if (size && num_blocks > SIZE_MAX / size) {
		errno = ENOMEM;
		return nullptr;
	}

	size_t total = num_blocks * size;
	return realloc(old, total);
}

EXPORT char* secure_getenv(const char* name) {
	// todo make this secure
	return getenv(name);
}

EXPORT char* canonicalize_file_name(const char* path) {
	return realpath(path, nullptr);
}

EXPORT char* mktemp(char* template_str) {
	__ensure(!"mktemp is not safe to use");
}

EXPORT int mkostemp(char* template_str, int flags) {
	hz::string_view str {template_str};
	if (!str.ends_with("XXXXXX")) {
		errno = EINVAL;
		return -1;
	}

	flags &= ~O_WRONLY;

	for (size_t i = 0; i < 999999; ++i) {
		__ensure(sprintf(template_str + str.size() - 6, "%06zu", i) == 6);

		int fd = open(template_str, O_RDWR | O_CREAT | O_EXCL | flags, 0600);
		if (fd < 0) {
			if (errno == EEXIST) {
				close(fd);
				continue;
			}
			return -1;
		}
		return fd;
	}

	errno = EEXIST;
	return -1;
}

EXPORT int initstate_r(
	unsigned int seed,
	char* __restrict state_buf,
	size_t state_len,
	random_data* __restrict buf) {
	if (state_len < 8) {
		errno = EINVAL;
		return -1;
	}

	if (state_len > UINT32_MAX) {
		state_len = UINT32_MAX;
	}

	*buf = {};

	buf->state = reinterpret_cast<int32_t*>(state_buf);
	buf->state[0] = static_cast<int32_t>(seed);

	size_t separate_nonce_len = state_len / 4 - 2;
	if (separate_nonce_len > 3) {
		separate_nonce_len = 3;
	}

	buf->state[1] = static_cast<int32_t>(separate_nonce_len);

	for (size_t i = 0; i < separate_nonce_len; ++i) {
		buf->state[2 + i] = 0;
	}

	buf->fptr = &buf->state[2];
	buf->end_ptr = &buf->state[2] + separate_nonce_len;
	return 0;
}

EXPORT int setstate_r(char* state_buf, struct random_data* buf) {
	if (!state_buf || !buf) {
		errno = EINVAL;
		return -1;
	}

	buf->state = reinterpret_cast<int32_t*>(state_buf);
	buf->fptr = &buf->state[2];
	buf->end_ptr = &buf->state[2] + static_cast<uint32_t>(buf->state[1]);
	return 0;
}

EXPORT int srandom_r(unsigned int seed, struct random_data* buf) {
	buf->state[0] = static_cast<int32_t>(seed);
	return 0;
}

EXPORT int random_r(random_data* __restrict buf, int32_t* __restrict result) {
	if (!buf || !result) {
		errno = EINVAL;
		return -1;
	}

	// todo not sure if its a good idea to use the nonce length as part of the key
	uint32_t key[8] {static_cast<uint32_t>(buf->state[0]), static_cast<uint32_t>(buf->state[1])};

	ChaCha20State state {};
	if (buf->end_ptr != buf->fptr) {
		Nonce nonce {};
		for (auto* ptr = buf->fptr; ptr != buf->end_ptr; ++ptr) {
			nonce.inner[ptr - buf->fptr] = static_cast<uint32_t>(*ptr);
		}

		state = cha_cha20_block(key, 0, nonce.inner);

		nonce.use();
		for (auto* ptr = buf->fptr; ptr != buf->end_ptr; ++ptr) {
			*ptr = static_cast<int32_t>(nonce.inner[ptr - buf->fptr]);
		}
	}
	else {
		// todo not sure if its a good idea to use the nonce length as part of the nonce
		Nonce nonce {static_cast<uint32_t>(buf->state[0]), static_cast<uint32_t>(buf->state[1])};

		state = cha_cha20_block(key, 0, nonce.inner);

		nonce.use();
		buf->state[0] = static_cast<int32_t>(nonce.inner[0]);
	}

	*result = static_cast<int32_t>(state.state[0] % RAND_MAX);
	return 0;
}

EXPORT void qsort_r(
	void* ptr,
	size_t count,
	size_t size,
	int (*comp)(const void* a, const void* b, void* arg),
	void* arg) {
	for (size_t i = 0; i < count; ++i) {
		bool changed = false;
		for (size_t j = 0; j < count - 1; ++j) {
			auto* a = reinterpret_cast<char*>(ptr) + j * size;
			auto* b = reinterpret_cast<char*>(ptr) + (j + 1) * size;
			if (comp(a, b, arg) > 0) {
				for (size_t k = 0; k < size; ++k) {
					char tmp = a[k];
					a[k] = b[k];
					b[k] = tmp;
				}
				changed = true;
			}
		}

		if (!changed) {
			break;
		}
	}
}

#if defined(__x86_64__) || defined(__i386__)

EXPORT int strfromf128(
	char* __restrict buf,
	size_t buf_size,
	const char* __restrict fmt,
	__float128 value) {
	__ensure(!"strfromf128 is not implemented");
}

EXPORT __float128 strtof128(const char* __restrict str, char** __restrict end_ptr) {
	__ensure(!"strtof128 is not implemented");
}

#elif defined(__aarch64__)
#else
#error missing architecture specific code
#endif
