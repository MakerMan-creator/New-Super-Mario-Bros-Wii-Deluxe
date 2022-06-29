#ifndef BEEPERFULL_H_INCLUDED
#define BEEPERFULL_H_INCLUDED

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

class daBeeper : public daEnBlockMain_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	TileRenderer tile;
	Physics::Info physicsInfo;

	enum TILES {
	    BLUE = 83,
	    BLUE_LINE = 84,
	};

	daPlBase_c *players[4];
	Remocon *control[4];

	TILES mode;
	int block;
	long int time = 150;
	long int t2 = 60;
	int beeps = 3;
	int timeplus;
	bool reset = true;
	int id;
	int driver;
	int spintime = 45;

	static daBeeper* build();

	USING_STATES(daBeeper);
	DECLARE_STATE(Active);
	DECLARE_STATE(NotActive);
};

#endif // BEEPERFULL_H_INCLUDED
