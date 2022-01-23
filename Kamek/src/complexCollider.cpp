#include "complexCollider.h"



dComplexCollider_c::dComplexCollider_c() {
	listPrev = 0;
	listNext = 0;
	isLinkedIntoList = false;
	_8 = 0;
	chainlinkMode = 1;
	layer = 0;
	clear();
}

dComplexCollider_c::~dComplexCollider_c() {
	removeFromList();
}

void dComplexCollider_c::clear() {
	result1 = 0;
	result2 = 0;
	result3 = 0;
	someFlagByte &= ~3;

	for (int i = 0; i < 8; i++) {
		lastCollisionDistanceX[i] = 0.0f;
		lastCollisionDistanceY[i] = 0.0f;
	}
}

void dComplexCollider_c::addToList() {
	if (isLinkedIntoList)
		return;

	listPrev = globalListHead;
	globalListHead = this;
	if (!listPrev) {
		listNext = globalListTail;
		globalListTail = this;
	} else {
		listNext = listPrev->listNext;
		listPrev->listNext = this;
	}

	isLinkedIntoList = true;
}

void dComplexCollider_c::removeFromList() {
	if (!isLinkedIntoList)
		return;

	if (listPrev) {
		listPrev->listNext = listNext;
	} else {
		globalListTail = listNext;
	}

	if (listNext) {
		listNext->listPrev = listPrev;
	} else {
		globalListHead = listPrev;
	}

	listPrev = 0;
	listNext = 0;
	isLinkedIntoList = false;
}

void dComplexCollider_c::initWithStruct(dStageActor_c *owner, const Info *info) {
	this->owner = owner;
	this->info = *info;
	someFlagByte = 0;
}

void dComplexCollider_c::initWithStruct(dStageActor_c *owner, const Info *info, u8 clMode) {
	initWithStruct(owner, info);
	chainlinkMode = clMode;
}

void dComplexCollider_c::setupManager() {
	// TODO
}

u16 dComplexCollider_c::checkResult1(u16 param) {
	return result1 & param;
}

u16 dComplexCollider_c::checkResult3(u16 param) {
	return result3 & param;
}

float dComplexCollider_c::top() {
	float cY = info.yDistToCenter + owner->pos.y;
	return info.yDistToEdge + cY;
}

float dComplexCollider_c::bottom() {
	float cY = info.yDistToCenter + owner->pos.y;
	return cY - info.yDistToEdge;
}

float dComplexCollider_c::centerY() {
	return info.yDistToCenter + owner->pos.y;
}

float dComplexCollider_c::right() {
	float cX = info.xDistToCenter + owner->pos.x;
	return info.xDistToEdge + cX;
}

float dComplexCollider_c::left() {
	float cX = info.xDistToCenter + owner->pos.x;
	return cX - info.xDistToEdge;
}

float dComplexCollider_c::centerX() {
	return info.xDistToCenter + owner->pos.x;
}


bool dComplexCollider_c::testForRectangularCollision(dComplexCollider_c *other) {
	if (collisionCheckType == COLL_ROUND) {
		Vec2 centreThis = {centerX(), centerY()};
		Vec2 centreOther = {other->centerX(), other->centerY()};

		// You morons
		// I won't bother with this sqrtf because it is POINTLESS
		// it's at 8008C620

		return true;
	} else {
		float xDiff = abs(centerX() - other->centerX());
		float xEdgeDistDiff = abs(info.xDistToEdge - other->info.xDistToEdge);

		if (xDiff > xEdgeDistDiff)
			return false;

		float yDiff = abs(centerY() - other->centerY());
		float yEdgeDistDiff = abs(info.yDistToEdge - other->info.yDistToEdge);

		return !(yDiff > yEdgeDistDiff);
	}
}


bool ColCheck_Normal_v_Normal(dComplexCollider_c *one, dComplexCollider_c *two);
bool ColCheck_Round_v_Round(dComplexCollider_c *one, dComplexCollider_c *two);
bool ColCheck_Round_v_Normal(dComplexCollider_c *one, dComplexCollider_c *two);
bool ColCheck_VertTrap_Trap(dComplexCollider_c *one, dComplexCollider_c *two);
bool ColCheck_NormalOrRound_v_VertTrap(dComplexCollider_c *trap, dComplexCollider_c *other);
bool ColCheck_HorzTrap_Trap(dComplexCollider_c *one, dComplexCollider_c *two);
bool ColCheck_NormalOrRound_v_HorzTrap(dComplexCollider_c *trap, dComplexCollider_c *other);
typedef bool (*ColCheckFunc)(dComplexCollider_c *, dComplexCollider_c *);
static ColCheckFunc CollisionCheckTable[4][4] = {
	{ColCheck_Normal_v_Normal,
		ColCheck_Round_v_Normal,
		ColCheck_NormalOrRound_v_VertTrap,
		ColCheck_NormalOrRound_v_HorzTrap},
	{ColCheck_Round_v_Normal,
		ColCheck_Round_v_Round,
		ColCheck_NormalOrRound_v_VertTrap,
		ColCheck_NormalOrRound_v_HorzTrap},
	{ColCheck_VertTrap_Trap,
		ColCheck_VertTrap_Trap,
		ColCheck_VertTrap_Trap,
		ColCheck_VertTrap_Trap},
	{ColCheck_HorzTrap_Trap,
		ColCheck_HorzTrap_Trap,
		ColCheck_HorzTrap_Trap,
		ColCheck_HorzTrap_Trap}
};

bool dComplexCollider_c::processMe(dComplexCollider_c *other, bool something) {
	if (other == this)
		return false;

	if (other->owner == owner)
		return false;

	if (other->owner == _8 || owner == other->_8)
		return false;

	if ((someFlagByte & 2) || (other->someFlagByte & 2))
		return false;

	if (!(chainlinkMode & other->chainlinkMode))
		return false;

	if (layer != other->layer)
		return false;

	Info thisInfo = info;
	Info otherInfo = other->info;

	u32 otherCatMask = 1 << otherInfo.category1;
	u32 thisBitfield = thisInfo.bitfield1;
	u32 thisBitfield2 = thisInfo.bitfield2;
	u32 thisCatMask = 1 << thisInfo.category1;
	u32 otherBitfield = otherInfo.bitfield1;
	u32 otherBitfield2 = otherInfo.bitfield2;

	if ((thisBitfield & otherCatMask) && (otherBitfield & thisCatMask)) {
		u32 thisCatMask2 = 1 << thisInfo.category2;
		u32 otherCatMask2 = 1 << otherInfo.category2;

		if ((!((thisBitfield & otherCatMask) & 2)) || (thisBitfield2 & otherCatMask2)) {
			if ((!((otherBitfield & thisCatMask) & 2)) || (otherBitfield2 & thisCatMask2)) {
				return false;
			}
		}


		if (CollisionCheckTable[other->collisionCheckType][collisionCheckType](this, other)) {
			if (!something)
				return true;

			if (!(otherInfo.unkShort1C & 4)) {
				u32 newResult1 = otherCatMask;
				u32 newResult3 = thisBitfield2 & otherCatMask2;

				result1 |= (u16)newResult1;

				if (newResult3)
					result3 |= (u16)newResult3;

				u32 newResult2 = thisCatMask2 & otherBitfield2;
				if (newResult2)
					result2 |= (u16)newResult2;
			}

			if (!(thisInfo.unkShort1C & 4)) {
				u32 newResult1 = thisCatMask;
				u32 newResult2 = thisBitfield2 & otherCatMask2;

				other->result1 |= (u16)newResult1;
				
				if (newResult2)
					other->result2 |= (u16)newResult2;

				u32 newResult3 = thisCatMask2 & otherBitfield2;
				if (newResult3)
					other->result3 |= (u16)newResult3;
			}

			if (!(otherInfo.unkShort1C & 4) && (thisInfo.callback != 0))
				thisInfo.callback(this, other);

			if (!(thisInfo.unkShort1C & 4) && (otherInfo.callback != 0))
				otherInfo.callback(other, this);
		}
	}

	return (result1 != 0);
}


void dComplexCollider_c::processAll() {
	dComplexCollider_c *outer = globalListHead;

	while (outer) {
		dComplexCollider_c *inner = outer->listPrev;
		while (inner) {
			outer->processMe(inner, true);
			inner = inner->listPrev;
		}

		outer = outer->listPrev;
	}
}


bool DoSomeKindOfCollisionChecking(dComplexCollider_c *one, dComplexCollider_c *two, Vec2 centreOne, Vec2 centreTwo) {
	float xDiff = (centreOne.x - centreTwo.x);
	float addedXDist = (one->info.xDistToEdge + two->info.xDistToEdge);

	float yDiff = (centreOne.y - centreTwo.y);
	float addedYDist = (one->info.yDistToEdge + two->info.yDistToEdge);

	if (abs(xDiff) < addedXDist) {
		if (abs(yDiff) < addedYDist) {
			one->positionOfLastCollision = centreOne;
			two->positionOfLastCollision = centreTwo;

			if (((one->info.unkShort1C & 1) || (two->info.unkShort1C & 1)) && one->info.category1 != 8 && two->info.category1 != 8)
				return true;

			float xDist = ((addedXDist) - abs(xDiff)) / 2;
			if (xDiff < 0.0f)
				xDist = -xDist;

			float yDist = ((addedYDist) - abs(yDiff)) / 2;
			if (yDiff < 0.0f)
				yDist = -yDist;

			one->lastCollisionDistanceX[two->info.category1] = xDist;
			one->lastCollisionDistanceY[two->info.category1] = yDist;

			two->lastCollisionDistanceX[one->info.category1] = -xDist;
			two->lastCollisionDistanceY[one->info.category1] = -yDist;

			return true;
		}
	}
	return false;
}

bool ColCheck_Normal_v_Normal(dComplexCollider_c *one, dComplexCollider_c *two) {
	Vec2 centreOne = {one->centerX(), one->centerY()};
	Vec2 centreTwo = {two->centerX(), two->centerY()};

	return DoSomeKindOfCollisionChecking(one, two, centreOne, centreTwo);
}

bool ColCheck_Normal_v_Normal_WithWrap(dComplexCollider_c *one, dComplexCollider_c *two) {
	Vec2 centreOne = {ConvertXPosForWrapping(one->centerX()), one->centerY()};
	Vec2 centreTwo = {ConvertXPosForWrapping(two->centerX()), two->centerY()};

	if (DoSomeKindOfCollisionChecking(one, two, centreOne, centreTwo)) {
		return true;
	} else {
		if (centreOne.x < centreTwo.x)
			centreOne.x += dBgGm_c::instance->_8FEA0;
		else
			centreTwo.x += dBgGm_c::instance->_8FEA0;

		return DoSomeKindOfCollisionChecking(one, two, centreOne, centreTwo);
	}
}

bool ColCheck_Round_v_Round(dComplexCollider_c *one, dComplexCollider_c *two) {
	Vec2 centerOne = {one->centerX(), one->centerY()};
	Vec2 centerTwo = {two->centerX(), two->centerY()};
	Vec2 diff = {centerTwo.x - centerOne.x, centerTwo.y - centerOne.y};

	float addedXDist = one->info.xDistToEdge + two->info.xDistToEdge;
	float addedYDist = one->info.yDistToEdge + two->info.yDistToEdge;

	float squareLength = (diff.x * diff.x) + (diff.y * diff.y);
	float halfAllDists = 0.5f *
		((one->info.xDistToEdge + two->info.xDistToEdge) + (one->info.yDistToEdge + two->info.yDistToEdge));

	if (sqrtf(squareLength) <= halfAllDists) {
		halfAllDists -= sqrtf((diff.x * diff.x) + (diff.y * diff.y));

		s16 direction = cM_atan2s(abs(diff.x), diff.y);
		// TODO: check: is just calling "...FIdx(direction)" correct?
		float multCos = halfAllDists * nw4r::math::CosFIdx(direction);
		float multSin = -halfAllDists * nw4r::math::SinFIdx(direction);

		one->lastCollisionDistanceX[two->info.category1] = multCos;
		one->lastCollisionDistanceY[two->info.category1] = multSin;

		one->positionOfLastCollision.x = centerOne.x;
		one->positionOfLastCollision.y = centerOne.y;

		two->lastCollisionDistanceX[one->info.category1] = -multCos;
		two->lastCollisionDistanceY[one->info.category1] = -multSin;

		two->positionOfLastCollision.x = centerTwo.x;
		two->positionOfLastCollision.y = centerTwo.y;

		return true;
	} else
		return false;
}

bool ColCheck_Round_v_Normal(dComplexCollider_c *one, dComplexCollider_c *two) {
	dComplexCollider_c *rect, *round;

	if (one->collisionCheckType == dComplexCollider_c::COLL_NORMAL) {
		rect = one;
		round = two;
	} else {
		rect = two;
		round = one;
	}

	Vec2 centerRound = {round->centerX(), round->centerY()};
	Vec2 centerRect = {rect->centerX(), rect->centerY()};

	int stateOne, stateTwo;

	if (rect->centerX() < round->centerX())
		stateOne = 0;
	else
		stateOne = 1;

	if (rect->centerY() < round->centerY())
		stateTwo = 0;
	else
		stateTwo = 1;

	float rectXPositions[2];
	rectXPositions[0] = rect->centerX() + rect->info.xDistToEdge;
	rectXPositions[1] = rect->centerX() - rect->info.xDistToEdge;

	float rectYPositions[2];
	rectYPositions[0] = rect->centerY() + rect->info.yDistToEdge;
	rectYPositions[1] = rect->centerY() - rect->info.yDistToEdge;

	if (rectYPositions[1] < centerRound.y) {
		if (centerRound.y < rectYPositions[0]) {
			float thing = (1.0f - (stateOne * 2.0f)) * (centerRound.x - rectXPositions[stateOne]);

			if (thing < round->info.xDistToEdge) {
				float wut = 0.5f * ((1.0f - (stateOne * 2.0f)) * (round->info.xDistToEdge - thing));

				round->lastCollisionDistanceX[round->info.category1] = wut;
				round->lastCollisionDistanceY[round->info.category1] = 0.0f;

				round->positionOfLastCollision.x = centerRound.x;
				round->positionOfLastCollision.y = centerRound.y;

				rect->lastCollisionDistanceX[round->info.category1] =
					-round->lastCollisionDistanceX[round->info.category1];

				rect->lastCollisionDistanceY[round->info.category1] = 0.0f;

				rect->positionOfLastCollision.x = centerRect.x;
				rect->positionOfLastCollision.y = centerRect.y;

				return true;
			}
		}
	}

	if (rectXPositions[1] < centerRound.x) {
		if (centerRound.x < rectXPositions[0]) {
			float thing = (1.0f - (stateTwo * 2.0f)) * (centerRound.y - rectYPositions[stateTwo]);

			if (thing < round->info.xDistToEdge) {
				float wut = 0.5f * ((1.0f - (stateTwo * 2.0f)) * (round->info.xDistToEdge - thing));

				round->lastCollisionDistanceX[round->info.category1] = 0.0f;
				round->lastCollisionDistanceY[round->info.category1] = wut;

				round->positionOfLastCollision.x = centerRound.x;
				round->positionOfLastCollision.y = centerRound.y;

				rect->lastCollisionDistanceX[round->info.category1] = 0.0f;

				rect->lastCollisionDistanceY[round->info.category1] =
					-round->lastCollisionDistanceY[round->info.category1];

				rect->positionOfLastCollision.x = centerRect.x;
				rect->positionOfLastCollision.y = centerRect.y;

				return true;
			}
		}
	}

	Vec2 what = {rectXPositions[stateOne], rectYPositions[stateTwo]};
	Vec2 norm = {centerRound.x - what.x, centerRound.y - what.y};
	float square = NormalizeVec2(&norm);

	if (square < round->info.xDistToEdge) {
		float multThing = (round->info.xDistToEdge - square) * 0.5f;

		round->lastCollisionDistanceX[round->info.category1] = norm.x * multThing;
		round->lastCollisionDistanceY[round->info.category1] = norm.y * multThing;

		round->positionOfLastCollision.x = centerRound.x;
		round->positionOfLastCollision.y = centerRound.y;

		rect->lastCollisionDistanceX[round->info.category1] =
			-round->lastCollisionDistanceX[round->info.category1];

		rect->lastCollisionDistanceY[round->info.category1] =
			-round->lastCollisionDistanceY[round->info.category1];

		rect->positionOfLastCollision.x = centerRect.x;
		rect->positionOfLastCollision.y = centerRect.y;

		return true;

	} else
		return false;
}

int UsedByVertTrapezoid(Vec2 one, Vec2 two, float three, float four) {
	if (one.x > three || two.x < three)
		return -1;

	Vec2 diff = {two.x - one.x, two.y - one.y};

	if (((((three - one.x) * diff.y) / diff.x) + one.y) < four)
		return 2;
	else
		return 3;
}

bool ColCheck_NormalOrRound_v_VertTrap(dComplexCollider_c *trap, dComplexCollider_c *other);

bool ColCheck_VertTrap_Trap(dComplexCollider_c *one, dComplexCollider_c *two) {
	return ColCheck_NormalOrRound_v_VertTrap(two, one);
}

bool ColCheck_NormalOrRound_v_VertTrap(dComplexCollider_c *trap, dComplexCollider_c *other) {
	Vec2 centerTrap = {trap->centerX(), trap->centerY()};
	Vec2 centerOther = {other->centerX(), other->centerY()};

	if (abs(centerTrap.x - centerOther.x) < (trap->info.xDistToEdge + other->info.xDistToEdge))
		return false;

	float uleft = other->left();
	float uright = other->right();
	float utop = other->top();
	float ubottom = other->bottom();

	Vec2 VecAt80 = {trap->left(), centerTrap.y + trap->special[0]};
	Vec2 VecAt88 = {trap->left(), centerTrap.y + trap->special[1]};
	Vec2 VecAt90 = {trap->right(), centerTrap.y + trap->special[2]};
	Vec2 VecAt98 = {trap->right(), centerTrap.y + trap->special[3]};

	if (VecAt80.y < ubottom && VecAt90.y < ubottom)
		return false;

	if (VecAt88.y > utop && VecAt98.y > utop)
		return false;

	if (uleft > VecAt80.x || VecAt90.x > uright) {
		bool whatever = (UsedByVertTrapezoid(VecAt80, VecAt90, uleft, ubottom) != 3);
		if (whatever)
			whatever = (UsedByVertTrapezoid(VecAt80, VecAt90, uright, ubottom) != 3);

		if (whatever)
			return false;

		bool whatever2 = (UsedByVertTrapezoid(VecAt88, VecAt98, uleft, utop) != 2);
		if (whatever)
			whatever = (UsedByVertTrapezoid(VecAt88, VecAt98, uright, utop) != 2);

		if (whatever2)
			return false;
	}

	trap->lastCollisionDistanceX[other->info.category1] = 0.0f;
	trap->lastCollisionDistanceY[other->info.category1] = 0.0f;
	trap->positionOfLastCollision.x = trap->centerX();
	trap->positionOfLastCollision.y = trap->centerY();

	other->lastCollisionDistanceX[trap->info.category1] = 0.0f;
	other->lastCollisionDistanceY[trap->info.category1] = 0.0f;
	other->positionOfLastCollision.x = other->centerX();
	other->positionOfLastCollision.y = other->centerY();

	return true;
}

int UsedByHorzTrapezoid(Vec2 one, Vec2 two, float three, float four) {
	if (one.y < four || two.y > four)
		return -1;

	Vec2 diff = {one.x - two.x, one.y - two.y};

	if (((((four - two.y) * diff.x) / diff.y) + two.x) < three)
		return 0;
	else
		return 1;
}


bool ColCheck_NormalOrRound_v_HorzTrap(dComplexCollider_c *trap, dComplexCollider_c *other);

bool ColCheck_HorzTrap_Trap(dComplexCollider_c *one, dComplexCollider_c *two) {
	return ColCheck_NormalOrRound_v_HorzTrap(two, one);
}

bool ColCheck_NormalOrRound_v_HorzTrap(dComplexCollider_c *trap, dComplexCollider_c *other) {
	Vec2 centerTrap = {trap->centerX(), trap->centerY()};
	Vec2 centerOther = {other->centerX(), other->centerY()};

	if (abs(centerTrap.x - centerOther.x) < (trap->info.yDistToEdge + other->info.yDistToEdge))
		return false;

	float uleft = other->left();
	float uright = other->right();
	float utop = other->top();
	float ubottom = other->bottom();

	Vec2 VecAt60 = {centerTrap.x + trap->special[0], trap->top()};
	Vec2 VecAt68 = {centerTrap.x + trap->special[1], trap->top()};
	Vec2 VecAt70 = {centerTrap.x + trap->special[2], trap->bottom()};
	Vec2 VecAt78 = {centerTrap.x + trap->special[3], trap->bottom()};

	if (VecAt60.x > uright && VecAt70.x > uright)
		return false;

	if (VecAt68.x < uleft && VecAt78.x < uleft)
		return false;

	if (ubottom > VecAt60.y || VecAt70.x > ubottom) {
		bool whatever = (UsedByHorzTrapezoid(VecAt60, VecAt70, uright, utop) != 0);
		if (whatever)
			whatever = (UsedByHorzTrapezoid(VecAt60, VecAt70, uright, ubottom) != 0);

		if (whatever)
			return false;

		bool whatever2 = (UsedByHorzTrapezoid(VecAt68, VecAt78, uleft, utop) != 1);
		if (whatever)
			whatever = (UsedByHorzTrapezoid(VecAt68, VecAt78, uleft, ubottom) != 1);

		if (whatever2)
			return false;
	}

	trap->lastCollisionDistanceX[other->info.category1] = 0.0f;
	trap->lastCollisionDistanceY[other->info.category1] = 0.0f;
	trap->positionOfLastCollision.x = trap->centerX();
	trap->positionOfLastCollision.y = trap->centerY();

	other->lastCollisionDistanceX[trap->info.category1] = 0.0f;
	other->lastCollisionDistanceY[trap->info.category1] = 0.0f;
	other->positionOfLastCollision.x = other->centerX();
	other->positionOfLastCollision.y = other->centerY();

	return true;
}
