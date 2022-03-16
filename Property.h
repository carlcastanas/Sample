typedef enum {
	Game_PT_GO,
	Game_PT_PURPLE,
	Game_PT_COMMUNITY_CHEST,
	Game_PT_TAX_INCOME,
	Game_PT_RAILROAD,
	Game_PT_LIGHTBLUE,
	Game_PT_CHANCE,
	Game_PT_JAIL,
	Game_PT_MAGNETA,
	Game_PT_UTILITY,
	Game_PT_ORANGE,
	Game_PT_FREE_PARK,
	Game_PT_RED,
	Game_PT_YELLOW,
	Game_PT_GOTO_JAIL,
	Game_PT_GREEN,
	Game_PT_BLUE,
	Game_PT_TAX_LUXURY
} Game_Prop_Type;

typedef struct {
	Game_Prop_Type type;
	int id;
	int owner;
	int mortaged;
	int upgrades;
	int price;
	int upgradeCost;
	int rentCost[6];
} Game_Prop;