#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

const char* NinjiArcNameList [] = { 
    "ninji",
    NULL
};
class daNinji_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

    int distance;
    int type;
	int direction;
	float initXpos;
	u32 cmgr_returnValue;
	int health = 4;
	bool isBouncing;
	int timer;
	int jumps = 0;
	bool falling;
	float xAdd = 1.0;

	m3d::anmChr_c chrAnimation;

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daNinji_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
    void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);

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

	bool calculateTileCollisions();
	bool walkOnOkayGround();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daNinji_c);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Hop);
	DECLARE_STATE(Idle);
	DECLARE_STATE(Die);
	DECLARE_STATE(DieFire);
};
extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

CREATE_STATE(daNinji_c, Walk);
CREATE_STATE(daNinji_c, Hop);
CREATE_STATE(daNinji_c, Idle);
CREATE_STATE(daNinji_c, Die);
CREATE_STATE(daNinji_c, DieFire);

void daNinji_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}
void daNinji_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}
daNinji_c* daNinji_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daNinji_c));
	return new(buffer) daNinji_c;
}
int daNinji_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("ninji", "g3d/ninji.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("ninji");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Player(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	this->distance = (this->settings & 0x000F0000) >> 16;
	this->type = this->settings >> 28 & 0xF;
	this->direction = this->settings >> 12 & 0xF;

	if (this->direction > 1) {
        this->direction -= (this->direction - 1);
	}

	allocator.unlink(); 

	ActivePhysics::Info HitMeBaby;

	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 11.0; 
	HitMeBaby.xDistToEdge = 7.0; 
	HitMeBaby.yDistToEdge = 7.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFDFEFAE; 
	HitMeBaby.unkShort1C = 0;

	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	this->initXpos = this->pos.x;

	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	static const lineSensor_s below(-5<<12, 5<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 10<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile()) {
		isBouncing = false;
	} else {
		isBouncing = true;
    }

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);

    //State switching 
    if (this->type == 2) {
		OSReport("Time to start hopping!\n");
		doStateChange(&StateID_Hop);
	} else if (this->type < 2) {
		OSReport("Checking my direction.. Ah, it's %d!\n", this->direction);
	    doStateChange(&StateID_Walk);
	} else {
		doStateChange(&StateID_DieFire);
	}

	this->onExecute();

	return true;
}
int daNinji_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone() && acState.getCurrentState() != &StateID_Die) {
		this->animationChr.setCurrentFrame(0.0);
	}

	if (this->type == 2 && acState.getCurrentState() != &StateID_Die) {
		u8 facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
		if (facing != this->direction) {
			this->direction = facing;
		}
	}

	this->rot.y = (this->direction == 0) ? 0x2800 : 0xD800;

    //Checks whether the enemy fell into a bottomless pit or not
	float rect[] = {0.0, 0.0, 38.0, 38.0};
	int gone = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
	if (gone) {
		this->Delete(1);
	}

	return true;
}
int daNinji_c::onDelete() {
    OSReport("Welp, I'm outta here.\n");

	return true;
}
int daNinji_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

//Taking A Morning Jog
void daNinji_c::beginState_Walk() {
	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);
	this->timer = 0;
}
void daNinji_c::executeState_Walk() {
    dStateBase_c *otherWay;
	otherWay = (!type) ? &StateID_Hop : &StateID_Idle;

    pos.x = (direction == 1) ? pos.x - this->xAdd : pos.x + this->xAdd;

	if (distance) {
		//Does this if a specific distance is set (but not 0)
        if (this->direction == 1) {
		    if (this->pos.x <= (this->initXpos - (14.0 * distance))) {
				doStateChange(otherWay);
			}
		} else {
			if (this->pos.x >= (this->initXpos + (14.0 * distance))) {
				doStateChange(otherWay);
			}
		}
	} else {
		if (collMgr.isOnTopOfTile()) {
			this->falling = false;

			if (!walkOnOkayGround()) {
				pos.x = (direction) ? pos.x - this->xAdd : pos.x + this->xAdd;
				doStateChange(otherWay);
            }
		} else {
			if (!this->falling) {
				this->falling = true;
				pos.x = direction ? pos.x + this->xAdd : pos.x - this->xAdd;
				doStateChange(otherWay);
			}
		}
	}

	bool ret = calculateTileCollisions();
	if (ret) {
		OSReport("I just collided with a wall, so it's time to change my direction from %d to %d.\n", this->direction, (this->direction ^ 1));
		pos.x = pos.x + 0.0f;
		doStateChange(otherWay);
	}
}
void daNinji_c::endState_Walk() {}

//Jump Up, Superst-- Nope
void daNinji_c::beginState_Hop() {
	this->timer = 0;
	this->speed.x = 0.0;
    if (this->type != 2) {
		this->direction ^= 1;
		this->speed.y = 7.0;
		PlaySound(this, SE_PLY_JUMPDAI);
	} else {
		if (this->jumps < 2) {
			this->speed.y = (this->jumps == 1) ? 8.5 : 7.5;
			this->jumps++;
			PlaySound(this, SE_PLY_JUMPDAI);
		} else {
			this->speed.y = 9.5;
			this->jumps = 0;
			PlaySound(this, SE_PLY_JUMPDAI_HIGH);
		}
	}
}
void daNinji_c::executeState_Hop() {
	//This code makes the enemy hop around
    if (this->speed.y >= 0) {
		this->HandleXSpeed();
		this->HandleYSpeed();
		this->UpdateObjectPosBasedOnSpeedValuesReal();
		this->speed.y -= 0.25;
	} else {
		this->calculateTileCollisions();
        if (isBouncing) {
		    dStateBase_c *switchState;
			switchState = (this->type == 2) ? &StateID_Idle : &StateID_Walk;
			doStateChange(switchState);
		}
	}
}
void daNinji_c::endState_Hop() {}

//Stay In Place
void daNinji_c::beginState_Idle() {
	this->timer = 0;
	this->speed.x = 0.0;
	if (this->type != 2) {
		this->direction ^= 1;
	}
}
void daNinji_c::executeState_Idle() {
	this->calculateTileCollisions();
	if (this->type == 0) { 
	    doStateChange(&StateID_Hop);
	} else {
		//Can't switch state while not being on the ground
        if (this->timer > 65 && isBouncing) {
			dStateBase_c *change;
			change = (this->type == 1) ? &StateID_Walk : &StateID_Hop;
            doStateChange(change);
		} else if (!isBouncing) {
			this->timer = 0;
		}
		this->timer++;
	}
}
void daNinji_c::endState_Idle() {}

//Default Death
void daNinji_c::beginState_Die() {
	this->timer = 0;
	this->removeMyActivePhysics();
	bindAnimChr_and_setUpdateRate("damage", 1, 0.0, 1.0);
}
void daNinji_c::executeState_Die() {
	this->calculateTileCollisions();
	if (this->animationChr.isAnimationDone() && acState.getCurrentState() == &StateID_Die) {
		S16Vec nullRot = {0,0,0};
	    Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_mr_cmnspark", 0, &pos, &nullRot, &oneVec);
		this->Delete(1);
	}
}
void daNinji_c::endState_Die() {}

//Alternate "Death"
void daNinji_c::beginState_DieFire() {
	this->timer = 0;
	this->removeMyActivePhysics();
	PlaySound(this, SE_EMY_KURIBO_M_DAMAGE);
}
void daNinji_c::executeState_DieFire() {
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_cmnsndlandsmk", 0, &pos, &nullRot, &oneVec);
	SpawnEffect("Wm_en_landsmoke", 0, &pos, &nullRot, &oneVec);
	SpawnEffect("Wm_en_sndlandsmk_s", 0, &pos, &nullRot, &oneVec);
	this->Delete(1);
}
void daNinji_c::endState_DieFire() {}

void daNinji_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    char hitType;
    hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
    if (hitType == 1 || hitType == 3) {
        this->health -= 2;
		if (this->health <= 0) {
            doStateChange(&StateID_Die);
	    }
    } else if (hitType == 0) {
        this->_vf220(apOther->owner);
    }
}
void daNinji_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->playerCollision(apThis, apOther);
}
void daNinji_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    u16 name = ((dEn_c*)apOther->owner)->name;
	if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
	|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND
	|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
	|| name == EN_JUMPDAI || name == EN_ITEM)
	{ return; }

	if (acState.getCurrentState() == &StateID_Walk) {
		dStateBase_c *switchState;
		switchState = (type == 0) ? &StateID_Hop : &StateID_Idle;
		doStateChange(switchState);
	}
}
bool daNinji_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_DieFire);
    return true;
}
bool daNinji_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
    return this->collisionCat3_StarPower(apThis, apOther);
}
bool daNinji_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
    return this->collisionCat3_StarPower(apThis, apOther);
}
bool daNinji_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {
    return true;
} 
bool daNinji_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
    return this->collisionCat3_StarPower(apThis, apOther);
}
bool daNinji_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
    return this->collisionCat3_StarPower(apThis, apOther);
} 
bool daNinji_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
    return this->collisionCat3_StarPower(apThis, apOther);
} 
bool daNinji_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat3_StarPower(apThis, apOther);
} 
bool daNinji_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat3_StarPower(apThis, apOther);
} 
bool daNinji_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
	return this->collisionCat3_StarPower(apThis, apOther);
}
bool daNinji_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 3, 0);
	return this->collisionCat3_StarPower(apThis, apOther);
} 
bool daNinji_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}
bool daNinji_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	//Does nothing, so...
	OSReport("Ha ha! Your iceballs don't work on me!\n");
	PlaySound(this, SE_EMY_ICE_BROS_ICE);
	return false;
}
bool daNinji_c::calculateTileCollisions() {
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
bool daNinji_c::walkOnOkayGround() {
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