#ifndef COLLISIONSENSOR_H
#define COLLISIONSENSOR_H 

#include "np.h"

class dClass2DC_c;
class dSimpleCollider_c;

enum BehaviourFlags {
	BF_SOLID = 1,
	BF_COIN = 2,
	BF_QUESTION = 4,
	BF_EXPLODABLE = 8,
	BF_BRICK = 0x10,
	BF_SLOPE = 0x20,
	BF_FLIPPED_SLOPE = 0x40,
	BF_ANY_SLOPE = BF_SLOPE|BF_FLIPPED_SLOPE,
	BF_80 = 0x80,
	BF_100 = 0x100, // DS doors and pipes?
	BF_WATER = 0x200,
	BF_FENCE = 0x400,
	BF_PARTIAL_SOLID = 0x800,
	BF_SPIKE = 0x1000,
	BF_INVISIBLE_BLOCK = 0x2000,
	BF_4000 = 0x4000, // NSMBHD thread says it "forces you down"?
	BF_SOLID_ON_TOP = 0x8000,
	BF_PIPE = 0x10000,
	BF_DONUT = 0x20000,
	BF_40000 = 0x40000,
	BF_LEDGE = 0x80000,
	BF_100000 = 0x100000,
	BF_200000 = 0x200000,
	BF_400000 = 0x400000,
	BF_800000 = 0x800000,
	BF_1000000 = 0x1000000,
	BF_2000000 = 0x2000000,
	BF_4000000 = 0x4000000,
	BF_8000000 = 0x8000000,
	BF_10000000 = 0x10000000,
	BF_20000000 = 0x20000000,
	BF_40000000 = 0x40000000,
	BF_80000000 = 0x80000000,

	BF_819 = BF_PARTIAL_SOLID|BF_BRICK|BF_EXPLODABLE|BF_SOLID,
	BF_4899 = BF_4000|BF_PARTIAL_SOLID|BF_80|BF_BRICK|BF_EXPLODABLE|BF_SOLID,
	BF_8899 = BF_SOLID_ON_TOP|BF_PARTIAL_SOLID|BF_80|BF_BRICK|BF_EXPLODABLE|BF_SOLID,
};

enum BehaviourSubType {
	B_SUB_NULL = 0,
	B_SUB_ICY = 1,
	B_SUB_SNOW = 2,
	B_SUB_QUICKSAND = 3,
	B_SUB_CONVEYOR_RIGHT = 4,
	B_SUB_CONVEYOR_LEFT = 5,
	B_SUB_ROPE = 6,
	B_SUB_SPIKE = 7,
	B_SUB_LEDGE = 8,
	B_SUB_POLE = 9,
	B_SUB_NO_SLIDING = 10,
	B_SUB_CARPET = 11,
	B_SUB_DUSTY = 12,
	B_SUB_GRASS = 13,
	B_SUB_14 = 14,
	B_SUB_BEACH = 15,
	B_SUB_16 = 16,
};

enum BehaviourParamsGeneric {
	BP_GENERIC_NULL = 0,
	// Todo: Rails
	BP_GENERIC_STOP_RAIL = 0x22,
	BP_GENERIC_STOP_VINE = 0x23,
	BP_GENERIC_COIN_OUTLINE = 0x28,
	BP_GENERIC_COIN_OUTLINE_END = 0x2A,
	BP_GENERIC_BATTLE_COIN = 0x35,
	BP_GENERIC_SWITCH_BLOCK_1 = 0x36,
	BP_GENERIC_SWITCH_BLOCK_2 = 0x37,
	BP_GENERIC_CAVE_ENTRANCE_RIGHT = 0x38,
	BP_GENERIC_CAVE_ENTRANCE_LEFT = 0x39,
	BP_GENERIC_CAVE_ENTRANCE_UP = 0x3B,
};

enum BehaviourParamsSpike {
	BP_SPIKE_LEFT = 0,
	BP_SPIKE_RIGHT = 1,
	BP_SPIKE_UP = 2,
	BP_SPIKE_DOWN = 3,
	BP_SPIKE_LONG_DOWN_1 = 4,
	BP_SPIKE_LONG_DOWN_2 = 5,
	BP_SPIKE_SINGLE_DOWN = 6,
};

enum TouchedSpikeFlags {
	SPIKE_FLAG_LEFT = 1,
	SPIKE_FLAG_RIGHT = 2,
	SPIKE_FLAG_UP = 4,
	SPIKE_FLAG_DOWN = 8,
	SPIKE_FLAG_LONG_DOWN_1 = 0x10,
	SPIKE_FLAG_LONG_DOWN_2 = 0x20,
	SPIKE_FLAG_SINGLE_DOWN = 0x40,
	SPIKE_FLAG_80 = 0x80,
};

enum BehaviourParamsCoin {
	BP_COIN_NULL = 0,
	BP_COIN_2 = 2, // red coin maybe?
	BP_COIN_POW = 4,
};

enum BehaviourParamsSlope {
	BP_SLOPE_1_UP = 0,
	BP_SLOPE_1_DOWN = 1,
	BP_SLOPE_2A_UP = 2,
	BP_SLOPE_2B_UP = 3,
	BP_SLOPE_2A_DOWN = 4,
	BP_SLOPE_2B_DOWN = 5,
	BP_SLOPE_TALL_A_UP = 6,
	BP_SLOPE_TALL_B_UP = 7,
	BP_SLOPE_TALL_A_DOWN = 8,
	BP_SLOPE_TALL_B_DOWN = 9,
	BP_SLOPE_EDGE = 10,
	BP_SLOPE_4A_UP = 11,
	BP_SLOPE_4B_UP = 12,
	BP_SLOPE_4C_UP = 13,
	BP_SLOPE_4D_UP = 14,
	BP_SLOPE_4A_DOWN = 15,
	BP_SLOPE_4B_DOWN = 16,
	BP_SLOPE_4C_DOWN = 17,
	BP_SLOPE_4D_DOWN = 18,
};

enum BehaviourParamsPipe {
	BP_PIPE_UP_A = 0,
	BP_PIPE_UP_B = 1,
	BP_PIPE_DOWN_A = 2,
	BP_PIPE_DOWN_B = 3,
	BP_PIPE_VERT_A = 4,
	BP_PIPE_VERT_B = 5,
	BP_PIPE_VERT_MIX_A = 6,
	BP_PIPE_VERT_MIX_B = 7,

	BP_PIPE_LEFT_A = 8,
	BP_PIPE_LEFT_B = 9,
	BP_PIPE_RIGHT_A = 10,
	BP_PIPE_RIGHT_B = 11,
	BP_PIPE_HORZ_A = 12,
	BP_PIPE_HORZ_B = 13,
	BP_PIPE_HORZ_MIX_A = 14,
	BP_PIPE_HORZ_MIX_B = 15,

	BP_PIPE_MINI_UP = 16,
	BP_PIPE_MINI_DOWN = 18,
	BP_PIPE_MINI_VERT_MIX = 22,
	BP_PIPE_MINI_LEFT = 24,
	BP_PIPE_MINI_RIGHT = 26,
	BP_PIPE_MINI_VERT = 28,
	BP_PIPE_MINI_HORZ = 29,
	BP_PIPE_MINI_HORZ_MIX = 30,

	BP_PIPE_JOINT = 31,
};

inline int getSubType(u32 props) {
	return (props >> 16) & 0xFF;
}
inline int getParam(u32 props) {
	return props & 0xFF;
}

enum SensorFlags {
	SENSOR_POINT = 0,
	SENSOR_LINE = 1,

	SENSOR_TYPE_MASK = 1,
	SENSOR_2 = 2, // has not shown up yet...?
	SENSOR_IGNORE_SIMPLE_COLL = 4,
	SENSOR_8 = 8, // can enter pipe?
	SENSOR_10 = 0x10, // Related to ice
	SENSOR_20 = 0x20, // related to ice, too
	SENSOR_40 = 0x40, // checked by simplecollider
	SENSOR_80 = 0x80, // checked by simplecollider
	SENSOR_100 = 0x100, // checked by simplecollider
	SENSOR_IGNORE_MOUSE_HOLE = 0x200, // checked by simplecollider
	SENSOR_400 = 0x400, // checked by simplecollider
	SENSOR_800 = 0x800, // checked by simplecollider
	SENSOR_NO_QUICKSAND = 0x1000,
	SENSOR_2000 = 0x2000, // something to do with fences maybe?
	SENSOR_BREAK_BLOCK = 0x4000,
	SENSOR_8000 = 0x8000, // used
	SENSOR_10000 = 0x10000, // used
	SENSOR_COIN_1 = 0x20000,
	SENSOR_COIN_2 = 0x40000,
	SENSOR_COIN_OUTLINE = 0x80000,
	SENSOR_ACTIVATE_QUESTION = 0x100000,
	SENSOR_ACTIVATE_DONUTS = 0x200000,
	SENSOR_HIT_BRICK = 0x400000,
	SENSOR_BREAK_BRICK = 0x800000,
	SENSOR_HIT_OR_BREAK_BRICK = 0xC00000,
	SENSOR_1000000 = 0x1000000, // has not shown up yet...?
	SENSOR_2000000 = 0x2000000, // corresponds to SCF_400?
	SENSOR_4000000 = 0x4000000, // something related to hitting blocks 
	SENSOR_8000000 = 0x8000000, // corresponds to SCF_20?
	SENSOR_10000000 = 0x10000000, // used - makes bricks non-solid when broken?
	SENSOR_20000000 = 0x20000000, // used
	SENSOR_40000000 = 0x40000000, // used
	SENSOR_NO_COLLISIONS = 0x80000000, // used - stops things from pushing you out? MAYBE?
};

// Output is split into...
// Above   :
// FC000000: 11111100000000000000000000000000
// Below   :
//  3FFE000:       11111111111110000000000000
//   1FE000:            111111110000000000000
//    18000:                11000000000000000
// Adjacent:
//     1FFF:                    1111111111111

enum SensorOutputFlags {
	CSOUT_ABOVE_ALL = 0xFC000000,
	CSOUT_BELOW_ALL = 0x3FFE000,
	CSOUT_ADJACENT_ALL = 0x1FFF,
};

// 0x8000 : Has object from Class 2DC?
// 0x4000 : Has slope from Class 2DC?

struct hLine_s {
	float x1, x2, y;
};
struct vLine_s {
	float x, y1, y2;
};

class dCollisionSensor_c {
	public:
		dCollisionSensor_c();
		virtual ~dCollisionSensor_c();

		struct pointSensor_s;
		struct lineSensor_s;
		struct infoBase_s {
			u32 flags;
			inline pointSensor_s *asPoint() const {
				return (pointSensor_s*)this;
			}
			inline lineSensor_s *asLine() const {
				return (lineSensor_s*)this;
			}
		};
		struct pointSensor_s : infoBase_s {
			s32 x, y;
		};
		struct lineSensor_s : infoBase_s {
			s32 lineA, lineB, distanceFromCenter;
		};

		dStageActor_c *owner;
		infoBase_s *pBelowInfo, *pAboveInfo, *pAdjacentInfo;

		VEC3 *pPos, *pLastPos, *pSpeed;
		VEC3 specialSpeedOffset;
		float xDeltaMaybe, yDeltaMaybe;
		float _34, _38;
		u32 _3C, _40, _44, _48;
		float initialXAsSetByJumpDai;
		dClass2DC_c *pClass2DC;
		u32 _54;
		dSimpleCollider_c *sCollBelow;
		dSimpleCollider_c *sCollAbove;
		dSimpleCollider_c *sCollAdjacentLast, *sCollAdjacent[2];
		dCollisionSensor_c *_6C, *_70, *_74;
		dCollisionSensor_c *_78[2];
		dSimpleCollider_c *_80[2];
		u32 outputMaybe;
		u32 _8C;
		u32 someStoredBehaviourFlags, someStoredProps;
		char whichPlayerOfParent____;
		char whichController____;
		u16 _9A, _9C;
		u32 tileBelowProps;
		u8 tileBelowSubType, lastTileBelowSubType;
		u32 tileAboveProps;
		u8 tileAboveSubType, lastTileAboveSubType;
		u32 adjacentTileProps[2];
		u8 adjacentTileSubType[2];
		u8 _BA, _BB;
		u8 _BC; // &8 makes able to enter mini pipes?
		u8 currentSlopeType;
		s16 currentSlopeAngle;
		u8 currentFlippedSlopeType;
		s16 currentFlippedSlopeAngle;
		u32 _C4;
		u16 currentAdjacentSlopeAngle;
		u32 currentFenceType; // 0=null 1=ladder 2=fence
		dSimpleCollider_c *_D0;
		float _D4;
		float _D8;
		float _DC;
		u8 touchedSpikeFlag;
		s8 maxTouchedSpikeType;
		u8 currentSlopeDirection, _E3, onGround_maybe, chainlinkMode;
		u8 *pLayerID;
		u8 layer;
		bool enteredPipeIsMini;

		// Setup
		void clear1();
		void clear2();
		void init(dStageActor_c *owner, infoBase_s *belowInfo, infoBase_s *aboveInfo, infoBase_s *adjacentInfo);

		// Basic Tile Interactions
	private:
		bool collectCoin(u32 coll1, u32 coll2, u32 sFlags, VEC3 *pVec, bool removeTile);
		bool handleCoinOutline(u32 coll1, u32 coll2, u32 sFlags, VEC3 *pVec);
		void clearTouchedSpikeData();
		bool checkForSpikeTile(int type, float x, float y);
		bool tryBreakBlock(u32 sFlags, u8 direction, float x, float y);
		static bool static_tryBreakBlock(u32 sFlags, u32 direction, u8 layer, int stageActorType, u8 p7, float x, float y);
		bool tryActivateQuestionBlock(u32 sFlags, u8 direction, float x, float y);
		u32 tryActivateBrick(u32 sFlags, u32 direction, float x, float y);

		// Collisions Below
	public:
		u32 calculateBelowCollision();
	private:
		u32 checkBelowAsPoint(pointSensor_s *info, u32 sFlags);
		u32 checkBelowAsLine(lineSensor_s *info, u32 sFlags);
		u32 checkSpecificPosForBelow(u32 sFlags, float xOffset, float yOffset);
		u32 tryActivateBlockFromAbove(u32 sFlags, float xOffset, float yOffset);
		int checkTypeOfTouchedTile(u32 *pColl1, u32 *pColl2, float x, float y);
		u32 assignGroundTileMaybe(u32 sFlags);
		u32 getSomethingAboutSlope(int slopeType, u32 sFlags);

	private:
		void handleNullGroundCB();
		void handleQuicksandCB();
		void handleConveyorRightCB();
		void handleConveyorLeftCB();

		// Collisions Above
	public:
		u32 calculateAboveCollision(u32 value);
	private:
		u32 checkAboveAsPoint(pointSensor_s *info, u32 thing);
		u32 checkAboveAsLine(lineSensor_s *info, u32 thing);
		u32 checkSpecificPosForAbove(u32 sFlags, float x, float y);
		u32 tryHitBlockFromBelow(u32 sFlags, float x, float y);
		void pushTopOutOfBlockIfNeeded(u32 resultThing, u32 sFlags, float y);
		u32 checkForFlipSlopeAbove(u32 sFlags);

		// Adjacent Collisions
	public:
		u32 calculateAdjacentCollision(float *pFloat=0);
	private:
		u32 calculateAdjacentCollisionInternal(int direction, u32 extraSensorFlags);
		u32 checkAdjacentAsPoint(pointSensor_s *info, int direction, u32 sFlags);
		u32 checkAdjacentAsLine(lineSensor_s *info, int direction, u32 sFlags);
		bool isTouchingSlope_maybe(float x, float y);
		u32 checkSpecificPosForAdjacent(u32 sFlags, int direction, float x, float y);

		bool setStoredX(float x, float y);

		s8 s_80070530(float y) const;
		dStageActor_c *s_800705B0(float y) const;

	public:
		// could this actually be protected, and be accessed as friend class? dunno
		bool setBelowSubType(u32 magic);
	private:
		bool setBelowPropsIgnoringSpike(u32 magic);
		bool setBelowProps(u32 magic);
		void setAboveProps(u32 magic);
		void setAdjacentProps(u32 magic, int direction);

	public:
		u8 getBelowSubType();
		u8 getAboveSubType();
		u8 getAdjacentSubType(int direction);

		int returnSomethingBasedOnSlopeAngle();
		bool doesSlopeGoUp();

		int getSlopeYDirectionForXDirection(int direction);
		int getSlopeYDirectionForXDirection(int direction, int slopeType);
		s16 getAngleOfSlopeInDirection(int direction);
		s16 getAngleOfSlopeWithXSpeed(float xSpeed);
		s16 getAngleOfSlopeInDirection2(int thing);
		s16 getAngleOfFlippedSlopeInDirection(int thing);
		s16 getAngleOfFlippedSlopeInDirection2(int thing);

		static bool checkPositionForTileOrSColl(float x, float y, u8 layer, u32 unk, u32 mask);
		static int getPartialBlockExistenceAtPos(float x, float y, u8 layer);
		static bool isPartialBlockOrBlockSolidAtPos(float x, float y, u8 layer);
		static u32 getTileBehaviour1At(float x, float y, u8 layer);
		static u32 getTileBehaviour2At(float x, float y, u8 layer);
		static u32 getTileBehaviour1At(u16 x, u16 y, u8 layer);
		static u32 getTileBehaviour2At(u16 x, u16 y, u8 layer);

		u8 getOwnerStageActorType() const;
		u32 returnOutputAnd18000() const;
		bool isOnTopOfTile() const;

	private:
		u32 s_80070CD0(dStageActor_c *player, u32 passedToBCLP);
		u32 s_80070E70(dStageActor_c *player);
		u32 s_80070EF0(dStageActor_c *actor);
		u32 s_80071060(int direction);
	public: // ?
		u32 s_80071210(dStageActor_c *player);
	private: // ?
		bool s_800713B0(dStageActor_c *player, u32 things);
	public: // ?
		bool tryToEnterPipeBelow(VEC3 *pOutVec, int *pOutEntranceID);
		bool tryToEnterPipeAbove(VEC3 *pOutVec, int *pOutEntranceID);
		bool tryToEnterAdjacentPipe(VEC3 *pOutVec, int unk, int *pOutEntranceID, float fp1, float fp2);
		u32 detectFence(lineSensor_s *info);
		bool detectClimbingPole(lineSensor_s *info);

	private:
		bool s_800722C0(VEC3 *twoVecs);
	public:
		bool s_80072440(VEC2 *v2one, VEC2 *v2two, float *pOutFloat);
	private:
		bool s_80072790(VEC3 *twoVecs);
	public:
		bool s_800728C0(VEC2 *vecParam, float *pOutFloat);

	public:
		u32 calculateBelowCollisionWithSmokeEffect();

		u32 calculateAdjacentCollisionAlternate(float *pFloat=0);

		static void setupEverything(int wrapType);

		bool getBelowSensorHLine(hLine_s *outLine);
		bool getAboveSensorHLine(hLine_s *outLine);
		bool getAdjacentSensorVLine(vLine_s *outLine, int direction);

		void s_800731E0(dSimpleCollider_c *sColl, VEC3 *somePos);

	private:
		u32 checkStuffCB_Wrap(u32 sFlags, float someFloat);
		bool s_80073290(dSimpleCollider_c *sColl, hLine_s *pLine, VEC3 *pOutVec, float someFloat);
		u32 checkStuffCB(u32 sFlags, float someFloat);
		void s_80073AC0(dSimpleCollider_c *sColl, VEC3 *vec);
		u32 secondCB_Wrap(u32 sFlags, float someFloat);
		bool s_80073BC0(dSimpleCollider_c *sColl, hLine_s *pLine, VEC3 *pOutVec, float someFloat);
		u32 secondCB(u32 sFlags, float someFloat);
		u32 thirdCB_Wrap(u32 sFlags, int direction, float someFloat);
		u32 s_80074470(dSimpleCollider_c *sColl, int direction, vLine_s *pLine, VEC3 *pOutVec, float someFloat);
		u32 thirdCB(u32 sFlags, int direction, float someFloat);

		bool s_80074FF0(dSimpleCollider_c *sColl, VEC3 *pVec, u32 sFlags);

	public:
		void clearStoredSColls();
		void s_80075090();
		bool s_800751C0(dSimpleCollider_c *sColl, dCollisionSensor_c *what);
		dCollisionSensor_c *s_80075230(dSimpleCollider_c *sColl);

		static int checkPositionForLiquids(float x, float y, int layer, float *unkDestFloat = 0);
		static int s_80075560(float x, float y, int layer, float *destFloat);
		static bool s_80075750(VEC2 *pVec);
		static bool s_80075780(s16 *pAngle);

		// to-be-decompiled and filled in later ...
		static bool sub_800757B0(VEC3 *vec, float *what, u8 layer, int p6, char p7);

	private:
		typedef void (dCollisionSensor_c::*handleGroundCBType)();
		static handleGroundCBType s_handleGroundCB[17];

		typedef u32 (dCollisionSensor_c::*checkStuffCBType)(u32 sFlags, float value);
		static checkStuffCBType s_checkStuffCB;
		static checkStuffCBType s_checkStuffCBs[3];

		typedef u32 (dCollisionSensor_c::*secondCBType)(u32 sFlags, float unkFloat);
		static secondCBType s_secondCB;
		static secondCBType s_secondCBs[3];

		typedef u32 (dCollisionSensor_c::*thirdCBType)(u32 sFlags, int direction, float value);
		static thirdCBType s_thirdCB;
		static thirdCBType s_thirdCBs[3];

		static u16 s_storedX, s_storedY;

		static u32 s_58F4;
		static VEC2 s_58F0;
		static s16 s_58E8;

	public:
		enum SlopeTypes {
			SLOPE_EDGE = 10,
			SLOPE_TYPE_COUNT = 19
		};
		struct slopeParams_s {
			float basePos, colHeight;
		};
		static const slopeParams_s slopeParams[SLOPE_TYPE_COUNT];
		static const slopeParams_s flippedSlopeParams[SLOPE_TYPE_COUNT];
		static const u8 slopeKinds[SLOPE_TYPE_COUNT];
		static const u8 slopeDirections[SLOPE_TYPE_COUNT];
};

#endif /* COLLISIONSENSOR_H */
