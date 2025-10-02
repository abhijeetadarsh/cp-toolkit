#pragma once

#include <ext/pb_ds/assoc_container.hpp>

struct chash
{
    static uint64_t splitmix64(uint64_t x)
    {
        // http://xorshift.di.unimi.it/splitmix64.c
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    size_t operator()(uint64_t x) const
    {
        static const uint64_t FIXED_RANDOM = std::chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + FIXED_RANDOM);
    }
};

template <typename U, typename V>
using HashTable = __gnu_pbds::gp_hash_table<U, V, chash>;

/*
int main()
{
    HashTable<int, int> mp;

    mp[1]++;
    mp[2]++;
    mp[1]++;

    if (mp[5] != 0)
    {
    }

    for (auto [val, freq] : mp)
    {
        printf("%d %d\n", val, freq);
    }
    printf("\n");

    HashTable<int, int> mp2({}, {}, {}, {}, {1 << 13});
    for (int i = 0; i < 10; ++i) {
		// initialize with certain capacity, must be power of 2
        mp2[i]++;
	}

    for (auto [val, freq] : mp2)
    {
        printf("%d %d\n", val, freq);
    }
}
*/