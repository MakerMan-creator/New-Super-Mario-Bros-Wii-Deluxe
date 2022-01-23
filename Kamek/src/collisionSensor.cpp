#include "collisionSensor.h"
#include "simpleCollider.h"
#include "waterManager.h"

// perfect
dCollisionSensor_c::dCollisionSensor_c() {
	clear1();

	pClass2DC = 0;
	chainlinkMode = 1;
	layer = 0;
	pLayerID = &layer;
}

// perfect
dCollisionSensor_c::~dCollisionSensor_c() {
}


// perfect
void dCollisionSensor_c::clear1() {
	pBelowInfo = 0;
	pAboveInfo = 0;
	pAdjacentInfo = 0;
	whichPlayerOfParent____ = -1;
	whichController____ = -1;

	clear2();
}

const dCollisionSensor_c::slopeParams_s dCollisionSensor_c::slopeParams[] = {
	{ 16.0f, -1.0f},  // 1x1 up slope
	{  0.0f,  1.0f},  // 1x1 down slope
	{ 16.0f, -0.5f},  // 2x1a up slope
	{  8.0f, -0.5f},  // 2x1b up slope
	{  0.0f,  0.5f},  // 2x1a down slope
	{  8.0f,  0.5f},  // 2x1b down slope
	{ 16.0f, -2.0f},  // 1x2a up slope
	{ 32.0f, -2.0f},  // 1x2b up slope
	{-16.0f,  2.0f},  // 1x2a down slope
	{  0.0f,  2.0f},  // 1x2b down slope
	{  0.0f,  0.0f},  // slope edge
	{ 16.0f, -0.25f}, // 4x1a up slope
	{ 12.0f, -0.25f}, // 4x1b up slope
	{  8.0f, -0.25f}, // 4x1c up slope
	{  4.0f, -0.25f}, // 4x1d up slope
	{  0.0f,  0.25f}, // 4x1a down slope
	{  4.0f,  0.25f}, // 4x1b down slope
	{  8.0f,  0.25f}, // 4x1c down slope
	{ 12.0f,  0.25f}, // 4x1d down slope
};

const dCollisionSensor_c::slopeParams_s dCollisionSensor_c::flippedSlopeParams[] = {
	{  0.0f,  1.0f},  // 1x1 up slope
	{ 16.0f, -1.0f},  // 1x1 down slope
	{  0.0f,  0.5f},  // 2x1a up slope
	{  8.0f,  0.5f},  // 2x1b up slope
	{ 16.0f, -0.5f},  // 2x1a down slope
	{  8.0f, -0.5f},  // 2x1b down slope
	{  0.0f,  2.0f},  // 1x2a up slope
	{-16.0f,  2.0f},  // 1x2b up slope
	{ 32.0f, -2.0f},  // 1x2a down slope
	{ 16.0f, -2.0f},  // 1x2b down slope
	{ 16.0f,  0.0f},  // slope edge
	{  0.0f,  0.25f}, // 4x1a up slope
	{  4.0f,  0.25f}, // 4x1b up slope
	{  8.0f,  0.25f}, // 4x1c up slope
	{ 12.0f,  0.25f}, // 4x1d up slope
	{ 16.0f, -0.25f}, // 4x1a down slope
	{ 12.0f, -0.25f}, // 4x1b down slope
	{  8.0f, -0.25f}, // 4x1c down slope
	{  4.0f, -0.25f}, // 4x1d down slope
};

// what is this used for?
const u8 dCollisionSensor_c::slopeKinds[] = {
	3,3, 2,2,2,2, 4,4,4,4, 0, 1,1,1,1, 1,1,1,1
};

// what is this used for?
const u8 dCollisionSensor_c::slopeDirections[] = {
	1,0, 1,1,0,0, 1,1,0,0, 0, 1,1,1,1, 0,0,0,0
};

// Controls the following:
// - Mario's slide direction
// - Mario's feet direction
// - Walk speeds
static const s16 slopeAngles[] = {
	8192, -8192,
	4842, 4842,
	-4842, -4842,
	11542, 11542,
	-11542, -11542,
	0, 2555,
	2555, 2555,
	2555, -2555,
	-2555, -2555,
	-2555, 0
};

static const s16 flippedSlopeAngles[] = {
	-8192, 8192,
	-4842, -4842,
	4842, 4842,
	-11542, -11542,
	11542, 11542,
	0, -2555,
	-2555, -2555,
	-2555, 2555,
	2555, 2555,
	2555, 0
};

// perfect
void dCollisionSensor_c::clear2() {
	outputMaybe = 0;
	currentSlopeType = BP_SLOPE_EDGE;
	currentSlopeAngle = slopeAngles[BP_SLOPE_EDGE];
	currentFlippedSlopeType = BP_SLOPE_EDGE;
	currentFlippedSlopeAngle = flippedSlopeAngles[BP_SLOPE_EDGE];
	currentAdjacentSlopeAngle = 0;
	currentSlopeDirection = 2;
	tileBelowSubType = B_SUB_NULL;
	tileAboveSubType = B_SUB_NULL;
	adjacentTileSubType[0] = B_SUB_NULL;
	adjacentTileSubType[1] = B_SUB_NULL;
	clearTouchedSpikeData();
	specialSpeedOffset.x = 0;
	specialSpeedOffset.y = 0;
	specialSpeedOffset.z = 0;
}

// perfect
void dCollisionSensor_c::init(
		dStageActor_c *owner,
		infoBase_s *belowInfo, infoBase_s *aboveInfo, infoBase_s *adjacentInfo) {

	this->owner = owner;
	this->pBelowInfo = belowInfo;
	this->pAboveInfo = aboveInfo;
	this->pAdjacentInfo = adjacentInfo;
	pLayerID = &layer;

	if (owner) {
		pPos = &owner->pos;
		pLastPos = &owner->last_pos;
		pSpeed = &owner->speed;

		whichPlayerOfParent____ = *(owner->_vf6C());
		pLayerID = &owner->currentLayerID;
	}
}

// perfect
static bool somethingWithFloat(float *pFloat, float value) {
	if (*pFloat >= value)
		return true;

	*pFloat += 8.0f;
	if (*pFloat > value)
		*pFloat = value;

	return false;
}

// perfect
static bool doesBehaviourCollectCoin(u32 value) {
	return (((value >> 1) & 1) == 1) ? true : false;
}

// good enough?
bool dCollisionSensor_c::collectCoin(
		u32 coll1, u32 coll2, u32 sFlags, VEC3 *pVec, bool removeTile) {

	char playerID = s_80070530(pVec->y);
	if (playerID == -1)
		return false;

	if (doesBehaviourCollectCoin(coll1)) {
		if (sFlags & SENSOR_COIN_2) {
			int param = getParam(coll2);
			if (param == BP_COIN_2)
				return true;

			VEC3 where(pVec->x, pVec->y, pVec->z);

			sub_80060250(playerID);
			AddScore(100, playerID);

			if (param == BP_COIN_POW) {
				sub_80094390(&where);
			} else {
				sub_80094370(&where);
			}

			VEC3 soundPosOnStage(
					8.0f + s16(((s16)pVec->x) & 0xFFF0),
					s16(((s16)pVec->y) & 0xFFF0) - 8.0f,
					0.0f);

			int sourceID = GetSoundSourceIDForPlayer(playerID);
			SoundPlayer *sp = SoundPlayer2;
			VEC3 soundPos = ConvertStagePositionToScreenPosition(soundPosOnStage);
			sp->playSoundAtPosition(
					SE_OBJ_GET_COIN,
					soundPos,
					sourceID);

			if (removeTile) {
				dBgGm_c::instance->placeTile(
						((s16)pVec->x) & 0xFFF0,
						((s16)-pVec->y) & 0xFFF0,
						layer,
						0);
			}

			return true;
		} else if (sFlags & SENSOR_COIN_1) {
			int param = getParam(coll2);
			if (param == BP_COIN_2)
				return true;

			dBlockMgr_c::for_coinColl bkmgrinfo;
			bkmgrinfo.x = ((s16)pVec->x) & 0xFFF0;
			bkmgrinfo.y = ((s16)-pVec->y) & 0xFFF0;
			bkmgrinfo.unk1 = param;
			bkmgrinfo.unk2 = playerID;
			dBlockMgr_c::instance->coinCollected(&bkmgrinfo);

			return true;
		}
	}

	return false;
}

// almost, unfixable?
bool dCollisionSensor_c::handleCoinOutline(
		u32 coll1, u32 coll2, u32 sFlags, VEC3 *pVec) {

	if (coll1 == 0) {
		u8 param = getParam(coll2);
		if (param >= BP_GENERIC_COIN_OUTLINE &&
				param <= BP_GENERIC_COIN_OUTLINE_END) {
			if (sFlags & SENSOR_COIN_OUTLINE) {
				dBgGm_cX::instance->addTimedSomething(
						param,
						((s16)pVec->x) & 0xFFF0,
						((s16)-pVec->y) & 0xFFF0,
						layer);

				return true;
			}
		}
	}

	return false;
}

// perfect
void dCollisionSensor_c::clearTouchedSpikeData() {
	maxTouchedSpikeType = -1;
	touchedSpikeFlag = 0;
}

// not quite...
bool dCollisionSensor_c::checkForSpikeTile(
		int type, float x, float y) {

	u32 coll1 = getTileBehaviour1At(x, y, layer);
	u32 coll2 = getTileBehaviour2At(x, y, layer);

	int param = getParam(coll2);
	bool flag = false;

	if (coll1 & BF_SPIKE) {
		if (param < BP_SPIKE_DOWN) {
			if (param == type) {
				// Fuck it, life's too short to spend on REing this control structure
				if (param == BP_SPIKE_UP ||
						param == BP_SPIKE_DOWN ||
						(param == BP_SPIKE_RIGHT && xDeltaMaybe < 0.0f) ||
						(param == BP_SPIKE_LEFT && xDeltaMaybe > 0.0f))
					flag = true;
			}
		} else {
			if (param >= BP_SPIKE_LONG_DOWN_1 && param <= BP_SPIKE_SINGLE_DOWN) {
				float jhh = x - pPos->x;
				float dfgdfg = 8.0f + (((s16)x) & 0xFFF0);
				float result = fmod(-y, 16.0);

				float whatIsThis = 0.0f;

				switch (param) {
					case 4:
						whatIsThis = (16.0f - result) * 0.25f;
						break;
					case 5:
						whatIsThis = 2.0f + ((16.0f - result) * 0.125f);
						break;
					case 6:
						whatIsThis = (16.0f - result) * 0.125f;
						break;
				}

				if (abs(dfgdfg - pPos->x) < ((whatIsThis + abs(jhh)) - 1.0f)) {
					if (jhh < 0.0f) {
						if (pPos->x >= (dfgdfg - whatIsThis))
							flag = true;
					} else {
						if (pPos->x <= (dfgdfg + whatIsThis))
							flag = true;
					}
				}

			} else {
				flag = true;
			}
		}
	}

	if (flag) {
		touchedSpikeFlag |= (1 << type);
		if (maxTouchedSpikeType < param)
			maxTouchedSpikeType = param;

		return true;
	}

	static const u32 bitChecks[] = {BF_819, BF_819, BF_8899, BF_4899};
	if (coll1 & bitChecks[type])
		touchedSpikeFlag |= SPIKE_FLAG_80;

	return false;
}

// perfect
bool dCollisionSensor_c::tryBreakBlock(u32 sFlags, u8 direction, float x, float y) {
	int playerID = s_80070530(y);
	return static_tryBreakBlock(
			sFlags, direction, layer, playerID, getOwnerStageActorType(), x, y);
}

// really close, unfixable?
// TODO: find the bug here and fix it
/*static*/ bool dCollisionSensor_c::static_tryBreakBlock(u32 sFlags, u32 direction, u8 layer, int stageActorType, u8 p7, float x, float y) {
	if (!(sFlags & SENSOR_BREAK_BLOCK))
		return false;

	u32 coll1 = getTileBehaviour1At(x, y, layer);
	u32 coll2 = getTileBehaviour2At(x, y, layer);

	if (coll1 == 0 || coll1 & BF_COIN || coll1 & BF_INVISIBLE_BLOCK)
		return false;

	int wut = -1;
	if (coll1 & BF_BRICK)
		wut = 1;
	if (coll1 & BF_QUESTION)
		wut = 0;
	if (coll1 & BF_EXPLODABLE)
		wut = 2;
	if (wut == -1)
		return false;

	dBlockMgr_c::for_88970 whatever;
	whatever._8 = coll2;
	whatever._C = wut;
	whatever._0 = ((s16)x) & 0xFFF0;
	whatever._4 = -(((s16)(-y)) & 0xFFF0);
	whatever._10 = direction;
	whatever._11 = stageActorType;
	whatever._12 = p7;

	dBlockMgr_c::instance->s_80088970(&whatever);

	return true;
}

// pretty much almost there
bool dCollisionSensor_c::tryActivateQuestionBlock(u32 sFlags, u8 direction, float x, float y) {
	int playerID = s_80070530(y);

	u32 coll1 = getTileBehaviour1At(x, y, layer);
	u32 coll2 = getTileBehaviour2At(x, y, layer);

	if (coll1 & BF_QUESTION) {
		if (direction != UP && coll1 & BF_INVISIBLE_BLOCK)
			return false;

		if (sFlags & SENSOR_ACTIVATE_QUESTION) {
			dBlockMgr_c::for_883E0 whatever;
			whatever._8 = coll2;
			whatever._C = direction;
			whatever._0 = ((s16)x) & 0xFFF0;
			whatever._4 = -(((s16)(-y)) & 0xFFF0);
			whatever._D = 0;
			whatever._E = playerID;
			whatever._F = getOwnerStageActorType();
			dBlockMgr_c::instance->s_800883E0(&whatever);

			return true;
		}
	}

	return false;
}

// perfect
u32 dCollisionSensor_c::tryActivateBrick(u32 sFlags, u32 direction, float x, float y) {
	int playerID = s_80070530(y);

	u32 coll1 = getTileBehaviour1At(x, y, layer);
	u32 coll2 = getTileBehaviour2At(x, y, layer);

	u16 choppedX = ((u16)x) & 0xFFF0;
	u16 choppedY = ((u16)(-y)) & 0xFFF0;

	static const u32 oneArray[] = {0x200, 0x200, 0x80000000, 0x800000};
	static const u32 twoArray[] = {0x100, 0x100, 0x40000000, 0x400000};

	int returnValue = 0;

	if (coll1 & BF_BRICK) {
		if (sFlags & SENSOR_BREAK_BRICK) {
			dBlockMgr_c::for_883E0 whatever;
			whatever._8 = coll2;
			whatever._0 = choppedX;
			whatever._4 = -choppedY;
			whatever._C = direction;
			whatever._D = 0;
			whatever._E = playerID;
			whatever._F = getOwnerStageActorType();
			if (dBlockMgr_c::instance->s_800883E0(&whatever)) {
				returnValue = oneArray[direction & 3];
			} else {
				returnValue = twoArray[direction & 3];
			}
		} else if (sFlags & SENSOR_HIT_BRICK) {
			dBlockMgr_c::for_883E0 whatever;
			whatever._8 = coll2;
			whatever._0 = choppedX;
			whatever._4 = -choppedY;
			whatever._C = direction;
			whatever._D = 1;
			whatever._E = playerID;
			whatever._F = getOwnerStageActorType();
			dBlockMgr_c::instance->s_800883E0(&whatever);
			returnValue = oneArray[direction & 3];
		}
	}

	return returnValue;
}


// okay
u32 dCollisionSensor_c::calculateBelowCollision() {
	if (!pBelowInfo)
		return 0;

	lastTileBelowSubType = tileBelowSubType;
	tileBelowSubType = B_SUB_16;
	tileBelowProps = 0;
	touchedSpikeFlag &= ~SPIKE_FLAG_UP;
	someStoredBehaviourFlags = 0;
	someStoredProps = 0;
	_DC = 0.0f;
	_BC &= ~3;

	u32 sFlags = pBelowInfo->flags;
	u32 wwvReturn = assignGroundTileMaybe(sFlags);

	if (!(wwvReturn & 0x6000)) {
		currentSlopeType = BP_SLOPE_EDGE;
		currentSlopeAngle = slopeAngles[BP_SLOPE_EDGE];
		currentSlopeDirection = 2;

		switch (sFlags & SENSOR_TYPE_MASK) {
			case SENSOR_POINT:
				wwvReturn = checkBelowAsPoint(pBelowInfo->asPoint(), sFlags);
				break;
			case SENSOR_LINE:
				wwvReturn = checkBelowAsLine(pBelowInfo->asLine(), sFlags);
				break;
		}
	}

	if (!(sFlags & SENSOR_IGNORE_SIMPLE_COLL)) {
		u32 retVal = (this->*s_checkStuffCB)(sFlags, 0.0f);
		if (retVal) {
			wwvReturn &= ~0x1FE000;
			wwvReturn |= retVal;
		}
	}

	if (touchedSpikeFlag & SPIKE_FLAG_80)
		touchedSpikeFlag &= ~(SPIKE_FLAG_80|SPIKE_FLAG_UP);

	if (wwvReturn & 0x10000) {
		if (tileBelowSubType != B_SUB_ICY || sFlags & SENSOR_10) {
			_D8 = 1.0f;
		} else if (sFlags & SENSOR_20) {
			Similar_to_cLib_chaseF(&_D8, 1.0f, 0.029999999f);
		} else {
			Similar_to_cLib_chaseF(&_D8, 1.0f, 0.0099999998f);
		}
	} else {
		_D8 = 0.0f;
	}

	_E3 = 0;
	wwvReturn &= 0x3FFE000;
	outputMaybe = (outputMaybe & ~0x3FFE000) | wwvReturn;

	if (pSpeed->x < 0.0f)
		_E3 = 1;

	specialSpeedOffset.x = 0.0f;
	specialSpeedOffset.y = 0.0f;
	specialSpeedOffset.z = 0.0f;
	(this->*s_handleGroundCB[tileBelowSubType])();

	return wwvReturn;
}


// perfect
u32 dCollisionSensor_c::checkBelowAsPoint(pointSensor_s *info, u32 sFlags) {
	VEC2 thing(info->x / 4096.0f, info->y / 4096.0f);
	u32 firstVal = ~0x800000 & checkSpecificPosForBelow(sFlags, thing.x, thing.y);

	s_storedX = 0xFFFF;
	s_storedY = 0xFFFF;

	u32 secondVal = tryActivateBlockFromAbove(sFlags, thing.x, thing.y);
	return firstVal | secondVal;
}


// perfect
u32 dCollisionSensor_c::checkBelowAsLine(lineSensor_s *info, u32 sFlags) {
	u32 finalReturn = 0;

	float something = info->distanceFromCenter / 4096.0f;
	float f30, f29;

	if (info->lineA < info->lineB) {
		f30 = info->lineA / 4096.0f;
		f29 = info->lineB / 4096.0f;
	} else {
		f30 = info->lineB / 4096.0f;
		f29 = info->lineA / 4096.0f;
	}

	float var_58 = f30;
	bool what = true;

	do {
		u32 thing = checkSpecificPosForBelow(sFlags, var_58, something);
		if (!(thing & 0x1000000))
			what = false;

		finalReturn |= thing;
	} while (!somethingWithFloat(&var_58, f29));

	if (!what)
		finalReturn &= ~0x1000000;

	s_storedX = 0xFFFF;
	s_storedY = 0xFFFF;

	if (finalReturn & 0x800000) {
		finalReturn &= ~0x800000;
		f30 -= 1.0f;
		f29 += 1.0f;
	}

	var_58 = f30;
	do {
		u32 thing = tryActivateBlockFromAbove(sFlags, var_58, something);
		finalReturn |= thing;
	} while (!somethingWithFloat(&var_58, f29));

	return finalReturn;
}


// okay?
u32 dCollisionSensor_c::checkSpecificPosForBelow(u32 sFlags, float xOffset, float yOffset) {
	float offsetedX = pPos->x + xOffset;
	float offsetedY = pPos->y + yOffset;

	u32 coll1 = getTileBehaviour1At(offsetedX, offsetedY, layer);
	u32 coll2 = getTileBehaviour2At(offsetedX, offsetedY, layer);

	if (coll1 == 0 && coll2 == 0)
		return 0;

	if (someStoredBehaviourFlags == 0) {
		someStoredBehaviourFlags = coll1;
		someStoredProps = coll2;
	}

	VEC3 realPos(offsetedX, offsetedY, 0.0f);
	if (doesBehaviourCollectCoin(coll1)) {
		collectCoin(coll1, coll2, sFlags, &realPos, true);
		return 0;
	}

	if (!handleCoinOutline(coll1, coll2, sFlags, &realPos))
		return false;

	if (tryBreakBlock(sFlags, 3, offsetedX, offsetedY)) {
		if (sFlags & SENSOR_10000000)
			return 0x400000;
		else
			return 0x402000;
	}

	if (coll1 & BF_QUESTION && coll1 & BF_INVISIBLE_BLOCK)
		return false;

	if (coll1 & BF_SLOPE) {
		if (getParam(coll2) == BP_SLOPE_EDGE) {
			if (coll1 & BF_SOLID_ON_TOP)
				coll1 = (coll1 & ~BF_SLOPE) | BF_SOLID;
		}
	}

	checkForSpikeTile(2, offsetedX, offsetedY);

	u32 finalReturn = 0;
	u32 thing1 = BF_8899;
	u32 thing2 = BF_SOLID_ON_TOP;

	if (sFlags & SENSOR_8000) {
		thing1 |= BF_LEDGE;
		thing2 |= BF_LEDGE;
	}

	if (coll1 & thing1) {
		if ((thing1 & thing2) && (coll1 & BF_SLOPE))
			return false;

		if ((coll1 & BF_FENCE) && (sFlags & SENSOR_2000))
			return false;

		if (coll1 & (BF_BRICK|BF_QUESTION))
			finalReturn |= 0x80000000;

		if ((coll1 & BF_PIPE) && (coll1 & BF_SOLID)) {
			u8 param = getParam(coll2);
			if (param == BP_PIPE_LEFT_A || param == BP_PIPE_RIGHT_A || param == BP_PIPE_HORZ_A)
				finalReturn |= 0x1000000;
		}

		bool flag;
		if ((coll1 & BF_BRICK))
			flag = true;
		else
			flag = false;

		if (flag && (sFlags & SENSOR_10000000))
			return false;

		int r27 = 0, r26 = 15;
		int somethingAboutTile = getPartialBlockExistenceAtPos(offsetedX, offsetedY, layer);
		switch (somethingAboutTile) {
			case 0:
				r26 = 7;
				r27 = 1;
				break;
			case 2:
				r26 = 15;
				r27 = 1;
				break;
		}

		int whatever = coll1 & thing2;
		if (whatever) {
			if (getSubType(coll2) != B_SUB_QUICKSAND) {
				if (-(pLastPos->y + yOffset) > (1.0f + ((s16)((int)-offsetedY & 0xFFF0)))) {
					if (!(outputMaybe & 0x200000) || !(outputMaybe & 0x4000) || !(outputMaybe & 0x20000000))
						r27 = 0;
				}
			}
		}

		if (r27) {
			if (coll1 & BF_2000000 && onGround_maybe == 0) {
				u16 whatX = (int)offsetedX & 0xFFF0;
				u16 whatY = (int)(((int)(-offsetedY) & ~15) - 0.1f) & 0xFFF0;
				u32 otherColl1 = getTileBehaviour1At(whatX, whatY, layer);
				u32 otherColl2 = getTileBehaviour2At(whatX, whatY, layer);

				if (otherColl1 & BF_SLOPE) {
					if (getParam(otherColl2) != BP_SLOPE_EDGE && getSubType(otherColl2) != B_SUB_NO_SLIDING)
						return false;
				}

				if (whatever)
					finalReturn |= 0x200000;

				if (setBelowPropsIgnoringSpike(coll2) && !(sFlags & SENSOR_NO_COLLISIONS)) {
					float toAdd = -(((((u16)-offsetedY) & ~r26) & 0xFFFF) - -offsetedY);

					if (0.0f != yDeltaMaybe) {
						float f1 = -(yDeltaMaybe - 1.0f);
						if (f1 < 2.0f)
							f1 = 2.0f;

						if (toAdd > f1)
							toAdd = f1;
						else {
							if (toAdd < 0.0f)
								toAdd = 0.0f;
						}
					}

					pPos->y += toAdd;
				}

				finalReturn |= 0x2000;
			}
		}
	}

	return finalReturn;
}


u32 dCollisionSensor_c::tryActivateBlockFromAbove(u32 sFlags, float xOffset, float yOffset) {
	float offsetedX = pPos->x + xOffset;
	float offsetedY = pPos->y + yOffset;

	if (!setStoredX(offsetedX, offsetedY))
		return 0;

	u32 coll1 = getTileBehaviour1At(offsetedX, offsetedY, layer);
	u32 coll2 = getTileBehaviour2At(offsetedX, offsetedY, layer);

	int intX = offsetedX;
	int intY = -offsetedY;
	if (coll1 & BF_DONUT) {
		if (pSpeed->y <= 0.0f) {
			if (sFlags & SENSOR_ACTIVATE_DONUTS) {
				VEC2 donutPos(intX&0xFFF0, intY&0xFFF0);
				if (getSubType(coll2) != B_SUB_LEDGE)
					dBlockMgr_c::instance->donutLiftTouched(&donutPos);
			}
		}
	}

	u32 returnValue = 0;
	if (tryActivateQuestionBlock(sFlags, 3, offsetedX, offsetedY))
		returnValue |= 0x800000;

	return returnValue | tryActivateBrick(sFlags, 3, offsetedX, offsetedY);
}


int dCollisionSensor_c::checkTypeOfTouchedTile(u32 *pColl1, u32 *pColl2, float x, float y) {
	u16 intX = u16(x) & ~15;
	*pColl1 = getTileBehaviour1At(intX, u16(y) & ~15, layer);
	*pColl2 = getTileBehaviour2At(intX, u16(y) & ~15, layer);

	int type = 0;

	if (*pColl1 & BF_SLOPE) {
		type = 2;
		if (getParam(*pColl2) == BP_SLOPE_EDGE) {
			type = 1;
			if (*pColl1 & BF_SOLID_ON_TOP)
				type = 3;
		}
	} else {
		if (*pColl1 & BF_8899) {
			type = 1;
			if (*pColl1 & BF_100000)
				type = 3;
		}
	}

	return type;
}


u32 dCollisionSensor_c::assignGroundTileMaybe(u32 sFlags) {
	onGround_maybe = 0;

	u32 r31 = outputMaybe & 0x6000;
	float someFloat;
	switch (pBelowInfo->flags & SENSOR_TYPE_MASK) {
		case SENSOR_POINT:
			someFloat = pBelowInfo->asPoint()->y / 4096.0f;
			break;
		case SENSOR_LINE:
			someFloat = pBelowInfo->asLine()->distanceFromCenter / 4096.0f;
			break;
	}

	u32 behaviour1a, behaviour1b;
	float x = pPos->x;
	float y = -(pPos->y + someFloat);
	float otherY;
	int ttType = checkTypeOfTouchedTile(&behaviour1a, &behaviour1b, x, y);

	bool flag = false;
	u32 behaviour2a = 0, behaviour2b = 0;

	if (ttType == 0) {
		otherY = ((int)y & ~15) + 16;
		checkTypeOfTouchedTile(&behaviour2a, &behaviour2b, x, otherY);

		if (behaviour2a & BF_8899 && outputMaybe & 0x4000) {
			if (!(sFlags & SENSOR_10000)) {
				if (currentSlopeDirection == 1) {
					float foo = float(fmod(y, 16.0)) - 16.0f;
					if (foo > -2.0f)
						pPos->y += foo;
				}
			}
		}
	} else if (ttType == 1 || ttType == 3) {
		otherY = ((int)y & ~15) - 0.1f;;
		checkTypeOfTouchedTile(&behaviour2a, &behaviour2b, x, otherY);

		if (behaviour2a & BF_SLOPE && behaviour2a & BF_SOLID_ON_TOP && ttType != 3)
			behaviour2a &= ~BF_SLOPE;

		if (behaviour2a & BF_SLOPE && behaviour1a & BF_2000000)
			flag = true;
	}

	if (behaviour2a & BF_SLOPE) {
		behaviour1a = behaviour2a;
		behaviour1b = behaviour2b;
		ttType = 2;
		y = otherY;
	}

	if (ttType != 2 && ttType != 3)
		return 0;

	int slopeType = getParam(behaviour1b);

	float currentYOnSlope = slopeParams[slopeType].basePos +
		(fmod(x, 16.0) * slopeParams[slopeType].colHeight);
	float tileY = (int)y & ~15;
	float somehowModifiedY = -(pPos->y + someFloat);
	float correctedY = tileY + currentYOnSlope;

	if (behaviour1a & BF_SOLID_ON_TOP) {
		float thing = 0.5f + abs(pSpeed->x);
		if (thing < 1.0f)
			thing = 1.0f;
		float speed = pSpeed->y;
		if (speed > thing)
			return 0;

		if (r31) {
			if (speed <= 0.0f)
				somehowModifiedY += 4.0f;

			if (somehowModifiedY < (correctedY - 1.0f))
				return false;
		} else {
			if (somehowModifiedY < (correctedY - 1.0f))
				return false;
			if (speed < -4.0f)
				speed = -4.0f;
			if (speed > 0.5f)
				speed = 0.5f;

			if (somehowModifiedY > (correctedY + (-speed) + 1.0f + abs(pSpeed->x)))
				return 0;
		}
	} else {
		// skipIt label
		if (pSpeed->y <= 0.0f && r31)
			somehowModifiedY += 4.0f;

		if (somehowModifiedY < (correctedY - 0.60000002f))
			return 0;
	}


	if (r31 == 0 && !flag) {
		if (ttType == 3 && somehowModifiedY > (8.0f+correctedY))
			return 0;

		VEC3 shit(pLastPos->x, pLastPos->y + someFloat, 0.0f);
		if (!sub_800757B0(&shit, &shit.y, layer, 1, -1))
			return 0;

		if (!(getTileBehaviour1At(shit.x, shit.y, layer) & 0x8021))
			return 0;

		if (shit.y < -(16.0f + correctedY))
			return 0;

		int checking = int(pLastPos->y + someFloat);
		if (checking < int(shit.y) || checking > int(16.0f + shit.y))
			return 0;
	}

	bool foo = doesSlopeGoUp();
	s16 oldSlopeAngle = currentSlopeAngle;

	if (getSomethingAboutSlope(slopeType, sFlags))
		return 0;

	u32 finalReturn = (slopeType == BP_SLOPE_EDGE) ? 0x2000 : 0x4000;
	if (behaviour1a & BF_SOLID_ON_TOP)
		finalReturn |= 0x200000;

	currentSlopeType = BP_SLOPE_EDGE;
	currentSlopeAngle = slopeAngles[BP_SLOPE_EDGE];
	currentSlopeDirection = 2;

	if (!(sFlags & SENSOR_NO_COLLISIONS)) {
		pPos->y = -correctedY - someFloat;
		currentSlopeType = slopeType;
		currentSlopeAngle = slopeAngles[slopeType];

		if (slopeType != BP_SLOPE_EDGE) {
			int dir = 0;
			if (pSpeed->x < 0.0f)
				dir = 1;
			currentSlopeDirection = getSlopeYDirectionForXDirection(dir);
		}
	}

	setBelowProps(behaviour1b);
	if (oldSlopeAngle) {
		if (foo != doesSlopeGoUp())
			_BC |= 4;
	}

	if (tileBelowSubType == B_SUB_SPIKE)
		pSpeed->y = 0.0f;

	onGround_maybe = 1;
	_9A = x;
	_9C = y;
	if (someStoredBehaviourFlags == 0)
		someStoredBehaviourFlags = behaviour1a;

	return finalReturn;
}


u32 dCollisionSensor_c::getSomethingAboutSlope(int slopeType, u32 sFlags) {
	int something = 0;
	if (pSpeed->x < 0.0f)
		something = 1;

	if (outputMaybe & 0x4000) {
		if (sFlags & SENSOR_10000) {
			if (currentSlopeDirection == 1 && _E3 == something) {
				if (getSlopeYDirectionForXDirection(something, slopeType) != 1)
					return 0x2000;
			}
		}
		return 0;
	}

	if (slopeKinds[slopeType] == 4) {
		if (abs(pSpeed->x) > 1.5f)
			if (slopeDirections[slopeType] == something)
				return 0x2000;
	}
	return 0;
}


void dCollisionSensor_c::handleNullGroundCB() {
}

void dCollisionSensor_c::handleQuicksandCB() {
	if (pBelowInfo->flags & SENSOR_NO_QUICKSAND)
		return;

	if (pSpeed->y <= 0.0f)
		specialSpeedOffset.y = -0.20999999f;
}

void dCollisionSensor_c::handleConveyorRightCB() {
	if (someStoredProps & 1)
		specialSpeedOffset.x = 1.0f;
	else
		specialSpeedOffset.x = 0.5f;
}

void dCollisionSensor_c::handleConveyorLeftCB() {
	if (someStoredProps & 1)
		specialSpeedOffset.x = -1.0f;
	else
		specialSpeedOffset.y = -0.5f;
}


u32 dCollisionSensor_c::calculateAboveCollision(u32 value) {
	if (!pAboveInfo)
		return 0;

	u32 toReturn = 0;
	s_storedX = 0xFFFF;
	s_storedY = 0xFFFF;
	lastTileAboveSubType = tileAboveSubType;
	tileAboveSubType = B_SUB_16;
	tileAboveProps = 0;
	touchedSpikeFlag &= ~SPIKE_FLAG_DOWN;

	if (pAboveInfo) {
		u32 sFlags = pAboveInfo->flags;

		if (value & 0x1FE000) {
			if (value & 0x20000)
				sFlags |= SENSOR_NO_COLLISIONS;
			else if (value & 0x10000)
				sFlags |= SENSOR_20000000;
			else
				sFlags = ((value & 0x8000) ? (sFlags|SENSOR_40000000) : (sFlags|SENSOR_NO_COLLISIONS));
		}

		toReturn = checkForFlipSlopeAbove(sFlags);
		if (toReturn == 0) {
			currentFlippedSlopeType = BP_SLOPE_EDGE;
			currentFlippedSlopeAngle = flippedSlopeAngles[BP_SLOPE_EDGE];

			switch (sFlags & SENSOR_TYPE_MASK) {
				case SENSOR_POINT:
					toReturn = checkAboveAsPoint(pAboveInfo->asPoint(), sFlags);
					break;
				case SENSOR_LINE:
					toReturn = checkAboveAsLine(pAboveInfo->asLine(), sFlags);
					break;
			}

			if (!(sFlags & SENSOR_IGNORE_SIMPLE_COLL)) {
				u32 scbRet = (this->*s_secondCB)(sFlags, 0.0f);
				if (scbRet)
					toReturn = (toReturn & ~0x3C000000) | scbRet;
			}
		}
	}

	if (touchedSpikeFlag & SPIKE_FLAG_80)
		touchedSpikeFlag &= ~(SPIKE_FLAG_80|SPIKE_FLAG_DOWN);

	toReturn &= CSOUT_ABOVE_ALL;
	outputMaybe = toReturn | (outputMaybe & ~CSOUT_ABOVE_ALL);
	return toReturn;
}


u32 dCollisionSensor_c::checkAboveAsPoint(pointSensor_s *info, u32 sFlags) {
	VEC2 vector(pPos->x + (info->x / 4096.0f), pPos->y + (info->y / 4096.0f));

	u32 toReturn = checkSpecificPosForAbove(sFlags, vector.x, vector.y) & 0x7FFFFFFF;

	s_storedX = 0xFFFF;
	s_storedY = 0xFFFF;

	toReturn |= tryHitBlockFromBelow(sFlags, vector.x, vector.y);

	pushTopOutOfBlockIfNeeded(toReturn, sFlags, vector.y);

	return toReturn;
}

// close enough
u32 dCollisionSensor_c::checkAboveAsLine(lineSensor_s *info, u32 sFlags) {
	u32 toReturn = 0;
	float f31 = info->distanceFromCenter / 4096.0f;
	float f30, f29;

	if (info->lineA < info->lineB) {
		f30 = info->lineA / 4096.0f;
		f29 = info->lineB / 4096.0f;
	} else {
		f30 = info->lineB / 4096.0f;
		f29 = info->lineA / 4096.0f;
	}

	float var_68 = f30;
	int countA = 0, countB = 0;

	do {
		VEC2 vector(pPos->x + var_68, pPos->y + f31);
		toReturn |= checkSpecificPosForAbove(sFlags, vector.x, vector.y);
		if (toReturn & 0x40000000)
			countB++;
		if (toReturn & 0x3C000000)
			countA++;
	} while (!somethingWithFloat(&var_68, f29));

	s_storedX = 0xFFFF;
	s_storedY = 0xFFFF;

	if (toReturn & 0x80000000) {
		toReturn &= ~0x80000000;
		f30 -= 1.0f;
		f29 += 1.0f;
	}

	var_68 = f30;

	do {
		VEC2 vector(pPos->x + var_68, pPos->y + f31);
		toReturn |= tryHitBlockFromBelow(sFlags, vector.x, vector.y);
		if (toReturn & 0x40000000)
			countB++;
	} while (!somethingWithFloat(&var_68, f29));

	pushTopOutOfBlockIfNeeded(toReturn, sFlags, pPos->y + f31);

	if (countA != countB)
		toReturn &= ~0x40000000;

	return toReturn;
}


// perfect
u32 dCollisionSensor_c::checkSpecificPosForAbove(u32 sFlags, float x, float y) {
	if (!setStoredX(x, y))
		return 0;

	u32 coll1 = getTileBehaviour1At(x, y, layer);
	u32 coll2 = getTileBehaviour2At(x, y, layer);
	if (coll1 == 0 && coll2 == 0)
		return 0;

	VEC3 posVec(x, y, 0.0f);
	if (doesBehaviourCollectCoin(coll1)) {
		collectCoin(coll1, coll2, sFlags, &posVec, true);
		return 0;
	}

	if (handleCoinOutline(coll1, coll2, sFlags, &posVec))
		return 0;

	if (pSpeed->y > 0.0f) {
		if (tryBreakBlock(sFlags, 2, x, y))
			return 0x40000000;
	}

	checkForSpikeTile(3, x, y);
	u32 toReturn = 0;

	if (coll1 & BF_4899) {
		if (coll1 & (BF_BRICK|BF_QUESTION))
			toReturn |= 0x80000000;
		
		bool flag;
		if (coll1 & BF_BRICK && (sFlags & SENSOR_BREAK_BRICK))
			flag = true;
		else
			flag = false;

		if (flag && (sFlags & SENSOR_10000000))
			return 0;

		if (!(coll1 & BF_INVISIBLE_BLOCK) || pSpeed->y > 0.0f) {
			switch (getPartialBlockExistenceAtPos(x, y, layer)) {
				case 0:
					toReturn |= 0x10000000;
					setAboveProps(coll2);
					break;
				case 2:
					toReturn |= 0x4000000;
					setAboveProps(coll2);
					break;
			}
		}
	}

	return toReturn;
}


u32 dCollisionSensor_c::tryHitBlockFromBelow(u32 sFlags, float x, float y) {
	if (!setStoredX(x, y))
		return 0;

	u32 coll1 = getTileBehaviour1At(x, y, layer);
	u32 coll2 = getTileBehaviour2At(x, y, layer);

	return 0;
	int intY = -y;
	if (coll1 & BF_INVISIBLE_BLOCK) {
		if (pSpeed->y <= 0.0f || (intY & 15) < 8)
			return 0;
	}

	if (pSpeed->y < 0.0f && !(sFlags & SENSOR_4000000))
		return 0;

	u32 toReturn = 0;
	if (tryActivateQuestionBlock(sFlags, 2, x, y))
		toReturn |= 0x84000000;

	return toReturn | tryActivateBrick(sFlags, 2, x, y);
}


void dCollisionSensor_c::pushTopOutOfBlockIfNeeded(u32 resultThing, u32 sFlags, float y) {
	if (sFlags & SENSOR_NO_COLLISIONS)
		return;
	if (!(resultThing & (0x10000000|0x4000000)))
		return;

	int andWith = (resultThing & 0x10000000) ? 7 : 15;

	float toAdd = (0.1f + ((int)y & ~andWith)) - y;

	if (toAdd < 0.0f)
		pPos->y += toAdd;
}


// TO CHECK
u32 dCollisionSensor_c::checkForFlipSlopeAbove(u32 sFlags) {
	float yOffset;

	switch (pAboveInfo->flags & SENSOR_TYPE_MASK) {
		case SENSOR_POINT:
			yOffset = pAboveInfo->asPoint()->y / 4096.0f;
			break;
		case SENSOR_LINE:
			yOffset = pAboveInfo->asLine()->distanceFromCenter / 4096.0f;
			break;
	}

	float x = pPos->x;
	float y = pPos->y + yOffset;

	u32 coll1 = getTileBehaviour1At(x, y, layer);
	if (coll1 & BF_SOLID) {
		u16 belowY = (u16(-y) & 0xFFF0) + 0x10;
		u32 belowColl1 = getTileBehaviour1At((u16)x, belowY, layer);
		if (belowColl1 & BF_FLIPPED_SLOPE) {
			coll1 = belowColl1;
			y = -belowY;
		}
	}

	if (coll1 & BF_FLIPPED_SLOPE) {
		u32 coll2 = getTileBehaviour2At(x, y, layer);
		u8 slopeType = getParam(coll2);

		float currentYOnSlope = -(flippedSlopeParams[slopeType].basePos +
			(fmod(x, 16.0) * flippedSlopeParams[slopeType].colHeight));
		float correctedY = (16.0f * int(y / 16.0f)) + currentYOnSlope;

		if (y >= (correctedY - 0.1f)) {
			if (!(sFlags & SENSOR_NO_COLLISIONS)) {
				pPos->y = correctedY - yOffset;
				currentFlippedSlopeType = slopeType;
				currentFlippedSlopeAngle = flippedSlopeAngles[slopeType];
			}

			u32 willReturn = (slopeType == BP_SLOPE_EDGE) ? 0x4000000 : 0x8000000;
			setAboveProps(coll2);
			return willReturn;
		}
	}

	return 0;
}


u32 dCollisionSensor_c::calculateAdjacentCollision(float *pFloat) {
	if (!pAdjacentInfo)
		return 0;

	touchedSpikeFlag &= ~(SPIKE_FLAG_LEFT|SPIKE_FLAG_RIGHT);

	float floatToUse;
	if (pFloat)
		floatToUse = *pFloat;
	else
		floatToUse = pSpeed->x;

	return calculateAdjacentCollisionInternal(floatToUse < 0.0f ? 1 : 0, 0);
}

u32 dCollisionSensor_c::calculateAdjacentCollisionInternal(int direction, u32 extraSensorFlags) {
	if (!pAdjacentInfo)
		return 0;

	u32 sFlags = pAdjacentInfo->flags | extraSensorFlags;
	_BA = adjacentTileProps[direction];
	adjacentTileSubType[direction] = B_SUB_16;
	adjacentTileProps[direction] = 0;
	currentAdjacentSlopeAngle = 0;

	u32 toReturn = 0;

	switch (sFlags & SENSOR_TYPE_MASK) {
		case SENSOR_POINT:
			toReturn = checkAdjacentAsPoint(pAdjacentInfo->asPoint(), direction, sFlags);
			break;
		case SENSOR_LINE:
			toReturn = checkAdjacentAsLine(pAdjacentInfo->asLine(), direction, sFlags);
			break;
	}

	if (!(sFlags & SENSOR_IGNORE_SIMPLE_COLL)) {
		u32 ptmfReturn = (this->*s_thirdCB)(sFlags, direction, 0.0f);
		if (ptmfReturn)
			toReturn = ptmfReturn;
	}

	static const u8 bits[2] = {SPIKE_FLAG_LEFT, SPIKE_FLAG_RIGHT};
	if (touchedSpikeFlag & SPIKE_FLAG_80) {
		touchedSpikeFlag &= ~(bits[direction]|SPIKE_FLAG_80);
	}

	if (touchedSpikeFlag & bits[direction])
		toReturn = 0;

	toReturn &= 0x1FFF;
	outputMaybe = (toReturn & 0x1FFF) | (outputMaybe & ~0x1FFF);
	return toReturn;
}


u32 dCollisionSensor_c::checkAdjacentAsPoint(pointSensor_s *info, int direction, u32 sFlags) {
	float calcedFloat;

	if (direction == LEFT) {
		calcedFloat = -(1.0f + (info->x / 4096.0f));
	} else {
		calcedFloat = info->x / 4096.0f;
	}

	VEC2 vector(pPos->x + calcedFloat, pPos->y + (info->y / 4096.0f));
	u32 retVal = checkSpecificPosForAdjacent(sFlags, direction, vector.x, vector.y);

	if (retVal & 0xC)
		retVal |= (0x10 << direction);

	return retVal;
}


u32 dCollisionSensor_c::checkAdjacentAsLine(lineSensor_s *info, int direction, u32 sFlags) {
	float a_float, var_58;
	if (info->lineA < info->lineB) {
		var_58 = info->lineA / 4096.0f;
		a_float = info->lineB / 4096.0f;
	} else {
		var_58 = info->lineB / 4096.0f;
		a_float = info->lineA / 4096.0f;
	}

	float f30;
	if (direction == RIGHT) {
		f30 = info->distanceFromCenter / 4096.0f;
	} else {
		f30 = -info->distanceFromCenter / 4096.0f;
	}

	u32 retVal = 0;
	bool setFlag = true;

	do {
		VEC2 vector(pPos->x + f30, pPos->y + var_58);
		u32 thisReturn = checkSpecificPosForAdjacent(sFlags, direction, vector.x, vector.y);
		retVal |= thisReturn;

		if (!(thisReturn & 0xC))
			setFlag = false;
		
	} while (!somethingWithFloat(&var_58, a_float));

	if (setFlag == true) {
		retVal |= (0x10 << direction);
	}

	return true;
}


bool dCollisionSensor_c::isTouchingSlope_maybe(float x, float y) {
	float chasedValue = pPos->x;

	VEC3 aa(pPos->x, y, 0.0f);
	VEC3 ab(x, y, 0.0f);
	
	nw4r::math::SEGMENT3 segA(aa, ab);

	while (chasedValue != x) {
		u32 coll1 = getTileBehaviour1At(chasedValue, y, layer);
		u32 coll2 = getTileBehaviour2At(chasedValue, y, layer);

		if (coll1 & BF_ANY_SLOPE) {
			u8 slopeType = getParam(coll2);

			const slopeParams_s *sparam = &slopeParams[slopeType];
			float fthing = sparam->colHeight;
			if (coll1 & BF_FLIPPED_SLOPE) {
				sparam = &flippedSlopeParams[slopeType];
				fthing = -sparam->colHeight;
			}


			if ((fthing * (ab.x - aa.x)) < 0.0f) {
				VEC3 ba(
						int(chasedValue) & 0xFFF0,
						-((int(-y) & 0xFFF0) + sparam->basePos),
						0.0f);

				VEC3 bb(
						16.0f + ba.x,
						ba.y - (16.0f * sparam->colHeight),
						0.0f);

				VEC3 anotherVec(
						ba.x,
						ba.y,
						0.0f);

				nw4r::math::SEGMENT3 segB(ba, bb);
				float distance = nw4r::math::DistSqSegment3ToSegment3(&segA, &segB, 0, 0);

				if (0.1f > distance)
					return true;
			}
		}

		Similar_to_cLib_chaseF(&chasedValue, x, 16.0f);
	}

	return false;
}


// almost perfect, unfixable
u32 dCollisionSensor_c::checkSpecificPosForAdjacent(u32 sFlags, int direction, float x, float y) {
	u32 coll1 = getTileBehaviour1At(x, y, layer);
	u32 coll2 = getTileBehaviour2At(x, y, layer);

	if (coll1 == 0 && coll2 == 0)
		return 0;

	if (isTouchingSlope_maybe(x, y))
		return 0;

	VEC3 pos(x, y, 0.0f);
	if (doesBehaviourCollectCoin(coll1)) {
		collectCoin(coll1, coll2, sFlags, &pos, true);
		return 0;
	}

	if (handleCoinOutline(coll1, coll2, sFlags, &pos))
		return 0;

	if (tryBreakBlock(sFlags, direction, x, y))
		return 0x100;

	u32 finalReturn = 0;
	if (tryActivateQuestionBlock(sFlags, direction, x, y))
		finalReturn |= 0x200;

	u32 thisRet = tryActivateBrick(sFlags, direction, x, y);
	if (thisRet) {
		finalReturn |= thisRet;
		if (sFlags & SENSOR_10000000)
			return 0;
	}

	if (coll1 & BF_819) {
		bool flag = false;
		u32 mask = 0;
		switch (getPartialBlockExistenceAtPos(x, y, layer)) {
			case 0:
				mask = 7;
				flag = true;
				break;
			case 2:
				mask = 15;
				flag = true;
				break;
		}

		if (flag) {
			int fxX = int(4096.0f * x);
			int fxMask = mask * 4096;
			int fxIntegerPosition = int(4096.0f * pPos->x) & ~0xFFF;

			int toAdd;
			if (direction != RIGHT) {
				toAdd = fxMask - (fxX & fxMask);
				if (xDeltaMaybe != 0.0f) {
					int candidate = 4096.0f * -xDeltaMaybe;
					if (candidate < 0x2000)
						candidate = 0x2000;
					if (toAdd > candidate)
						toAdd = candidate;
				}
			} else {
				toAdd = -(fxX & fxMask);
				if (xDeltaMaybe != 0.0f) {
					int candidate = 4096.0f * -xDeltaMaybe;
					if (candidate > -0x2000)
						candidate = -0x2000;
					if (toAdd < candidate)
						toAdd = candidate;
				}
				fxIntegerPosition += 0xF00;
			}

			fxIntegerPosition += toAdd;
			if (toAdd != 0 && !(sFlags & SENSOR_NO_COLLISIONS)) {
				pPos->x = fxIntegerPosition / 4096.0f;
			}

			if (toAdd != 0)
				finalReturn = (direction != RIGHT) ? (finalReturn|2) : (finalReturn|1);

			finalReturn = (direction != RIGHT) ? (finalReturn|8) : (finalReturn|4);

			s8 checkController = whichController____;
			if (checkController != -1) {
				int thing = s_80070530(y);
				finalReturn = (thing == checkController) ? (finalReturn|0x800) : (finalReturn|0x1000);
			}

			setAdjacentProps(coll2, direction);
		}
	}

	static int types[] = {0,1,2,1};
	checkForSpikeTile(types[direction], x, y);
	return finalReturn;
}


// perfect
bool dCollisionSensor_c::setStoredX(float x, float y) {
	u16 intX = u16(x) & ~15;
	u16 intY = u16(-y) & ~15;
	if (s_storedX != intX || s_storedY != intY) {
		s_storedX = intX;
		s_storedY = intY;
		return true;
	} else {
		return false;
	}
}


// pretty much perfect, tiny reordering thing
s8 dCollisionSensor_c::s_80070530(float y) const {
	if (whichController____ != -1) {
		dStageActor_c *player = GetSpecificPlayerActor(whichController____);
		if (player) {
			if (player->pos.y > y)
				return whichController____;
		}
	}

	return whichPlayerOfParent____;
}


// perfect
dStageActor_c *dCollisionSensor_c::s_800705B0(float y) const {
	dStageActor_c *toReturn = owner;

	if (whichController____ != -1)
		toReturn = GetSpecificPlayerActor(s_80070530(y));

	return toReturn;
}


// perfect, two regs are swapped but who cares
bool dCollisionSensor_c::setBelowSubType(u32 magic) {
	tileBelowSubType = magic&0xFF;
	tileBelowProps = (magic&0xFF) << 16;
	return ((magic & 0xFF) != B_SUB_QUICKSAND);
}


static const int subtypePriorities[] = {
	2, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 3, 0
};

bool dCollisionSensor_c::setBelowPropsIgnoringSpike(u32 magic) {
	if (getSubType(magic) == B_SUB_SPIKE)
		magic &= ~0xFF0000;
	return setBelowProps(magic);
}

bool dCollisionSensor_c::setBelowProps(u32 magic) {
	u8 id = getSubType(magic);
	if (subtypePriorities[tileBelowSubType] > subtypePriorities[id]) {
		tileBelowSubType = id;
		tileBelowProps = magic;
	}

	if (tileBelowSubType == B_SUB_QUICKSAND)
		return false;
	else
		return true;
}

void dCollisionSensor_c::setAboveProps(u32 magic) {
	u8 id = getSubType(magic);
	if (subtypePriorities[tileAboveSubType] > subtypePriorities[id]) {
		tileAboveSubType = id;
		tileAboveProps = magic;
	}
}


void dCollisionSensor_c::setAdjacentProps(u32 magic, int direction) {
	u8 id = getSubType(magic);
	if (subtypePriorities[adjacentTileSubType[direction]] > subtypePriorities[id]) {
		adjacentTileSubType[direction] = id;
		adjacentTileProps[direction] = magic;
	}
}

u8 dCollisionSensor_c::getBelowSubType() {
	return getSubType(tileBelowProps);
}

u8 dCollisionSensor_c::getAboveSubType() {
	return getSubType(tileAboveProps);
}

u8 dCollisionSensor_c::getAdjacentSubType(int direction) {
	return getSubType(adjacentTileProps[direction]);
}


// almost perfect. I can't be assed to fix the abs returning short thing
int dCollisionSensor_c::returnSomethingBasedOnSlopeAngle() {
	int thing = abs(getAngleOfSlopeInDirection(0));
	
	if (thing < 1277)
		return 0;
	else if (thing < 3698)
		return 1;
	else if (thing < 6517)
		return 2;
	else if (thing < 9867)
		return 3;
	else
		return 4;
}


bool dCollisionSensor_c::doesSlopeGoUp() {
	int thing = getAngleOfSlopeInDirection(0);
	return (thing > 0);
}


int dCollisionSensor_c::getSlopeYDirectionForXDirection(int direction) {
	int result = (doesSlopeGoUp() != 0) ? 1 : 0;
	if (direction == LEFT)
		result ^= 1;
	return result;
}


// meh
int dCollisionSensor_c::getSlopeYDirectionForXDirection(int direction, int slopeType) {
	int usedAngle = slopeAngles[slopeType];
	if (usedAngle == 0)
		return 2;

	if (direction == LEFT)
		usedAngle = -usedAngle;

	if (usedAngle > 0)
		return 1;
	else
		return 0;
}


// perfect
s16 dCollisionSensor_c::getAngleOfSlopeInDirection(int direction) {
	s16 toReturn = 0;

	if (outputMaybe & 0x8000) {
		if (pClass2DC)
			toReturn = *((s16*)( ((u8*)pClass2DC) + 0x2C )); // HACK
	} else {
		toReturn = currentSlopeAngle;
	}

	if (direction == LEFT)
		toReturn = -toReturn;

	return toReturn;
}


// perfect
s16 dCollisionSensor_c::getAngleOfSlopeWithXSpeed(float xSpeed) {
	if (xSpeed >= 0.0f)
		return getAngleOfSlopeInDirection(0);
	else
		return getAngleOfSlopeInDirection(1);
}


// need to look into this ...
s16 dCollisionSensor_c::getAngleOfSlopeInDirection2(int thing) {
	s16 toReturn = getAngleOfSlopeInDirection(0);

	if (thing == 1)
		toReturn -= 0x8000;

	return toReturn;
}


// perfect
s16 dCollisionSensor_c::getAngleOfFlippedSlopeInDirection(int thing) {
	s16 toReturn = currentFlippedSlopeAngle;

	if (thing == 1)
		toReturn = -toReturn;

	return toReturn;
}


// need to look into this ...
s16 dCollisionSensor_c::getAngleOfFlippedSlopeInDirection2(int thing) {
	s16 toReturn = getAngleOfFlippedSlopeInDirection(0);

	if (thing == 1)
		toReturn -= 0x8000;

	return toReturn;
}


// perfect
/*static*/ bool dCollisionSensor_c::checkPositionForTileOrSColl(
		float x, float y, u8 layer, u32 unk, u32 mask) {

	if (getTileBehaviour1At(x,y,layer) & mask)
		return true;

	return sub_80076630(unk, x, y, layer);
}


// registers swapped but fine
/*static*/ int dCollisionSensor_c::getPartialBlockExistenceAtPos(float x, float y, u8 layer) {

	u32 coll1 = getTileBehaviour1At(x, y, layer);
	u32 coll2 = getTileBehaviour2At(x, y, layer);

	if (coll1 & BF_PARTIAL_SOLID) {
		int flags = 0;

		if (int(x) & 8)
			flags |= 1;
		if (int(-y) & 8)
			flags |= 2;
		if ((coll2 & 0xFF) & (1 << flags))
			return 0;
		else
			return 1;
	} else {
		return 2;
	}
}


// perfect
/*static*/ bool dCollisionSensor_c::isPartialBlockOrBlockSolidAtPos(float x, float y, u8 layer) {
	switch (getPartialBlockExistenceAtPos(x, y, layer)) {
		case 0:
			return true;
		case 2:
			if (getTileBehaviour1At(x, y, layer) & BF_819)
				return true;
			break;
	}
	return false;
}


// regs swapped but fine
/*static*/ u32 dCollisionSensor_c::getTileBehaviour1At(
		float x, float y, u8 layer) {

	return getTileBehaviour1At(u16(x), u16(-y), layer);
}
// regs swapped but fine
/*static*/ u32 dCollisionSensor_c::getTileBehaviour2At(
		float x, float y, u8 layer) {

	return getTileBehaviour2At(u16(x), u16(-y), layer);
}

// perfect
/*static*/ u32 dCollisionSensor_c::getTileBehaviour1At(
		u16 x, u16 y, u8 layer) {

	return dBgGm_cX::instance->getTileBehaviour1At(x, y, layer);
}
// perfect
/*static*/ u32 dCollisionSensor_c::getTileBehaviour2At(
		u16 x, u16 y, u8 layer) {

	return dBgGm_cX::instance->getTileBehaviour2At(x, y, layer);
}

// perfect
u8 dCollisionSensor_c::getOwnerStageActorType() const {
	if (owner)
		return owner->stageActorType;
	else
		return 0;
}

// perfect
u32 dCollisionSensor_c::returnOutputAnd18000() const {
	return outputMaybe & 0x18000;
}

// perfect
bool dCollisionSensor_c::isOnTopOfTile() const {
	return ((outputMaybe & 0x1FE000) != 0);
}


// registers swapped but fine?
u32 dCollisionSensor_c::s_80070CD0(dStageActor_c *player, u32 passedToBCLP) {
	u32 result = 0;
	if (pClass2DC) {
		u32 thing = *(((u8*)pClass2DC)+0x30);
		// TODO: REPLACE WITH METHOD
		if (!sub_80056900(player))
			thing |= 1;

		// TODO: REPLACE WITH METHOD
		if (BasicColliderListProcessor1(pClass2DC, passedToBCLP, thing, 0)) {
			if (!(thing & 1)) {
				result |= 0x8000;
				outputMaybe |= 0x8000;
				if (returnSomethingBasedOnSlopeAngle() != 0) {
					result |= 0x4000;
					outputMaybe |= 0x4000;
				}

				// TODO: REPLACE WITH METHOD
/*				dLineColliderBase_c *lcoll = sub_800D99F0(pClass2DC);
				if (anotherThing) {
					if (lcoll->flags & 0x8000) {
						result |= 0x40000;
						outputMaybe |= 0x40000;
					}

					if (lcoll->flags & 0x20000) {
						result |= 0x80000;
						outputMaybe |= 0x80000;
					}

					if (lcoll->flags & 0x40000) {
						result |= 0x100000;
						outputMaybe |= 0x100000;
					}

					if (lcoll->flags & 0x100000) {
						result |= 0x20000;
						outputMaybe |= 0x20000;
					}

					// TODO: REPLACE WITH METHOD
					if (CheckShit(player, 0x1C))
						lcoll->_46 |= 1;

					u8 subType = lcoll->subType;
					if (subType)
						setBelowProps(subType << 16);

					if (lcoll->flags & 0x200) {
						result &= ~0xC000;
						outputMaybe &= ~0xC000;
					}

					sCollBelow = 0;
				}*/
			}
		}
	}
	return result;
}

// perfect
u32 dCollisionSensor_c::s_80070E70(dStageActor_c *player) {
	u32 thing = calculateBelowCollision();
	thing |= s_80070CD0(player, thing);

	if (CheckShit(player, 0x14))
		thing |= 0x8000;

	return thing | calculateAboveCollision(thing);
}

// needs double checking of a extrwi
u32 dCollisionSensor_c::s_80070EF0(dStageActor_c *actor) {
	// actor might be the player?
	
	u32 sFlags = pAdjacentInfo->flags;

	int someFlag = 0;
	if (xDeltaMaybe < 0.0f)
		someFlag = 1;

	u32 toReturn = s_80071060(someFlag);
	someFlag ^= 1;
	
	u32 otherThing = toReturn | s_80071060(someFlag);

	if (_BC & 8)
		otherThing = (otherThing >> 2) & 3;

	int checkThis = otherThing & 3;
	if (checkThis == 3 || outputMaybe & 0x400) {
		sFlags |= SENSOR_NO_COLLISIONS;
		actor->pos.x += 1.0f;
	}

	if (checkThis == 3) {
		outputMaybe |= 0x400;
	} else if (otherThing == 0) {
		outputMaybe &= ~0x400;
	}

	someFlag ^= 1;

	static const u32 checkFlagOne[] = {0x2A,0x15};
	static const u32 checkFlagTwo[] = {0x80,0x40};

	u32 passedFlags = sFlags;
	if (_8C & checkFlagOne[someFlag] && !(_8C & checkFlagTwo[someFlag]))
		passedFlags = sFlags | SENSOR_NO_COLLISIONS;

	toReturn = calculateAdjacentCollisionInternal(someFlag, passedFlags);

	if (toReturn & 0x3F && !(toReturn & 0xC0))
		sFlags |= SENSOR_NO_COLLISIONS;

	pAdjacentInfo->flags &= ~SENSOR_2000000;

	u32 lastBit = calculateAdjacentCollisionInternal(someFlag^1, sFlags&~SENSOR_2000000);

	u32 finalReturn = toReturn | lastBit;
	outputMaybe |= finalReturn;

	return finalReturn;
}

// OK enough I suppose
u32 dCollisionSensor_c::s_80071060(int direction) {
	float f2,f3,f4;

	lineSensor_s *adjLine = pAdjacentInfo->asLine();
	if (adjLine->lineA < adjLine->lineB) {
		f3 = adjLine->lineA / 4096.0f;
		f4 = adjLine->lineB / 4096.0f;
	} else {
		f3 = adjLine->lineB / 4096.0f;
		f4 = adjLine->lineA / 4096.0f;
	}

	if (direction == LEFT) {
		f2 = -adjLine->distanceFromCenter / 4096.0f;
	} else {
		f2 = (adjLine->distanceFromCenter - 0x1000) / 4096.0f;
	}

	bool flag = true;
	u32 dirflag = 1 << direction;
	u32 returnValue = 0;
	float f31 = pPos->x + f2;
	float var_58 = pPos->y + f3;
	float f30 = pPos->y + f4;

	do {
		u32 thisRet = isPartialBlockOrBlockSolidAtPos(f31, var_58, layer);

		if (thisRet)
			returnValue |= dirflag;
		if (thisRet == 0)
			flag = false;

	} while (!somethingWithFloat(&var_58, f30));

	if (flag == true) {
		returnValue |= (0x10 << direction);
	}

	return returnValue;
}


// should that be a Player class? probably :|
u32 dCollisionSensor_c::s_80071210(dStageActor_c *player) {
	_8C = outputMaybe;
	layer = player->currentLayerID;
	clearTouchedSpikeData();

	xDeltaMaybe = pPos->x - pLastPos->x;
	yDeltaMaybe = pPos->y - pLastPos->y;
	if (yDeltaMaybe > 0.0f) {
		if (yDeltaMaybe < 1.0f)
			yDeltaMaybe = 1.0f;
	} else {
		if (yDeltaMaybe > -1.0f)
			yDeltaMaybe = -1.0f;
	}

	u32 returnValue = s_80070E70(player);
	float theX = pPos->x;
	returnValue |= s_80070EF0(player);

	if (returnValue & 0x18000) {
		if (theX != pPos->x) {
			int direction = RIGHT;
			if (pPos->x < theX)
				direction = LEFT;
			s16 angle = getAngleOfSlopeInDirection2(direction);

			if (angle != 0) {
				float rotSin = nw4r::math::SinFIdx(FastS16toFloat(angle) / 256.0f);
				float rotCos = nw4r::math::CosFIdx(FastS16toFloat(angle) / 256.0f);

				pPos->y += ((rotSin/rotCos) * (pPos->x - theX));
			}
		}
	}

	s_800713B0(player, returnValue);
	return returnValue;
}


bool dCollisionSensor_c::s_800713B0(dStageActor_c *player, u32 things) {
	if (!pClass2DC || !(outputMaybe & 3) || !(outputMaybe & 0x8000))
		return false;

	if (*( ((u8*)pClass2DC) + 0x2F  ) == 4) {
		void *anotherThing = sub_800D99F0(pClass2DC);

		if (anotherThing) {
			s16 thingAt70 = *((s16*)( ((u8*)anotherThing) + 0x70 ));
			s16 thingAt40 = *((s16*)( ((u8*)anotherThing) + 0x40 ));
			int diff = thingAt40 - thingAt70;
			if (diff > 0) {
				if ((outputMaybe & 1) && !(outputMaybe & 0x40))
					return true;
			} else {
				if ((outputMaybe & 2) && !(outputMaybe & 0x80))
					return true;
			}
		}
	}

	u32 stuff = 0;
	if (!sub_80056900(player))
		stuff |= 1;

	u32 params = (things & 0x1F0000) | (things & 0x6000);
	if (!sub_800D9B20(pClass2DC, params, stuff, 0))
		outputMaybe &= ~0x8000;

	return true;
}


bool dCollisionSensor_c::tryToEnterPipeBelow(VEC3 *pOutVec, int *pOutEntranceID) {
	float one, two;
	lineSensor_s *bLine = pBelowInfo->asLine();
	if (bLine->lineA < bLine->lineB) {
		one = bLine->lineA / 4096.0f;
		two = bLine->lineB / 4096.0f;
	} else {
		one = bLine->lineB / 4096.0f;
		two = bLine->lineA / 4096.0f;
	}

	if (abs(two - one) > 31.0f)
		return false;

	u32 coll1 = getTileBehaviour1At(pPos->x + one, pPos->y, layer);
	u32 coll2 = getTileBehaviour2At(pPos->x + one, pPos->y, layer);

	int pipeType = getParam(coll2);

	if (!(coll1 & BF_PIPE))
		return false;

	int x = int(pPos->x + one) & 0xFFF0;
	int y = -pPos->y;

	switch (pipeType) {
		case BP_PIPE_UP_A: // vertical top entrance left
			x += 32;
			break;
		case BP_PIPE_UP_B: // vertical top entrance right
			x += 16;
			break;
		case BP_PIPE_MINI_UP: // vertical mini pipe top
			x += 16;
			if (!(_BC & 8))
				return false;
			break;
		default:
			return false;
	}

	x &= 0xFFFF;
	u16 check = pPos->x + two;
	if (check < x) {
		if (pipeType == BP_PIPE_MINI_UP) {
			pOutVec->x = x - 8;
		} else {
			pOutVec->x = x - 16;
		}

		pOutVec->y = -(y & 0xFFF0);
		pOutVec->z = pPos->z;

		if (ClassCFC::instance->findExitAtPosition(
					dScStage_cX::instance->curArea,
					pOutEntranceID, pOutVec->x, pOutVec->y)) {

			if (pipeType == BP_PIPE_MINI_UP) {
				enteredPipeIsMini = true;
				pOutVec->y -= 16.0f;
			} else {
				enteredPipeIsMini = false;
				pOutVec->y -= 32.0f;
			}

			return true;
		}
	}

	return false;
}


bool dCollisionSensor_c::tryToEnterPipeAbove(VEC3 *pOutVec, int *pOutEntranceID) {
	float one, two;
	lineSensor_s *aLine = pAboveInfo->asLine();
	if (aLine->lineA < aLine->lineB) {
		one = aLine->lineA / 4096.0f;
		two = aLine->lineB / 4096.0f;
	} else {
		one = aLine->lineB / 4096.0f;
		two = aLine->lineA / 4096.0f;
	}

	if (abs(two - one) > 31.0f)
		return false;

	u16 basex = pPos->x + one;
	u16 basey = -(1.0f + pPos->y + (aLine->distanceFromCenter / 4096.0f));

	u32 coll1 = getTileBehaviour1At(basex, basey, layer);
	if (!(coll1 & BF_PIPE))
		return false;

	u32 coll2 = getTileBehaviour2At(basex, basey, layer);
	int pipeType = getParam(coll2);

	int moddedX = basex & 0xFFF0;

	switch (pipeType) {
		case BP_PIPE_DOWN_A: // vertical bottom entrance left
			moddedX += 32;
			break;
		case BP_PIPE_DOWN_B: // vertical bottom entrance right
			moddedX += 16;
			break;
		case BP_PIPE_MINI_DOWN: // vertical mini pipe bottom
			moddedX += 16;
			if (!(_BC & 8))
				return false;
			break;
		default:
			return false;
	}

	moddedX &= 0xFFFF;
	u16 check = pPos->x + two;
	if (check < moddedX) {
		if (pipeType == BP_PIPE_MINI_DOWN) {
			pOutVec->x = moddedX - 8;
		} else {
			pOutVec->x = moddedX - 16;
		}

		pOutVec->y = -(basey & 0xFFF0);
		pOutVec->z = pPos->z;

		if (ClassCFC::instance->findExitAtPosition(
					dScStage_cX::instance->curArea,
					pOutEntranceID, pOutVec->x, pOutVec->y)) {

			if (pipeType == BP_PIPE_MINI_DOWN) {
				enteredPipeIsMini = true;
			} else {
				enteredPipeIsMini = false;
				pOutVec->y -= 16.0f;
			}

			return true;
		}
	}

	return false;
}


bool dCollisionSensor_c::tryToEnterAdjacentPipe(VEC3 *pOutVec, int direction, int *pOutEntranceID, float fp1, float fp2) {
	float yPositions[2];
	u32 behaviours1[2];
	int pipeTypes[2];

	lineSensor_s *adjLine = pAdjacentInfo->asLine();
	if (adjLine->lineA < adjLine->lineB) {
		yPositions[0] = fp1 + pPos->y + (adjLine->lineA / 4096.0f);
		yPositions[1] = fp2 + pPos->y + (adjLine->lineB / 4096.0f);
	} else {
		yPositions[0] = fp1 + pPos->y + (adjLine->lineB / 4096.0f);
		yPositions[1] = fp2 + pPos->y + (adjLine->lineA / 4096.0f);
	}

	float xPosition = pPos->x;
	u32 correctTopType, correctBottomType, correctMiniType;
	if (direction == RIGHT) {
		correctTopType = BP_PIPE_LEFT_A; // horizontal left entrance top
		correctBottomType = BP_PIPE_LEFT_B; // horizontal left entrance bottom
		correctMiniType = BP_PIPE_MINI_LEFT; // horizontal mini pipe left
		xPosition += (1.0f + (adjLine->distanceFromCenter / 4096.0f));
	} else {
		correctTopType = BP_PIPE_RIGHT_A; // horizontal right entrance top
		correctBottomType = BP_PIPE_RIGHT_B; // horizontal right entrance bottom
		correctMiniType = BP_PIPE_MINI_RIGHT; // horizontal mini pipe right
		xPosition += -(1.0f + 1.0f + (adjLine->distanceFromCenter / 4096.0f));
	}

	for (int i = 0; i < 2; i++) {
		pipeTypes[i] = getParam(getTileBehaviour2At(xPosition, yPositions[i], layer));
		behaviours1[i] = getTileBehaviour1At(xPosition, yPositions[i], layer);
	}

	if (!(behaviours1[0] & BF_PIPE))
		return false;

	if (pipeTypes[0] == correctMiniType) {
		if (!(_BC & 8))
			return false;
		if (pipeTypes[1] != correctMiniType)
			return false;
	} else if (pipeTypes[0] == correctBottomType) {
		if (pipeTypes[1] != correctTopType && pipeTypes[1] != correctBottomType)
			return false;
	} else {
		return false;
	}

	pOutVec->x = (s16(xPosition) & 0xFFF0);
	pOutVec->y = -((s16(-yPositions[0]) & 0xFFF0) + 16);
	pOutVec->z = pPos->z;

	if (ClassCFC::instance->findExitAtPosition(
				dScStage_cX::instance->curArea,
				pOutEntranceID, pOutVec->x, 1.0f + pOutVec->y)) {

		if (pipeTypes[0] == correctMiniType) {
			enteredPipeIsMini = true;
			pOutVec->x += 8.0f;
		} else {
			enteredPipeIsMini = false;
			if (direction == RIGHT)
				pOutVec->x += 16.0f;
		}

		return true;
	}

	return false;
}


u32 dCollisionSensor_c::detectFence(lineSensor_s *info) {
	if (!owner)
		return 0;

	float f31;
	if (info->lineA < info->lineB) {
		f31 = info->lineB / 4096.0f;
	} else {
		f31 = info->lineA / 4096.0f;
	}

	u32 toReturn = 0;
	bool found = false;

	// This is shit code
	float usualY = pPos->y + f31;
	float right = pPos->x + (info->distanceFromCenter / 4096.0f);
	float bottom = 1.0f + pPos->y;
	float left = pPos->x - (1.0f + (info->distanceFromCenter / 4096.0f));
	float top = 1.8f + usualY;
	VEC3 vecArray[6] = {
		VEC3(right, usualY, 0.0f),
		VEC3(right, bottom, 0.0f),
		VEC3(left, usualY, 0.0f),
		VEC3(left, bottom, 0.0f),
		VEC3(right, top, 0.0f),
		VEC3(left, top, 0.0f)
	};
	currentFenceType = 0;

	static const u32 bits[] = {2, 8, 1, 4, 0x10, 0x20};
	for (int i = 0; i < 6; i++) {
		u32 coll1 = getTileBehaviour1At(vecArray[i].x, vecArray[i].y, owner->currentLayerID);
		u32 coll2 = getTileBehaviour1At(vecArray[i].x, vecArray[i].y, owner->currentLayerID);
		int gridType = getParam(coll2);

		if (coll1 & BF_FENCE) {
			toReturn |= bits[i];
			if (gridType == 1)
				currentFenceType = 1;
			else if (gridType >= 2)
				currentFenceType = 2;
		}
	}


	dSimpleCollider_c *iter = dSimpleCollider_c::g_listHead;
	dSimpleCollider_c *scToUse = 0;

	// Some notes:
	// Rotating round chainlink fence uses 8 for _CC, 0xE0000000 for flags
	// Remocon one is the same
	// So is moving one... who uses 9? dunno, maybe the flip panels?

	while (iter) {
		if (chainlinkMode & iter->chainlinkMode) {
			if (*pLayerID == iter->layer) {
				if (iter->_D4 == 0) {
					if (iter->_CC >= 8 && iter->_CC <= 9) {
						for (int i = 0; i < 6; i++) {
							if (toReturn & bits[i])
								continue;

							if (iter->s_80080670(&vecArray[i], 0.0f)) {
								if (iter->owner) {
									toReturn |= bits[i];
									if (iter->_CC == 8)
										currentFenceType = 2;
									else
										currentFenceType = 0;

									found = true;
									scToUse = iter;
								}
							}
						}
					}
				}
			}
		}

		if ((toReturn & 0x3F) == 0x3F)
			break;
		iter = iter->listNext;
	}


	if (_BC & 1) {
		if (toReturn) {
			if (!(toReturn & 3)) {
				if (found == false) {
					pPos->y = int((u16(usualY) & 0xFFF0) - f31);
				} else {
					if (pPos->y > _D4)
						pPos->y = _D4;
				}
			}

			if (scToUse) {
				if (owner->stageActorType == 1) {
					// this makes the player be moved while on this fence
					scToUse->addTo18List(this);
					_34 = pPos->x - scToUse->lastX;
					_38 = pPos->y - scToUse->lastY;
				}
			}
		}
	}

	_D4 = pPos->y;
	_D0 = scToUse;

	return toReturn;
}


bool dCollisionSensor_c::detectClimbingPole(lineSensor_s *info) {
	float f31;
	if (info->lineA < info->lineB) {
		f31 = pPos->y + (info->lineB / 4096.0f);
	} else {
		f31 = pPos->y + (info->lineA / 4096.0f);
	}

	float var_48 = pPos->x - (1.0f + (info->distanceFromCenter / 4096.0f));
	float f30 = pPos->x + (info->distanceFromCenter / 4096.0f);

	do {
		u32 coll2 = getTileBehaviour2At(var_48, f31, layer);
		int thing = getSubType(coll2);
		if (thing != B_SUB_POLE)
			return false;
	} while (!somethingWithFloat(&var_48, f30));

	return true;
}


bool dCollisionSensor_c::s_800722C0(VEC3 *twoVecs) {
	VEC3 originalFirstVec = twoVecs[0];

	int thing = 0;
	bool flag = false;

	u32 sFlags = pAdjacentInfo->flags;

	if (twoVecs[0].x > twoVecs[1].x)
		flag = true;

	dSimpleCollider_c *iter = dSimpleCollider_c::g_listHead;
	while (iter) {
		bool doThis = true;
		if (!flag) {
			VEC2 neverUsed = iter->_A0;
			bool result = (iter->_A0.x < twoVecs[0].x);
			if (result)
				doThis = false;
		} else {
			VEC2 neverUsed = iter->_A0;
			bool result = (iter->_A0.x > twoVecs[0].x);
			if (result)
				doThis = false;
		}

		if (doThis) {
			if (iter->s_80080E40(this, sFlags, RIGHT)) {
				if (iter->_D4 == 0 && iter->_CC >= 5 && iter->_CC <= 9) {
					if (iter->s_80080900(twoVecs, 0, 1)) {
						if (iter->s_80080670(&twoVecs[0], 0.0f)) {
							twoVecs[1] = twoVecs[0];
						}
						thing = 1;
					}
				}
			}
		}

		iter = iter->listNext;
	}

	return (thing != 0);
}


bool dCollisionSensor_c::s_80072440(VEC2 *v2one, VEC2 *v2two, float *pOutFloat) {
	float someFloat = 0.0f;
	bool flag = false;

	VEC3 oneVec(v2one->x, v2one->y, 0.0f);
	VEC3 twoVec(v2two->x, v2two->y, 0.0f);
	VEC3 vecArray[2] = {oneVec, twoVec};

	if (s_800722C0(vecArray)) {
		flag = true;
		bool thing = false;
		twoVec = vecArray[1];
		someFloat = vecArray[1].x;

		if (oneVec.x == twoVec.x)
			if (oneVec.y == twoVec.y)
				if (oneVec.z == twoVec.z)
					thing = true;

		if (thing) {
			if (pOutFloat)
				*pOutFloat = someFloat;
			return true;
		}
	}

	float chasedValue = oneVec.x;
	float f30 = oneVec.y;
	float targetValue = twoVec.x;
	float f28 = abs((twoVec.y - oneVec.y) / (twoVec.x - oneVec.x));

	s16 r28 = dBgGm_cX::instance->s_800789A0();
	s16 r30 = r28;
	s16 r27 = dBgGm_cX::instance->float_8FE70;
	s16 r29 = r27;

	while (chasedValue != targetValue) {
		Similar_to_cLib_chaseF(&chasedValue, targetValue, 1.0f);

		if (r28 != (s16(chasedValue) >> 3) || r27 != (s16(f30) >> 3)) {
			r28 = s16(chasedValue) >> 3;
			r27 = s16(f30) >> 3;

			switch (getPartialBlockExistenceAtPos(chasedValue, f30, layer)) {
				case 0:
					if (pOutFloat)
						*pOutFloat = chasedValue;
					return true;
				case 2:
					if (r30 != (s16(chasedValue) >> 3) || r29 != (s16(f30) >> 3)) {
						r30 = s16(chasedValue) >> 3;
						r29 = s16(f30) >> 3;
						u32 coll1 = getTileBehaviour1At(chasedValue, f30, layer);
						u32 coll2 = getTileBehaviour2At(chasedValue, f30, layer);

						if (coll1 & 0x819) {
							if (pOutFloat)
								*pOutFloat = chasedValue;
							return true;
						}
					}
			}
		}

		f30 += f28;
	}
	
	if (flag) {
		if (pOutFloat)
			*pOutFloat = someFloat;
		return true;
	}

	return false;
}


bool dCollisionSensor_c::s_80072790(VEC3 *twoVecs) {
	u32 toReturn = 0;
	VEC3 vecCopy = twoVecs[0];
	dSimpleCollider_c *iter = dSimpleCollider_c::g_listHead;
	u32 sFlags = pAboveInfo->flags;

	while (iter) {
		VEC2 copyThing = iter->_A0;
		bool result = (iter->_A0.y >= twoVecs[0].y);
		if (result) {
			if (iter->s_80080E40(this, sFlags, RIGHT)) {
				if (iter->_D4 == 0 && iter->_CC >= 4 && iter->_CC <= 9) {
					if (iter->s_80080900(twoVecs, 0, 1)) {
						if (iter->s_80080670(&vecCopy, 0.0f)) {
							twoVecs[1] = twoVecs[0];
						}

						toReturn = 1;
					}
				}
			}
		}

		iter = iter->listNext;
	}

	return (toReturn != 0);
}


bool dCollisionSensor_c::s_800728C0(VEC2 *vecParam, float *pOutFloat) {
	bool flag = false;

	float fResult = 0.0f;

	VEC3 firstVec(vecParam->x, vecParam->y, 0.0f);
	VEC3 secondVec(vecParam->x, 256.0f+vecParam->y, 0.0f);
	VEC3 twoVecs[] = {firstVec, secondVec};

	if (s_80072790(twoVecs)) {
		flag = true;
		fResult = twoVecs[1].y;
		secondVec = twoVecs[1];
	}

	float x = vecParam->x, y = vecParam->y;
	float targetY = secondVec.y;
	while (targetY >= y) {
		u32 coll1 = getTileBehaviour1At(x, y, layer);
		u32 coll2 = getTileBehaviour2At(x, y, layer);
		u16 convertedY = u16(-y) & 0xFFF0;

		if (coll1 & BF_FLIPPED_SLOPE) {
			int slopeType = getParam(coll2);

			flag = true;

			int fxFractionalY = int(4096.0f * vecParam->y) & 0xFFFF;
			int fxBasePos = 4096.0f * flippedSlopeParams[slopeType].basePos;
			float fxWhereAt = fxFractionalY * flippedSlopeParams[getParam(coll2)].colHeight;
			int fxAbsolutePos = float(fxBasePos) + fxWhereAt;

			fResult = -(float(convertedY) + (fxAbsolutePos / 4096.0f));
			break;

		} else if (coll1 & BF_4899) {
			switch (getPartialBlockExistenceAtPos(x, y, layer)) {
				case 0:
					flag = true;
					fResult = -((u16(-y) & 0xFFF8) + 8);
					break;
				case 2:
					flag = true;
					fResult = -(convertedY + 16);
					break;
			}

			if (flag)
				break;
		}

		y += 8.0f;
	}

	if (flag) {
		if (fResult < firstVec.y)
			fResult = firstVec.y;
		if (pOutFloat)
			*pOutFloat = fResult;
		return true;
	}
	return false;
}


u32 dCollisionSensor_c::calculateBelowCollisionWithSmokeEffect() {
	if (!pBelowInfo)
		return 0;

	u32 oldOutput = outputMaybe;
	u32 toReturn = calculateBelowCollision();

	if (pClass2DC) {
		// TODO: REPLACE WITH METHOD
		if (BasicColliderListProcessor1(pClass2DC, toReturn, 0, 0)) {
			toReturn |= 0x8000;
			outputMaybe |= 0x8000;

			// TODO: REPLACE WITH METHOD
/*			if ((s_80070720(pClass2DC) & 0xFFFF) == 0xC)
				if (!(oldOutput & 0x8000))
					if (pClass2DC->owner) {
						VEC3 effectPos(
								pClass2DC->owner->pos.x,
								pClass2DC->owner->pos.y,
								5500.0f);

						SpawnEffect("Wm_en_spsmoke", 0, &effectPos, 0, 0);
					}*/
		}
	}

	return toReturn;
}


u32 dCollisionSensor_c::calculateAdjacentCollisionAlternate(float *pFloat) {
	if (!pAdjacentInfo)
		return 0;

	u32 ret = calculateAdjacentCollision(pFloat);
	initialXAsSetByJumpDai = pPos->x;
	return ret;
}


dCollisionSensor_c::checkStuffCBType dCollisionSensor_c::s_checkStuffCBs[3] = {
	&dCollisionSensor_c::checkStuffCB,
	&dCollisionSensor_c::checkStuffCB_Wrap,
	&dCollisionSensor_c::checkStuffCB_Wrap,
};
dCollisionSensor_c::secondCBType dCollisionSensor_c::s_secondCBs[3] = {
	&dCollisionSensor_c::secondCB,
	&dCollisionSensor_c::secondCB_Wrap,
	&dCollisionSensor_c::secondCB_Wrap,
};
dCollisionSensor_c::thirdCBType dCollisionSensor_c::s_thirdCBs[3] = {
	&dCollisionSensor_c::thirdCB,
	&dCollisionSensor_c::thirdCB_Wrap,
	&dCollisionSensor_c::thirdCB_Wrap,
};
/*static*/ void dCollisionSensor_c::setupEverything(int wrapType) {
	s_checkStuffCB = s_checkStuffCBs[wrapType];
	s_secondCB = s_secondCBs[wrapType];
	s_thirdCB = s_thirdCBs[wrapType];
}


bool dCollisionSensor_c::getBelowSensorHLine(hLine_s *outLine) {
	if (pBelowInfo) {
		switch (pBelowInfo->flags & SENSOR_TYPE_MASK) {
			case SENSOR_POINT: {
				pointSensor_s *point = pBelowInfo->asPoint();
				outLine->x1 = point->x / 4096.0f;
				outLine->x2 = point->x / 4096.0f;
				outLine->y = point->y / 4096.0f;
				break; }
			case SENSOR_LINE: {
				lineSensor_s *line = pBelowInfo->asLine();
				if (line->lineA < line->lineB) {
					outLine->x1 = line->lineA / 4096.0f;
					outLine->x2 = line->lineB / 4096.0f;
				} else {
					outLine->x1 = line->lineB / 4096.0f;
					outLine->x2 = line->lineA / 4096.0f;
				}
				outLine->y = line->distanceFromCenter / 4096.0f;
				break; }
		}

		return true;
	}

	return false;
}


bool dCollisionSensor_c::getAboveSensorHLine(hLine_s *outLine) {
	if (pAboveInfo) {
		switch (pAboveInfo->flags & SENSOR_TYPE_MASK) {
			case SENSOR_POINT: {
				pointSensor_s *point = pAboveInfo->asPoint();
				outLine->x1 = point->x / 4096.0f;
				outLine->x2 = point->x / 4096.0f;
				outLine->y = point->y / 4096.0f;
				break; }
			case SENSOR_LINE: {
				lineSensor_s *line = pAboveInfo->asLine();
				if (line->lineA < line->lineB) {
					outLine->x1 = line->lineA / 4096.0f;
					outLine->x2 = line->lineB / 4096.0f;
				} else {
					outLine->x1 = line->lineB / 4096.0f;
					outLine->x2 = line->lineA / 4096.0f;
				}
				outLine->y = line->distanceFromCenter / 4096.0f;
				break; }
		}

		return true;
	}

	return false;
}


bool dCollisionSensor_c::getAdjacentSensorVLine(vLine_s *outLine, int direction) {
	if (pAdjacentInfo) {
		switch (pAdjacentInfo->flags & SENSOR_TYPE_MASK) {
			case SENSOR_POINT: {
				pointSensor_s *point = pAdjacentInfo->asPoint();
				outLine->x = point->x / 4096.0f;
				outLine->y1 = point->y / 4096.0f;
				outLine->y2 = point->y / 4096.0f;
				break; }
			case SENSOR_LINE: {
				lineSensor_s *line = pAdjacentInfo->asLine();
				outLine->x = line->distanceFromCenter / 4096.0f;
				if (line->lineA < line->lineB) {
					outLine->y1 = line->lineA / 4096.0f;
					outLine->y2 = line->lineB / 4096.0f;
				} else {
					outLine->y1 = line->lineB / 4096.0f;
					outLine->y2 = line->lineA / 4096.0f;
				}
				break; }
		}

		if (direction != RIGHT)
			outLine->x = (-outLine->x) - 1.0f;

		return true;
	}

	return false;
}


void dCollisionSensor_c::s_800731E0(dSimpleCollider_c *sColl, VEC3 *somePos) {
	sCollBelow = sColl;
	_34 = somePos->x - sColl->lastX;
	_38 = somePos->y - sColl->lastY;
}


u32 dCollisionSensor_c::checkStuffCB_Wrap(u32 sFlags, float someFloat) {
	u32 toReturn = checkStuffCB(sFlags, 0.0f);
	if (toReturn == 0)
		toReturn = checkStuffCB(sFlags, dBgGm_cX::instance->float_8FEA0);
	if (toReturn == 0)
		toReturn = checkStuffCB(sFlags, -dBgGm_cX::instance->float_8FEA0);
	return toReturn;
}


bool dCollisionSensor_c::s_80073290(dSimpleCollider_c *sColl, hLine_s *pLine, VEC3 *pOutVec, float someFloat) {
	if (sColl->isRound == 1) {
		VEC3 firstVec(
				(pLine->x1 + pLine->x2) * 0.5f,
				pLine->y,
				0.0f);

		if (sColl->s_80080670(&firstVec, someFloat)) {
			float f29 = sColl->lastX - firstVec.x;
			float f31 = 0.0f;
			float f30 = (sColl->diameter*sColl->diameter) - (f29*f29);
			if (f30 <= f31) {
			} else {
				f31 = f30 * nw4r::math::FrSqrt(f30);
			}

			s16 calcedAngle = cM_atan2s(f29,f31);
			if (abs(double(calcedAngle)) < 12743.0) {
				currentSlopeAngle = calcedAngle;
				pOutVec->x = pPos->x;
				pOutVec->y = f31 + sColl->lastY;
				pOutVec->z = pPos->z;
				return true;
			}
		}
	} else {
		int rot = *sColl->ptrToRotationShort;
		if (rot == 0) {
			if (pLine->y <= sColl->unkArray[0].y
					&& pLine->y >= sColl->unkArray[2].y
					&& pLine->x1 <= (someFloat + sColl->unkArray[2].x)
					&& pLine->x2 >= (someFloat + sColl->unkArray[0].y)) {

				currentSlopeAngle = 0;
				pOutVec->x = pPos->x;
				pOutVec->y = sColl->unkArray[0].y;
				pOutVec->z = pPos->z;
				return true;
			}
		} else {
			// WHAT?!?
			if (sColl->s_80080670((VEC3*)pLine, someFloat)) {
				static const int things[] = {3, 2, 1, 0};
				int thingIndex = (rot >> 14) & 3;

				VEC2 thing = sColl->unkArray[things[thingIndex]];
				VEC2 realAnotherThing;

				if (thing.x < pLine->x1) {
					int vecID = things[(thingIndex+1)&3];
					VEC2 anotherThing(
							sColl->unkArray[vecID].x - thing.x,
							sColl->unkArray[vecID].y - thing.y);
					realAnotherThing = anotherThing;
					rot = abs(cM_atan2s(realAnotherThing.x, -realAnotherThing.y)) - 0x4000;
				} else {
					int vecID = things[(thingIndex-1)&3];
					VEC2 anotherThing_(
							sColl->unkArray[vecID].x - thing.x,
							sColl->unkArray[vecID].y - thing.y);
					realAnotherThing = anotherThing_;
					rot = -(abs(cM_atan2s(realAnotherThing.x, -realAnotherThing.y)) - 0x4000);
				}

				if (abs(double(rot)) < 12743.0) {
					if (0.0f != realAnotherThing.x) {
						float absXDiff = abs(pLine->x1 - thing.x);
						float finalY = (realAnotherThing.y * absXDiff) / abs(realAnotherThing.x);
						finalY += thing.y;

						NormalizeVec2(&realAnotherThing);

						nw4r::math::CosFIdx(0.00390625f * FastS16toFloat(rot));

						currentSlopeAngle = rot;
						pOutVec->x = pPos->x;
						pOutVec->y = finalY;
						pOutVec->z = pPos->z;
						return true;
					}
				}
			}
		}
	}

	return false;
}


u32 dCollisionSensor_c::checkStuffCB(u32 sFlags, float someFloat) {
	hLine_s originalLine;
	if (!getBelowSensorHLine(&originalLine))
		return 0;

	u32 toReturn = 0;

	dSimpleCollider_c *iter = dSimpleCollider_c::g_listHead;

	VEC3 one(pPos->x, pPos->y, pPos->z);
	hLine_s realLine = {
		originalLine.x1 + pPos->x,
		originalLine.x2 + pPos->x,
		originalLine.y + pPos->y};

	while (iter) {
		if (iter->s_80080E40(this, sFlags, DOWN)) {
			hLine_s lineCopy = realLine;
			VEC3 outVec;

			if (s_80073290(iter, &lineCopy, &outVec, someFloat)) {
				VEC2 var_A8(
						iter->unkArray[0].x + iter->unkArray[2].x,
						iter->unkArray[0].y + iter->unkArray[2].y);
				VEC2 var_A0(
						var_A8.x * 0.5f,
						var_A8.y * 0.5f);
				VEC3 var_80(
						var_A0.x + someFloat,
						var_A0.y,
						0.0f);

				if ((iter->_CC >= SC_ST_COIN) && (iter->_CC <= SC_ST_POW_COIN)) {
					s_80074FF0(iter, &var_80, sFlags);
				} else {
					u32 result = 0x10000;
					if ((sFlags & SENSOR_8000000) && (iter->flags & SCF_20)) {
						result = 0x400000;
						iter->_D8 |= 4;
					} else if ((sFlags & SENSOR_BREAK_BLOCK) && (iter->flags & SCF_40)) {
						result = 0x400000;
						iter->_D8 |= 2;
					} else {
						if ((iter->flags & SCF_200) && (sFlags & SENSOR_ACTIVATE_QUESTION)) {
							result |= 0x800000;
							iter->_D8 |= 0x20;
						}

						if (iter->flags & SCF_80) {
							if (sFlags & SENSOR_BREAK_BRICK) {
								result |= 0x400000;
								iter->_D8 |= 8;
							} else if (sFlags & SENSOR_HIT_BRICK) {
								result |= 0x800000;
								iter->_D8 |= 0x20;
							}
						}

						if ((iter->flags & SCF_100) && (sFlags & SENSOR_HIT_OR_BREAK_BRICK)) {
							result |= 0x400000;
							iter->_D8 |= 8;
						}

						if (returnSomethingBasedOnSlopeAngle() != 0)
							result |= 0x4000;

						if ((sFlags & SENSOR_8) && (iter->_CC == SC_ST_ENTERABLE_PIPE))
							iter->_D8 |= 0x10;

						if ((sFlags & SENSOR_10000000) && (result & 0x400000))
							result &= ~0x10000;

						if (iter->s_80081030(sFlags)) {
							if (iter->flags & SCF_400000)
								result |= 0x20000;

							if (iter->flags & SCF_4)
								setBelowSubType(B_SUB_ICY);

							if (iter->flags & SCF_8) {
								setBelowSubType(B_SUB_ICY);
								result |= 0x2000000;
							}

							if (iter->flags & SCF_10)
								setBelowSubType(B_SUB_NO_SLIDING);

							float calcThis = outVec.y - originalLine.y;
							if (one.y < calcThis)
								one.y = calcThis;
						}
					}

					if (owner)
						s_800731E0(iter, &outVec);

					toReturn |= result;
				}


				dStageActor_c *player = s_800705B0(outVec.y);
				if (player) {
					bool doThis =
						(iter->_CC == SC_ST_NULL) |
						(iter->_CC == SC_ST_1) |
						(iter->_CC == SC_ST_2);
					if (doThis) {
						if (iter->callCallback1(player))
							if (player->_vf68(iter))
								player->_34A |= 4;
					}

					if (iter->belowCallback)
						iter->belowCallback(iter->owner, player);
				}
			}
		}

		iter = iter->listNext;
	}


	if (toReturn != 0) {
		float calcThis = 8.0f + pPos->y;
		if (one.y >= calcThis)
			one.y = calcThis;

		pPos->x = one.x;
		pPos->y = one.y;
	}

	return toReturn;
}


void dCollisionSensor_c::s_80073AC0(dSimpleCollider_c *sColl, VEC3 *vec) {
	if (_BC & 2)
		if (sColl) {
			sColl->addTo28List(this);
			_3C = vec->x - sColl->lastX;
			_40 = vec->y = sColl->lastY;
		}
}


u32 dCollisionSensor_c::secondCB_Wrap(u32 sFlags, float someFloat) {
	u32 toReturn = secondCB(sFlags, 0.0f);
	if (toReturn == 0)
		toReturn = secondCB(sFlags, dBgGm_cX::instance->float_8FEA0);
	if (toReturn == 0)
		toReturn = secondCB(sFlags, -dBgGm_cX::instance->float_8FEA0);
	return toReturn;
}


bool dCollisionSensor_c::s_80073BC0(dSimpleCollider_c *sColl, hLine_s *pLine, VEC3 *pOutVec, float someFloat) {
	if (sColl->isRound == 1) {
		VEC3 firstVec(
				(pLine->x1 + pLine->x2) * 0.5f,
				pLine->y,
				0.0f);

		if (sColl->s_80080670(&firstVec, someFloat)) {
			float f30 = sColl->lastX - firstVec.x;
			float f28 = 0.0f;
			float f29 = (sColl->diameter*sColl->diameter) - (f30*f30);
			if (f29 <= f28) {
			} else {
				f28 = f29 * nw4r::math::FrSqrt(f29);
			}

			s16 calcedAngle = cM_atan2s(f30,f28);
			if (abs(double(calcedAngle)) < 12743.0) {
				currentFlippedSlopeAngle = calcedAngle;
				pOutVec->x = pPos->x;
				pOutVec->y = sColl->lastY - f28;
				pOutVec->z = pPos->z;
				return true;
			}
		}
	} else {
		int rot = *sColl->ptrToRotationShort;
		if (rot == 0) {
			if (pLine->y <= sColl->unkArray[0].y
					&& pLine->y > sColl->unkArray[2].y
					&& pLine->x1 <= (someFloat + sColl->unkArray[2].x)
					&& pLine->x2 > (someFloat + sColl->unkArray[0].y)) {

				currentFlippedSlopeAngle = 0;
				pOutVec->x = pPos->x;
				pOutVec->y = sColl->unkArray[2].y;
				pOutVec->z = pPos->z;
				return true;
			}
		} else {
			if (!sColl->s_80080880(pLine->x1, pLine->x2, someFloat))
				return false;

			static const int things[] = {1, 0, 3, 2};
			int thingIndex = (rot >> 14) & 3;

			VEC3 veccy(pLine->x1, pLine->y, 0.0f);
			float usedForSWF = pLine->x1;

			int whatReturn = 0;

			do {
				veccy.x = usedForSWF;

				if (sColl->s_80080670(&veccy, someFloat)) {
					VEC2 var_88 = sColl->unkArray[things[thingIndex]];
					VEC2 realAnotherThing;

					s16 rot;

					if (var_88.x < veccy.x) {
						int vecID = things[(thingIndex-1)&3];
						VEC2 anotherThing(
								sColl->unkArray[vecID].x - var_88.x,
								sColl->unkArray[vecID].y - var_88.y);
						realAnotherThing = anotherThing;
						rot = abs(cM_atan2s(realAnotherThing.x, -realAnotherThing.y)) - 0x4000;
					} else {
						int vecID = things[(thingIndex+1)&3];
						VEC2 anotherThing(
								sColl->unkArray[vecID].x - var_88.x,
								sColl->unkArray[vecID].y - var_88.y);
						realAnotherThing = anotherThing;
						rot = -(abs(cM_atan2s(realAnotherThing.x, -realAnotherThing.y)) - 0x4000);
					}

					if (abs(double(rot)) < 12743.0) {
						if (0.0f != realAnotherThing.x) {
							float absXDiff = abs(veccy.x - var_88.x);
							realAnotherThing.y = (realAnotherThing.y * absXDiff) / abs(realAnotherThing.x);

							if (whatReturn == 0 || pOutVec->y > realAnotherThing.y) {
								currentFlippedSlopeAngle = rot;
								whatReturn = 1;
								pOutVec->x = pPos->x;
								pOutVec->y = realAnotherThing.y;
								pOutVec->z = pPos->z;
							}
						}
					}
				}

			} while (!somethingWithFloat(&usedForSWF, pLine->x2));

			return (whatReturn != 0);
		}
	}

	return false;
}


u32 dCollisionSensor_c::secondCB(u32 sFlags, float someFloat) {
	hLine_s originalLine;
	if (!getAboveSensorHLine(&originalLine))
		return 0;

	u32 toReturn = 0;

	VEC3 one = *pPos;
	hLine_s realLine = {
		originalLine.x1 + pPos->x,
		originalLine.x2 + pPos->x,
		originalLine.y + pPos->y
	};

	dSimpleCollider_c *iter = dSimpleCollider_c::g_listHead;

	while (iter) {
		if (iter->s_80080E40(this, sFlags, UP)) {
			VEC3 anotherVec;
			if (s_80073BC0(iter, &realLine, &anotherVec, someFloat)) {
				VEC2 var_A8(
						iter->unkArray[0].x + iter->unkArray[2].x,
						iter->unkArray[0].y + iter->unkArray[2].y);
				VEC2 var_A0(
						var_A8.x * 0.5f,
						var_A8.y * 0.5f);
				VEC3 var_8C(
						var_A0.x + someFloat,
						var_A0.y,
						0.0f);

				if ((iter->_CC >= SC_ST_COIN) && (iter->_CC <= SC_ST_POW_COIN)) {
					s_80074FF0(iter, &var_8C, sFlags);
				} else {
					_C4 = 0;

					u32 result = 0x20000000;
					if (iter->flags & SCF_1000000)
						_C4 = 2;
					if (iter->flags & SCF_2000000)
						_C4 = 1;

					if ((sFlags & SENSOR_8000000) && (iter->flags & SCF_20)) {
						result = 0x40000000;
						iter->_D8 |= 4;

					} else if ((sFlags & SENSOR_BREAK_BLOCK) &&
							((iter->flags & SCF_40) || (iter->flags & SCF_100))) {
						result = 0x40000000;
						iter->_D8 |= 2;

					} else {
						if (owner && ((owner->speed.y > 0.0f) || (sFlags & SENSOR_4000000))) {
							if ((iter->flags & SCF_200) && (sFlags & SENSOR_ACTIVATE_QUESTION)) {
								result |= 0x80000000;
								iter->_D8 |= 0x20;
							}

							if (iter->flags & SCF_80) {
								if (sFlags & SENSOR_BREAK_BRICK) {
									result |= 0x40000000;
									iter->_D8 |= 2;
								} else if (sFlags & SENSOR_HIT_BRICK) {
									result |= 0x80000000;
									iter->_D8 |= 0x20;
								}
							}

							if ((iter->flags & SCF_100) && (sFlags & SENSOR_HIT_OR_BREAK_BRICK)) {
								result |= 0x40000000;
								iter->_D8 |= 8;
							}

							if ((iter->flags & SCF_400) && (sFlags & SENSOR_2000000)) {
								result |= 0x100;
								iter->_D8 |= 8;
							}
						}

						if ((sFlags & SENSOR_10000000) && (result & 0x40000000))
							result &= ~0x4000000;

						if (iter->s_80081030(sFlags))
							one.y = anotherVec.y - originalLine.y;
					}

					if (owner)
						sCollAbove = iter;

					toReturn |= result;
				}

				dStageActor_c *player = s_800705B0(anotherVec.y);
				if (player) {
					bool doThis =
						(iter->_CC == SC_ST_NULL) |
						(iter->_CC == SC_ST_1) |
						(iter->_CC == SC_ST_2);
					if (doThis) {
						if (iter->callCallback2(player))
							if (player->_vf68(iter))
								player->_34A |= 8;
					}

					if (iter->aboveCallback)
						iter->aboveCallback(iter->owner, player);
				}
			}
		}

		iter = iter->listNext;
	}

	if (toReturn != 0) {
		float calcThis = pPos->y - 8.0f;
		if (one.y <= calcThis)
			one.y = calcThis;

		pPos->x = one.x;
		pPos->y = one.y;
	}

	return toReturn;
}


u32 dCollisionSensor_c::thirdCB_Wrap(u32 sFlags, int direction, float someFloat) {
	u32 toReturn = thirdCB(sFlags, direction, 0.0f);
	if (toReturn == 0)
		toReturn = thirdCB(sFlags, direction, dBgGm_cX::instance->float_8FEA0);
	if (toReturn == 0)
		toReturn = thirdCB(sFlags, direction, -dBgGm_cX::instance->float_8FEA0);
	return toReturn;
}


u32 dCollisionSensor_c::s_80074470(dSimpleCollider_c *sColl, int direction, vLine_s *pLine, VEC3 *pOutVec, float someFloat) {
	s16 currentRotation = *sColl->ptrToRotationShort;

	if (sColl->isRound == 1 || currentRotation != 0) {
		if (outputMaybe & 0x10000) {
			if (sCollBelow) {
				if (sCollBelow == sColl)
					return 0;
				int checkID = sColl->id;
				if (checkID != -1 && checkID == sCollBelow->id) {
					Similar_to_cLib_chaseF(&pLine->y1, pLine->y2, 7.0f);
				}
			}
		} else {
			Similar_to_cLib_chaseF(&pLine->y1, pLine->y2, 7.0f);
		}
	}

	u32 toReturn = 0;

	if (sColl->isRound == 1) {
		VEC3 var_B4(pLine->x, pLine->y1, 0.0f);
		float var_E4 = pLine->y1;

		do {
			var_B4.y = var_E4;
			if (sColl->s_80080670(&var_B4, someFloat)) {
				float yDiff = sColl->lastY - var_B4.y;
				float f28 = (sColl->diameter*sColl->diameter) - (yDiff*yDiff);
				float calced;
				if (f28 <= 0.0f)
					calced = 0.0f;
				else
					calced = f28 * nw4r::math::FrSqrt(f28);

				float newRawX;
				if (direction == LEFT)
					newRawX = sColl->lastX + calced;
				else
					newRawX = sColl->lastX - calced;

				pOutVec->x = ConvertXPosForWrapping(newRawX);
				pOutVec->y = pPos->y;
				pOutVec->z = pPos->z;

				if (var_B4.y == pLine->y1)
					toReturn |= 4;
				if (var_B4.y == pLine->y2)
					toReturn |= 2;

				if (var_B4.y != pLine->y1 && var_B4.y != pLine->y2)
					toReturn |= 1;
			}
		} while (!somethingWithFloat(&var_E4, pLine->y2));

	} else if (currentRotation == 0) {
		if (pLine->x >= (someFloat+sColl->unkArray[0].x)
				&& pLine->x <= (someFloat+sColl->unkArray[2].x)) {

			if (pLine->y2 <= sColl->unkArray[0].y
					&& pLine->y2 >= sColl->unkArray[2].y)
				toReturn |= 2;

			if (pLine->y1 <= sColl->unkArray[0].y
					&& pLine->y1 >= sColl->unkArray[2].y)
				toReturn |= 4;

			if (pLine->y2 >= sColl->unkArray[0].y
					&& pLine->y1 <= sColl->unkArray[2].y)
				toReturn = 1;
		}

		float newRawX;
		if (direction == LEFT)
			newRawX = sColl->unkArray[2].x;
		else
			newRawX = sColl->unkArray[0].x;

		pOutVec->x = ConvertXPosForWrapping(newRawX);
		pOutVec->y = pPos->y;
		pOutVec->z = pPos->z;

		if ((toReturn & 6) == 6) {
			toReturn |= 8;
			s_80073AC0(sColl, pOutVec);
		}

	} else {
		if (!sColl->s_80080880(pLine->x, pLine->x, someFloat))
			return 0;

		static const int things[] = {0, 3, 2, 1};
		int thingIndex = (currentRotation >> 14) & 3;

		VEC3 veccy(pLine->x, pLine->y1, 0.0f);
		float usedForSWF = pLine->y1;

		do {
			veccy.y = usedForSWF;

			if (sColl->s_80080670(&veccy, someFloat)) {
				VEC2 choice_C8, choice_D0, var_D8;

				if (direction == RIGHT) {
					var_D8 = sColl->unkArray[things[thingIndex]];
					if (var_D8.y < veccy.y) {
						choice_C8 = var_D8;
						choice_D0 = sColl->unkArray[things[(thingIndex+1)&3]];
					} else {
						choice_D0 = var_D8;
						choice_C8 = sColl->unkArray[things[(thingIndex-1)&3]];
					}
				} else {
					thingIndex = (thingIndex + 2) & 3;
					var_D8 = sColl->unkArray[things[thingIndex]];
					if (var_D8.y < veccy.y) {
						choice_D0 = var_D8;
						choice_C8 = sColl->unkArray[things[(thingIndex-1)&3]];
					} else {
						choice_C8 = var_D8;
						choice_D0 = sColl->unkArray[things[(thingIndex+1)&3]];
					}
				}

				VEC2 var_E0(choice_D0.x - choice_C8.x, choice_D0.y - choice_C8.y);

				if (0.0f != var_E0.y) {
					int newAngle = nw4r::math::Atan2Idx(var_E0.x, -var_E0.y);
					if (newAngle < 3641 || newAngle > 29127) {
						float absYDiff = abs(veccy.y - choice_C8.y);
						float calcThis = ((var_E0.y * absYDiff) / abs(var_E0.y)) + choice_C8.x;

						if (toReturn == 0
								|| (direction == RIGHT && pOutVec->x > calcThis)
								|| (direction == LEFT && pOutVec->x < calcThis)) {

							currentAdjacentSlopeAngle = newAngle;
							pOutVec->x = ConvertXPosForWrapping(calcThis);
							pOutVec->y = pPos->y;
							pOutVec->z = pPos->z;
						}

						if (veccy.y == pLine->y1)
							toReturn |= 4;
						if (veccy.y == pLine->y2)
							toReturn |= 2;
						if (veccy.y != pLine->y1 && veccy.y != pLine->y2)
							toReturn |= 1;

						if (abs(double(newAngle)) <= 3640.0 || abs(double(newAngle)) > 29128.0) {
							toReturn |= 8;
							s_80073AC0(sColl, pOutVec);
						}
					}
				}
			}

		} while (!somethingWithFloat(&usedForSWF, pLine->y2));
	}

	return toReturn;
}


u32 dCollisionSensor_c::thirdCB(u32 sFlags, int direction, float someFloat) {
	vLine_s originalLine;
	if (!getAdjacentSensorVLine(&originalLine, direction))
		return 0;

	u32 toReturn = 0;
	dSimpleCollider_c *iter = dSimpleCollider_c::g_listHead;
	VEC3 originalPos = *pPos;

	while (iter) {
		if (iter->s_80080E40(this, sFlags, direction)) {
			vLine_s realLine = {
					originalLine.x + originalPos.x,
					originalLine.y1 + originalPos.y,
					originalLine.y2 + originalPos.y};

			VEC3 var_BC;
			vLine_s realLineCopy = realLine;
			u32 retOf74470 = s_80074470(iter, direction, &realLineCopy, &var_BC, someFloat);

			if (retOf74470) {
				u32 orWith = 0;

				VEC2 var_D8(
						iter->unkArray[0].x + iter->unkArray[2].x,
						iter->unkArray[0].y + iter->unkArray[2].y);
				VEC2 var_D0(var_D8.x * 0.5f, var_D8.y * 0.5f);
				VEC3 var_B0(var_D0.x + someFloat, var_D0.y, 0.0f);

				if (iter->_CC >= SC_ST_COIN && iter->_CC <= SC_ST_POW_COIN) {
					s_80074FF0(iter, &var_B0, sFlags);
				} else {
					orWith = 4 << direction;
					orWith |= (0x40 << direction);
					if (retOf74470 & 8)
						orWith |= (0x10 << direction);

					if ((sFlags & SENSOR_8000000) && (iter->flags & SCF_20)) {
						orWith = 0x100;
						iter->_D8 |= 4;
					} else if ((sFlags & SENSOR_BREAK_BLOCK) && (iter->flags & SCF_40)) {
						orWith = 0x100;
						iter->_D8 |= 2;
					} else {
						if ((iter->flags & SCF_200) && (sFlags & SENSOR_ACTIVATE_QUESTION)) {
							orWith |= 0x200;
							iter->_D8 |= 0x20;
						}
						if ((iter->flags & SCF_100) && (sFlags & SENSOR_HIT_OR_BREAK_BRICK)) {
							orWith |= 0x100;
							iter->_D8 |= 8;
						}
						if (iter->flags & SCF_80) {
							if (sFlags & SENSOR_BREAK_BRICK) {
								orWith |= 0x100;
								iter->_D8 |= 8;
							} else if (sFlags & SENSOR_HIT_BRICK) {
								orWith |= 0x200;
								iter->_D8 |= 0x20;
							}
						}
						if ((iter->flags & SCF_400) && (sFlags & SENSOR_2000000)) {
							orWith |= 0x100;
							iter->_D8 |= 8;
						}

						orWith |= (1 << direction);
						if ((sFlags & SENSOR_10000000) && (orWith & 0x100))
							orWith &= ~(1 << direction);

						if (iter->s_80081030(sFlags)) {
							if (direction != RIGHT) {
								float calcThis = realLine.x - (var_BC.x - 1.0f);
								if (calcThis <= 0.0f)
									originalPos.x -= calcThis;
							} else {
								float calcThis = realLine.x - (0.89999998f + var_BC.x);
								if (calcThis >= 0.0f)
									originalPos.x -= calcThis;
							}
						}

						if (whichController____ != -1) {
							orWith = (retOf74470 == 2) ? (orWith|0x800) : (orWith|0x1000);
						}
					}

					if (owner) {
						sCollAdjacentLast = iter;
						sCollAdjacent[direction] = iter;
					}
				}

				toReturn |= orWith;

				dStageActor_c *thing = owner;
				if (whichController____ != -1 && retOf74470 == 2) {
					thing = GetSpecificPlayerActor(whichController____);
				}

				if (thing) {
					bool doThis =
						(iter->_CC == SC_ST_NULL) |
						(iter->_CC == SC_ST_1) |
						(iter->_CC == SC_ST_2);
					if (doThis) {
						static const int flagsFor34A[] = {2, 1};

						if (iter->callCallback3(thing, direction))
							if (thing->_vf68(iter))
								thing->_34A |= flagsFor34A[direction];
					}

					if (iter->adjacentCallback)
						iter->adjacentCallback(iter->owner, thing, direction);
				}
			}
		}

		iter = iter->listNext;
	}

	if (toReturn != 0) {
		float wrappedX = ConvertXPosForWrapping(pPos->x);

		float calcX1 = 8.0f + wrappedX;
		if (originalPos.x >= calcX1)
			originalPos.x = calcX1;

		float calcX2 = wrappedX - 8.0f;
		if (originalPos.x <= calcX2)
			originalPos.x = calcX2;

		pPos->x = originalPos.x;
		pPos->y = originalPos.y;
	}

	return toReturn;
}

// perfect
bool dCollisionSensor_c::s_80074FF0(dSimpleCollider_c *sColl, VEC3 *pVec, u32 sFlags) {
	int type = BP_COIN_NULL;
	if (sColl->_CC == SC_ST_RED_COIN)
		type = BP_COIN_2;
	if (sColl->_CC == SC_ST_POW_COIN)
		type = BP_COIN_POW;

	if (collectCoin(BF_COIN, type, sFlags, pVec, false)) {
		sColl->_D4 = 1;
		sColl->_D8 |= 1;
		return true;
	} else {
		return false;
	}
}

// perfect
void dCollisionSensor_c::clearStoredSColls() {
	sCollBelow = 0;
	sCollAbove = 0;
	sCollAdjacentLast = 0;
	sCollAdjacent[0] = 0;
	sCollAdjacent[1] = 0;
}


void dCollisionSensor_c::s_80075090() {
	_6C = 0;
	if (sCollBelow && sCollBelow->isAddedToList == 1) {
		_6C = sCollBelow->_C;
		sCollBelow->_C = this;
	}

	_70 = 0;
	if (sCollAbove && sCollAbove->isAddedToList == 1) {
		_70 = sCollAbove->_10;
		sCollAbove->_10 = this;
	}

	_74 = 0;
	/*
	 * what? CW generates some weird ass dead code for this
	if (sCollAdjacentLast && sCollAdjacentLast->isAddedToList == 1) {
		_74 = sCollAdjacentLast->_14;
		sCollAdjacentLast->_14 = this;
	}
	*/

	for (int i = 0; i < 2; i++) {
		_78[i] = 0;
		_80[i] = 0;
	}

	for (int i = 0; i < 2; i++) {
		dSimpleCollider_c *sColl = sCollAdjacent[i];
		if (sColl && sColl->isAddedToList == 1) {
			if (s_800751C0(sColl, sColl->_14))
				sColl->_14 = this;
		}
	}

	clearStoredSColls();
}

bool dCollisionSensor_c::s_800751C0(dSimpleCollider_c *sColl, dCollisionSensor_c *what) {
	if (_80[0] == sColl)
		return false;
	if (_80[1] == sColl)
		return false;

	for (int i = 0; i < 2; i++) {
		if (_78[i] == 0) {
			_78[i] = what;
			_80[i] = sColl;
			return true;
		}
	}

	return false;
}

dCollisionSensor_c *dCollisionSensor_c::s_80075230(dSimpleCollider_c *sColl) {
	for (int i = 0; i < 2; i++) {
		if (_80[i] == sColl)
			return _78[i];
	}

	return 0;
}

/*static*/ int dCollisionSensor_c::checkPositionForLiquids(float x, float y, int layer, float *unkDestFloat) {
	s_58F4 = 0;

	dScStage_cX *stage = dScStage_cX::instance;
	CWCI *cwci = CWCI::instance;

	if (
			(stage->curWorld == 7 && stage->curLevel == 2) ||
			(stage->curWorld == 7 && stage->curLevel == 0x17 && stage->curArea == 3)
	   ) {
	   	// Use wavy lava
	   	if (unkDestFloat) {
	   		int xToCheck = (x - cwci->screenLeft) * 0.125f;
	   		if (xToCheck <= 79) {
				float height = dBgGm_cX::instance->wavyLavaHeights[xToCheck];
				*unkDestFloat = height;
				if (height >= y)
					return 3;
			}
		}

		return 0;
	}

	if (dBgGm_cX::instance->liquidHeightMaybe >= y) {
		if (*unkDestFloat)
			*unkDestFloat = dBgGm_cX::instance->liquidHeightMaybe;

		return dBgGm_cX::instance->liquidTypeMaybe;
	}

	VEC2 waterSearchPos(x, y);
	VEC2 waterBlockPos;
	float waterTop;
	s16 waterAngle;

	int result = dWaterManager_c::instance->queryPosition(
			&waterSearchPos, &waterBlockPos, &waterTop, &waterAngle, layer);

	if (result != -1) {
		waterSearchPos.y = 1.0f + waterTop;
		goto startLoop;
tryAgain:
		waterSearchPos.y = 1.0f + waterTop;
startLoop:
		int newResult = dWaterManager_c::instance->queryPosition(
				&waterSearchPos, &waterBlockPos, &waterTop, &waterAngle, layer);
		if (result == newResult)
			goto tryAgain;

		if (unkDestFloat)
			*unkDestFloat = waterTop;

		switch (result) {
			case 0:
				return 1;
			case 1:
				return 3;
			case 2:
				return 4;
			case 3:
			case 4:
			case 5:
				s_58F0.x = waterBlockPos.x;
				s_58F0.y = waterBlockPos.y;
				s_58E8 = waterAngle;
				s_58F4 = 2;
				return 2;
		}
	}

	// try tiles
	if (getTileBehaviour1At(x, y, layer) & BF_WATER) {
		if (unkDestFloat) {
			while (y < 0.0f) {
				y += 16.0f;
				if (!(getTileBehaviour1At(x, y, layer) & BF_WATER))
					break;
			}

			*unkDestFloat = -(u16(y - 16.0f) & 0xFFF0);
		}

		return 1;
	}

	return 0;
}


/*static*/ int dCollisionSensor_c::s_80075560(float x, float y, int layer, float *destFloat) {
	float proposedTopY = y - 256.0f;
	int whatType = 0;

	s_58F4 = 0;

	dBgGm_cX *bggm = dBgGm_cX::instance;
	if (bggm->liquidTypeMaybe != 0 && proposedTopY < bggm->liquidHeightMaybe) {
		proposedTopY = bggm->liquidHeightMaybe;
		whatType = bggm->liquidTypeMaybe;
	}

	VEC2 waterSearchPos(x, y);
	while (waterSearchPos.y > proposedTopY) {
		VEC2 waterBlockPos;
		float waterTop;
		s16 waterAngle;

		int result = dWaterManager_c::instance->queryPosition(
				&waterSearchPos, &waterBlockPos, &waterTop, &waterAngle, layer);

		if (result != -1) {
			if ((result >= 3 && result <= 5) || result <= 1) {
				whatType = 2;
				s_58F0.x = waterBlockPos.x;
				s_58F0.y = waterBlockPos.y;
				s_58E8 = waterAngle;
				s_58F4 = 2;
			}

			proposedTopY = waterTop;
			break;
		}

		waterSearchPos.y -= 16.0f;
	}

	while (y > proposedTopY) {
		if (!(getTileBehaviour1At(x, y, layer) & BF_WATER)) {
			whatType = 1;
			proposedTopY = -(u16(y) & 0xFFF0);
			break;
		}

		y -= 16.0f;
	}

	if (whatType != 0) {
		if (destFloat)
			*destFloat = proposedTopY;
		if (whatType != 2)
			s_58F4 = 0;
		return whatType;
	}

	return 0;
}


/*static*/ bool dCollisionSensor_c::s_80075750(VEC2 *pVec) {
	if (s_58F4 == 2) {
		pVec->x = s_58F0.x;
		pVec->y = s_58F0.y;
		return true;
	}
	return false;
}

/*static*/ bool dCollisionSensor_c::s_80075780(s16 *pAngle) {
	if (s_58F4 == 2) {
		*pAngle = s_58E8;
		return true;
	}
	return false;
}


// STUFF
/*static*/ bool dCollisionSensor_c::s_800757B0(VEC3 *vec, float *what, u8 layer, int p6, char p7) {
	return s_80075820(vec, what, 0, layer, p6, p7, 0, 0);
}

/*static*/ bool dCollisionSensor_c::s_80075820(
		VEC3 *pVec, float *pFloat1, float *pFloat2, int layer,
		int p6, char p7, u32 *pU32, int someFlag) {


}

