#include <common.h>
#include <game.h>
#include <sfx.h>
#include <g3dhax.h>
#include "boss.h"

//I took some part of the code of "bossFuzzyBear.cpp" ans "mummiPokey.cpp" (of Mihi) because it has properties in comment with this boss 

const char* SBarcNameList[] = {
	"sanbo",
	"sanboBoss",
	NULL
};

class daPowerPokey_c : public daBoss {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c headModelsleep, headModelInvulnerable, headModelVulnerable, BodyModelInvulnerable[4], BodyModelVulnerable[4];
	nw4r::g3d::ResFile resFile;
    m3d::anmClr_c animationClr;

	ActivePhysics bodyPhysics[4];

	int timer;
	float xSpeed;
	float ySpeed = 0.5;
	char damage;
	char dying;
	Vec initialPos;
	float Baseline;
	float storeSpeed;
	char falldown;
	int nbBody;
	bool isInvulnerableHead = false;
	bool isInvulnerableBody = true;
	int direction;
	bool spin;

	static daPowerPokey_c *build();

	void setupHeadSleepModel();
	void setupHeadInvulnerableModel();
	void setupHeadVulnerableModel();
	void setupBodyInvulnerableModel();
	void setupBodyVulnerableModel();
	void updateModelMatricesSleep();
	void updateModelMatricesHeadInvulnerable();
	void updateModelMatricesHeadVulnerable();
	void ModelMatricesBody();


	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);


	USING_STATES(daPowerPokey_c);
	DECLARE_STATE(Intro);
	DECLARE_STATE(Move);
	DECLARE_STATE(Dig);
	DECLARE_STATE(Outro);

};

daPowerPokey_c *daPowerPokey_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daPowerPokey_c));
	return new(buffer) daPowerPokey_c;
}

CREATE_STATE(daPowerPokey_c, Intro);
CREATE_STATE(daPowerPokey_c, Move);
CREATE_STATE(daPowerPokey_c, Dig);
CREATE_STATE(daPowerPokey_c, Outro);


void daPowerPokey_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { DamagePlayer(this, apThis, apOther); }
void daPowerPokey_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { DamagePlayer(this, apThis, apOther); }

bool daPowerPokey_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }
bool daPowerPokey_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	
	apOther->someFlagByte |= 2;

	dActor_c *block = apOther->owner;
	dEn_c *mario = (dEn_c*)block;

	mario->speed.y = -mario->speed.y;
	mario->pos.y += mario->speed.y;

	if (mario->direction == 0) { mario->speed.x = 4.0; }
	else					  { mario->speed.x = -4.0; }

	mario->doSpriteMovement();
	mario->doSpriteMovement();
	return true;
 }
bool daPowerPokey_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->counter_504[apOther->owner->which_player] = 0;
	return this->collisionCat9_RollingObject(apThis, apOther);
}
bool daPowerPokey_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {

	dActor_c *block = apOther->owner;
	dEn_c *blah = (dEn_c*)block;

	if (block->name == AC_LIGHT_BLOCK) {
		
		if (blah->_12C & 3 || strcmp(blah->acState.getCurrentState()->getName(), "daLightBlock_c::StateID_Throw")) {
			return true;			
		}
	}

	blah->doSpriteMovement();
	blah->doSpriteMovement();

	if (this->isInvulnerableHead == true) { return true; }
	
	PlaySound(this, SE_EMY_BLOW_PAKKUN_DOWN);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_kickhit", 0, &blah->pos, &nullRot, &oneVec);

	this->isInvulnerableHead = true;
	this->timer = 0;
	this->damage = this->damage + 5;
	
	if (this->damage >= 20) { doStateChange(&StateID_Outro); }
	else { 
		ModelMatricesBody();
		doStateChange(&StateID_Move); 
	}
	return true;
}
bool daPowerPokey_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }
bool daPowerPokey_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }
bool daPowerPokey_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }
bool daPowerPokey_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}


void daPowerPokey_c::setupHeadSleepModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("sanboBoss", "g3d/sanboBoss.brres");
	nw4r::g3d::ResMdl headSleep = this->resFile.GetResMdl("sanbo_head_sleep");
	nw4r::g3d::ResAnmClr anmClr = this->resFile.GetResAnmClr("sanbo_head_sleep");
	this->animationClr.setup(headSleep, anmClr, &this->allocator, 0, 1);

	this->headModelsleep.setup(headSleep, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&headModelsleep, 0);

	allocator.unlink();
}

void daPowerPokey_c::setupHeadInvulnerableModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("sanboBoss", "g3d/sanboBoss.brres");
	nw4r::g3d::ResMdl headInvulnerable = this->resFile.GetResMdl("sanbo_head");
	nw4r::g3d::ResAnmClr anmClr = this->resFile.GetResAnmClr("sanbo_head");
	this->animationClr.setup(headInvulnerable, anmClr, &this->allocator, 0, 1);

	this->headModelInvulnerable.setup(headInvulnerable, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&headModelInvulnerable, 0);

	allocator.unlink();
}

void daPowerPokey_c::setupHeadVulnerableModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("sanboBoss", "g3d/sanboBoss.brres");
	nw4r::g3d::ResMdl headVulnerable = this->resFile.GetResMdl("sanbo_head_orange");
	nw4r::g3d::ResAnmClr anmClr = this->resFile.GetResAnmClr("sanbo_head");
	this->animationClr.setup(headVulnerable, anmClr, &this->allocator, 0, 1);


	this->headModelVulnerable.setup(headVulnerable, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&headModelVulnerable, 0);

	allocator.unlink();
}


void daPowerPokey_c::setupBodyInvulnerableModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("sanboBoss", "g3d/sanboBoss.brres");
	nw4r::g3d::ResMdl BodyInvulnerable = this->resFile.GetResMdl("sanbo_Body");
	nw4r::g3d::ResAnmClr anmClr = this->resFile.GetResAnmClr("sanbo_Body");
	this->animationClr.setup(BodyInvulnerable, anmClr, &this->allocator, 0, 1);

	for (int i = 0; i < 4; i++) {
		this->BodyModelInvulnerable[i].setup(BodyInvulnerable, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&BodyModelInvulnerable[i], 0);
	}
	
	allocator.unlink();
}

void daPowerPokey_c::setupBodyVulnerableModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("sanboBoss", "g3d/sanboBoss.brres");
	nw4r::g3d::ResMdl BodyVulnerable = this->resFile.GetResMdl("sanbo_orange");
	nw4r::g3d::ResAnmClr anmClr = this->resFile.GetResAnmClr("sanbo_Body");
	this->animationClr.setup(BodyVulnerable, anmClr, &this->allocator, 0, 1);

	for (int i = 0; i < 4; i++) {
		this->BodyModelVulnerable[i].setup(BodyVulnerable, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&BodyModelVulnerable[i], 0);
	}
	
	allocator.unlink();
}


int daPowerPokey_c::onCreate() {
	
	setupHeadSleepModel();

	this->scale = (Vec){2, 2, 2};

	speed.x = 0.0;
	speed.y = 0.0;

	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.xDistToEdge = 20.0;
	HitMeBaby.yDistToEdge = 20.0;
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028E;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	for (int i = 0; i < 4; i++) { this->bodyPhysics[i].initWithStruct(this, &HitMeBaby); }
	this->aPhysics.addToList();

	this->damage = 0;
	this->dying = 0;
	this->direction = 0;
	this->spin = true;
	this->falldown = 0;
	this->nbBody = 0;
	this->initialPos = this->pos;
	
	doStateChange(&StateID_Intro);

	this->onExecute();

	return true;
}

int daPowerPokey_c::onDelete() {
	return true;
}

int daPowerPokey_c::onExecute() {
	acState.execute();
	updateModelMatricesSleep();

	return true;
}

int daPowerPokey_c::onDraw() {
	if (this->damage == 0) {

		headModelsleep.scheduleForDrawing();
		headModelsleep._vf1C();

	} else if (this->isInvulnerableHead == true) {

		headModelInvulnerable.scheduleForDrawing();
		headModelInvulnerable._vf1C();

	} else {

		headModelVulnerable.scheduleForDrawing();
		headModelVulnerable._vf1C();

	}
		
	return true;
}


void daPowerPokey_c::updateModelMatricesSleep() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	headModelsleep.setDrawMatrix(matrix);
	headModelsleep.setScale(&scale);
	headModelsleep.calcWorld(false);
}

void daPowerPokey_c::updateModelMatricesHeadInvulnerable() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	headModelInvulnerable.setDrawMatrix(matrix);
	headModelInvulnerable.setScale(&scale);
	headModelInvulnerable.calcWorld(false);
}

void daPowerPokey_c::updateModelMatricesHeadVulnerable() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);
	
	headModelVulnerable.setDrawMatrix(matrix);
	headModelVulnerable.setScale(&scale);
	headModelVulnerable.calcWorld(false);
}

//this function must create a body put under the head/body or replaces
void daPowerPokey_c::ModelMatricesBody() {
	
	setupBodyInvulnerableModel();

	for (int i = 0; i < 4; i++) {
			
		this->bodyPhysics[i].addToList();

		mMtx partMatrix;
		Vec partPos = this->pos;

		partPos.y = (f32)(this->pos.y - (i * 4));
		
		S16Vec partRot = this->rot;

		partMatrix.translation(partPos.x, partPos.y, partPos.z);
		partMatrix.applyRotationYXZ(&partRot.x, &partRot.y, &partRot.z);

		this->BodyModelInvulnerable[i].setDrawMatrix(partMatrix);
		this->BodyModelInvulnerable[i].setScale(&scale);
		this->BodyModelInvulnerable[i].calcWorld(false);

		this->bodyPhysics[i].info.xDistToCenter = partPos.x - this->pos.x;
		this->bodyPhysics[i].info.yDistToCenter = partPos.y - this->pos.y;

		this->nbBody += 1;
	}
		
}


//State_Intro
void daPowerPokey_c::beginState_Intro(){
	this->timer = 0;
	//this->xSpeed;
}

void daPowerPokey_c::executeState_Intro(){
	this->timer++;
}

void daPowerPokey_c::endState_Intro(){ this->Baseline = this->pos.y + 5; }


//State_Move
void daPowerPokey_c::beginState_Move(){
	if (this->direction == 0) { this->speed.x = 1.0; }
	else { this->speed.x = -1.0; }
}

void daPowerPokey_c::executeState_Move(){

	this->HandleXSpeed();
	this->HandleYSpeed();

	if (this->spin == true) {
		this->rot.y += 0x100;
		if (this->rot.y >= 0x1000) {
			this->spin = false;
		}
	} else {
		this->rot.y -= 0x100;
		if (this->rot.y <= -0x1000) {
			this->spin = true;
		}
	}

	//rot.y = (direction) ? 0xD800 : 0x2800;
	
	this->UpdateObjectPosBasedOnSpeedValuesReal();
	
	float wallDistance;

	wallDistance = 38.0;

	if (this->timer < 200) {
		this->isInvulnerableHead = true;
		setupHeadInvulnerableModel();
		updateModelMatricesHeadInvulnerable();
	}

	if (this->timer > 200) {
		this->isInvulnerableHead = false;
		setupHeadVulnerableModel();
		updateModelMatricesHeadVulnerable();
	}

	if (this->timer > 400) {
		this->timer = 0;
	}

	if (this->pos.x <= this->initialPos.x - ((17 * 16.0) + wallDistance))  { // Hit left wall, head right.
		this->speed.x = -this->speed.x;
		this->direction = 1;
		this->pos.x = this->pos.x + 1.0;
	}

	if (this->pos.x >= this->initialPos.x + ((7.5 * 16.0) - wallDistance))  { // Hit right wall, head left.
		this->speed.x = -this->speed.x;
		this->direction = 0;
		this->pos.x = this->pos.x - 1.0;
	}

	this->pos.y = this->Baseline;
	this->timer = this->timer + 1;

}

void daPowerPokey_c::endState_Move(){ }


//State_Dig
void daPowerPokey_c::beginState_Dig(){
	
	this->timer = 0;
	//this->pos.z = -3000;


}

void daPowerPokey_c::executeState_Dig(){
	
/*
	Vec tempPos = (Vec){ this->pos.x, this->BaseLine - 12.0, this->pos.z };
	S16Vec nullRot = { 0,0,0 };
	Vec oneVec = { 2.0, 1.0, 1.0 };
	SpawnEffect("Wm_mr_sanddive_out", 0, &tempPos, &nullRot, &oneVec);
	SpawnEffect("Wm_mr_sanddive_smk", 0, &tempPos, &nullRot, &oneVec);
*/
	
	
	
	
	doStateChange(&StateID_Move);
}

void daPowerPokey_c::endState_Dig(){ }


//State_Outro
void daPowerPokey_c::beginState_Outro(){
	OutroSetup(this);
}

void daPowerPokey_c::executeState_Outro(){
	if (this->dying == 1) {
		if (this->timer > 180) { ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE); }
		if (this->timer == 60) { PlayerVictoryCries(this); }

		this->timer += 1;
		return;
	}

	bool ret;
	ret = ShrinkBoss(this, &this->pos, 2.25, this->timer);
	this->pos.y -= 0.02;

	if (ret == true) 	{
		BossExplode(this, &this->pos);
		this->dying = 1;
		this->timer = 0;
	}
	else 		{ PlaySound(this, SE_EMY_CHOROPU_SIGN); }

	this->timer += 1;
}

void daPowerPokey_c::endState_Outro(){ }