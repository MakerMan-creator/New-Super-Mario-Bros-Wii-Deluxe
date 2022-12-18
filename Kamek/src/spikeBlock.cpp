#include <common.h>
#include <game.h>
#include <sfx.h>
#include "boss.h"

class daSpikeBlock_c : public daEnBlockMain_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	ActivePhysics::Info HitMeBaby;

	enum Mode {
		TIMED,
		PERMANENT,
	};

	mEf::es2 charge;

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);

	int index = 1;
	u8 timer = 0;
	u8 playerID = 0;
	bool wasHit = false;
	int electric = 0;
	Mode mode;

	TileRenderer tile;
	Physics::Info physicsInfo;

	static daSpikeBlock_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	
	USING_STATES(daSpikeBlock_c);
	DECLARE_STATE(Idle);
};

CREATE_STATE(daSpikeBlock_c, Idle);

    extern "C" void dAcPy_vf3F4(void* mario, void* other, int t);

	void daSpikeBlock_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (mode == TIMED) {
			if (electric) {
			    dAcPy_vf3F4(apOther->owner, this, 9);
		    }
		} else {
			if (!wasHit) {
			    dAcPy_vf3F4(apOther->owner, this, 9);
		    }
		}
	}
	void daSpikeBlock_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (mode == TIMED) {
			if (electric) {
			    DamagePlayer(this, apThis, apOther);
		    }
		} else {
			if (!wasHit) {
			    DamagePlayer(this, apThis, apOther);
		    }
		}
	}
	void daSpikeBlock_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {}

	bool daSpikeBlock_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 


daSpikeBlock_c* daSpikeBlock_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daSpikeBlock_c));
	return new(buffer) daSpikeBlock_c;
}


int daSpikeBlock_c::onCreate() {
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

	mode = (Mode)((settings >> 28 & 0xF) % 2);

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 13.5;
	HitMeBaby.xDistToEdge = 7.7;
	HitMeBaby.yDistToEdge = 10.2;

	if (mode == PERMANENT) {
        HitMeBaby.yDistToEdge = 8.0;
		HitMeBaby.yDistToCenter = 12.0;
	}

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFC00000;
	HitMeBaby.unkShort1C = 0;

	HitMeBaby.callback = &dEn_c::collisionCallback;
	this->aPhysics.initWithStruct(this, &HitMeBaby);

	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->add(&tile);

	tile.x = pos.x - 8;
	tile.y = -(16 + pos.y);
	tile.tileNumber = 49;

	initialY = pos.y;

	if (mode == PERMANENT) {
		aPhysics.addToList();
	}

	doStateChange(&StateID_Idle);

	this->onExecute();

	return true;
}

int daSpikeBlock_c::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

    tile.setVars(scale.x);

	if (!wasHit) {
	    HitMeBaby.callback = &dEn_c::collisionCallback;
	    this->aPhysics.initWithStruct(this, &HitMeBaby);

		if (mode == TIMED) {
			if (timer > 210) {
			    if (electric) {
				    this->aPhysics.removeFromList();
				    electric = 0;
			    } else {
				    this->aPhysics.addToList();
				    electric = 1;
			    }

			    timer = 0;
		    } else {
			    timer++;
		    }
		}
	}

	return true;
}

int daSpikeBlock_c::onDelete() {
    TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->remove(&tile);

	physics.removeFromList();

	return true;
}

int daSpikeBlock_c::onDraw() {
	return true;
}

void daSpikeBlock_c::blockWasHit(bool isDown) {
	pos.y = initialY;

	if (mode == PERMANENT) {
		aPhysics.removeFromList();
	}

	if (wasHit) { return; }

	nw4r::snd::SoundHandle clear;
    PlaySoundWithFunctionB4(SoundRelatedClass, &clear, SE_OBJ_COIN_POP_UP, 1);

	wasHit = true;

	doStateChange(&StateID_Idle);
}

void daSpikeBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y) { blockWasHit(false); }
}

void daSpikeBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y) { blockWasHit(true); }
}

void daSpikeBlock_c::beginState_Idle() {
	
}
void daSpikeBlock_c::executeState_Idle() {
	if (wasHit) { return; }

	if (mode == TIMED) {
		if (electric != 0) {
		    Vec efPos = {pos.x, (f32)(pos.y + 8.0), pos.z-500.0f};
		    Vec efScale = {scale.x*0.8f, scale.y*0.8f, scale.z*0.8f};
		    S16Vec nullRot = {0,0,0};
		    charge.spawn("Wm_en_birikyu", 0, &efPos, &nullRot, &efScale);
	    } else {
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
	} else {
		Vec efPos = {pos.x, (f32)(pos.y + 16.0), pos.z-500.0f};
		Vec efScale = {scale.x*0.8f, scale.y*0.8f, scale.z*0.8f};
		S16Vec nullRot = {0,0,0};
		charge.spawn("Wm_en_birikyu", 0, &efPos, &nullRot, &efScale);

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
}
void daSpikeBlock_c::endState_Idle() {
	timer = 0;
}