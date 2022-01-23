void daEnBigHanachan_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	daPlBase_c *player = apOther->owner;

	apOther->someFlagByte |= 2;

	if (apOther->info.category2 == 10) {
		// I dunno what category this even IS
		player->_vf3F8(this, 3);
		counter_504[*player->_vf6C()] = 10; // SETS A COUNTER THINGY
		return;
	}

	bool flag = false;
	if (apThis->_18 == 1) {
		if (!player->collMgr.isOnTopOfTile())
			if (player->pos.y > apThis->getBottom())
				flag = true;
	}

	if (flag) {
		_vf224();
		_vf24C(player);
		playEmyCmnStepSound(player); // THIS JUST PLAYS A SOUND
		counter_504[*player->_vf6C()] = 10; // SETS A COUNTER THINGY
	} else {
		if (player->_vf3D4()) {
			player->_vf3F8(apThis, 3);
			counter_504[*player->_vf6C()] = 28; // SETS A COUNTER THINGY
		} else {
			if (!player->_vf3F0())
				player->_vf3F8(apThis, 0);
		}
	}

}

void daEnBigHanachan_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	bool flag = false;
	daPlBase_c *yoshi = apOther->owner;

	apOther->someFlagByte |= 2;

	if (apThis->_18 == 1) {
		if (!yoshi->collMgr.isOnTopOfTile())
			if (yoshi->pos.y > apThis->getBottom())
				flag = true;
	}

	if (flag) {
		_vf224();
		_vf258(yoshi);
		s_80096D00(yoshi); // THIS JUST PLAYS A SOUND
		counter_504[*yoshi->_vf6C()] = 10; // SETS A COUNTER THINGY
	} else {
		if (yoshi->_vf3D4()) {
			yoshi->_vf3F8(this, 3);
			counter_504[*yoshi->_vf6C()] = 28; // SETS A COUNTER THINGY
		} else {
			if (!yoshi->_vf3F0())
				yoshi->_vf3F8(this, 0);
		}
	}

}

