#ifndef WATERMANAGER_H
#define WATERMANAGER_H 
#include "np.h"

struct dWaterInfo_s {
	float x, y, z, width, height;
	int isInUse;
	u8 type, layer;
};

class dWaterManager_c {
	private:
		dWaterInfo_s blocks[80];
	public:
		float current;

		static dWaterManager_c *instance;

		dWaterManager_c() { instance = this; }
		~dWaterManager_c() { instance = 0; }

		void setup();
		int addBlock(dWaterInfo_s *block);

	private: // ?
		int isPointWithinSpecifiedBlock(VEC2 *pos, int blockID);
		int getAngleOfVector(VEC2 *vec);
		int isPointWithinBubbleInternal(VEC2 *pos, int blockID, VEC2 *pOutVec, float *pFloat, s16 *pShort);

	public:
		int queryPosition(VEC2 *pos, VEC2 *pOutBlockPos, float *pOutFloat, s16 *pOutAngle, int layer);
		int isPositionWithinBubble(VEC2 *pos, VEC2 *pOutBlockPos, int blockID, int layer);
		void removeBlock(int blockID);
		void setPosition(VEC3 *pos, int blockID);
		void setGeometry(VEC3 *pos, float width, float height, int blockID);
};

#endif /* WATERMANAGER_H */
