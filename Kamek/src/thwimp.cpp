#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

const char* ThwimpFileList [] = { "dossun", NULL };

class daThwimp : public dEn_c {
public:
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	float initY;
	int direction = 0;
	int timer = 0;

	void updateModelMatrices();

	static daThwimp* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daThwimp);
	DECLARE_STATE(Stop);
	DECLARE_STATE(Jump);
	DECLARE_STATE(Die);
};

CREATE_STATE(daThwimp, Stop);
CREATE_STATE(daThwimp, Jump);
CREATE_STATE(daThwimp, Die);

void daThwimp::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

daThwimp* daThwimp::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daThwimp));
	return new(buffer) daThwimp;
}


int daThwimp::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("dossun", "g3d/t00.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("dossun");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	allocator.unlink(); 

	this->scale.x = 0.3333; 
	this->scale.y = 0.3333; 
	this->scale.z = 0.3333;

	initY = pos.y;
	direction = ((settings >> 28 & 0xF) % 2);

	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 11.0; 
	HitMeBaby.xDistToEdge = 7.0; 
	HitMeBaby.yDistToEdge = 7.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFBEFAE; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	doStateChange(&StateID_Stop);

	this->onExecute();

	return true;
}

int daThwimp::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	float rect[] = {0.0, 0.0, 38.0, 38.0};
	int gone = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
	if (gone) {
		this->Delete(1);
	}

	return true;
}

int daThwimp::onDelete() {
	return true;
}

int daThwimp::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daThwimp::beginState_Stop() {
	timer = 0;
}
void daThwimp::executeState_Stop() {
	if (timer > 70) {
		pos.y += 1.0;
		doStateChange(&StateID_Jump);
	}

	timer++;
}
void daThwimp::endState_Stop() {}

void daThwimp::beginState_Jump() {
	this->speed.y = 4.0;
	this->speed.x = (direction) ? 1.0 : -1.0;

	PlaySound(this, SE_EMY_KANIBO_THROW);
}
void daThwimp::executeState_Jump() {
	if (this->pos.y <= this->initY) {
		this->pos.y = this->initY;

		doStateChange(&StateID_Stop);

		return;
	}

	this->pos.y += this->speed.y;
	this->pos.x += this->speed.x;

	this->speed.y -= 0.125;
}
void daThwimp::endState_Jump() {
	direction ^= 1;

	this->speed = (Vec){0.0f, 0.0f, 0.0f};

	PlaySound(this, SE_EMY_DOSSUN);
}

void daThwimp::beginState_Die() {
	rot.z += 0x8000;
	speed.y = 3.0;
}
void daThwimp::executeState_Die() {
	pos.y += speed.y;

	speed.y -= 0.25;
}
void daThwimp::endState_Die() {}

void daThwimp::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
}
	void daThwimp::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	    DamagePlayer(this, apThis, apOther);
    }

	bool daThwimp::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
		doStateChange(&StateID_Die);
		return true;
	}
	bool daThwimp::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
		playerCollision(apThis, apOther);
		return true;
	} 
	bool daThwimp::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		playerCollision(apThis, apOther);
		return true;
	}  
	bool daThwimp::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	} 
	bool daThwimp::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		playerCollision(apThis, apOther);
		return true;
	}  
	bool daThwimp::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		playerCollision(apThis, apOther);
		return true;
	} 
	bool daThwimp::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
		playerCollision(apThis, apOther);
		return true;
	}  
	bool daThwimp::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
		doStateChange(&StateID_Die);
		return true;
	} 
	bool daThwimp::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		doStateChange(&StateID_Die);
		return true;
	} 
	bool daThwimp::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	} 
	bool daThwimp::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	} 
	bool daThwimp::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		doStateChange(&StateID_Die);
		return true;
	}
	bool daThwimp::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	}