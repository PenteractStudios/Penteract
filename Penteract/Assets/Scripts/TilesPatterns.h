#pragma once

#define CORRIDOR_PATTERNS 11
#define CORRIDOR_TILES 28
#define ARENA_TILES 32
#define BOSS_TILES 1

bool corridorPatterns[CORRIDOR_PATTERNS][CORRIDOR_TILES] = { /*N� 1*/{	false, false, true, true, true, true, true, true, true, true, true, true, true, true,
																		true, false, true, false, false, false, true, true, true, true, true, true, true, true},
															 /*N� 2*/{	false, false, true, false, true, false, true, true, true, true, true, true, true, true,
																		true, true, true, true, false, false, false, true, false, true, true, true, true, true},
															 /*N� 3*/{	true, true, true, false, false, false, false, true, true, true, true, true, true, true,
																		true, true, true, true, true, true, false, true, false, false, true, false, true, true},
															 /*N� 4*/{	true, true, true, true, false, true, false, true, false, true, false, true, true, true,
																		true, true, true, true, true, true, true, true, true, false, true, false, false, false},
															 /*N� 5*/{	true, true, true, true, true, true, true, true, false, true, false, true, false, false,
																		true, true, true, true, true, true, true, true, true, true, false, true, false, true},
															 /*N� 6*/{	false, false, true, true, true, true, true, true, true, false, true, false, true, false,
																		true, false, true, false, true, true, true, true, true, true, true, true, true, true},
															 /*N� 7*/{	false, false, true, false, true, false, true, true, true, true, true, true, true, true,
																		true, false, true, false, false, false, true, true, true, true, true, true, true, true},
															 /*N� 8*/{	true, true, true, false, false, false, false, true, true, true, true, true, true, true,
																		true, true, true, true, false, false, false, true, false, true, true, true, true, true},
															 /*N� 9*/{	true, true, true, true, false, true, false, true, false, true, false, true, true, true,
																		true, true, true, true, true, true, false, true, false, false, true, false, true, true},
															 /*N� 10*/{	true, true, true, true, true, true, true, true, false, true, false, true, false, false,
																		true, true, true, true, true, true, true, true, true, false, true, false, false, false},
															 /*N� 11*/{	true, true, true, true, true, true, true, true, true, false, true, false, true, false,
																		true, false, true, false, true, true, true, true, true, true, false, true, false, true} };

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