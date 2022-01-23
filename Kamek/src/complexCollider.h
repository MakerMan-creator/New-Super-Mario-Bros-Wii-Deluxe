#ifndef COMPLEXCOLLIDER_H
#define COMPLEXCOLLIDER_H 

#include "np.h"

class dComplexCollider_c {
public:
	struct Info; // forward declaration
	typedef void (*Callback)(dComplexCollider_c *self, dComplexCollider_c *other);

	struct Info {
		float xDistToCenter;
		float yDistToCenter;
		float xDistToEdge;
		float yDistToEdge;
		u8 category1;
		u8 category2;
		u32 bitfield1;
		u32 bitfield2;
		u16 unkShort1C;
		Callback callback;
	};

	dComplexCollider_c();
	virtual ~dComplexCollider_c();

	dStageActor_c *owner;
	void *_8;
	u32 _C;
	dComplexCollider_c *listPrev, *listNext;
	u32 _18;
	Info info;
	float special[4];
	float lastCollisionDistanceX[8]; // indexed by category 1
	float lastCollisionDistanceY[8];
	Vec2 positionOfLastCollision;
	u16 result1;
	u16 result2;
	u16 result3;
	u8 collisionCheckType;
	u8 chainlinkMode;
	u8 layer;
	u8 someFlagByte;
	bool isLinkedIntoList;

	enum CollisionType {
		COLL_NORMAL = 0,
		COLL_ROUND,
		COLL_TRAPEZOID_VERT,
		COLL_TRAPEZOID_HORZ,
	};

	void clear();
	void addToList();
	void removeFromList();

	void initWithStruct(dStageActor_c *owner, const Info *info);
	void initWithStruct(dStageActor_c *owner, const Info *info, u8 clMode);

	u16 checkResult1(u16 param);
	u16 checkResult3(u16 param);

	float top();
	float bottom();
	float centerY();
	float right();
	float left();
	float centerX();

	bool testForRectangularCollision(dComplexCollider_c *other);

	bool processMe(dComplexCollider_c *other, bool something);

	// Plus more stuff that isn't needed in the public API, I'm pretty sure.
	
	static void setupManager();
	static dComplexCollider_c *globalListHead;
	static dComplexCollider_c *globalListTail;
	static void processAll();
};

#endif /* COMPLEXCOLLIDER_H */
