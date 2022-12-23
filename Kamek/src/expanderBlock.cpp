#include <common.h>
#include <game.h>
#include <sfx.h>

class daExpanderBlock : public daEnBlockMain_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	TileRenderer tile;
	Physics::Info physicsInfo;
	bool growToMax, isMax;
	int timer;
	
	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);

	static daExpanderBlock* build();

	USING_STATES(daExpanderBlock);
	DECLARE_STATE(Small);
	DECLARE_STATE(Grow);
	DECLARE_STATE(Shrink);
};

void daExpanderBlock::blockWasHit(bool isDown) {
	pos.y = initialY;
	growToMax = isDown;

    if (!isMax) {
        doStateChange(&StateID_Grow);
	}
}

void daExpanderBlock::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y) { blockWasHit(false); }
}

void daExpanderBlock::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y) { blockWasHit(true); }
}

CREATE_STATE(daExpanderBlock, Small);
CREATE_STATE(daExpanderBlock, Grow);
CREATE_STATE(daExpanderBlock, Shrink);

daExpanderBlock* daExpanderBlock::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daExpanderBlock));
	return new(buffer) daExpanderBlock;
}


int daExpanderBlock::onCreate() {
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
	physics.addToList();

	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->add(&tile);

	timer = 0;

	tile.x = pos.x - 8;
	tile.y = -(16 + pos.y);
	tile.tileNumber = 49;

	doStateChange(&StateID_Small);

	this->onExecute();

	return true;
}

int daExpanderBlock::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

    tile.setVars(scale.x);

	this->scale = (Vec){scale.x, scale.y, scale.z};

	tile.tileNumber = (isMax ? 50 : 49);

	physicsInfo.x1 = -8 * scale.x;
	physicsInfo.y1 = 16 * scale.x;
	physicsInfo.x2 = 8 * scale.x;
	physicsInfo.y2 = 0 * scale.x;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);

	return true;
}

int daExpanderBlock::onDelete() {
	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->remove(&tile);

	physics.removeFromList();
	return true;
}

int daExpanderBlock::onDraw() {
	return true;
}

void daExpanderBlock::beginState_Small() {}
void daExpanderBlock::executeState_Small() {
	if (isMax) {
		if (timer > 400) {
			doStateChange(&StateID_Shrink);
		}

		timer++;
		return;
	}
	
	int result = blockResult();

	if (result == 0)
		return;

	if (result == 1) {
		doStateChange(&daEnBlockMain_c::StateID_UpMove);
		anotherFlag = 2;
		isGroundPound = false;
	} else {
		doStateChange(&daEnBlockMain_c::StateID_DownMove);
		anotherFlag = 1;
		isGroundPound = true;
	}
}
void daExpanderBlock::endState_Small() {
	timer = 0;
}

void daExpanderBlock::beginState_Grow() {
	PlaySound(this, SE_OBJ_KINOKO_YOKO_LONGER);
}
void daExpanderBlock::executeState_Grow() {
	if (scale.x < 4.0) {
        for (int i = 0; i < 16; i++) {
			scale.x += 0.0125;
		}
		return;
	}

	isMax = true;

	doStateChange(&StateID_Small);
}
void daExpanderBlock::endState_Grow() {}

void daExpanderBlock::beginState_Shrink() {
	PlaySound(this, SE_OBJ_KINOKO_YOKO_SHORTER);
}
void daExpanderBlock::executeState_Shrink() {
	if (scale.x > 1.0) {
		scale.x -= 0.125;
		return;
	}
	
	doStateChange(&StateID_Small);
}
void daExpanderBlock::endState_Shrink() {
	isMax = false;
}