#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

const char* JPPfileList[] = { "pakkun_water", NULL };

class daEnWorldPakkun_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

    int timer = 0;
	char triggers;
	int fireballs;
	int deathDirection;
	float inity;
	u8 event;

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daEnWorldPakkun_c* build();

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

	USING_STATES(daEnWorldPakkun_c);
	DECLARE_STATE(In);
	DECLARE_STATE(Out);
	DECLARE_STATE(Death);
};

CREATE_STATE(daEnWorldPakkun_c, In);
CREATE_STATE(daEnWorldPakkun_c, Out);
CREATE_STATE(daEnWorldPakkun_c, Death);

void daEnWorldPakkun_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daEnWorldPakkun_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daEnWorldPakkun_c* daEnWorldPakkun_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnWorldPakkun_c));
	return new(buffer) daEnWorldPakkun_c;
}


int daEnWorldPakkun_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("pakkun_water", "g3d/pakkun_water.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("pakkun_water");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("change");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	this->triggers = ((settings >> 24 & 0xF) % 2); // Nybble 6
	this->event = (u8)((settings >> 20 & 0xF) - 1); // nybble 7

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 12.0; 
	HitMeBaby.xDistToEdge = 8.0; 
	HitMeBaby.yDistToEdge = 10.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFFFFFF; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	bindAnimChr_and_setUpdateRate("change", 1, 0.0, 1.0);

	doStateChange(&StateID_In);

	this->inity = this->pos.y;
	this->pos.x++;

	this->onExecute();

	return true;
}

int daEnWorldPakkun_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	float rect[] = {0.0, 0.0, 38.0, 38.0};
	int gone = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);

	if (gone) {
		this->Delete(1);
	}

	return true;
}

int daEnWorldPakkun_c::onDelete() {
	return true;
}

int daEnWorldPakkun_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daEnWorldPakkun_c::beginState_In() {}
void daEnWorldPakkun_c::executeState_In() {
	if (this->timer > 130) {
        doStateChange(&StateID_Out);
	}

	this->timer++;
}
void daEnWorldPakkun_c::endState_In() { this->timer = 0; }

void daEnWorldPakkun_c::beginState_Out() {
	this->speed.y = 6.2;
}
void daEnWorldPakkun_c::executeState_Out() {
	HandleXSpeed();
	HandleYSpeed();
	UpdateObjectPosBasedOnSpeedValuesReal();

	this->speed.y = ((speed.y <= -0.009) ? (-0.009) : (this->speed.y - 0.009));

	if (this->pos.y <= this->inity) {
		this->pos.y = this->inity;
		
		doStateChange(&StateID_In);
	}
}
void daEnWorldPakkun_c::endState_Out() {}

void daEnWorldPakkun_c::beginState_Death() {
	if (this->triggers) {
		bool active = dFlagMgr_c::instance->active(this->event);

	    if (!active) {
		    dFlagMgr_c::instance->set(this->event, 0, true, false, false);
	    }
	}
}
void daEnWorldPakkun_c::executeState_Death() {
	this->removeMyActivePhysics();

	PlaySound(this, SE_EMY_BLOW_PAKKUN_DOWN);
	PlaySound(this, SE_EMY_DOWN);

	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};

	SpawnEffect("Wm_mr_cmnsndlandsmk", 0, &pos, &nullRot, &oneVec);
	SpawnEffect("Wm_en_landsmoke", 0, &pos, &nullRot, &oneVec);
	SpawnEffect("Wm_en_sndlandsmk_s", 0, &pos, &nullRot, &oneVec);

	this->Delete(1);
}
void daEnWorldPakkun_c::endState_Death() {}

void daEnWorldPakkun_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
}
	void daEnWorldPakkun_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	    DamagePlayer(this, apThis, apOther);
    } 

	bool daEnWorldPakkun_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
		dStageActor_c *obj = apOther->owner;
        this->deathDirection = (((int)(obj->pos.x > this->pos.x)) ^ 1);

		doStateChange(&StateID_Death);

		return true;
	} 
	bool daEnWorldPakkun_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
		DamagePlayer(this, apThis, apOther);
		return true;
	} 
	bool daEnWorldPakkun_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat5_Mario(apThis, apOther);
	} 
	bool daEnWorldPakkun_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	} 
	bool daEnWorldPakkun_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat5_Mario(apThis, apOther);
	} 
	bool daEnWorldPakkun_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat5_Mario(apThis, apOther);
	}
	bool daEnWorldPakkun_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	} 
	bool daEnWorldPakkun_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	} 
	bool daEnWorldPakkun_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}  
	bool daEnWorldPakkun_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
		return this->collisionCat3_StarPower(apThis, apOther);
	}  
    bool daEnWorldPakkun_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	} 
	bool daEnWorldPakkun_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
	} 
	bool daEnWorldPakkun_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
	} 