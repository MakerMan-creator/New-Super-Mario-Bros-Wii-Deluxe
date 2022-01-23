#include "stageActorMgr.h"

// clearSomeVars

bool dStageActorMgr_c::s_80068070(s32 r4, s32 r5, s32 r6, s32 r7, s32 r8, s32 r9) {
	if (r4 == r5) {
		if (r7 < r8 || r6 > r9)
			return false;
		return true;
	}

	return false;
}

bool dStageActorMgr_c::s_800680A0(s32 r4, s32 r5, s32 r6, s32 r7, s32 r8, s32 r9) {
	return s_80068070(r4, r5, r6, r7, r8, r9);
}

// setupAndSpawnSprites
// setupAndSpawnSpritesAlternate
// doStuffForWholeArea
// doStuffForCurrentZone
// checkSpriteAgainstLookupTable
// doGroupIDStuff

bool dStageActor_c::doesSpriteNotHaveGroupID(u32 settings) {
	return ((settings >> 24) == 0);
}

// doStuff()

void dStageActorMgr_c::everyFrame() {
	if (static_8042A260 & 1)
		return;
	if (dontRunThings == 1)
		return;

	dBgGm_cX *bggm = dBgGm_cX::instance;
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;

	int r8 = 0, r9 = 0;

	s32 screenLeft = int(cwci->screenLeft);
	s32 screenRight = screenLeft + int(cwci->screenWidth);
	// are these swapped around?!?
	s32 screenTop = -int(cwci->screenTop);
	s32 screenBottom = screenTop + int(cwci->screenHeight);

	s32 paddedScreenLeftInTiles = (screenLeft - 0x20) >> 4;
	s32 paddedScreenRightInTiles = (screenRight + 0x30) >> 4;
	s32 paddedScreenTopInTiles = (screenTop - 0x10) >> 4;
	s32 paddedScreenBottomInTiles = (screenBottom + 0x20) >> 4;

	s32 r22, r21;

	spawnContext_s context;

	if (bggm->zoomDivisor != bggm->float_90014) {
		context.x1 = paddedScreenLeftInTiles;
		context.y1 = paddedScreenBottomInTiles;
		context.leftXBound = paddedScreenLeftInTiles;
		context.rightXBound = paddedScreenRightInTiles;
		context.topYBound = paddedScreenTopInTiles;
		context.bottomYBound = paddedScreenBottomInTiles;
		context.unk1 = 0;
		context.unk2 = -1;
		createActorsIfNeedBe(&context, 0);

		context.x1 = paddedScreenRightInTiles;
		context.y1 = paddedScreenBottomInTiles;
		context.leftXBound = paddedScreenLeftInTiles;
		context.rightXBound = paddedScreenRightInTiles;
		context.topYBound = paddedScreenTopInTiles;
		context.bottomYBound = paddedScreenBottomInTiles;
		context.unk1 = -1;
		context.unk2 = -1;
		createActorsIfNeedBe(&context, 0);

		lastScreenLeft = screenLeft;
		lastScreenTop = screenTop;

		r22 = paddedScreenRightInTiles;
		r21 = paddedScreenTopInTiles;

		context.x1 = paddedScreenRightInTiles;
		context.y1 = paddedScreenTopInTiles;
		context.unk1 = -1;
		context.unk2 = 0;
		createActorsIfNeedBe(&context, 0);
	} else {
		if (cwci->initedTo2 != 0) {
			if (cwci->initedTo2 == 2) {
				r22 = paddedScreenRightInTiles;
				r8 = -1;
			} else {
				r22 = paddedScreenLeftInTiles;
			}
		} else {
			r22 = _BD8;
			r8 = 1;
		}

		if (cwci->_81 != 0) {
			if (cwci->_81 == 1) {
				r21 = paddedScreenBottomInTiles;
				r9 = -1;
			} else {
				r21 = paddedScreenTopInTiles;
			}
		} else {
			r21 = _BD8;
			r9 = 1;
		}

		if (_BD4 != r22 || _BD8 != r21) {
			context.x1 = r22;
			context.y1 = r21;
			context.leftXBound = paddedScreenLeftInTiles;
			context.rightXBound = paddedScreenRightInTiles;
			context.topYBound = paddedScreenTopInTiles;
			context.bottomYBound = paddedScreenBottomInTiles;
			context.unk1 = r8;
			context.unk2 = r9;
			createActorsIfNeedBe(&context, 1);
		}
	}

	lastScreenLeft = screenLeft;
	lastScreenTop = screenTop;
	_BD4 = r22;
	_BD8 = r21;
}


void dStageActorMgr_c::createActorsIfNeedBe(spawnContext_s *context, bool what) {
	u8 areaID = dScStage_cX::instance->curArea;
	u8 zoneID = dScStage_cX::instance->curZone;
	dCourse_c *course = dCourseFull_c::instance->getArea(areaID);

	int zoneSpriteCount = course->zoneSpriteCount[zoneID];
	if (zoneSpriteCount == 0)
		return;
	dSprite_s *sprite = course->zoneFirstSprite[zoneID];
	if (!sprite)
		return;

	int zoneFirstSpriteIdx = course->zoneFirstSpriteIdx[zoneID];
	int spriteIdx = 0;

	s32 cx1 = context->x1;
	s32 cy1 = context->y1;

	u16 *currentShortPtr = &storedShorts[zoneFirstSpriteIdx];
	u8 *currentBytePtr = &storedBytes[zoneFirstSpriteIdx];

	s32 leftXBound = context->leftXBound;
	s32 rightXBound = context->rightXBound;
	s32 topYBound = context->topYBound;
	s32 bottomYBound = context->bottomYBound;
	s32 cunk1 = context->unk1;
	s32 cunk2 = context->unk2;

	while (spriteIdx < zoneSpriteCount) {
		if (!(*currentBytePtr & 1)) {
			bool willCreate = false;

			dSpriteInfo_s *spriteInfo = &SpriteInfos[sprite->type];

			s32 offsetedX = sprite->x + spriteInfo->rect_x1;
			s32 rx1 = offsetedX - spriteInfo->rect_x2;
			s32 rx2 = offsetedX + spriteInfo->rect_x2;
			s32 acLeft = (rx1 - spriteInfo->field_20) >> 4;
			s32 acRight = (rx2 + spriteInfo->field_22) >> 4;

			s32 offsetedY = sprite->y - spriteInfo->rect_y1;
			s32 ry1 = offsetedY - spriteInfo->rect_y2;
			s32 ry2 = offsetedY + spriteInfo->rect_y2;
			s32 acTop = (ry1 - spriteInfo->field_1C) >> 4;
			s32 acBottom = (ry2 + spriteInfo->field_1E) >> 4;

			s32 chosenXPos = (cunk1 != 0) ? acLeft : acRight;
			s32 chosenYPos = (cunk2 != 0) ? acTop : acBottom;

			bool doesGroup = false;
			if (spriteInfo->flags & 0x10) {
				if (!doesSpriteNotHaveGroupID(sprite->settings))
					doesGroup = true;
			}

			/*
				if (r4 == r5) {
					if (r7 < r8 || r6 > r9)
						return false;
					return true;
				}

				return false;
			   */
			if (!doesGroup) {
				if (what) {
					if (chosenXPos == cx1) {
						if (cunk1 != 1) {
							if (cunk1 < 0)
								willCreate = s_80068070(
										rightXBound, acLeft,
										topYBound, bottomYBound,
										acTop, acBottom);
							else
								willCreate = s_80068070(
										leftXBound, acRight,
										topYBound, bottomYBound,
										acTop, acBottom);
						}
					} else if (chosenYPos == cy1) {
						if (cunk2 != 1) {
							if (cunk2 < 0)
								willCreate = s_800680A0(
										bottomYBound, acTop,
										leftXBound, rightXBound,
										acLeft, acRight);
							else
								willCreate = s_800680A0(
										topYBound, acBottom,
										leftXBound, rightXBound,
										acLeft, acRight);
						}
					}
				} else {
					if (s_80068070(cx1, chosenXPos, topYBound, topYBound, acTop, acBottom))
						willCreate = true;
					if (s_80068070(cy1, chosenYPos, leftXBound, rightXBound, acLeft, acRight))
						willCreate = true;
				}
			}

			if (willCreate) {
				createOneActor(sprite, spriteInfo, currentBytePtr, currentShortPtr, zoneID);
			}
		}

		sprite++;
		currentBytePtr++;
		currentShortPtr++;
		spriteIdx++;
	}
}

