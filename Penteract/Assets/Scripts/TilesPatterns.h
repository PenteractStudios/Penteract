#pragma once

#include <cstdarg>

#define CORRIDOR_PATTERNS 11
#define CORRIDOR_TILES 28
#define ARENA_TILES 32
#define BOSS_TILES 1

class TilesPattern{
public:
	TilesPattern(int size, ...) {
		pattern = new bool[size];
		va_list args;
		va_start(args, size);
		for (int i = 0; i < size; ++i) {
			pattern[i] = va_arg(args, bool);
		}
		va_end(args);
	}

public:
	bool* pattern;
};

std::vector<TilesPattern> corridorPatterns = {	/*N� 1*/TilesPattern(CORRIDOR_TILES, false, false, true, true, true, true, true, true, true, true, true, true, true, true,
																					 true, false, true, false, false, false, true, true, true, true, true, true, true, true),
												/*N� 2*/TilesPattern(CORRIDOR_TILES, false, false, true, false, true, false, true, true, true, true, true, true, true, true,
																					 true, true, true, true, false, false, false, true, false, true, true, true, true, true),
												/*N� 3*/TilesPattern(CORRIDOR_TILES, true, true, true, false, false, false, false, true, true, true, true, true, true, true,
																					 true, true, true, true, true, true, false, true, false, false, true, false, true, true),
												/*N� 4*/TilesPattern(CORRIDOR_TILES, true, true, true, true, false, true, false, true, false, true, false, true, true, true,
																					 true, true, true, true, true, true, true, true, true, false, true, false, false, false),
												/*N� 5*/TilesPattern(CORRIDOR_TILES, true, true, true, true, true, true, true, true, false, true, false, true, false, false,
																					 true, true, true, true, true, true, true, true, true, true, false, true, false, true),
												/*N� 6*/TilesPattern(CORRIDOR_TILES, false, false, true, true, true, true, true, true, true, false, true, false, true, false,
																					 true, false, true, false, true, true, true, true, true, true, true, true, true, true),
												/*N� 7*/TilesPattern(CORRIDOR_TILES, false, false, true, false, true, false, true, true, true, true, true, true, true, true,
																					 true, false, true, false, false, false, true, true, true, true, true, true, true, true),
												/*N� 8*/TilesPattern(CORRIDOR_TILES, true, true, true, false, false, false, false, true, true, true, true, true, true, true,
																					 true, true, true, true, false, false, false, true, false, true, true, true, true, true),
												/*N� 9*/TilesPattern(CORRIDOR_TILES, true, true, true, true, false, true, false, true, false, true, false, true, true, true,
																					 true, true, true, true, true, true, false, true, false, false, true, false, true, true),
												/*N� 10*/TilesPattern(CORRIDOR_TILES, true, true, true, true, true, true, true, true, false, true, false, true, false, false,
																					  true, true, true, true, true, true, true, true, true, false, true, false, false, false),
												/*N� 11*/TilesPattern(CORRIDOR_TILES, true, true, true, true, true, true, true, true, true, false, true, false, true, false,
																					  true, false, true, false, true, true, true, true, true, true, false, true, false, true) };

TilesPattern arenaPattern1 = TilesPattern(ARENA_TILES, true, true, true, true, true, true, true, true, true, true,
									true, true, true, true, true, true, true, true, true, true,
									true, true, true, true, false, false, false, false, false, false,
									false, false );
TilesPattern arenaPattern2 = TilesPattern(ARENA_TILES, false, false, false, false, false, false, false, false, false, false,
									false, false, false, false, false, false, false, false, false, false,
									false, false, false, false, true, true, true, true, true, true,
									true, true );
TilesPattern arenaPattern3 = TilesPattern(ARENA_TILES, true, true, true, false, true, true, true, true, true, false,
									true, true, true, true, true, false, true, true, true, true,
									true, false, true, true, true, false, true, false, true, false,
									true, false );

TilesPattern bossPattern1 = TilesPattern(BOSS_TILES, true);
TilesPattern bossPattern2 = TilesPattern(BOSS_TILES, true);
TilesPattern bossPattern3 = TilesPattern(BOSS_TILES, true);