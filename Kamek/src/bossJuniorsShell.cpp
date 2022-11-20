#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"
#include "bowserjr.h" // Both classes for these are here

const char* CSarcNameList [] = {
	"nokonokoA",
    NULL
};

CREATE_STATE(daCustomShell, Slide);
CREATE_STATE(daCustomShell, Gone);

void daCustomShell::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

daCustomShell* daCustomShell::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daCustomShell));
	return new(buffer) daCustomShell;
}

int daCustomShell::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("nokonokoA", "g3d/t00.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("nokonoko_shell");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	allocator.unlink();

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

	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 8.0;
	HitMeBaby.xDistToEdge = 7.0;
	HitMeBaby.yDistToEdge = 9.0;
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFFBEFAE;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->scale.x = 1.0;
	this->scale.y = 1.0;
	this->scale.z = 1.0;

	if (!this->bjrThrown) {
        direction = (((this->settings >> 28) % 2) ^ 1);
	}

	doStateChange(&StateID_Slide);

	this->onExecute();

	return true;
}

int daCustomShell::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	float rect[] = {0.0, 0.0, 38.0, 38.0};
	int gone = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);

	if (gone) {
		this->Delete(1);
	}

	return true;
}

int daCustomShell::onDelete() {
	return true;
}

int daCustomShell::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daCustomShell::beginState_Slide() {
}
void daCustomShell::executeState_Slide() {

	rot.y = (direction) ? rot.y - 0x1600 : rot.y + 0x1600;
	pos.x = (direction) ? pos.x - posAdd : pos.x + posAdd;

	bool turnTime = calculateTileCollisions();

	if (turnTime) {
		this->direction ^= 1;
	}

}
void daCustomShell::endState_Slide() {}

void daCustomShell::beginState_Gone() {
    shell = (dEn_c*)CreateActor(54, 0x12, pos, 0, 0);

    shell->scale.x = 1.0;
    shell->scale.y = 1.0;
    shell->scale.z = 1.0;

    shell->speed.x = (direction) ? -1.0 : 1.0;

	shell->rot.y = this->rot.y; // REALLY sell the illusion

    this->Delete(true);
}
void daCustomShell::executeState_Gone() {
}
void daCustomShell::endState_Gone() {}

void daCustomShell::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
    if (hitType == 1 || hitType == 3) {
        doStateChange(&StateID_Gone);
    } else if (hitType == 0) {
        DamagePlayer(this, apThis, apOther);
    }
}
void daCustomShell::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->playerCollision(apThis, apOther);
}
void daCustomShell::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    dStageActor_c *actor = (dStageActor_c*)apOther->owner;
    dEn_c *block = (dEn_c*)actor;

    if (block->name == EN_BIRIKYU_MAKER) {
        return;
    }

    block->collisionCat9_RollingObject(apThis, apOther);
}

	bool daCustomShell::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
		PlaySound(this, SE_EMY_DOWN);
        doStateChange(&StateID_Gone);

        if (shell) {
            shell->collisionCat9_RollingObject(apThis, apOther);
        }
        return true;
    }
	bool daCustomShell::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}
	bool daCustomShell::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}
	bool daCustomShell::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}
	bool daCustomShell::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}
	bool daCustomShell::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}
	bool daCustomShell::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}
	bool daCustomShell::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}
	bool daCustomShell::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat3_StarPower(apThis, apOther);
	}
	bool daCustomShell::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
    	return this->collisionCat14_YoshiFire(apThis, apOther);
    }
	bool daCustomShell::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
        return false;
	}

    bool daCustomShell::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
    	return this->collisionCat14_YoshiFire(apThis, apOther);
    }
	bool daCustomShell::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
		StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
		return this->collisionCat3_StarPower(apThis, apOther);
	}
bool daCustomShell::calculateTileCollisions() {
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
