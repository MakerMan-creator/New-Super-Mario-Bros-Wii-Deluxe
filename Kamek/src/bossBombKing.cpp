#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"
#include "externs.h"

const char* KBBarcNameList [] = { 
	"bombking", 
	"bombhei", 
	NULL 
};

class daBombKing : public daBoss {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	int health = 15;
	daPlBase_c *players[4];
	float initX;
	int timer = 0;

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daBombKing* build();

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

	USING_STATES(daBombKing);
	DECLARE_STATE(Intro);
	DECLARE_STATE(TravelX);
	DECLARE_STATE(Throw);
	DECLARE_STATE(Damage);
	DECLARE_STATE(BlowUp);
	DECLARE_STATE(Outro);
};

CREATE_STATE(daBombKing, Intro);
CREATE_STATE(daBombKing, TravelX);
CREATE_STATE(daBombKing, Throw);
CREATE_STATE(daBombKing, Damage);
CREATE_STATE(daBombKing, BlowUp);
CREATE_STATE(daBombKing, Outro);

void daBombKing::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daBombKing::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daBombKing* daBombKing::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBombKing));
	return new(buffer) daBombKing;
}


int daBombKing::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("bombking", "g3d/bombking.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("bombking");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	this->scale.x = 4.0; 
	this->scale.y = 4.0; 
	this->scale.z = 4.0;

	this->direction = 1;

	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 41.0; 
	HitMeBaby.xDistToEdge = 26.0; 
	HitMeBaby.yDistToEdge = 32.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFFEFAE; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 0.5);

	rot.y = (direction) ? 0x2800 : 0xD800;
	initX = pos.x;

	doStateChange(&StateID_Intro);

	this->onExecute();

	return true;
}

int daBombKing::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	rot.y = (direction) ? 0x2800 : 0xD800;
	
	dStateBase_c *cs = acState.getCurrentState();

	if ((cs == &StateID_TravelX) || (cs == &StateID_Intro)) {
		if (this->animationChr.isAnimationDone()) {
		    this->animationChr.setCurrentFrame(0.0);
	    }
	}

	return true;
}

int daBombKing::onDelete() {
	return true;
}

int daBombKing::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daBombKing::beginState_Intro() {
	this->timer = 0;

	StopBGMMusic();

	// Set the necessary Flags and make Mario enter Demo Mode
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	WLClass::instance->_4 = 4;
	WLClass::instance->_8 = 0;

	MakeMarioEnterDemoMode();

	for (int i = 0; i < 4; i++)
	{
		players[i] = GetPlayerOrYoshi(i);

		if (players[i]) {
			players[i]->direction ^= 1;
		}
	}
}
void daBombKing::executeState_Intro() {
	if ((animationChr.getCurrentFrame() == 5) || 
	(animationChr.getCurrentFrame() == 15)) {
        PlaySound(this, SE_PLY_FOOTNOTE_DIRT);
	}

	if (pos.x < (initX + 80.0)) { 
		pos.x = (direction) ? pos.x + 0.5 : pos.x - 0.5; 
	} else {
        animationChr.setCurrentFrame(animationChr.getCurrentFrame());

        if (timer > 70) {
			doStateChange(&StateID_TravelX);
		}

		timer++;
	}
}
void daBombKing::endState_Intro() {
	for (int i = 0; i < 4; i++)
	{
		players[i] = GetPlayerOrYoshi(i);

		if (players[i]) {
			players[i]->direction ^= 1;

			if (i == 0) { PlaySound(this, SE_VOC_MA_QUAKE); }
			if (i == 1) { PlaySound(this, SE_VOC_LU_QUAKE); }
			if (i == 2) { PlaySound(this, SE_VOC_KO_QUAKE); }
			if (i == 3) { PlaySound(this, SE_VOC_KO2_QUAKE); }
		}
	}

	dStage32C_c::instance->freezeMarioBossFlag = 0;
	WLClass::instance->_8 = 1;

	MakeMarioExitDemoMode();
	StartBGMMusic();
}

void daBombKing::beginState_TravelX() {
	this->timer = 0;

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 0.5);

	if (acState.getPreviousState() == &StateID_Throw) {
		animationChr.setCurrentFrame(14.0);
	}
}
void daBombKing::executeState_TravelX() {
	u8 facing = (dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos) ^ 1);

	pos.x = (direction) ? pos.x + 1.0 : pos.x - 1.0; 

	direction = facing;

    if (timer > 400) {
		doStateChange(&StateID_Throw);
	}

	timer++;
}
void daBombKing::endState_TravelX() {}

void daBombKing::beginState_Throw() {
	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1);
	this->timer = 0;
}
void daBombKing::executeState_Throw() {
	if (animationChr.getCurrentFrame() == 9) {
		dEn_c *bomb = (dEn_c*)CreateActor(133, 0x2, (Vec){(direction) ? (pos.x - 20) : (pos.x + 20), pos.y, pos.z}, 0, 0);

		bomb->direction = this->direction;
		bomb->speed.x = (bomb->direction) ? 4.0 : -4.0;
		bomb->speed.y = 8.0;
	}

	if (animationChr.getCurrentFrame() == 14) {
		doStateChange(&StateID_TravelX);
	}
}
void daBombKing::endState_Throw() {}

void daBombKing::beginState_Damage() {
	bindAnimChr_and_setUpdateRate((health <= 0) ? "stop" : "carry", 1, 0.0, 1);
	this->timer = 0;
	PlaySound(this, SE_EMY_BIG_DOSSUN_DEAD);
}
void daBombKing::executeState_Damage() {
	if (this->health <= 0) {
		doStateChange(&StateID_BlowUp);
	}

	if (animationChr.isAnimationDone()) {
		animationChr.setCurrentFrame(0.0);

		if (timer > 4) {
            doStateChange(&StateID_TravelX);
		}

		timer++;
	}
}
void daBombKing::endState_Damage() {}

void daBombKing::beginState_BlowUp() { 
	this->timer = 0; 

	this->removeMyActivePhysics();

	StopBGMMusic();

	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;

	BossGoalForAllPlayers();

	for (int i = 0; i < 4; i++)
	{
		players[i] = GetPlayerOrYoshi(i);

		if (players[i]) {
			players[i]->direction = (this->pos.x < players[i]->pos.x);
		}
	}
}
void daBombKing::executeState_BlowUp() {
	if (timer > 390) {
		doStateChange(&StateID_Outro);
	}
	else if (timer > 240) {
		PlaySound(this, SE_EMY_KERONPA_ALARM);
	}

	timer++;
}
void daBombKing::endState_BlowUp() {
	this->scale = (Vec){0.0, 0.0, 0.0};

	S16Vec nullRot = {0,0,0};

    Vec oneVec = {3.0f, 3.0f, 3.0f};
	SpawnEffect("Wm_en_explosion", 0, &this->pos, &nullRot, &oneVec);

	Vec smokeScale = {4.5f, 4.5f, 4.5f};
	SpawnEffect("Wm_en_explosion_smk", 0, &this->pos, &nullRot, &smokeScale);

	PlaySound(this, SE_EMY_KERONPA_BOMB);
}

void daBombKing::beginState_Outro() { 
	this->timer = 0;
	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_BOSS_CMN_DAMAGE_LAST, 1);
}
void daBombKing::executeState_Outro() {
    if (timer == 70) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_SHIRO_BOSS_CLEAR, 1);
		PlayerVictoryCries(this);
	}
	else if (timer > 250) {
		ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
	}

	timer++;
}
void daBombKing::endState_Outro() {}

void daBombKing::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
}
	void daBombKing::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		dEn_c *enemy = (dEn_c*)apOther->owner;
		u16 name = enemy->name;

		if (name == EN_BUBBLE || name == EN_FIRESNAKE || name == DUMMY_DOOR_PARENT) {
			doStateChange(&StateID_BlowUp);
		}
	}
	void daBombKing::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	    DamagePlayer(this, apThis, apOther);
    }

	bool daBombKing::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	} 
	bool daBombKing::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		dEn_c *actor = (dEn_c*)apOther->owner;

		actor->collisionCat9_RollingObject(apThis, apOther);

		if (actor->name == EN_BOMHEI) {
			if (acState.getCurrentState() == &StateID_Damage) {
				return false;
			}

			this->health -= 3;

			doStateChange(&StateID_Damage);

			return true;
		}

		return false;
	} 
	bool daBombKing::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}  
	bool daBombKing::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	} 