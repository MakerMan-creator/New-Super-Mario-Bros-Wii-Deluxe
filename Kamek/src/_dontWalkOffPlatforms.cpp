// PROTOTYPE TO GO INTO collisionMgr_c:
static bool sub_800757B0(VEC3 *vec, float *what, u8 layer, int p6, char p7);


// CODE:
bool daEnNoko_c::willWalkOntoSuitableGround() {
	static const float deltas[] = {2.5f, -2.5f};
	VEC3 checkWhere(
			pos.x + deltas[direction],
			4.0f + pos.y,
			pos.z);

	u32 props = collMgr.getTileBehaviour2At(checkWhere.x, checkWhere.y, currentLayerID);

	//if (getSubType(props) == B_SUB_LEDGE)
	if (((props >> 16) & 0xFF) == 8)
		return false;

	float someFloat = 0.0f;
	if (collMgr.sub_800757B0(&checkWhere, &someFloat, currentLayerID, 1, -1)) {
		if (someFloat < checkWhere.y && someFloat > (pos.y - 5.0f))
			return true;
	}

	return false;
}

