#include <common.h>
#include <game.h>

#define EA 1
#define EUA 0

struct Random {
	u64 eventFlag;
	u32 settings;
};

void RandomCreate(Random gain);
void RandomExecute(Random gain);
void RandomEnd(Random gain);
void RandomUpdate(Random gain);