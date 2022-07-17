#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"


const char* FinalBowserFileList[] = { 
	"koopa", 
    "boss_ef_attack", 
    "choropoo",
	"koopa_clown_bomb",
	"dossun", 
	"iron_ball",
	NULL 
};

class daEnFinalBowser_c : public daBoss {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daEnFinalBowser_c* build();

	daPlBase_c *players[4];
	char *willAutoscroll;
	int direction;
	float initPlayerX[4];
	bool GotInitX[4];
	int there = 0;
	bool buttonSet[4];
	float initYPos, initXPos;
	dEn_c *fire;
	int health = 30;
	int phases;
	int timerAlt = 0;
	ActivePhysics face;
	int fireballTime = 0;
	u8 event;
	int waitTime = 100;
	int wrenchAmount = 3;
	bool animSet;
	int number, num;

	void powBlockActivated(bool isNotMPGP);

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

	USING_STATES(daEnFinalBowser_c);
	DECLARE_STATE(PlayerLook);
	DECLARE_STATE(PlayerOutOfWay);
	DECLARE_STATE(Jump);
	DECLARE_STATE(Land);
	DECLARE_STATE(Roar);
	DECLARE_STATE(Walk);
	DECLARE_STATE(WaitForAutoscroll);
	DECLARE_STATE(SpitFire);
	DECLARE_STATE(ThrowShit);
	DECLARE_STATE(JumpAlt);
	DECLARE_STATE(Damage);
	DECLARE_STATE(EndPhase1);
	DECLARE_STATE(BeginPhase2);
	DECLARE_STATE(BossEnd);
};

void BowserCollCallBack(ActivePhysics *apThis, ActivePhysics *apOther);

extern u32 GameTimer;
extern "C" void *StageScreen;
extern "C" int CheckExistingPowerup(void * Player);
extern "C" void *dAcPy_c__ChangePowerupWithAnimation(void * Player, int powerup);

CREATE_STATE(daEnFinalBowser_c, PlayerLook);
CREATE_STATE(daEnFinalBowser_c, PlayerOutOfWay);
CREATE_STATE(daEnFinalBowser_c, Jump);
CREATE_STATE(daEnFinalBowser_c, Land);
CREATE_STATE(daEnFinalBowser_c, Roar);
CREATE_STATE(daEnFinalBowser_c, WaitForAutoscroll);
CREATE_STATE(daEnFinalBowser_c, Walk);
CREATE_STATE(daEnFinalBowser_c, SpitFire);
CREATE_STATE(daEnFinalBowser_c, ThrowShit);
CREATE_STATE(daEnFinalBowser_c, JumpAlt);
CREATE_STATE(daEnFinalBowser_c, Damage);
CREATE_STATE(daEnFinalBowser_c, EndPhase1);
CREATE_STATE(daEnFinalBowser_c, BeginPhase2);
CREATE_STATE(daEnFinalBowser_c, BossEnd);

void daEnFinalBowser_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void BowserCollCallBack(ActivePhysics *apThis, ActivePhysics *apOther) {
	if ((apOther->owner->name != 544) && (apOther->owner->name != 261)) {
		dEn_c::collisionCallback(apThis, apOther);
	}
}

// To set a timer
#define time *(u32*)((GameTimer) + 0x4)

void daEnFinalBowser_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daEnFinalBowser_c* daEnFinalBowser_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnFinalBowser_c));
	return new(buffer) daEnFinalBowser_c;
}

extern char CameraLockEnabled;
extern VEC2 CameraLockPosition;

int daEnFinalBowser_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("koopa", "g3d/koopa.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("koopa");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Boss(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("kp_walk");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	phases = ((settings >> 28 & 0xF) % 2) + 1;

	if (phases == 1) {
		event = ((settings >> 24 & 0xF) - 1);
	}

	if (phases == 1) { this->scale = (Vec){0.0,0.0,0.0}; }
	else { this->scale = (Vec){6.25,6.25,6.25}; }

	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 110.0; 
	HitMeBaby.xDistToEdge = 106.0; 
	HitMeBaby.yDistToEdge = 130.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFCE4EA6; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &BowserCollCallBack; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 

    /*if (phases == 1) {
		ActivePhysics::Info HitMeBabyTwo;

		HitMeBabyTwo.xDistToCenter = -185.0; 
		HitMeBabyTwo.yDistToCenter = 159.0; 
		HitMeBabyTwo.xDistToEdge = 17.5; 
		HitMeBabyTwo.yDistToEdge = 15.0; 
		HitMeBabyTwo.category1 = 0x3; 
		HitMeBabyTwo.category2 = 0x0; 
		HitMeBabyTwo.bitfield1 = 0x4F; 
		HitMeBabyTwo.bitfield2 = 0xFFCFCEA6; 
		HitMeBabyTwo.unkShort1C = 0; 
		HitMeBabyTwo.callback = &BowserCollCallBack; 
		this->face.initWithStruct(this, &HitMeBabyTwo); 
	}*/

	const char* AnimName = (phases == 1) ? "kp_jump" : "kp_death2";

	bindAnimChr_and_setUpdateRate(AnimName, 1, 0.0, ((phases == 1) ? 1.0 : 0.1));

	// Hard coding positions so the boss spawns properly

	if (phases == 1) {
		pos.x = 5856;
		pos.y = 1664;
		initYPos = pos.y;
		initXPos = pos.x;
		rot.y = -0x4000;
	    time = (800 << 0xC) - 1;
	} else {
        for (int i = 0; i < 2; i++)
        { rot.y += 0x4000; }

        pos.x = 11608;
		pos.y = -5256;
	}

	direction = 0;

	if (phases == 1) { doStateChange(&StateID_PlayerLook); }
	else { doStateChange(&StateID_BeginPhase2); }

	if (dFlagMgr_c::instance->active(event)) {
		dFlagMgr_c::instance->set(event, 0, false, false, false);
	}

	char *autoscrInfo = ((char*)dBgGm_c::instance) + 0x900AC;
	this->willAutoscroll = autoscrInfo;

	this->onExecute();

	return true;
}

int daEnFinalBowser_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	dStateBase_c *s = acState.getCurrentState();

	if ((s != &StateID_PlayerLook) && (s != &StateID_PlayerOutOfWay) && (s != &StateID_Jump) && 
		(s != &StateID_Land) && (s != &StateID_Roar)) {

		if (direction) {
			CameraLockPosition.x = (pos.x + (16 * 8));
			CameraLockPosition.y = -(pos.y + (16 * 2));
		}
		else {
			CameraLockPosition.x = (pos.x - (16 * 8));
			CameraLockPosition.y = -(pos.y + (16 * 2));
		}

		checkZoneBoundaries(0);

	    if (acState.getCurrentState() == &StateID_Walk) {
	    	if (pos.x > 808.0) {
	    		if (this->isOutOfView()) {
	    		    speed.x = (direction) ? 1.0 : -1.0;
	    		} else {
	    		    for (int i = 0; i < 4; i++) {
	    	            players[i] = GetPlayerOrYoshi(i);
	    	
	    	            if (players[i]) {
	    	                if (direction == 0) {
	    	                    if ((this->pos.x > (players[i]->pos.x + 144))) {
	    	                    	   speed.x = -0.8;
	    	                    } else {
	    	                    	   speed.x = -0.6;
	    	                    }
	    	                } else {
	    	                    if (this->pos.x < (players[i]->pos.x - 144)) {
	    	                    	   speed.x = 0.8;
	    	                    } else {
	    	                    	   speed.x = 0.6;
	    	                    }
	    	                }
	    	            }
	    	
	    		    }
	    	
	    		}
	        } else {
	        	pos.x = 808.0;
	        }
	    }
	    
	}

	return true;
}

int daEnFinalBowser_c::onDelete() {
	this->aPhysics.removeFromList();
    this->face.removeFromList();

	return true;
}

int daEnFinalBowser_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

static float manipFourPlayerPos(int id, float pos) {
	int fromRight = 3 - id;
	return pos - (fromRight * 20.0f);
}

// ALL the states

void daEnFinalBowser_c::beginState_PlayerLook() {
	this->timer = 0;

	StopBGMMusic();
	
	// Set the necessary Flags and make Mario enter Demo Mode
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	WLClass::instance->_4 = 4;
	WLClass::instance->_8 = 0;
	
	MakeMarioEnterDemoMode();

	_120 |= 8;
	lookAtMode = 2;
}
void daEnFinalBowser_c::executeState_PlayerLook() {
	if (timer >= 100) {
		_120 &= ~8;
	}

	if (timer == 25) {
		for (int i = 0; i < 4; i++) {
		    players[i] = GetPlayerOrYoshi(i);

		    if (players[i]) {
			    SFX look[4] = { 
				    SE_VOC_MA_CS_NOTICE_JR, 
				    SE_VOC_LU_CS_NOTICE_JR,
			        SE_VOC_KO_CS_NOTICE_JR, 
			        SE_VOC_KO2_CS_NOTICE_JR 
			    };
			
			    nw4r::snd::SoundHandle handle;
			    PlaySoundWithFunctionB4(SoundRelatedClass, &handle, look[Player_ID[i]], 1);
		    }
	    }
	}

	if (timer >= 125) {
		doStateChange(&StateID_PlayerOutOfWay);
		return;
	}

	timer++;
}
void daEnFinalBowser_c::endState_PlayerLook() {}

void daEnFinalBowser_c::beginState_PlayerOutOfWay() {
	timerAlt = 73;
	// The beginning will use this to start moving the players
	for (int i = 0; i < 4; i++) {
		players[i] = GetPlayerOrYoshi(i);
	
		if (players[i]) {
			//if (!this->GotInitX[i]) {
                initPlayerX[i] = players[i]->pos.x;

            //   GotInitX[i] = true;
			//}

			if (i > 1) {
				timerAlt = (73 * i);
			}
		}
	}
}
void daEnFinalBowser_c::executeState_PlayerOutOfWay() {
	for (int i = 0; i < 4; i++) {
		players[i] = GetPlayerOrYoshi(i);

		if (players[i]) {
			if (!(players[i]->isReadyForDemoControlAction())) {
				return;
			}

			// Setting necessary flags...

			//if (timerAlt > 0) {
                if (!buttonSet[i]) {
                	if (players[i]->isReadyForDemoControlAction()) {
                		players[i]->demoMoveSpeed = 0.0;
                		players[i]->direction ^= 1;

                		players[i]->input.setPermanentForcedButtons(WPAD_LEFT);
                		//players[i]->input.setPermanentForcedButtons(WPAD_ONE);

                		buttonSet[i] = true;
                	}
                }

            //    timerAlt--;
			//} else 
            if (players[i]->pos.x <= (initPlayerX[i] - 160.0)) {
            	players[i]->pos.x = (initPlayerX[i] - 160.0);
				if (buttonSet[i]) {
					players[i]->input.unsetPermanentForcedButtons(WPAD_LEFT);
					//players[i]->input.unsetPermanentForcedButtons(WPAD_ONE);

					players[i]->direction ^= 1;

					buttonSet[i] = false;

					//return;
				}

				if (timer > 130) {
					doStateChange(&StateID_Jump);
					return;
				}

				if ((timer == 30) || (timer == 60) || (timer == 95)) {
					players[i]->input.setPermanentForcedButtons(WPAD_TWO);
				}

				if ((timer == 32) || (timer == 62) || (timer == 97)) {
					players[i]->input.unsetPermanentForcedButtons(WPAD_TWO);
				}

				timer++;
			}
		}
	}
}
void daEnFinalBowser_c::endState_PlayerOutOfWay() {
	timer = 0;

	this->scale.x = 6.255; 
	this->scale.y = 6.255; 
	this->scale.z = 6.255;
}

void daEnFinalBowser_c::beginState_Jump() {
	this->aPhysics.addToList();
    //this->face.addToList();

    bindAnimChr_and_setUpdateRate("kp_jump", 1, 0.0, 0.9);

    speed.y = -3;
    speed.x = 0.0;
}
void daEnFinalBowser_c::executeState_Jump() {
    if (pos.y <= -600) {
    	pos.y = -600;

    	if (animationChr.isAnimationDone()) {
    		doStateChange(&StateID_Land);
    		return;
	    }

	    return;
    }

	pos.y += speed.y;
	pos.x += speed.x;

    /*HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();*/
}
void daEnFinalBowser_c::endState_Jump() {}

void daEnFinalBowser_c::beginState_Land() {
	bindAnimChr_and_setUpdateRate("kp_jump_ed", 1, 0.0, 1.0);

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_BOSS_KOOPA_L_LAND, 1);

	for (int i = 0; i < 4; i++) {
		players[i] = GetPlayerOrYoshi(i);

		if (players[i]) {
			players[i]->speed.y = 5.5;
		}
	}
}
void daEnFinalBowser_c::executeState_Land() {
    pos.y = -600;

    if (timer < 3) { ShakeScreen(StageScreen, 2, 1, 0, 0); }

	if (animationChr.isAnimationDone()) {
        doStateChange(&StateID_Roar);
	}

	timer++;
}
void daEnFinalBowser_c::endState_Land() {
	if (!dFlagMgr_c::instance->active(30)) {
		dFlagMgr_c::instance->set(30, 0, true, false, false);
	}
}

void daEnFinalBowser_c::beginState_Roar() {
	bindAnimChr_and_setUpdateRate("kp_roar3", 1, 0.0, 1.0);

	if (acState.getPreviousState() != &StateID_Damage) { StartBGMMusic(); }
	else {
		dStage32C_c::instance->freezeMarioBossFlag = 1;
	    WLClass::instance->_4 = 4;
	    WLClass::instance->_8 = 0;
	
	    MakeMarioEnterDemoMode();

        for (int i = 0; i < 4; i++)
		{
			players[i] = GetPlayerOrYoshi(i);

			if (players[i]) {
				players[i]->direction = (players[i]->pos.x > this->pos.x);
			}
		}
	}

	timer = 0;
}
void daEnFinalBowser_c::executeState_Roar() {
	pos.y = -600;

	if (this->animationChr.getCurrentFrame() == 53.0) {
		nw4r::snd::SoundHandle handle;
	    PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_VOC_KP_L_SHOUT, 1);
    }
    else if ((this->animationChr.getCurrentFrame() > 53.0)) {
    	ShakeScreen(StageScreen, 2, 1, 0, 0);

    	if (acState.getPreviousState() != &StateID_Damage) {
		    for (int i = 0; i < 4; i++) {
			    players[i] = GetPlayerOrYoshi(i);
    
			    if (players[i] != 0) {
				    players[i]->pos.x -= 0.5;
			    }
		    }
	    }
    }

    if (animationChr.isAnimationDone()) {
    	if (this->health <= 0) {
    		doStateChange(&StateID_EndPhase1);
    	} else {
    	    doStateChange(&StateID_WaitForAutoscroll); 
    	}
    }
}
void daEnFinalBowser_c::endState_Roar() {
	dStage32C_c::instance->freezeMarioBossFlag = 0;
	WLClass::instance->_8 = 1;
	
	MakeMarioExitDemoMode();

	for (int i = 0; i < 4; i++)
	{
		players[i] = GetPlayerOrYoshi(i);

		if (players[i]) {
			players[i]->direction ^= 1;
		}
	}
}

void daEnFinalBowser_c::beginState_WaitForAutoscroll() {
	bindAnimChr_and_setUpdateRate("kp_wait", 1, 0.0, 1.0);
}
void daEnFinalBowser_c::executeState_WaitForAutoscroll() {
	OSReport("waitTime = %d;\n", waitTime);

    if (waitTime < 1) {
    	doStateChange(&StateID_Walk);
    	return;
    }

    waitTime--;
}
void daEnFinalBowser_c::endState_WaitForAutoscroll() {}

void daEnFinalBowser_c::beginState_Walk() {
	bindAnimChr_and_setUpdateRate("kp_walk", 1, 0.0, 0.75);

	int ab7 = AbsVal(GenerateRandomNumber(7));

	/*number = AbsVal(GenerateRandomNumber(2));
            
    number = (number % 3);*/
    ab7 = ((ab7 % 7) + 1);

	this->fireballTime = (110 * ab7);
}
void daEnFinalBowser_c::executeState_Walk() {
	pos.y = -600;

	// Timer for the items to be spawned

	if (this->fireballTime < 0) {
		if (this->animationChr.isAnimationDone()) {
			doStateChange(&StateID_ThrowShit);
			return;
		}
	} else {
		fireballTime--;
	}

	if (animationChr.getCurrentFrame() == 48) {
		if (!this->isOutOfView()) { 
			nw4r::snd::SoundHandle handle;
	        PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_BOSS_KOOPA_L_FOOT, 1); 
			ShakeScreen(StageScreen, 2, 1, 0, 0);
		}
	}

	if (animationChr.getCurrentFrame() == 83) {
		if (!this->isOutOfView()) { 
			nw4r::snd::SoundHandle handle;
	        PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_BOSS_KOOPA_L_FOOT, 1);
			ShakeScreen(StageScreen, 2, 1, 0, 0);
		}
	}

	if (animationChr.isAnimationDone()) { animationChr.setCurrentFrame(0.0); }

	speed.y = 0;

	pos.y += speed.y;
	pos.x += speed.x;

	/*HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();*/
}
void daEnFinalBowser_c::endState_Walk() {}

void daEnFinalBowser_c::beginState_SpitFire() {
	//*((u8*)(this->willAutoscroll + 0x1A)) = 0;
	bindAnimChr_and_setUpdateRate("fire1", 1, 0.0, 1.0);
}
void daEnFinalBowser_c::executeState_SpitFire() {
	pos.y = -600;

	if (this->isOutOfView()) {
		OSReport("Hey, Mario! WHERE THE HELL DID YOU GO?! Look at him, trying to escape from one of my oldest tricks.\n");
		doStateChange(&StateID_Walk);
        return;
	}

	if (animationChr.getCurrentFrame() == 63) {
		nw4r::snd::SoundHandle handle;
	    PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_BOSS_KOOPA_L_FIRE_SHOT, 1);

		fire = (dEn_c*)CreateActor(261, 0x2, 
			(Vec){(pos.x + (float)((direction) ? (16 * 5) : -(16 * 5))), (pos.y + 800), (pos.z + 8)}, 0, 0);

		fire->speed.x = -1.5;
		
		for (int i = 0; i < 4; i++) {
			players[i] = GetPlayerOrYoshi(i);

			if (players[i]) {
				fire->speed.y = -((fire->pos.y - players[i]->pos.y) / 8.0f);
			}
		}

		OSReport("FirePos: {X; %f, Y; %f, Z; %f}\n", fire->pos.x, fire->pos.y, fire->pos.z);
		OSReport("FireSpeed: {X; %f, Y; %f, Z; %f}\n", fire->speed.x, fire->speed.y, fire->speed.z);

		fire->scale = (Vec){1.5,1.5,1.5};
		fire->direction = this->direction;
	}

	if (animationChr.isAnimationDone()) {
		doStateChange(&StateID_Walk);
	}
}
void daEnFinalBowser_c::endState_SpitFire() {
	//*((u8*)(this->willAutoscroll + 0x1A)) = 1;
}

void daEnFinalBowser_c::beginState_ThrowShit() {
	bindAnimChr_and_setUpdateRate("break", 1, 0.0, 1.0);

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_VOC_KP_L_SWING_S, 1);
}
void daEnFinalBowser_c::executeState_ThrowShit() {
	pos.y = -600;

	if (this->isOutOfView()) {
		OSReport("Hey, Mario! WHERE THE HELL DID YOU GO?! I may be an old man, but I've still got it!\n");
		doStateChange(&StateID_Walk);
        return;
	}

	// Code from Captain Bowser and slightly tweaked to work better here. Thanks, Newer Team! :D

	if (this->animationChr.getCurrentFrame() == 60.0) { // throw back
		for (int i = 0; i < 3; i++) {
			dEn_c *item = (dEn_c*)CreateActor(544, (direction ^ 1), (Vec){(float)((direction) ? (pos.x + 256) : (pos.x - 256)), (pos.y+144), (pos.z + 8)}, 0, 0);
		
			item->direction = this->direction;

			// item->speed.x += (i * 2.5);
			item->speed.y -= (i * 2.5);

			OSReport("ItemPos: {X; %f, Y; %f, Z; %f}\n", item->pos.x, item->pos.y, item->pos.z);
			OSReport("ItemSpeed: {X; %f, Y; %f, Z; %f}\n", item->speed.x, item->speed.y, item->speed.z);
		}
	}

	if (this->animationChr.getCurrentFrame() == 126.0) { // throw front
		for (int i = 0; i < 5; i++) {
			dEn_c *item = (dEn_c*)CreateActor(544, (direction ^ 1), (Vec){(float)((direction) ? (pos.x + 256) : (pos.x - 256)), (pos.y+144), (pos.z + 8)}, 0, 0);
		
			item->direction = (this->direction ^ 1);

			// item->speed.x += (i * 2.5);
			item->speed.y -= (i * 2.5);
		
			OSReport("ItemPos: {X; %f, Y; %f, Z; %f}\n", item->pos.x, item->pos.y, item->pos.z);
			OSReport("ItemSpeed: {X; %f, Y; %f, Z; %f}\n", item->speed.x, item->speed.y, item->speed.z);
		}
	}

	if (animationChr.isAnimationDone()) {
		doStateChange(&StateID_Walk);
	}
}
void daEnFinalBowser_c::endState_ThrowShit() {
	//*((u8*)(this->willAutoscroll + 0x1A)) = 1;
}

void daEnFinalBowser_c::beginState_JumpAlt() {}
void daEnFinalBowser_c::executeState_JumpAlt() {}
void daEnFinalBowser_c::endState_JumpAlt() {}

void daEnFinalBowser_c::beginState_Damage() {
	bindAnimChr_and_setUpdateRate("grow_big", 1, 0.0, 1.0);
	this->animationChr.setCurrentFrame(9.0);

	PlaySound(this, SE_VOC_KP_DAMAGE_HPDP);

	this->aPhysics.removeFromList();
	//if (phases == 1) { this->face.removeFromList(); }

	if (this->health == 15) {
		if (dFlagMgr_c::instance->active(event)) {
            dFlagMgr_c::instance->set(event, 0, false, false, false);
		}
	}
}
void daEnFinalBowser_c::executeState_Damage() {
	pos.y = -600;

	if (this->animationChr.getCurrentFrame() == 65.0) { // stop it here before it's too late
		if ((this->health == 15) || (this->health <= 0)) {
			doStateChange(&StateID_Roar);
		}
		else {
			doStateChange(&StateID_Walk);
		}
	}
}
void daEnFinalBowser_c::endState_Damage() {
	this->aPhysics.addToList();
	//if (phases == 1) { this->face.addToList(); }
}

void daEnFinalBowser_c::beginState_EndPhase1() {
	bindAnimChr_and_setUpdateRate("jump_st", 1, 0.0, 1.0);
	speed.y = 0.0;
	speed.x = 0.0;
}
void daEnFinalBowser_c::executeState_EndPhase1() {
	if (pos.y > initYPos) {
        this->removeMyActivePhysics();
	    this->scale = (Vec){0.0,0.0,0.0};

	    speed.y = 0.0;

	    if (!dFlagMgr_c::instance->active(event)) {
		    dFlagMgr_c::instance->set(event, 0, true, false, false);
	    }

	    return;
	}

	if (animationChr.isAnimationDone()) {
		if (!animSet) { 
			bindAnimChr_and_setUpdateRate("jump", 1, 0.0, 1.0); 
			this->animSet = true;
		}

		speed.y = 6.0;
	}

	pos.y += speed.y;
}
void daEnFinalBowser_c::endState_EndPhase1() {}

void daEnFinalBowser_c::beginState_BeginPhase2() {}
void daEnFinalBowser_c::executeState_BeginPhase2() {
	if (dFlagMgr_c::instance->active(event)) {
        doStateChange(&StateID_BossEnd);
	}

	if (animationChr.isAnimationDone()) {
		animationChr.setCurrentFrame(0.0);
	}

	speed.y = 1.0;

	pos.y += speed.y;
}
void daEnFinalBowser_c::endState_BeginPhase2() {}

extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

void daEnFinalBowser_c::beginState_BossEnd() {}
void daEnFinalBowser_c::executeState_BossEnd() {}
void daEnFinalBowser_c::endState_BossEnd() {}

// Collisions

void daEnFinalBowser_c::powBlockActivated(bool isNotMPGP) {}

void daEnFinalBowser_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
    int p = CheckExistingPowerup(apOther->owner);
    if ((p != 0) && (p != 3)) {
        dAcPy_c__ChangePowerupWithAnimation(apOther->owner, 0);
    } else { DamagePlayer(this, apThis, apOther); }
}
	void daEnFinalBowser_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
		dEn_c *obj = ((dEn_c*)apOther->owner);

		if ((obj->name == AC_BG_LAVA) || (obj->name == AC_BG_POISON)) {
			this->pos.z = (obj->pos.z - 2);
			return;
		}

		if ((obj->name == EN_HAMMERBROS) || (obj->name == EN_FIREBROS) || (obj->name == EN_BIGKARON) || 
			(obj->name == EN_KARON)) {
			obj->collisionCat9_RollingObject(apThis, apOther);
		}
	}
	void daEnFinalBowser_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
        this->playerCollision(apThis, apOther);
	}

	bool daEnFinalBowser_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){
		if (phases == 2) { return false; }

        dEn_c *acto = (dEn_c*)apOther->owner;
        u16 name = acto->name;

        OSReport("Item Class and State: %s", acto->acState.getCurrentState()->getName());

        if ((name == POW_BLOCK) || (name == AC_LIGHT_BLOCK) || (name == AC_PROP_BLOCK)
        	|| (name == SLIDE_BLOCK) || (name == BLOCK_TARU)) {

        	S16Vec nullRot = {0,0,0};
		    Vec oneVec = {1.0f, 1.0f, 1.0f};

		    SpawnEffect("Wm_en_obakedoor_sm", 0, &acto->pos, &nullRot, &oneVec);
		    SpawnEffect("Wm_mr_yoshistep", 0, &acto->pos, &nullRot, &oneVec);

		    PlaySound(this, SE_EMY_DOWN);

            acto->Delete(1);

        	if ((acState.getCurrentState() == &StateID_Damage) || 
        		(acState.getCurrentState() == &StateID_Roar) || 
        		(acState.getCurrentState() == &StateID_EndPhase1)) {
        		return false;
        	}

        	if (name == POW_BLOCK) {
        		if (this->health <= 0) {
        	        if (acState.getCurrentState() != &StateID_EndPhase1) { doStateChange(&StateID_EndPhase1); }
        	    } else {
        	        if (acState.getCurrentState() != &StateID_Damage) { doStateChange(&StateID_Damage); }
        	        this->health -= 3;
        	    }

        	    OSReport("Health before: %d; Health now: %d;\n", (health + 3), health);
        	}

        } else {
        	S16Vec nullRot = {0,0,0};
		    Vec oneVec = {1.0f, 1.0f, 1.0f};

		    SpawnEffect("Wm_en_obakedoor_sm", 0, &acto->pos, &nullRot, &oneVec);
		    SpawnEffect("Wm_mr_yoshistep", 0, &acto->pos, &nullRot, &oneVec);

		    PlaySound(this, SE_OBJ_WOOD_BOX_BREAK);

            acto->Delete(1);
        }

        return true;
	} 
	bool daEnFinalBowser_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	} 
	bool daEnFinalBowser_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
        return false;
	}