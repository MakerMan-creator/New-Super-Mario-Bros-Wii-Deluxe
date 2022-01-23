#ifndef BOWSER_JR
#define BOWSER_JR

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

//Bowser Jr
class daBowserJr : public daBoss {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	int health;
	float sped;
	int timer;
	bool letsEndStage;
	int fireballHits = 0;
	char isHardBoss;
	int direction = 0;
	u8 facing;
	bool isBouncing;
	u32 cmgr_returnValue;
	bool hasAlreadyPlayed;
	dStageActor_c *koopa;
	bool falling;
	int Is3Or5;

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daBowserJr* build();

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

	bool calculateTileCollisions();
	bool willWalkOnSuitableGround();


	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daBowserJr);
	DECLARE_STATE(Intro);
	DECLARE_STATE(Attack);
	DECLARE_STATE(Chase);
	DECLARE_STATE(Damage);
	DECLARE_STATE(Outro);
	DECLARE_STATE(Turn);
};

// Bowser Jr's shell
class daCustomShell : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void _vf14C();
	void _vf148();
	bool CreateIceActors();
	//int KoopaJrDirection();

    dEn_c *shell;
    bool isBouncing;
    u32 cmgr_returnValue;
    int direction;
    f32 posAdd = 1.4;

	void updateModelMatrices();
	
	static daCustomShell* build();

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

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daCustomShell);
	DECLARE_STATE(Slide);
	DECLARE_STATE(Gone);
};

#endif