// Nybbles to bitfield...
// 5  0 6  4  7  8 8 12  9 16 A 20  B 24 C 28
// 0000 0000  0000 0000  0000 0000  0000 0000

struct limitLineInfo_s {
	// USED FOR Y LIMITS
	float leftX, rightX;
	float yLimitVarA, yLimitVarB, yLimitVarC;
	float spriteYPos;

	// USED FOR X LIMITS
	float _18, _1C; // these are never used??
	float xLimitVarA, xLimitVarB, xLimitVarC;
	float spriteXPos;
	
	// TYPE-AGNOSTIC STUFFS
	float zoneTop, zoneBottom, zoneLeft, zoneRight;
	short flags;
};


const int HAVE_LEFT_UP = 2;
const int HAVE_RIGHT_DOWN = 1;

const int FLAG_8 = 8; // is UE2
const int FLAG_10 = 0x10; // MAX_UE nyb5 non-zero
const int FLAG_20 = 0x20; // MAX_UE nyb5 zero
const int FLAG_40 = 0x40; // LR_STOP nyb5 non-zero
const int FLAG_80 = 0x80; // LR_STOP nyb5 zero
const int FLAG_100 = 0x100; // nyb5 non-zero and another condition, for both
const int FLAG_200 = 0x200; // nyb5 zero and another condition, for both
const int FLAG_1000 = 0x1000; // depending on nyb6 for both
const int FLAG_2000 = 0x2000; // depending on nyb6 for both
const int FLAG_4000 = 0x4000; // depending on nyb6 for both





int BgGmBase::makeAndReturnLimitLineEntrySlot(limitLineInfo_s *info, int groupID, int zoneID) {
	LimitLineGroup *group = &limitLineData[zoneID].groups[groupID];

	int i;
	for (i = 0; i < 16; i++) {
		if (group->lines[i].spriteYPos >= info->spriteYPos)
			break;
	}

	if (i >= 16)
		i = 0;


	int copyToID = 15;
	int copyAmount = 15 - i;
	if (i != 15) {
		for (int j = 0; j < copyAmount; j++) {
			limitLineInfo_s *copyTo = &group->lines[copyToID];
			limitLineInfo_s *copyFrom = &group->lines[--copyToID];

			copyTo->leftX = copyFrom->leftX;
			// LOTSA JUNK HERE COPYING OVER ALL THE FIELDS
			// so I won't bother with it
			copyTo->flags = copyFrom->flags;
		}
	}

	group->lines[i].leftX = 0.0f;
	group->lines[i].rightX = 0.0f;
	group->lines[i].yLimitVarA = 0.0f;
	group->lines[i].yLimitVarB = 0.0f;
	group->lines[i].yLimitVarC = 0.0f;
	group->lines[i].spriteYPos = 0.0f;
	group->lines[i]._18 = 0.0f;
	group->lines[i]._1C = 0.0f;
	group->lines[i].xLimitVarA = 0.0f;
	group->lines[i].xLimitVarB = 0.0f;
	group->lines[i].xLimitVarC = 0.0f;
	group->lines[i].spriteXPos = 0.0f;
	group->lines[i].flags = 0;

	return i;
}


void BgGmBase::addLimitLine(limitLineInfo_s *info, int zoneID, int type, int groupID) {
	switch (type) {
		case 0: case 3:
			{
				int i = makeAndReturnLimitLineEntrySlot(info, groupID, zoneID);
				limitLineInfo_s *dest = &limitLineData[zoneID].groups[groupID].lines[i];

				dest->leftX = info->leftX;
				dest->rightX = info->rightX;
				dest->yLimitVarA = info->yLimitVarA;
				dest->yLimitVarB = info->yLimitVarB;
				dest->yLimitVarC = info->yLimitVarC;
				dest->spriteYPos = info->spriteYPos;
				dest->_18 = info->_18;
				dest->_1C = info->_1C;
				dest->xLimitVarA = info->xLimitVarA;
				dest->xLimitVarB = info->xLimitVarB;
				dest->xLimitVarC = info->xLimitVarC;
				dest->spriteXPos = info->spriteXPos;
				dest->zoneTop = info->zoneTop;
				dest->zoneBottom = info->zoneBottom;
				dest->zoneLeft = info->zoneLeft;
				dest->zoneRight = info->zoneRight;
				dest->flags = info->flags | HAVE_LEFT_UP | HAVE_RIGHT_DOWN;
			}
			break;

		case 1: // MAX_UE left
			{
				LimitLineGroup *group = &limitLineData[zoneID].groups[groupID];
				int i;
				for (i = 0; i < 16; i++) {
					if (!(group->lines[i].flags & HAVE_LEFT_UP) && group->lines[i].spriteYPos == info->spriteYPos)
						break;
				}

				if (i >= 0x10)
					i = makeAndReturnLimitLineEntrySlot(info, groupID, zoneID);

				limitLineInfo_s *dest = &group->lines[i];

				dest->leftX = info->leftX;
				dest->yLimitVarA = info->yLimitVarA;
				dest->yLimitVarB = info->yLimitVarB;
				dest->yLimitVarC = info->yLimitVarC;
				dest->spriteYPos = info->spriteYPos;
				dest->zoneTop = info->zoneTop;
				dest->zoneBottom = info->zoneBottom;
				dest->zoneLeft = info->zoneLeft;
				dest->zoneRight = info->zoneRight;
				dest->flags = info->flags | HAVE_LEFT_UP;
			}
			break;

		case 2: // MAX_UE right
			{
				LimitLineGroup *group = &limitLineData[zoneID].groups[groupID];
				int i;
				for (i = 0; i < 16; i++) {
					if (!(group->lines[i].flags & HAVE_RIGHT_DOWN) && group->lines[i].spriteYPos == info->spriteYPos)
						break;
				}

				if (i >= 0x10)
					i = makeAndReturnLimitLineEntrySlot(info, groupID, zoneID);

				limitLineInfo_s *dest = &group->lines[i];

				dest->rightX = float(s16(info->rightX + 16.0f));
				dest->yLimitVarA = info->yLimitVarA;
				dest->yLimitVarB = info->yLimitVarB;
				dest->yLimitVarC = info->yLimitVarC;
				dest->spriteYPos = info->spriteYPos;
				dest->zoneTop = info->zoneTop;
				dest->zoneBottom = info->zoneBottom;
				dest->zoneLeft = info->zoneLeft;
				dest->zoneRight = info->zoneRight;
				dest->flags = info->flags | HAVE_RIGHT_DOWN;
			}

		case 4: // LR_STOP_UP
			{
				LimitLineGroup *group = &limitLineData[zoneID].groups[groupID];
				int i;
				for (i = 0; i < 16; i++) {
					if (!(group->lines[i].flags & HAVE_LEFT_UP) && group->lines[i].spriteXPos == info->spriteXPos)
						break;
				}

				if (i >= 0x10)
					i = s_8007D770(info, groupID, zoneID);

				limitLineInfo_s *dest = &group->lines[i];

				dest->_18 = info->_18;
				dest->xLimitVarA = info->xLimitVarA;
				dest->xLimitVarB = info->xLimitVarB;
				dest->xLimitVarC = info->xLimitVarC;
				dest->spriteXPos = info->spriteXPos;
				dest->zoneTop = info->zoneTop;
				dest->zoneBottom = info->zoneBottom;
				dest->zoneLeft = info->zoneLeft;
				dest->zoneRight = info->zoneRight;
				dest->flags = info->flags | HAVE_LEFT_UP;
			}

		case 5: // LR_STOP_DW
			{
				LimitLineGroup *group = &limitLineData[zoneID].groups[groupID];
				int i;
				for (i = 0; i < 16; i++) {
					if (!(group->lines[i].flags & HAVE_RIGHT_DOWN) && group->lines[i].spriteXPos == info->spriteXPos)
						break;
				}

				if (i >= 0x10)
					i = s_8007D770(info, groupID, zoneID);

				limitLineInfo_s *dest = &group->lines[i];

				dest->_1C = float(s16(info->_1C + 16.0f));
				dest->xLimitVarA = info->xLimitVarA;
				dest->xLimitVarB = info->xLimitVarB;
				dest->xLimitVarC = info->xLimitVarC;
				dest->spriteXPos = info->spriteXPos;
				dest->zoneTop = info->zoneTop;
				dest->zoneBottom = info->zoneBottom;
				dest->zoneLeft = info->zoneLeft;
				dest->zoneRight = info->zoneRight;
				dest->flags = info->flags | HAVE_RIGHT_DOWN;
			}
			break;
	}
}



int daEnMaxUe_c::onCreate() {
	int nyb11 = (settings >> 4) & 0xF;
	int nyb12 = (settings & 0xF);

	float thing = (16.0f * nyb11) + nyb12;

	dCourse_c *course =
		dCourseFull_c::instance->getCourse(dScStage_c::instance->currentArea);
	dCourse_c::zone_s *zone =
		course->getZoneByID(currentZone);

	limitLineInfo_s lineInfo;
	lineInfo.zoneTop = float(zone->y);
	lineInfo.zoneBottom = float(zone->y + zone->height);
	lineInfo.zoneLeft = float(zone->x);
	lineInfo.zoneRight = float(zone->x + zone->width);

	lineInfo.spriteYPos = pos.y * -1.0f;

	// query Nyb5 non-zero
	if (settings & 0xF0000000) {
		lineInfo.yLimitVarC = lineInfo.spriteYPos - 16.0f;
		lineInfo.yLimitVarA = lineInfo.yLimitVarC - thing;
		lineInfo.flags = FLAG_10;
		lineInfo.spriteYPos = float(zone->y + zone->height);
		lineInfo.yLimitVarB = float(zone->y + zone->height);
		if (!(settings & 0x1000))
			lineInfo.flags |= FLAG_100;
	} else {
		lineInfo.yLimitVarB = lineInfo.spriteYPos + thing;
		lineInfo.flags = FLAG_20;
		lineInfo.yLimitVarC = float(zone->y);
		lineInfo.yLimitVarA = float(zone->y);
		if (!(settings & 0x1000))
			lineInfo.flags |= FLAG_200;
	}

	int nyb6 = (settings >> 24) & 0xF;
	switch (nyb6) {
		case 0:
			lineInfo.flags |= FLAG_1000;
			break;
		case 1:
			lineInfo.flags |= FLAG_2000;
			break;
		case 2:
			lineInfo.flags |= FLAG_4000;
			break;
	}

	if (name == EN_MAX_UE_LEFT || name == EN_MAX_UE2_LEFT) {
		if (name == EN_MAX_UE2_LEFT)
			lineInfo.flags |= FLAG_8;

		entry.rightX = 0.0f;
		entry.leftX = pos.x - 5.0f;

		int nyb10 = (settings >> 8) & 0xF;
		dBgGm_c::instance->addLimitLine(&lineInfo, currentZone, 1, nyb10);
	}

	if (name == EN_MAX_UE_RIGHT || name == EN_MAX_UE2_RIGHT) {
		if (name == EN_MAX_UE2_RIGHT)
			lineInfo.flags |= FLAG_8;

		entry.leftX = 0.0f;
		entry.rightX = pos.x + 5.0f;

		int nyb10 = (settings >> 8) & 0xF;
		dBgGm_c::instance->addLimitLine(&lineInfo, currentZone, 2, nyb10);
	}

	if (current_sprite_storage_ptr)
		*current_sprite_storage_ptr |= 1;

	return 1;
}

int daEnLrStopDw_c::onCreate() {
	int nyb11 = (settings >> 4) & 0xF;
	int nyb12 = (settings & 0xF);

	float thing = (16.0f * nyb11) + nyb12;

	dCourse_c *course =
		dCourseFull_c::instance->getCourse(dScStage_c::instance->currentArea);
	dCourse_c::zone_s *zone =
		course->getZoneByID(currentZone);

	limitLineInfo_s lineInfo;
	lineInfo.zoneTop = float(zone->y);
	lineInfo.zoneBottom = float(zone->y + zone->height);
	lineInfo.zoneLeft = float(zone->x);
	lineInfo.zoneRight = float(zone->x + zone->width);

	lineInfo.spriteXPos = pos.x;

	// query Nyb5 non-zero
	if (settings & 0xF0000000) {
		lineInfo.xLimitVarC = pos.x;
		lineInfo.xLimitVarA = pos.x - thing;
		lineInfo.flags = FLAG_40;
		lineInfo.spriteXPos = float(zone->x + zone->width);
		lineInfo.xLimitVarB = float(zone->x + zone->width);
		if (!(settings & 0x1000))
			lineInfo.flags |= FLAG_100;
	} else {
		lineInfo.flags = FLAG_80;
		lineInfo.xLimitVarC = float(zone->x);
		lineInfo.xLimitVarA = float(zone->x);
		lineInfo.xLimitVarB = pos.x + 16.0f + thing;
		if (!(settings & 0x1000))
			lineInfo.flags |= FLAG_200;
	}

	int nyb6 = (settings >> 24) & 0xF;
	switch (nyb6) {
		case 0:
			lineInfo.flags |= FLAG_1000;
			break;
		case 1:
			lineInfo.flags |= FLAG_2000;
			break;
		case 2:
			lineInfo.flags |= FLAG_4000;
			break;
	}

	if (name == EN_LR_STOP_UP) {
		int nyb10 = (settings >> 8) & 0xF;
		dBgGm_c::instance->addLimitLine(&lineInfo, currentZone, 4, nyb10);
	}

	if (name == EN_LR_STOP_DW) {
		int nyb10 = (settings >> 8) & 0xF;
		dBgGm_c::instance->addLimitLine(&lineInfo, currentZone, 5, nyb10);
	}

	if (current_sprite_storage_ptr)
		*current_sprite_storage_ptr |= 1;

	return 1;
}
