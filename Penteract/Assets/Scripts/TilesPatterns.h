#pragma once

#define CORRIDOR_TILES 28
#define ARENA_TILES 32
#define BOSS_TILES 1

bool corridorPattern1[CORRIDOR_TILES] = { true, false, true, false, true, false, false, false, false, true,
												false, true, false, true, false, false, true, false, true, false,
												true, false, false, false, false, true, false, true };
bool corridorPattern2[CORRIDOR_TILES] = { false, false, false, false, true, true, true, true, false, false,
											false, false, true, true, true, true, false, false, false, false,
											true, true, true, true, false, false, false, false };
bool corridorPattern3[CORRIDOR_TILES] = { true, true, true, true, false, false, false, false, true, true,
											true, true, false, false, false, false, true, true, true, true,
											false, false, false, false, true, true, true, true };

bool arenaPattern1[ARENA_TILES] = { true, true, true, true, true, true, true, true, true, true,
									true, true, true, true, true, true, true, true, true, true,
									true, true, true, true, false, false, false, false, false, false,
									false, false };
bool arenaPattern2[ARENA_TILES] = { false, false, false, false, false, false, false, false, false, false,
									false, false, false, false, false, false, false, false, false, false,
									false, false, false, false, true, true, true, true, true, true,
									true, true };
bool arenaPattern3[ARENA_TILES] = { true, true, true, false, true, true, true, true, true, false,
									true, true, true, true, true, false, true, true, true, true,
									true, false, true, true, true, false, true, false, true, false,
									true, false };

bool bossPattern1[BOSS_TILES] = { true };
bool bossPattern2[BOSS_TILES] = { true };
bool bossPattern3[BOSS_TILES] = { true };