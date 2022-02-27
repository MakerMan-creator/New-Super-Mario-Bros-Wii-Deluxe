#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

const char* KingBooFileList [] = { "teresa", NULL };

class daEnKingBoo_c : public daBoss {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	daPlBase_c *players[4];
	int soundCount = 0;

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daEnKingBoo_c* build();

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
	m3d::mdl_c bodyModel, fogModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResAnmTexSrt resTexSrt;

	USING_STATES(daEnKingBoo_c);
	DECLARE_STATE(Intro);
	DECLARE_STATE(Chase);
	DECLARE_STATE(Shoot);
	DECLARE_STATE(Spinning);
	DECLARE_STATE(Outro);
};

CREATE_STATE(daEnKingBoo_c, Intro);
CREATE_STATE(daEnKingBoo_c, Chase);
CREATE_STATE(daEnKingBoo_c, Shoot);
CREATE_STATE(daEnKingBoo_c, Spinning);
CREATE_STATE(daEnKingBoo_c, Outro);

void daEnKingBoo_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daEnKingBoo_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daEnKingBoo_c* daEnKingBoo_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnKingBoo_c));
	return new(buffer) daEnKingBoo_c;
}


int daEnKingBoo_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("teresa", "g3d/teresa.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("teresaA");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

    nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("fog");
	this->resTexSrt = anmSrt;

	nw4r::g3d::ResMdl fogMdl = this->resFile.GetResMdl("fog");
	this->fogModel.setup(fogMdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->fogModel, 0);

	SetupTextures_Boss(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 99.0; 
	HitMeBaby.xDistToEdge = 90.0; 
	HitMeBaby.yDistToEdge = 90.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFE6FAE; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList(); 

	this->scale.x = 10.0; 
	this->scale.y = 10.0; 
	this->scale.z = 10.0; 

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);

	for (int i = 0; i < 4; i++)
	{
		this->players[i] = GetPlayerOrYoshi(i);

		if (this->players[i]) {
			players[i]->direction ^= 1;
		}
	}

	doStateChange(&StateID_Intro);

	this->onExecute();

	return true;
}

int daEnKingBoo_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}

int daEnKingBoo_c::onDelete() {
	return true;
}

int daEnKingBoo_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daEnKingBoo_c::beginState_Intro() {
	this->timer = 0;
	
	StopBGMMusic();

	// Set the necessary Flags and make Mario enter Demo Mode
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	WLClass::instance->_4 = 4;
	WLClass::instance->_8 = 0;

	MakeMarioEnterDemoMode();
}
void daEnKingBoo_c::executeState_Intro() {
    if (this->soundCount < 4) {
    	if (this->players[this->soundCount]) {
    		SFX vocals;

    		switch (this->soundCount) {
    			case 0:
    			    vocals = SE_VOC_MA_QUAKE;
    			    break;
				case 1:
    			    vocals = SE_VOC_LU_QUAKE;
    			    break;
				case 2:
    			    vocals = SE_VOC_KO_QUAKE;
    			    break;
				case 3:
    			    vocals = SE_VOC_KO2_QUAKE;
    			    break;
    		}

			PlaySound(this, vocals);
    	}

    	this->soundCount++;

    	return;
    }

    if (this->timer > 100) {
    	doStateChange(&StateID_Chase);
    	return;
    }

    this->timer++;
}
void daEnKingBoo_c::endState_Intro() {
	dStage32C_c::instance->freezeMarioBossFlag = 0;
	WLClass::instance->_8 = 1;

	MakeMarioExitDemoMode();
	StartBGMMusic();

	for (int i = 0; i < 4; i++)
	{
		this->players[i] = GetPlayerOrYoshi(i);

		if (this->players[i]) {
			players[i]->direction ^= 1;
		}
	}
}

void daEnKingBoo_c::beginState_Chase() {this->timer = 0;}
void daEnKingBoo_c::executeState_Chase() {}
void daEnKingBoo_c::endState_Chase() {}

void daEnKingBoo_c::beginState_Shoot() {this->timer = 0;}
void daEnKingBoo_c::executeState_Shoot() {}
void daEnKingBoo_c::endState_Shoot() {}

void daEnKingBoo_c::beginState_Spinning() {this->timer = 0;}
void daEnKingBoo_c::executeState_Spinning() {}
void daEnKingBoo_c::endState_Spinning() {}

void daEnKingBoo_c::beginState_Outro() {this->timer = 0;}
void daEnKingBoo_c::executeState_Outro() {}
void daEnKingBoo_c::endState_Outro() {}

extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);

void daEnKingBoo_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	EN_LandbarrelPlayerCollision(this, apThis, apOther);
	DamagePlayer(this, apThis, apOther);
}
	void daEnKingBoo_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->playerCollision(apThis, apOther);
	}

	bool daEnKingBoo_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 
	bool daEnKingBoo_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}  
	bool daEnKingBoo_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 