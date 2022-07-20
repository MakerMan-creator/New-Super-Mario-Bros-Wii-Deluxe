#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"


const char* MystMushFileList[] = { "myst_mush", NULL };

class daMysteryMushroom : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	// Powerups - 0 = small; 1 = big; 2 = fire; 3 = mini; 4 = prop; 5 = peng; 6 = ice; 7 = hammer

	enum POWERUPS {
        SMALL,
		BIG,
		FIRE,
		MINI,
		PROPELLER,
		PENGUIN,
		ICE,
		HAMMER,
	};

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daMysteryMushroom* build();

	bool done = false;
    POWERUPS num;
	int playerID;

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
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

};


void daMysteryMushroom::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daMysteryMushroom::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daMysteryMushroom* daMysteryMushroom::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daMysteryMushroom));
	return new(buffer) daMysteryMushroom;
}


int daMysteryMushroom::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("myst_mush", "g3d/myst_mush.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("myst_mush");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Item(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait2");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

    this->playerID = ((settings >> 28 & 0xF) % 4);

	if (GetPlayerOrYoshi(playerID) == 0) {
		done = true;
		this->Delete(1);
	}

	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 11.0; 
	HitMeBaby.xDistToEdge = 7.0; 
	HitMeBaby.yDistToEdge = 7.0; 
	HitMeBaby.category1 = 0x5; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFC00000; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	num = (POWERUPS)GenerateRandomNumber(5);

    if (num < (POWERUPS)0) {
		num = (POWERUPS)(-num);
	}

	num = (POWERUPS)(num % (POWERUPS)6);

	int index = (int)(num + (POWERUPS)2);

	const char* powerup[8] = {
		"small",
        "big",
		"fire",
		"mini",
		"propeller",
		"penguin",
		"ice",
		"hammer"
	}; 

	bindAnimChr_and_setUpdateRate("wait2", 1, 0.0, 1.0);

	OSReport("Powerup status: %s\n", powerup[index]);

	this->onExecute();

	return true;
}

int daMysteryMushroom::onExecute() {
	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	rot.y -= 0x100;

	return true;
}

int daMysteryMushroom::onDelete() {
	return true;
}

int daMysteryMushroom::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

extern "C" void *dAcPy_c__ChangePowerupWithAnimation(void * Player, int powerup);

void daMysteryMushroom::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (done) {
        this->Delete(1);
        return;
	}

    dAcPy_c__ChangePowerupWithAnimation(apOther->owner, (int)(num + (POWERUPS)2));

    done = true;
    this->Delete(1);
}
	void daMysteryMushroom::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {}
	void daMysteryMushroom::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->playerCollision(apThis, apOther);
	}

	bool daMysteryMushroom::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
	bool daMysteryMushroom::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 
	bool daMysteryMushroom::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 
	bool daMysteryMushroom::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
	bool daMysteryMushroom::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
	bool daMysteryMushroom::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
	bool daMysteryMushroom::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 
	bool daMysteryMushroom::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 
	bool daMysteryMushroom::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 
	bool daMysteryMushroom::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 
	bool daMysteryMushroom::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {return true;} 
	bool daMysteryMushroom::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
	bool daMysteryMushroom::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}