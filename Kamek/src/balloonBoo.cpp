#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

// Initially made for BruhHummus' NSMB DS remake, 
// and tweaked to be a little different here

const char* BalloonBooFileList[] = { "teresa", NULL };

class dBalloonBoo_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	ActivePhysics::Info HitMeBaby; 
	int cackleTimer = 0;
	daPlBase_c *player;
	float target[2];
    u8 direction;
    int still;
	float maxSpeed = 0.0;

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	bool Shrink(float scale);
	bool Grow(float scale);

	void setUpTheseDamnEffects();

	static dBalloonBoo_c* build();

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
	nw4r::g3d::ResAnmTexSrt resTexSrt;
	m3d::mdl_c fogModel;

	USING_STATES(dBalloonBoo_c);
	DECLARE_STATE(Inflate);
	DECLARE_STATE(Deflate);
	DECLARE_STATE(Die);
};

CREATE_STATE(dBalloonBoo_c, Inflate);
CREATE_STATE(dBalloonBoo_c, Deflate);
CREATE_STATE(dBalloonBoo_c, Die);

void dBalloonBoo_c::setUpTheseDamnEffects() {
    allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("teresa", "g3d/teresa.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("teresa_otherB");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("fog");
	this->resTexSrt = anmSrt;

	nw4r::g3d::ResMdl fogMdl = this->resFile.GetResMdl("fog");
	this->fogModel.setup(fogMdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->fogModel, 0);

	SetupTextures_Enemy(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait_otherB");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();
}

void dBalloonBoo_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

bool dBalloonBoo_c::Shrink(float scale) {
    if ((this->scale.x <= scale) && (this->scale.y <= scale) && (this->scale.y <= scale)) {
    	this->scale = (Vec){scale, scale, scale};
    	return true;
    }
    
    this->scale.x -= (1.2f / 80.0f);
    return false;
}

bool dBalloonBoo_c::Grow(float scale) {
	if ((this->scale.x >= scale) && (this->scale.y >= scale) && (this->scale.y >= scale)) {
    	this->scale = (Vec){scale, scale, scale};
    	return true;
    }

    this->scale.x += (1.2f / 80.0f);
    return false;
}


void dBalloonBoo_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

dBalloonBoo_c* dBalloonBoo_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dBalloonBoo_c));
	return new(buffer) dBalloonBoo_c;
}

int dBalloonBoo_c::onCreate() {
	this->setUpTheseDamnEffects(); 

	this->still = ((this->settings & 0x000F0000) >> 16);
	int set = this->settings >> 28 & 0xF;
	this->maxSpeed = ((float)(set / 2));

	this->pos.z += 1.5;

	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 11.0; 
	HitMeBaby.xDistToEdge = 45.0; 
	HitMeBaby.yDistToEdge = 45.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFFFFFF; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList(); 

	this->scale.x = 5.0; 
	this->scale.y = 5.0; 
	this->scale.z = 5.0; 

	bindAnimChr_and_setUpdateRate("wait_otherB", 1, 0.0, 1.2);

	if (!still) { doStateChange(&StateID_Inflate); }

	this->onExecute();

	return true;
}

int dBalloonBoo_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	u8 facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
	this->direction = facing;

	this->rot.y = ((direction) ? 0xD800 : 0x2800);

    this->scale = (Vec){this->scale.x, this->scale.x, this->scale.x};

    for (int i = 0; i < 4; i++)
    {
    	player = GetPlayerOrYoshi(i);

    	if (player) {
    		break;
    	}
    }

    if (acState.getCurrentState() != &StateID_Die) {
    	HitMeBaby.xDistToEdge = (9.0 * this->scale.x);
        HitMeBaby.yDistToEdge = HitMeBaby.xDistToEdge;
    	HitMeBaby.yDistToCenter = (11.0 * this->scale.x);
    
        this->aPhysics.initWithStruct(this, &HitMeBaby);
    }

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	if (!still) {
		if (player) {
			if (acState.getCurrentState() != &StateID_Die) {
			    if (player->pos.x < this->pos.x) {
				    dStateBase_c *state = ((player->direction) ? &StateID_Deflate : &StateID_Inflate);
				    doStateChange(state);
			    } else if (player->pos.x > this->pos.x) {
				    dStateBase_c *state = ((player->direction) ? &StateID_Inflate : &StateID_Deflate);
				    doStateChange(state);
			    }
		    }
		}
	}

	if (acState.getCurrentState() != &StateID_Die) {
		this->cackleTimer++;
	
		if (this->cackleTimer > 770) {
	        PlaySound(this, SE_EMY_TERESA);
	        this->cackleTimer = 0;
		}
	}

	float rect[] = {0.0, 0.0, 38.0, 38.0};

	int gone = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);

	if (gone) {
		this->Delete(1);
	}

	return true;
}

int dBalloonBoo_c::onDelete() {
	return true;
}

int dBalloonBoo_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void dBalloonBoo_c::beginState_Inflate() {}
void dBalloonBoo_c::executeState_Inflate() {
	bool maxGrown = this->Grow(5.0f);

	if (!maxGrown) {
		PlaySound(this, SE_EMY_HUHU_BREATH_IN);
	}
}
void dBalloonBoo_c::endState_Inflate() {}

void dBalloonBoo_c::beginState_Deflate() {}
void dBalloonBoo_c::executeState_Deflate() {
	bool minGrown = this->Shrink(1.1f);

	if (!minGrown) {
		PlaySound(this, SE_EMY_HUHU_BREATH_OUT);
	}

	if (player) {
		this->target[0] = player->pos.x;
		this->target[1] = player->pos.y;
	}

	this->HandleXSpeed();
	this->HandleYSpeed();

	const float divider = 100.5f;

	if ((this->pos.x != target[0]) || (this->pos.y != target[1])) {
		if (player) {
			// Editing the speed
			this->speed.x = ((this->pos.x < target[0]) ? ((target[0] + this->pos.x) / divider) : ((target[0] - this->pos.x) / divider));  
			this->speed.y = ((this->pos.y < target[1]) ? ((target[1] - this->pos.y) / divider) : ((target[1] + this->pos.y) / divider)); 

			for (int i = 0; i < 8; i++)
			{
                const float divTwo = (this->maxSpeed + 0.1f);

				if ((speed.y < -divTwo) || (speed.y > divTwo)) {
				    speed.y /= 2.2;
			    }
			    
			    if ((speed.x < -divTwo) || (speed.x > divTwo)) {
				    speed.x /= 2.2;
			    }
			}  
		} else {
			this->speed.y = this->speed.x = 0.0;
		}
	}
	
	this->UpdateObjectPosBasedOnSpeedValuesReal();
}
void dBalloonBoo_c::endState_Deflate() {}

void dBalloonBoo_c::beginState_Die() {
    this->speed.x = ((direction) ? 1.5 : -1.5);
    this->speed.y = 2.5;

	PlaySound(this, SE_EMY_BIG_TERESA_DEAD);

	this->removeMyActivePhysics();
}
void dBalloonBoo_c::executeState_Die() {
	this->rot.x = ((direction) ? (this->rot.x + 0x200) : (this->rot.x - 0x200));

	this->HandleXSpeed();
	this->HandleYSpeed();
	this->UpdateObjectPosBasedOnSpeedValuesReal();

	this->speed.y -= 0.125;
}
void dBalloonBoo_c::endState_Die() {}

extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);

void dBalloonBoo_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    EN_LandbarrelPlayerCollision(this, apThis, apOther);
	DamagePlayer(this, apThis, apOther);
}
	void dBalloonBoo_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
        EN_LandbarrelPlayerCollision(this, apThis, apOther);
	    DamagePlayer(this, apThis, apOther);
    }

	bool dBalloonBoo_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
        doStateChange(&StateID_Die);

        return true;
	} 
	bool dBalloonBoo_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }  
	bool dBalloonBoo_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }  
	bool dBalloonBoo_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }  
	bool dBalloonBoo_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }  
	bool dBalloonBoo_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }  
	bool dBalloonBoo_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }  
	bool dBalloonBoo_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }  
	bool dBalloonBoo_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	}  
	bool dBalloonBoo_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }  
	bool dBalloonBoo_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }
	bool dBalloonBoo_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		const float newScale = 1.75f;

        if ((this->scale.x <= newScale) && (this->scale.y <= newScale) && (this->scale.z <= newScale)) {
        	StageE4::instance->spawnCoinJump(pos, 0, 3, 0);
        	
        	doStateChange(&StateID_Die);

        	return true;
        }

		return false;
	}  
	bool dBalloonBoo_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 