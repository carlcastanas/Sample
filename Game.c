#define _CRT_SECURE_NO_WARNINGS

#include "Player.h"
#include "Game.h"
#include "Property.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
	Game_STATE_BEGIN_MOVE,
	Game_STATE_BUY_PROPERTY,
	Game_STATE_PLAYER_IN_DEBT,
	Game_STATE_END
} Game_State;

static const int TOTAL_PLAYERS = 2;
static int current_move_player = 0;
static Game_Player Game_players[2];
static Game_Prop gProperties[40];
static Game_State gState;
static int lastRoll[2];
static int selectedProperty;
static char * message;
static char * message2;

char* Game_getFormattedStatus(int player) {
	char* buffer = (char *)malloc(sizeof(char) * 20);
	if (current_move_player == player)
		sprintf(buffer, "%1s%-7s %4i$", ">",Game_players[player].name, Game_players[player].money);
	else
		sprintf(buffer, "%1s%-7s %4i$", "",Game_players[player].name, Game_players[player].money);
	return buffer;
}

static void updateMessage2(char * newMessage) {
	free(message2);
	message2 = newMessage;
}

static void Game_property_init() {
	int i;
	for (i = 0; i < 40; i++) {
		gProperties[i].id = i;
		gProperties[i].mortaged = 0;
		gProperties[i].owner = -1;
		gProperties[i].upgrades = 0;
		gProperties[i].price = 0;
	}

	gProperties[0].type = Game_PT_GO;

	gProperties[1].type = Game_PT_PURPLE;
	gProperties[1].price = 60;
	gProperties[1].rentCost[0] = 2;
	gProperties[1].rentCost[1] = 10;
	gProperties[1].rentCost[2] = 30;
	gProperties[1].rentCost[3] = 90;
	gProperties[1].rentCost[4] = 160;
	gProperties[1].rentCost[5] = 250;
	gProperties[1].upgradeCost = 50;

	gProperties[2].type = Game_PT_COMMUNITY_CHEST;
	gProperties[17].type = Game_PT_COMMUNITY_CHEST;
	gProperties[33].type = Game_PT_COMMUNITY_CHEST;

	gProperties[3].type = Game_PT_PURPLE;
	gProperties[3].price = 60;
	gProperties[3].rentCost[0] = 4;
	gProperties[3].rentCost[1] = 20;
	gProperties[3].rentCost[2] = 60;
	gProperties[3].rentCost[3] = 180;
	gProperties[3].rentCost[4] = 320;
	gProperties[3].rentCost[5] = 450;
	gProperties[3].upgradeCost = 50;

	gProperties[4].type = Game_PT_TAX_INCOME;

	for (i = 5; i < 40; i+= 10) {
		gProperties[i].type = Game_PT_RAILROAD;
		gProperties[i].price = 200;
	}

	for (i = 6; i <= 8; i+=2) {
		gProperties[i].type = Game_PT_LIGHTBLUE;
		gProperties[i].price = 100;
		gProperties[i].rentCost[0] = 6;
		gProperties[i].rentCost[1] = 30;
		gProperties[i].rentCost[2] = 90;
		gProperties[i].rentCost[3] = 270;
		gProperties[i].rentCost[4] = 400;
		gProperties[i].rentCost[5] = 550;
		gProperties[i].upgradeCost = 50;
	}

	gProperties[7].type = Game_PT_CHANCE;
	gProperties[22].type = Game_PT_CHANCE;
	gProperties[36].type = Game_PT_CHANCE;

	gProperties[9].type = Game_PT_LIGHTBLUE;
	gProperties[9].price = 100;
	gProperties[9].rentCost[0] = 8;
	gProperties[9].rentCost[1] = 40;
	gProperties[9].rentCost[2] = 100;
	gProperties[9].rentCost[3] = 300;
	gProperties[9].rentCost[4] = 450;
	gProperties[9].rentCost[5] = 600;
	gProperties[9].upgradeCost = 50;

	gProperties[10].type = Game_PT_JAIL;

	for (i = 11; i <= 13; i+=2) {
		gProperties[i].type = Game_PT_MAGNETA;
		gProperties[i].price = 140;
		gProperties[i].rentCost[0] = 10;
		gProperties[i].rentCost[1] = 50;
		gProperties[i].rentCost[2] = 150;
		gProperties[i].rentCost[3] = 450;
		gProperties[i].rentCost[4] = 625;
		gProperties[i].rentCost[5] = 750;
		gProperties[i].upgradeCost = 100;
	}

	for (i = 12; i <= 28; i+= 16) {
		gProperties[i].type = Game_PT_UTILITY;
		gProperties[i].price = 150;
	}

	gProperties[14].type = Game_PT_MAGNETA;
	gProperties[14].price = 160;
	gProperties[14].rentCost[0] = 12;
	gProperties[14].rentCost[1] = 60;
	gProperties[14].rentCost[2] = 180;
	gProperties[14].rentCost[3] = 500;
	gProperties[14].rentCost[4] = 700;
	gProperties[14].rentCost[5] = 900;
	gProperties[14].upgradeCost = 100;

	for (i = 16; i <= 18; i+=2) {
		gProperties[i].type = Game_PT_ORANGE;
		gProperties[i].price = 180;
		gProperties[i].rentCost[0] = 14;
		gProperties[i].rentCost[1] = 70;
		gProperties[i].rentCost[2] = 200;
		gProperties[i].rentCost[3] = 550;
		gProperties[i].rentCost[4] = 750;
		gProperties[i].rentCost[5] = 950;
		gProperties[i].upgradeCost = 100;
	}

	gProperties[19].type = Game_PT_ORANGE;
	gProperties[19].price = 200;
	gProperties[19].rentCost[0] = 16;
	gProperties[19].rentCost[1] = 80;
	gProperties[19].rentCost[2] = 220;
	gProperties[19].rentCost[3] = 600;
	gProperties[19].rentCost[4] = 800;
	gProperties[19].rentCost[5] = 1000;
	gProperties[19].upgradeCost = 100;

	gProperties[20].type = Game_PT_FREE_PARK;

	for (i = 21; i <= 23; i+=2) {
		gProperties[i].type = Game_PT_RED;
		gProperties[i].price = 220;
		gProperties[i].rentCost[0] = 18;
		gProperties[i].rentCost[1] = 90;
		gProperties[i].rentCost[2] = 250;
		gProperties[i].rentCost[3] = 700;
		gProperties[i].rentCost[4] = 875;
		gProperties[i].rentCost[5] = 1050;
		gProperties[i].upgradeCost = 150;
	}

	gProperties[24].type = Game_PT_RED;
	gProperties[24].price = 240;
	gProperties[24].rentCost[0] = 20;
	gProperties[24].rentCost[1] = 100;
	gProperties[24].rentCost[2] = 300;
	gProperties[24].rentCost[3] = 750;
	gProperties[24].rentCost[4] = 925;
	gProperties[24].rentCost[5] = 1100;
	gProperties[24].upgradeCost = 150;

	for (i = 26; i <= 27; i+=1) {
		gProperties[i].type = Game_PT_YELLOW;
		gProperties[i].price = 260;
		gProperties[i].rentCost[0] = 22;
		gProperties[i].rentCost[1] = 110;
		gProperties[i].rentCost[2] = 330;
		gProperties[i].rentCost[3] = 800;
		gProperties[i].rentCost[4] = 975;
		gProperties[i].rentCost[5] = 1150;
		gProperties[i].upgradeCost = 150;
	}

	gProperties[29].type = Game_PT_YELLOW;
	gProperties[29].price = 280;
	gProperties[29].rentCost[0] = 24;
	gProperties[29].rentCost[1] = 120;
	gProperties[29].rentCost[2] = 360;
	gProperties[29].rentCost[3] = 850;
	gProperties[29].rentCost[4] = 1025;
	gProperties[29].rentCost[5] = 1200;
	gProperties[29].upgradeCost = 150;

	gProperties[30].type = Game_PT_GOTO_JAIL;

	for (i = 31; i <= 32; i+=1) {
		gProperties[i].type = Game_PT_GREEN;
		gProperties[i].price = 300;
		gProperties[i].rentCost[0] = 26;
		gProperties[i].rentCost[1] = 130;
		gProperties[i].rentCost[2] = 390;
		gProperties[i].rentCost[3] = 900;
		gProperties[i].rentCost[4] = 1100;
		gProperties[i].rentCost[5] = 1275;
		gProperties[i].upgradeCost = 200;
	}

	gProperties[34].type = Game_PT_GREEN;
	gProperties[34].price = 320;
	gProperties[34].rentCost[0] = 28;
	gProperties[34].rentCost[1] = 150;
	gProperties[34].rentCost[2] = 450;
	gProperties[34].rentCost[3] = 1000;
	gProperties[34].rentCost[4] = 1200;
	gProperties[34].rentCost[5] = 1400;
	gProperties[34].upgradeCost = 200;

	gProperties[37].type = Game_PT_BLUE;
	gProperties[37].price = 350;
	gProperties[37].rentCost[0] = 35;
	gProperties[37].rentCost[1] = 175;
	gProperties[37].rentCost[2] = 500;
	gProperties[37].rentCost[3] = 1100;
	gProperties[37].rentCost[4] = 1300;
	gProperties[37].rentCost[5] = 1500;
	gProperties[37].upgradeCost = 200;

	gProperties[38].type = Game_PT_TAX_LUXURY;

	/***********$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*******************/

	gProperties[39].type = Game_PT_BLUE;
	gProperties[39].price = 400;
	gProperties[39].rentCost[0] = 50;
	gProperties[39].rentCost[1] = 200;
	gProperties[39].rentCost[2] = 600;
	gProperties[39].rentCost[3] = 1400;
	gProperties[39].rentCost[4] = 1700;
	gProperties[39].rentCost[5] = 2000;
	gProperties[39].upgradeCost = 200;
}

void Game_init() {
	

	int i;

	message = (char *)malloc(sizeof(char) * 40);
	message = "";
	message2 = (char*)malloc(2);
	sprintf(message2, "");
	Game_property_init();

	srand(time(NULL));
	current_move_player = 0;
	for (i = 0; i < TOTAL_PLAYERS; i++) {
		Game_players[i].id = i;
		Game_players[i].money = 1500;
		Game_players[i].position = 0;
	}

	(Game_players[0]).name = "Red";
	(Game_players[1]).name = "Blue";

	gState = Game_STATE_BEGIN_MOVE;
}

static void nextPlayer() {
	if (Game_STATE_PLAYER_IN_DEBT != gState)
		current_move_player = (current_move_player + 1) % TOTAL_PLAYERS;
}

static void Game_process_debts() {
	if (Game_STATE_PLAYER_IN_DEBT != gState)
		return;

	message = "DEBTS! Sell something";

	if (Game_players[current_move_player].money >= 0) {
		gState = Game_STATE_BEGIN_MOVE;
		nextPlayer();
		message = "";
	}	
}

static void Game_pay_player(int payer, int receiever, int sum) {
	char * buffer = (char *) malloc(sizeof(char) * 30);
	sprintf(buffer, "%s paid %s $%i", Game_players[payer].name, Game_players[receiever].name, sum);
	updateMessage2(buffer);

	Game_players[payer].money -= sum;
	Game_players[receiever].money += sum;
	if (Game_players[payer].money < 0) {
		gState = Game_STATE_PLAYER_IN_DEBT;	
		Game_process_debts();
	}

}

static void Game_remove_money(int player, int amount) {
	char * buffer = (char *) malloc(sizeof(char) * 30);
	sprintf(buffer, "%s paid $%i", Game_players[player].name, amount);
	updateMessage2(buffer);
	
	
	Game_players[player].money -= amount;

	if (Game_players[player].money < 0) {
		gState = Game_STATE_PLAYER_IN_DEBT;		
		Game_process_debts();
	}
}

static void Game_roll_dice() {
	int i;
	for (i = 0; i < 2; i++)
		lastRoll[i] = rand() % 6 + 1;	
}

static int Game_isPlayerAMonopolist(int player, Game_Prop_Type type);

static void Game_player_land(int newposition) {
	/* if a buyable property */
	if (gProperties[newposition].price > 0 && gProperties[newposition].mortaged == 0) {
		if (gProperties[newposition].owner == -1) {
			gState = Game_STATE_BUY_PROPERTY;
		}
		else if (gProperties[newposition].owner != current_move_player) {
			if (gProperties[newposition].type != Game_PT_UTILITY &&
				gProperties[newposition].type != Game_PT_RAILROAD) {
					int mult = 1;
					if (Game_isPlayerAMonopolist(gProperties[newposition].owner, gProperties[newposition].type)
						&& gProperties[newposition].upgrades == 0)
						mult = 2;
				Game_pay_player(current_move_player, gProperties[newposition].owner,
					mult * gProperties[newposition].rentCost[gProperties[newposition].upgrades]);
			}
			if (gProperties[newposition].type == Game_PT_RAILROAD) {
				int owner = gProperties[newposition].owner;
				int i, m = 1;
				for (i = 5; i <= 35; i+=10) {
					if (gProperties[i].owner == owner && gProperties[i].mortaged == 0)
						m *= 2;
    			}
				Game_pay_player(current_move_player, owner, 25 * m / 2);
			}
			if (gProperties[newposition].type == Game_PT_UTILITY) {
				int owner = gProperties[newposition].owner;
				int i, m = 0;
				for (i = 12; i <= 38; i+=16) {
					if (gProperties[i].owner == owner && gProperties[i].mortaged == 0)
						m += 1;
    			}

				i = (1 == m ? 4 : 10) * (lastRoll[0] + lastRoll[1]);
				Game_pay_player(current_move_player, owner, i);
			}
			
		}
	}
	else {
		switch (gProperties[newposition].type)
		{
		case Game_PT_GO:
			break;
			Game_players[current_move_player].money += 200;
			break;
		case Game_PT_TAX_INCOME:
			Game_remove_money(current_move_player, 200);			
			break;
		case Game_PT_TAX_LUXURY:
			Game_remove_money(current_move_player, 75);			
			break;
		default:
			break;
		}
	}
}

void Game_cycle() {
	int roll;
	Game_roll_dice();
	roll = lastRoll[0] + lastRoll[1];
	if (Game_players[current_move_player].position + roll >= 40)
		Game_players[current_move_player].money += 200;
	Game_players[current_move_player].position = (Game_players[current_move_player].position + roll) % 40;
	Game_player_land(Game_players[current_move_player].position);

	if (gState == Game_STATE_BEGIN_MOVE && lastRoll[0] != lastRoll[1])
		nextPlayer();
}

void Game_buyProperty() {
	int newposition = Game_getPlayerPosition(current_move_player);
	if (gProperties[newposition].price > 0) {
		if (gProperties[newposition].owner == -1) {
			gState = Game_STATE_BEGIN_MOVE;
			Game_remove_money(current_move_player, gProperties[newposition].price);
			gProperties[newposition].owner = current_move_player;
		}
	}
}

int Game_getTotalPlayers() {
	return TOTAL_PLAYERS;
}

int Game_getPlayerPosition(int playerid) {
	return Game_players[playerid].position;
}

int Game_getPropOwner(int propid) {
	if (gProperties[propid].price > 0 ) {
		return gProperties[propid].owner;
	}
	else return -1;
}

static void Game_mortageProp(int propid) {
	if (gProperties[propid].owner == current_move_player) {
		int i;
		if (gProperties[propid].upgrades > 0) {
			message = "Can't mortage with houses";
			return;
		}

		for (i = 0; i < 40; i++) {
			if (gProperties[i].type == gProperties[propid].type) {
				if (gProperties[i].upgrades > 0) {
					message = "Destroy other houses first";
					return;
				}
			}
		}

		if (gProperties[propid].mortaged == 0 && gProperties[propid].upgrades == 0) {
			Game_players[current_move_player].money += gProperties[propid].price / 2;
			gProperties[propid].mortaged = 1;
		}
		else {
			if (Game_players[current_move_player].money >= gProperties[propid].price / 2 * 1.1) {
				gProperties[propid].mortaged = 0;
				Game_remove_money(current_move_player, (int)(gProperties[propid].price / 2 * 1.1));
			}
		}
	}
	else {
		message = "Can't mortage that";
	}
}

static int Game_isPlayerAMonopolist(int player, Game_Prop_Type type) {
	int result = 1;
	int i;

	if (Game_PT_UTILITY == type || Game_PT_RAILROAD == type)
		return 0;

	for (i = 0; i < 40; i++) {
		if (gProperties[i].type == type)
			if (gProperties[i].owner != player) {
				return 0;
			}
	}

	return result;
}

static int Game_isLegitUpgrade(int player, int propid) {
	int highest_upgrade = 0;
	int lowest_upgrade = 5;
	int i;
	int this_upgrade = gProperties[propid].upgrades;
	Game_Prop_Type p_type = gProperties[propid].type;

	if (Game_isPlayerAMonopolist(player, p_type)) {
		for (i = 0; i < 40; i++) {
			if (gProperties[i].type == p_type) {
				if (gProperties[i].mortaged == 1)
					return 0;
				highest_upgrade = gProperties[i].upgrades > highest_upgrade ?
				gProperties[i].upgrades : highest_upgrade;
				lowest_upgrade = gProperties[i].upgrades < lowest_upgrade ?
				gProperties[i].upgrades : lowest_upgrade;
			}
		}
	}

	if (this_upgrade == lowest_upgrade)
		return 1;
	if (highest_upgrade == lowest_upgrade)
		return 1;
	return 0;
}

static int Game_isLegitDowngrade(int player, int propid) {
	int highest_upgrade = 0;
	int lowest_upgrade = 5;
	int i;
	int this_upgrade = gProperties[propid].upgrades;
	Game_Prop_Type p_type = gProperties[propid].type;

	if (Game_isPlayerAMonopolist(player, p_type)) {
		for (i = 0; i < 40; i++) {
			if (gProperties[i].type == p_type) {
				if (gProperties[i].mortaged == 1)
					return 0;
				highest_upgrade = gProperties[i].upgrades > highest_upgrade ?
				gProperties[i].upgrades : highest_upgrade;
				lowest_upgrade = gProperties[i].upgrades < lowest_upgrade ?
				gProperties[i].upgrades : lowest_upgrade;
			}
		}
	}

	if (this_upgrade == highest_upgrade)
		return 1;
	if (highest_upgrade == lowest_upgrade)
		return 1;
	return 0;
}

static int Game_upgradeProp(int propid, int flag) {
	/*upgrade*/
	if (flag >= 0) { 
		if (gProperties[propid].owner == current_move_player && gProperties[propid].mortaged == 0
			&& gProperties[propid].upgrades < 5) {

				if (Game_isLegitUpgrade(current_move_player, propid)) {
					if (Game_players[current_move_player].money >= gProperties[propid].upgradeCost) {
						gProperties[propid].upgrades++;
						Game_remove_money(current_move_player, gProperties[propid].upgradeCost);
					}
					else {
						message = "Not enough $";
					}
				}
				else {
					message = "Not allowed to build";
				}
		}
		else
			message = "Can't build there";
	}
	/*downgrade*/
	else { 
		if (gProperties[propid].owner == current_move_player && gProperties[propid].mortaged == 0
			&& gProperties[propid].upgrades > 0 && Game_isLegitDowngrade(current_move_player, propid)) {
				gProperties[propid].upgrades--;
				Game_players[current_move_player].money += gProperties[propid].upgradeCost / 2;
		}
		else {
			message = "Can't destroy there";
		}
	}
}

static int Game_goBankrupt() {
	if (Game_players[current_move_player].money < 0) {
		char * f = (char *) malloc(sizeof(char) * 40);
		int t = (current_move_player + 1) % TOTAL_PLAYERS;
		sprintf(f, "%s lost! %s won!", Game_players[current_move_player].name, Game_players[t].name);
		message = f;
		gState = Game_STATE_END;
	}
}

void Game_receiveinput(SDL_Keycode key) {
	if (key == SDLK_SPACE) {
		message = "";
		switch (gState) {
		case Game_STATE_BEGIN_MOVE:
			Game_cycle();
			break;
		case Game_STATE_BUY_PROPERTY:
			Game_buyProperty();						
			if (lastRoll[0] != lastRoll[1])		
				nextPlayer();			
			break;
		case Game_STATE_PLAYER_IN_DEBT:
			Game_process_debts();
			break;
		default:
			break;
		}
	}
	if (key == SDLK_m) {
		if (selectedProperty >= 0 && selectedProperty < 40) {
			Game_mortageProp(selectedProperty);
		}
	}
	if (key == SDLK_x) {
		Game_goBankrupt();
	}
	if (key == SDLK_b) {
		Game_upgradeProp(selectedProperty, 1);
	}
	if (key == SDLK_d) {
		Game_upgradeProp(selectedProperty, -1);
	}
}

void Game_getLastRoll(int* a, int* b) {
	*a = lastRoll[0];
	*b = lastRoll[1];
}

char * Game_getText(int line) {
	if (Game_STATE_END == gState) {
		if (line == 0)
			return message;
		else
			return NULL;
	}

	switch(line) {
	case 4:
		if (Game_STATE_BEGIN_MOVE == gState)
			return "SPACE) Roll and jump";
		if (Game_STATE_BUY_PROPERTY == gState)
			return "SPACE) Buy property";
		if (Game_players[current_move_player].money < 0)
			return "    X) Go Bankrupt";
		return "";
		break;
	case 1:
		return "    M) Mortage";
		break;
	case 2:
		return "    B) Build";
		break;
	case 3:
		return "    D) Destroy";
		break;
	case 5:
		return message2;
		break;
	case 0:
		return message;
		break;
	}

	return NULL;
}

void Game_selectProperty(int propid) {
	if (propid > 0 && propid < 40)
		selectedProperty = propid;
}

int Game_getPropLevel(int id) {
	return gProperties[id].upgrades;
}

int Game_getPropMortageStatus(int id) {
	return gProperties[id].mortaged;
}