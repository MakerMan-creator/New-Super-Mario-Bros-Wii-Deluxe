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
	    RED_LINE = 88
	};

	daPlBase_c *players[4];
	daBeeper *blocks[99];

	TILES mode;
	int block;
	long int time = 150;
	long int t2 = 60;
	int beeps = 3;
	int timeplus;
	bool reset = true;
	int id;
	int driver;

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
    this->timeplus = (this->settings >> 28 & 0xF) * 10; // Nybble 5
    this->driver = (settings >> 24 & 0xF) % 2; // Nybble 6
    this->id = settings >> 8 & 0xFF; // Nybbles 9 and 10

    if (block == 0) {
	    physics.addToList();
    }

	if (block == 0) { doStateChange(&StateID_Active); }
    else { doStateChange(&StateID_NotActive); }

    this->mode = (TILES)((block == 0) ? 83 : 88);

    TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->add(&tile);

	tile.x = pos.x - 8;
	tile.y = -(16 + pos.y);
	tile.tileNumber = (u16)mode;

	OSReport("0.");

	time += timeplus;

	this->onExecute();

	return true;
}

int daBeeper::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	dStateBase_c *a = acState.getCurrentState();

	if (driver) {
        for (int i = 0; i < 99; i++) {
            this->blocks[i] = (daBeeper*)Actor_SearchByID(384);

            if (blocks[i]) {
                if (blocks[i] == this) {
                    break;
                }

                if (i > 0) {
                    if (blocks[i] == blocks[(i - 1)]) {
                        i--;
                        continue;
                    }
                }

                if (blocks[i]->id == this->id) {
                    OSReport("Block linked!\n");

                    blocks[i]->acState.setState(a);
                }
            }
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
    if (block == 0) {
        tile.tileNumber = 83;
    } else {
        tile.tileNumber = 87;
    }
}
void daBeeper::executeState_Active() {
        if (time < 0) {
            if (t2 < 0) {
                beeps--;

                if (beeps < 1) {
                    PlaySound(this, SE_OBJ_STEP_ON_SWITCH);
                    doStateChange(&StateID_NotActive);
                    return;
                }

                PlaySound(this, SE_SYS_RED_RING);

                t2 = 60;

                return;
            }

            t2--;

            return;
        }

        time--;
}
void daBeeper::endState_Active() {
    time = 150 + timeplus; t2 = 60;
    physics.removeFromList();
}

void daBeeper::beginState_NotActive() {
    if (block == 0) {
        tile.tileNumber = 84;
    } else {
        tile.tileNumber = 88;
    }
}
void daBeeper::executeState_NotActive() {
        if (time < 0) {
            if (t2 < 0) {
                beeps--;

                if (beeps < 1) {
                    PlaySound(this, SE_OBJ_STEP_ON_SWITCH);
                    doStateChange(&StateID_Active);
                    return;
                }

                PlaySound(this, SE_SYS_RED_RING);

                t2 = 60;

                return;
            }

            t2--;

            return;
        }

        time--;

}
void daBeeper::endState_NotActive() {
    time = 150 + timeplus; t2 = 60;

    physics.addToList();
}
