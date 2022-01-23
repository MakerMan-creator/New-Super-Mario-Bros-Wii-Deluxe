#include "waterManager.h"

void dWaterManager_c::setup() {
	current = 0.0f;

	for (int i = 0; i < 80; i++) {
		blocks[i].x = 0.0f;
		blocks[i].y = 0.0f;
		blocks[i].z = 0.0f;
		blocks[i].width = 0.0f;
		blocks[i].height = 0.0f;
		blocks[i].isInUse = 0;
		blocks[i].type = 0;
	}
}

int dWaterManager_c::addBlock(dWaterInfo_s *block) {
	for (int i = 0; i < 80; i++) {
		if (blocks[i].isInUse == 0) {
			blocks[i].x = block->x;
			blocks[i].y = block->y;
			blocks[i].z = block->z;
			blocks[i].width = block->width;
			blocks[i].height = block->height;
			blocks[i].layer = block->layer;
			blocks[i].type = block->type;
			blocks[i].isInUse = 1;
			return i;
		}
	}

	return -1;
}

int dWaterManager_c::isPointWithinSpecifiedBlock(VEC2 *pos, int blockID) {
	dWaterInfo_s *block = &blocks[blockID];

	if (
			block->x <= pos->x &&
			(block->x + block->width) >= pos->x &&
			block->y >= pos->y &&
			(block->y - block->height) <= pos.y
	   ) {
	   	return block->type;
	}

	return -1;
}

int dWaterManager_c::getAngleOfVector(VEC2 *vec) {
	return cM_atan2s(vec->x, vec->y);
}

int dWaterManager_c::isPointWithinBubbleInternal(VEC2 *pos, int blockID, VEC2 *pOutVec, float *pFloat, s16 *pShort) {
	dWaterInfo_s *block = &blocks[blockID];

	VEC2 posWithinBubble(pos->x - block->x, pos->y - block->y);
	float width = block->width, height = block->height;

	if (block->type == 3) {
		width *= 0.5f;
		height *= 0.5f;
	} else if (block->type == 4) {
		width *= 0.25f;
		height *= 0.5f;
	} else {
		width *= 0.5f;
		height *= 0.25f;
	}

	float divX = posWithinBubble.x / width;
	float divY = posWithinBubble.y / height;

	if (((divX*divX)+(divY*divY)) < 1.0f) {
		if (0.0f != posWithinBubble.x) {
			// THIS IS A MESS!!!
			float yDivByX = posWithinBubble.y / posWithinBubble.x;
			float widthRatio = width * yDivByX;
			float widthTimesHeight = width * height;
			float heightSquared = height * height;
			widthRatio *= widthRatio;
			widthTimesHeight *= widthTimesHeight;
			float what = widthRatio + heightSquared;
			float f1 = widthTimesHeight / what;
			float thing = 0.0f;
			if (f1 <= 0.0f) {
			} else {
				thing = (widthTimesHeight / what) * nw4r::math::FrSqrt(f1);
			}
			if (posWithinBubble.x < 0.0f)
				thing = -thing;
			pOutVec->x = block->x + thing;
			pOutVec->y = block->y + (yDivByX * thing);

			float f27 = posWithinBlock.x / width;
			f27 *= f27;
			f1 = 1.0f - f27;
			float topYOffset = 0.0f;
			if (f1 <= 0.0f) {
			} else {
				topYOffset = (1.0f - f27) * nw4r::math::FrSqrt(f1);
			}
			*pFloat = block->y + (height * topYOffset);

		} else {
			pOutVec->x = block->x;

			float thing;
			if (posWithinBubble.y > 0.0f)
				thing = height;
			else
				thing = -height;

			pOutVec->y = block->y + thing;
			*pFloat = block->y + height;
		}

		*pShort = getAngleOfVector(&posWithinBubble);
		return block->type;
	}

	return -1;
}

int dWaterManager_c::queryPosition(VEC2 *pos, VEC2 *pOutBlockPos, float *pOutFloat, s16 *pOutAngle, int layer) {
	for (int i = 0; i < 80; i++) {
		dWaterInfo_s *block = &blocks[i];

		if (block->isInUse == 0)
			continue;
		if (layer != block->layer)
			continue;

		switch (block->type) {
			case 0: case 1: case 2:
				int result = isPointWithinSpecifiedBlock(pos, i);
				if (result == -1)
					continue;

				if (pOutBlockPos) {
					pOutBlockPos->x = block->x;
					pOutBlockPos->y = block->y;
				}

				if (pOutFloat)
					*pOutFloat = block->y;

				if (pOutAngle)
					*pOutAngle = 0;

				return result;

			case 3: case 4: case 5:
				VEC2 bubbleVec;
				float bubbleFloat;
				s16 bubbleAngle;
				int result = isPointWithinBubbleInternal(pos, i, &bubbleVec, &bubbleFloat, &bubbleAngle);

				if (result == -1)
					continue;

				if (pOutBlockPos)
					*pOutBlockPos = bubbleVec;

				if (pOutFloat) {
					if (pos->y > bubbleFloat)
						*pOutFloat = pos->y;
					else
						*pOutFloat = bubbleFloat;
				}

				if (pOutAngle)
					*pOutAngle = bubbleAngle;

				return result;
		}
	}

	return -1;
}


int dWaterManager_c::isPositionWithinBubble(VEC2 *pos, VEC2 *pOutBlockPos, int blockID, int layer) {
	dWaterInfo_s *block = &blocks[blockID];

	if (layer == block->layer) {
		VEC2 bubbleVec;
		float bubbleFloat;
		s16 bubbleAngle;
		int result = isPointWithinBubbleInternal(pos, blockID, &bubbleVec, &bubbleFloat, &bubbleAngle);

		if (result != -1) {
			if (pOutBlockPos)
				*pOutBlockPos = bubbleVec;
			return result;
		}
	}

	return -1;
}


void dWaterManager_c::removeBlock(int blockID) {
	blocks[blockID].x = 0.0f;
	blocks[blockID].y = 0.0f;
	blocks[blockID].z = 0.0f;
	blocks[blockID].width = 0.0f;
	blocks[blockID].height = 0.0f;
	blocks[blockID].layer = 0;
	blocks[blockID].type = 0;
	blocks[blockID].isInUse = 0;
}


void dWaterManager_c::setPosition(VEC3 *pos, int blockID) {
	if (blocks[blockID].isInUse == 0)
		return;

	blocks[blockID].x = pos->x;
	blocks[blockID].y = pos->y;
	blocks[blockID].z = pos->z;
}


void dWaterManager_c::setGeometry(VEC3 *pos, float width, float height, int blockID) {
	if (blocks[blockID].isInUse == 0)
		return;

	blocks[blockID].x = pos->x;
	blocks[blockID].y = pos->y;
	blocks[blockID].z = pos->z;
	blocks[blockID].width = width;
	blocks[blockID].height = height;
}

