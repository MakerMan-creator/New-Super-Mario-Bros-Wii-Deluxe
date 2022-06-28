#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <beeperfull.h>

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
    this->id = settings >> 8 & 0xFF; // Nybbles 9 and 10

    if (block == 0) {
	    physics.addToList();
    }

	if (block == 0) { doStateChange(&StateID_Active); }
    else { doStateChange(&StateID_NotActive); }

    this->mode = (TILES)((block == 0) ? 83 : 84);

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

    int i;

    if (spintime < 60) {
        spintime++; 
    }

    for (i = 0; i < 4; i++) {
        control[i] = GetRemoconMng()->controllers[i];

        if (control[i]) {
            break;
        }
    }

    if ((control[i]->isShaking) && (spintime == 60)) {
        PlaySound(this, SE_OBJ_STEP_ON_SWITCH);

        if (acState.getCurrentState() == &StateID_Active) {
            doStateChange(&StateID_NotActive);
        } else {
            doStateChange(&StateID_Active);
        }

        spintime = 60;
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
    tile.tileNumber = 83;
}
void daBeeper::executeState_Active() {
    
}
void daBeeper::endState_Active() {
    physics.removeFromList();
}

void daBeeper::beginState_NotActive() {
    tile.tileNumber = 84;
}
void daBeeper::executeState_NotActive() {
    

}
void daBeeper::endState_NotActive() {
    physics.addToList();
}
