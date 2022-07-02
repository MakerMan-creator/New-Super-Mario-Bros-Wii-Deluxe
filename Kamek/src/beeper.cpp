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
		RED = 87,
		RED_LINE = 88,
	};

	daPlBase_c *players[4];
	Remocon *control[4];

	TILES mode;
	int block;
	int time = 10;
	bool reset = true;
	int driver;
	u8 event;

	static daBeeper* build();

	USING_STATES(daBeeper);
	DECLARE_STATE(Active);
	DECLARE_STATE(NotActive);
};

CREATE_STATE(daBeeper, Active);
CREATE_STATE(daBeeper, NotActive);

daBeeper* daBeeper::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBeeper));
	return new(buffer) daBeeper;
}

int times = 0;

int daBeeper::onCreate() {
    blockInit(pos.y);

    physicsInfo.x1 = -8;
    physicsInfo.y1 = 16;
    physicsInfo.x2 = 8;
    physicsInfo.y2 = 0;

    physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
    physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
    physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

    physics.setup(this, &physicsInfo, 3, currentLayerID);
    physics.flagsMaybe = 0x260;
    physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
    physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
    physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;

    this->block = ((this->settings & 0x000F0000) >> 16) % 2; // Nybble 8
	this->event = (this->settings >> 28 & 0xF) - 1; // Nybble 5

    if (block == 0) {
	    physics.addToList();
    }

	if (block == 0) { 
		if (dFlagMgr_c::instance->active(event)) { 
			doStateChange(&StateID_Active); 
		} else {
			doStateChange(&StateID_NotActive);
		}
	}
    else { 
		if (dFlagMgr_c::instance->active(event)) { 
			doStateChange(&StateID_NotActive); 
		} else {
			doStateChange(&StateID_Active);
		} 
	}

    this->mode = (TILES)((block == 0) ? 83 : 88);

    TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->add(&tile);

	tile.x = pos.x - 8;
	tile.y = -(16 + pos.y);
	tile.tileNumber = (u16)mode;

	this->onExecute();

	return true;
}

int daBeeper::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	if (block == 0) { 
		if (dFlagMgr_c::instance->active(event)) { 
			doStateChange(&StateID_Active); 
		} else {
			doStateChange(&StateID_NotActive);
		}
	}
    else { 
		if (dFlagMgr_c::instance->active(event)) { 
			doStateChange(&StateID_NotActive); 
		} else {
			doStateChange(&StateID_Active);
		} 
	}

	return true;
}

int daBeeper::onDelete() {
	return true;
}

int daBeeper::onDraw() {
	return true;
}

void daBeeper::beginState_Active() {
    tile.tileNumber = (block == 0) ? 84 : 88;
}
void daBeeper::executeState_Active() {
    
}
void daBeeper::endState_Active() {
    physics.removeFromList();
}

void daBeeper::beginState_NotActive() {
    tile.tileNumber = (block == 0) ? 83 : 87;
}
void daBeeper::executeState_NotActive() {
    

}
void daBeeper::endState_NotActive() {
    physics.addToList();
}