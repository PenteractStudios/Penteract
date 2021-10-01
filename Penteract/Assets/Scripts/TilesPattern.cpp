#include "TilesPatterns.h"

const TilesPattern corridorPatterns[CORRIDOR_PATTERNS] = {	/*Nº 1*/TilesPattern(CORRIDOR_TILES, false, false, true, true, true, true, true, true, true, true, true, true, true, true,
																					 true, false, true, false, false, false, true, true, true, true, true, true, true, true),
	/*Nº 2*/TilesPattern(CORRIDOR_TILES, false, false, true, false, true, false, true, true, true, true, true, true, true, true,
										 true, true, true, true, false, false, false, true, false, true, true, true, true, true),
	/*Nº 3*/TilesPattern(CORRIDOR_TILES, true, true, true, false, false, false, false, true, true, true, true, true, true, true,
										 true, true, true, true, true, true, false, true, false, false, true, false, true, true),
	/*Nº 4*/TilesPattern(CORRIDOR_TILES, true, true, true, true, false, true, false, true, false, true, false, true, true, true,
										 true, true, true, true, true, true, true, true, true, false, true, false, false, false),
	/*Nº 5*/TilesPattern(CORRIDOR_TILES, true, true, true, true, true, true, true, true, false, true, false, true, false, false,
										 true, true, true, true, true, true, true, true, true, true, false, true, false, true),
	/*Nº 6*/TilesPattern(CORRIDOR_TILES, false, false, true, true, true, true, true, true, true, false, true, false, true, false,
										 true, false, true, false, true, true, true, true, true, true, true, true, true, true),
	/*Nº 7*/TilesPattern(CORRIDOR_TILES, false, false, true, false, true, false, true, true, true, true, true, true, true, true,
										 true, false, true, false, false, false, true, true, true, true, true, true, true, true),
	/*Nº 8*/TilesPattern(CORRIDOR_TILES, true, true, true, false, false, false, false, true, true, true, true, true, true, true,
										 true, true, true, true, false, false, false, true, false, true, true, true, true, true),
	/*Nº 9*/TilesPattern(CORRIDOR_TILES, true, true, true, true, false, true, false, true, false, true, false, true, true, true,
										 true, true, true, true, true, true, false, true, false, false, true, false, true, true),
	/*Nº 10*/TilesPattern(CORRIDOR_TILES, true, true, true, true, true, true, true, true, false, true, false, true, false, false,
										  true, true, true, true, true, true, true, true, true, false, true, false, false, false),
	/*Nº 11*/TilesPattern(CORRIDOR_TILES, true, true, true, true, true, true, true, true, true, false, true, false, true, false,
										  true, false, true, false, true, true, true, true, true, true, false, true, false, true) };

const TilesPattern arenaPattern1 = TilesPattern(ARENA_TILES, true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, false, false, false, false, false, false,
	false, false);
const TilesPattern arenaPattern2 = TilesPattern(ARENA_TILES, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, true, true, true, true, true, true,
	true, true);
const TilesPattern arenaPattern3 = TilesPattern(ARENA_TILES, true, true, true, false, true, true, true, true, true, false,
	true, true, true, true, true, false, true, true, true, true,
	true, false, true, true, true, false, true, false, true, false,
	true, false);

const TilesPattern bossPattern1 = TilesPattern(BOSS_TILES, true);
const TilesPattern bossPattern2 = TilesPattern(BOSS_TILES, true);
const TilesPattern bossPattern3 = TilesPattern(BOSS_TILES, true);