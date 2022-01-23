#include <common.h>
#include <game.h>
#include <sfx.h>
#include <g3dhax.h>
#include "boss.h"

const char* SBarcNameList[] = {
	"sanbo",
	"sanboBoss",
	NULL
};

class daPowerPokey_c : public daBoss {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c headModelsleep;
	nw4r::g3d::ResFile resFile;
    nw4r::g3d::ResAnmClr animationClr;

	int timer;
	float xSpeed;
	float ySpeed = 0.5;
	int direction;

	static daPowerPokey_c *build();

	void setupBodyModelSleep();
	void updateModelMatricesSleep();

	USING_STATES(daPowerPokey_c);
	DECLARE_STATE(Intro);

};

daPowerPokey_c *daPowerPokey_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daPowerPokey_c));
	return new(buffer) daPowerPokey_c;
}

CREATE_STATE(daPowerPokey_c, Intro);



void daPowerPokey_c::setupBodyModelSleep() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("sanboBoss", "g3d/sanboBoss.brres");
	nw4r::g3d::ResMdl headSleep = this->resFile.GetResMdl("sanbo_head_sleep");
	nw4r::g3d::ResAnmClr anmClr = this->resFile.GetResAnmClr("");// Write animation name in the ""
	this->animationClr.setup(headSleep, anmClr, &this->allocator, 0);

	this->headModelsleep.setup(headSleep, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&headModelsleep, 0);

	allocator.unlink();
}


int daPowerPokey_c::onCreate() {
	
	setupBodyModelSleep();

	this->scale = (Vec){1.5, 1.5, 1.5};

	speed.x = 0.0;
	speed.y = 0.0;

/*
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.xDistToEdge = 35.0;
	HitMeBaby.yDistToEdge = 35.0; }
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028E;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
*/
	doStateChange(&StateID_Intro);

	this->onExecute();
	return true;
}

int daPowerPokey_c::onDelete() {
	return true;
}

int daPowerPokey_c::onExecute() {
	acState.execute();
	updateModelMatricesSleep();

	return true;
}

int daPowerPokey_c::onDraw() {
	headModelsleep.scheduleForDrawing();
	headModelsleep._vf1C();
	return true;
}


void daPowerPokey_c::updateModelMatricesSleep() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	headModelsleep.setDrawMatrix(matrix);
	headModelsleep.setScale(&scale);
	headModelsleep.calcWorld(false);
}

//State_Intro
void daPowerPokey_c::beginState_Intro(){
	this->timer = 0;
	this->xSpeed;
}

void daPowerPokey_c::executeState_Intro(){
	this->timer++;
}

void daPowerPokey_c::endState_Intro(){ }
