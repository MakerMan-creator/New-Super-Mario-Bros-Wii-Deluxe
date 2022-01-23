void executeAutoscroll() {
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;

	float screenWidth = cwci->screenWidth;
	float screenHeight = cwci->screenHeight;

	dRail_s *rail = dRail::getRailByID(autoscrollInfoMaybe.autoscrollPathID);
	dCourse_c *course = dCourseFull_c::instance->get(dScStage_c::instance->currentAreaID);

	float minZoneX = getManipulatedZoneLeftCopy1();
	float maxZoneX = getManipulatedZoneRightCopy1() - screenWidth;
	float minZoneY = zoneBottom_copy1 + screenHeight;
	float maxZoneY = zoneTop_copy1;

	bool firstWutVar = true;
	bool secondWutVar = true;

	if (in_2_castle) {
		handleCastleMaze();
		return;
	}

	if (StageActorDisableFlags & 8)
		return;

	if (usedByAutoscroll_900FC != 0)
		return;

	dRailNode_s *node = course->railNodes[rail->startNode];
	if (usedByAutoscroll_900FD != 0) {
		for (int i = 0; i < 999; i++) {
			if (valueFromMbBias_1 == 0) {
				float thing = node[currentAutoscrollNode].x;
				if (autoscrollInfoMaybe.vector.x > thing) {
					currentAutoscrollNode++;
				} else {
					break;
				}
			}
		}
	}

	usedByAutoscroll_900FD = 0;

	VEC3 someVec = {
		autoscrollInfoMaybe.vector.x,
		autoscrollInfoMaybe.vector.y,
		0.0f
	};

	if (usedByAutoscroll_900F0 < 999) {
		VEC3 current, previous, diffToPrev, diffToSome;

		current.x = float(node[currentAutoscrollNode].x);
		current.y = -(float(node[currentAutoscrollNode].y));
		current.z = 0.0f;

		if (currentAutoscrollNode > 0) {
			previous.x = float(node[currentAutoscrollNode - 1].x);
			previous.y = -(float(node[currentAutoscrollNode - 1].y));
			previous.z = 0.0f;
		} else {
			previous.x = someVec.x;
			previous.y = someVec.y;
			previous.z = 0.0f;
		}

		diffToPrev.x = current.x - previous.x;
		diffToPrev.y = current.y - previous.y;
		diffToPrev.z = current.z - previous.z;

		VEC3 thingyToPrev = diffToPrev;
		WeirdCrapWithVector(&thingyToPrev);

		float thing1 = node[currentAutoscrollNode].thing1;
		float thing2 = node[currentAutoscrollNode].thing2;

		diffToSome.x = current.x - someVec.x;
		diffToSome.y = current.y - someVec.y;
		diffToSome.z = current.z - someVec.z;

		VEC3 thingyToSome = diffToSome;
		WeirdCrapWithVector(&thingyToSome);

		float squarey = (thingyToSome.x*thingyToSome.x);
		squarey += (thingyToSome.y*thingyToSome.y);

		short angle_1 = cM_atan2s(thingyToSome.y, sqrtf(squarey));
		short angle_2 = cM_atan2s(thingyToSome.x, thingyToSome.z);

		if (((usedByAutoscroll_900F0 == 0) && (currentAutoscrollNode <= 1)) || (autoscrollInfoMaybe.autoscrollUnk11 != 0)) {
			field_900F8 = angle_1;
			field_900FA = angle_2;
		} else {
			sub_8015F340(&field_900F8, angle_1, 60, 250, 1);
			sub_8015F340(&field_900FA, angle_2, 60, 250, 1);
		}

		VEC3 butts;

		f25 = nw4r::math::CosFIdx(convertAngleToFloat(field_900F8) / 256.0f);
		butts.x = f25 * nw4r::math::SinFIdx(convertAngleToFloat(field_900FA) / 256.0f);

		butts.y = nw4r::math::SinFIdx(convertAngleToFloat(field_900F8) / 256.0f);

		f25 = nw4r::math::CosFIdx(convertAngleToFloat(field_900F8) / 256.0f);
		butts.z = f25 * nw4r::math::CosFIdx(convertAngleToFloat(field_900FA) / 256.0f);

		cLib_addCalc(&autoscrollInfoMaybe.field_C, thing1, 1.0f, thing2, 0.000099999997f);

		autoscrollInfoMaybe.vector.z = 0.0f;
		autoscrollInfoMaybe.vector.x += (butts.x * autoscrollInfoMaybe.field_C);
		autoscrollInfoMaybe.vector.y += (butts.y * autoscrollInfoMaybe.field_C);

		float distance = GetDistanceBetweenVectors_Maybe(&autoscrollInfoMaybe.vector, &current);
		float distanceRoot = sqrt(distance);

		bool ignoreHorizontal = false, ignoreVertical = false;
		if (autoscrollInfoMaybe.autoscrollMode == 1)
			ignoreVertical = true;
		else if (autoscrollInfoMaybe.autoscrollMode == 2)
			ignoreHorizontal = true;

		if (!ignoreHorizontal) {
			if (thingyToPrev.x != 0.0f) {
				if (thingyToPrev.x > 0.0f || (autoscrollInfoMaybe.vector.x < current.x)) {
					// wtf
					if (thingyToPrev.x > 0.0f)
						goto willNotEndAutoscroll;
					if (autoscrollInfoMaybe.vector.x > current.x)
						goto willNotEndAutoscroll;
				}
			}
		}

		if (!ignoreVertical) {
			if (thingyToPrev.y != 0.0f) {
				if (thingyToPrev.y > 0.0f || (autoscrollInfoMaybe.vector.y < current.y)) {
					// wtf
					if (thingyToPrev.y > 0.0f)
						goto willNotEndAutoscroll;
					if (autoscrollInfoMaybe.vector.y > current.y)
						goto willNotEndAutoscroll;
				}
			}
		}

		currentAutoscrollNode++;
		if (currentAutoscrollNode >= rail->nodeCount) {
			switch (autoscrollInfoMaybe.atEndMode) {
				case 0:
					disableAutoscroll();
					break;
				case 1:
					currentAutoscrollNode = autoscrollInfoMaybe.atEndNodeNumber;
					usedByAutoscroll_900F0++;
					break;
				default:
					currentAutoscrollNode = rail->nodeCount - 1;
					usedByAutoscroll_900F0 = 999;
					break;
			}
		}

willNotEndAutoscroll:

		previousScreenLeft = cwci->screenLeft;
		previousScreenTop = cwci->screenTop;

		VEC3 cameraPos;
		cameraPos.x = autoscrollInfoMaybe.vector.x;
		cameraPos.y = autoscrollInfoMaybe.vector.y;
		cameraPos.z = autoscrollInfoMaybe.vector.z;

		if (cameraPos.x < minZoneX)
			cameraPos.x = minZoneX;
		else if (cameraPos.x > maxZoneX)
			cameraPos.x = maxZoneX;

		if (cameraPos.y < minZoneY)
			cameraPos.y = minZoneY;
		else if (cameraPos.y > maxZoneY)
			cameraPos.y = maxZoneY;

		// FINAL THINGIES
		bool canHaveHorz, canHaveVert;

		switch (autoscrollInfoMaybe.autoscrollMode) {
			case 0:
				canHaveHorz = true; canHaveVert = true;
				break;
			case 1:
				canHaveHorz = true; canHaveVert = false;
				break;
			case 2:
				canHaveHorz = false; canHaveVert = true;
				break;
		}


		if (canHaveHorz) {
			float dupa = -999999.0f;
			int id;

			for (id = 0; id < 4; id++) {
				if (dAcPy_c *player = dAcPy_c::findByID(id)) {
					if (player->field_152C > dupa)
						dupa = player->field_152C;
				}
			}

			field_8FE88 = cameraPos.x;
		}

		if (canHaveVert) {
			field_8FE8C = cameraPos.y;
		}

		zoomDivisor = 1.0f / calculateZoomLevel();
		float ratioThing = 1.0f / zoomDivisor;
		zoomedWidth = screenWidth * ratioThing;
		zoomedHeight = screenHeight * ratioThing;
	}
}