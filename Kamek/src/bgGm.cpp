float ConvertXPosForWrapping(float x) {
	dScStage_c *stage = dScStage_c::instance;

	if (currentWrapType == 0)
		return x;

	float thisWidth = dBgGm_c::instance->zoneWidthAfterManipulations;
	int count = 0;

	while ((x < (dBgGm_c::instance->getManipulatedZoneLeft() - 1.0f)) ||
		   (x > (dBgGm_c::instance->getManipulatedZoneRight() + 1.0f))) {

		if (x < dBgGm_c::instance->getManipulatedZoneLeft()) {
			x += thisWidth;
		} else if (x > dBgGm_c::instance->getManipulatedZoneRight()) {
			x -= thisWidth;
		}

		count++;
		if (count > 1) {
			if (currentWrapType == 1)
				break;
			if (stage->world == 1 && stage->level == Stage_Castle1 && stage->area == 0)
				break;
		}
	}

	return x;
}
