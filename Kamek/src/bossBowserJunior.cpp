#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"
#include "bowserjr.h" // Both classes for the boss's actors are here

const char* BJarcNameList[] = { 
	"koopaJr",
	"nokonokoA",
	NULL
};

CREATE_STATE(daBowserJr, Intro);
CREATE_STATE(daBowserJr, Attack);
CREATE_STATE(daBowserJr, Chase);
CREATE_STATE(daBowserJr, Turn);
CREATE_STATE(daBowserJr, Damage);
CREATE_STATE(daBowserJr, Outro);

void daBowserJr::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void daBowserJr::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daBowserJr* daBowserJr::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBowserJr));
	return new(buffer) daBowserJr;
}

int daBowserJr::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("koopaJr", "g3d/koopaJr.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("koopaJr");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Boss(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("prov");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	this->isHardBoss = (this->settings & 0x000F0000) >> 16;

	allocator.unlink();

	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 10.0; 
	HitMeBaby.xDistToEdge = 8.0; 
	HitMeBaby.yDistToEdge = 10.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFE6FAE; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(-5<<12, 5<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
	{	isBouncing = false;   }
	else
	{	isBouncing = true;   }

	const char* animFirst = (this->isHardBoss) ? "prov2" : "prov"; 

	this->scale.x = 1.2; 
	this->scale.y = 1.2; 
	this->scale.z = 1.2; 

	this->health = 15;
	this->times = 0;
	this->playSoundOnce = false;

	rot.y = (direction) ? 0x2800 : 0xD800;

    bindAnimChr_and_setUpdateRate(animFirst, 1, 0.0, 1.0);

	doStateChange(&StateID_Intro);

	this->onExecute();

	return true;
}

int daBowserJr::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	dStateBase_c *s = acState.getCurrentState();

	if (s != &StateID_Chase) {
	    if (s != &StateID_Turn) {
	    	if (s != &StateID_Outro) {
	    		this->timer = 0;
	    	}
		} else {
			if (this->timer >= 300) {
		        this->timer = 0;
                doStateChange(&StateID_Attack);
	        }
		}
	} else {
		if (this->timer >= 300) {
		    this->timer = 0;
            doStateChange(&StateID_Attack);
	    }
	}

	if ((s == &StateID_Chase) || (s == &StateID_Turn)) {
		if ((animationChr.getCurrentFrame() == 7) || (animationChr.getCurrentFrame() == 26)) {
			PlaySound(this, SE_EMY_KANIBO_THROW);
		}
	}

	this->timer++;

	return true;
}

int daBowserJr::onDelete() {
	return true;
}

int daBowserJr::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daBowserJr::beginState_Intro() {
    this->timer = 0;
	
	StopBGMMusic();

	// Set the necessary Flags and make Mario enter Demo Mode
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	WLClass::instance->_4 = 4;
	WLClass::instance->_8 = 0;

	MakeMarioEnterDemoMode();
}
void daBowserJr::executeState_Intro() {
	calculateTileCollisions();

    if (this->timer == 1) { 
    	this->times++;
    	SFX intro = (isHardBoss) ? SE_VOC_JR_PROV1 : SE_VOC_JR_PROV2;
    	if (this->times >= 2) {
    		if (!this->playSoundOnce) {
    			PlaySound(this, intro);
    			this->playSoundOnce = true;
    		}
    	}
    }

	if (animationChr.isAnimationDone()) {
		doStateChange(&StateID_Chase);
	}

	this->timer++;
}
void daBowserJr::endState_Intro() {
	dStage32C_c::instance->freezeMarioBossFlag = 0;
	WLClass::instance->_8 = 1;

	MakeMarioExitDemoMode();
	StartBGMMusic();

	OSReport("Get prepared, Mario and/or friends, because here I come!\n");
}

void daBowserJr::beginState_Attack() {
	this->timer = 0; 
	bindAnimChr_and_setUpdateRate("demo_airshipA2", 1, 0.0, 1.0);
	rot.y = (direction) ? 0xD800 : 0x2800;
}
void daBowserJr::executeState_Attack() {
	//dStageActor_c *spawn;
	calculateTileCollisions();

	if (animationChr.getCurrentFrame() == 28) {

		PlaySound(this, SE_BOSS_JR_KICK);
		PlaySound(this, SE_VOC_JR_CS_FALL_OUT_CASTLE);

		OSReport("Here's a Koopa shell for ya!\n");

        // Creates the custom Bowser Jr. shell. I had to do that to make a moving shell, okay?
        this->koopa = CreateActor(205, 0x2, (Vec){(direction) ? (pos.x - 4) : (pos.x + 4), pos.y, pos.z}, 0, 0);

        this->koopa->scale.x = 1.0;
        this->koopa->scale.y = 1.0;
        this->koopa->scale.z = 1.0; 

        //this->koopa->speed.x = (direction) ? -1.0 : 1.0;
        this->koopa->speed.y = 1.0;

		daCustomShell *troopa = (daCustomShell*)koopa;

		troopa->direction = this->direction;

		if (isHardBoss) {
			troopa->posAdd = 1.9;
		}

	}
	else if (animationChr.isAnimationDone()) {
		doStateChange(&StateID_Chase);
	}
}
void daBowserJr::endState_Attack() {
	this->timer = 0;
}

void daBowserJr::beginState_Chase() {
	bindAnimChr_and_setUpdateRate("run", 1, 0.0, 1.0);
}
void daBowserJr::executeState_Chase() {
	if (animationChr.isAnimationDone()) {
		animationChr.setCurrentFrame(0.0);
	}
	f32 a = (isHardBoss) ? 1.5 : 1.2;

	pos.x = (direction) ? pos.x - a : pos.x + a;

	bool ret = calculateTileCollisions();
	bool retTwo = willWalkOnSuitableGround();

	this->facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

	
	if (collMgr.isOnTopOfTile()) {
		this->falling = false;

		if (!retTwo) {
			pos.x = (direction) ? pos.x + a : pos.x - a;
			doStateChange(&StateID_Turn);
		}
	} else {
		if (!this->falling) {
			this->falling = true;
			pos.x = (direction) ? pos.x + a : pos.x - a;
			doStateChange(&StateID_Turn);
		}
	}

	if ((facing != direction) || (ret)) {
		doStateChange(&StateID_Turn);
	}
}
void daBowserJr::endState_Chase() {}

void daBowserJr::beginState_Turn() {
	this->direction = facing;
}
void daBowserJr::executeState_Turn() {
	if (animationChr.isAnimationDone()) {
		animationChr.setCurrentFrame(0.0);
	}

	calculateTileCollisions();

	u16 amt = (direction) ? 0xD800 : 0x2800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if (done) {
		doStateChange(&StateID_Chase);
	}
}
void daBowserJr::endState_Turn() {}

void daBowserJr::beginState_Damage() {
	this->timer = 0;

    const char* a = (health > 0) ? "shock" : "clown_damage_s";

	OSReport("OW!\n");
    bindAnimChr_and_setUpdateRate(a, 1, 0.0, 0.9);

    if (this->fireballHits) { this->fireballHits = 0; }

	if (this->health > 0) {
		PlaySound(this, SE_VOC_JR_DAMAGE_L);
	}
}
void daBowserJr::executeState_Damage() {
	if (animationChr.getCurrentFrame() >= 78) {
		doStateChange(&StateID_Chase);
	}

	if (this->health <= 0) {
		doStateChange(&StateID_Outro);
	}
}
void daBowserJr::endState_Damage() { this->timer = 0; }

void daBowserJr::beginState_Outro() {
	this->timer = 0;
	this->speed.y = 8.0;

    PlaySound(this, SE_VOC_JR_DAMAGE_L_LAST);

	OSReport("NOOOOO!! Beaten by you AGAIN?! Come on!\n");

	OutroSetup(this);
	BossGoalForAllPlayers();
}
void daBowserJr::executeState_Outro() {
	HandleXSpeed();
	HandleYSpeed();
	UpdateObjectPosBasedOnSpeedValuesReal();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	float rect[] = {0.0, 0.0, 38.0, 38.0};
	int gone = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);

	if (gone) {
		this->speed.y = 0.0;

		if (!this->hasAlreadyPlayed) {
			// So this function doesn't lag the game, because of the fx spawned
		    BossExplode(this, &this->pos);
			this->hasAlreadyPlayed = true;
		}
	} else {
        this->speed.y -= 0.125;
        this->timer = 0;
	}

	nw4r::snd::SoundHandle handle;

	if (this->dying) {
		if (this->timer >= 310) {
	        ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
	    }
	
		if (this->timer == 60) {    
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_SHIRO_BOSS_CLEAR, 1);
			PlayerVictoryCries(this);   
		}

		this->timer++;
		
		return;
	}
}
void daBowserJr::endState_Outro() {
}

void daBowserJr::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    DamagePlayer(this, apThis, apOther);
}
void daBowserJr::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
}

bool daBowserJr::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	doStateChange(&StateID_Outro);
	return true;
} 
bool daBowserJr::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
} 
bool daBowserJr::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
} 
bool daBowserJr::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
} 
bool daBowserJr::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
} 
bool daBowserJr::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
} 
bool daBowserJr::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return false;
} 
bool daBowserJr::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daBowserJr::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (acState.getCurrentState() != &StateID_Damage) {
        this->health -= (this->isHardBoss) ? 3 : 5;
		doStateChange(&StateID_Damage);
	}
	
	dActor_c *shell = apOther->owner;
	dEn_c *blah = (dEn_c*)shell;

    if ((blah->name == EN_NOKONOKO) || (blah->name == EN_MECHA_KOOPA) || (blah->name == EN_MET) || (blah->name == EN_TOGEZO)) {
    	// kill the thrown enemies
		blah->collisionCat9_RollingObject(apThis, apOther);
	}
	return false;
}
bool daBowserJr::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
} 
bool daBowserJr::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daBowserJr::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daBowserJr::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat3_StarPower(apThis, apOther);
}
bool daBowserJr::calculateTileCollisions() {
	//returns true if the sprite should turn

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (this->isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f) {
		    this->isBouncing = false;
	    }
	}
	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch
		if (cmgr_returnValue == 0) {
			this->isBouncing = true;
	    }
		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}
		speed.y = 0.0f;
	} else {
		x_speed_inc = 0.0f;
	}

	// Bouncing/Off Ground checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe)) {
			speed.y = 0.0f;
	    }
		collMgr.pSpeed = &speed;
	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe)) {
			speed.y = 0.0f;
	    }
	}
	collMgr.calculateAdjacentCollision(0);
	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			this->isBouncing = true;
		}
		return true;
	}
	return false;
}
bool daBowserJr::willWalkOnSuitableGround() {
	static const float deltas[] = {2.5f, -2.5f};
	VEC3 checkWhere = {
		pos.x + deltas[direction],
		4.0f + pos.y,
		pos.z
	};

	u32 props = collMgr.getTileBehaviour2At(checkWhere.x, checkWhere.y, currentLayerID);

	if (((props >> 16) & 0xFF) == 8) {
		return false;
	}
	float someFloat = 0.0f;
	if (collMgr.sub_800757B0(&checkWhere, &someFloat, currentLayerID, 1, -1)) {
		if (someFloat < checkWhere.y && someFloat > (pos.y - 5.0f)) {
			return true;
	    }
	}

	return false;
}