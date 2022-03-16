#pragma once
#include <SDL.h>

char* Game_getFormattedStatus(int player);

void Game_init();

void Game_cycle();

int Game_getTotalPlayers();

int Game_getPlayerPosition(int playerid);

int Game_getPropOwner(int propid);

void Game_receiveinput(SDL_Keycode);

void Game_getLastRoll(int* a, int* b);

char * Game_getText(int line);

int Game_getPropLevel(int);

int Game_getPropMortageStatus(int);

void Game_selectProperty(int propid);