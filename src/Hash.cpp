#include "Hash.hpp"

#define mmix(h, k) do { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; } while (0)

uint64_t MurMur(const void *data, size_t length, uint64_t seed)
{
	const uint64_t m = 0x5bd1e9955bd1e995;
	const uint64_t r = 24;
	uint64_t       l = length;

	const unsigned char* d = (const unsigned char*)data;

	uint64_t h = seed;

	while(length >= 8)
	{
		uint64_t k = *(uint64_t*)d;

		mmix(h, k);

		d += 8;
		length -= 8;
	}

	uint64_t t = 0;

	switch(length)
	{
	case 7:
		t ^= (static_cast<uint64_t>(d[6]) << 48);
		break;
	case 6:
		t ^= (static_cast<uint64_t>(d[5]) << 40);
		break;
	case 5:
		t ^= (static_cast<uint64_t>(d[4]) << 32);
		break;
	case 4:
		t ^= (d[3] << 24);
		break;
	case 3:
		t ^= (d[2] << 16);
		break;
	case 2:
		t ^= (d[1] << 8);
		break;
	case 1:
		t ^= d[0];
		break;
	default:
		break;
	};

	mmix(h, t);
	mmix(h, l);

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
