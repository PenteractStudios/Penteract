#pragma once

#include <cstdarg>

#define CORRIDOR_PATTERNS 11
#define CORRIDOR_TILES 28
#define ARENA_TILES 32
#define BOSS_TILES 1

class TilesPattern{
public:
	TilesPattern(int size, ...) {
		bool* patternExt = new bool[size];
		va_list args;
		va_start(args, size);
		for (int i = 0; i < size; ++i) {
			patternExt[i] = va_arg(args, bool);
		}
		va_end(args);
		pattern = patternExt;
	}

public:
	const bool* pattern;
};

const extern TilesPattern corridorPatterns [CORRIDOR_PATTERNS];

const extern TilesPattern arenaPattern1;
const extern TilesPattern arenaPattern2;
const extern TilesPattern arenaPattern3;

const extern TilesPattern bossPattern1;
const extern TilesPattern bossPattern2;
const extern TilesPattern bossPattern3;