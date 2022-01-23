#ifndef STAGEACTORMGR_H
#define STAGEACTORMGR_H 
#include "np.h"

class dStageActorMgr_c {
	public:
		u32 _0, _4, _8, _C;
		u16 storedShorts[1000];
		u8 storedBytes[1000];
		u16 _BC8;
		u8 _BCA, dontRunThings;
		s32 lastScreenLeft, lastScreenTop, _BD4, _BD8;

		struct spawnContext_s {
			s32 x1, y1, leftXBound, rightXBound, topYBound, bottomYBound;
			s32 unk1, unk2;
		};

		// clearSomeVars
		bool s_80068070(s32 r4, s32 r5, s32 r6, s32 r7, s32 r8, s32 r9);
		bool s_800680A0(s32 r4, s32 r5, s32 r6, s32 r7, s32 r8, s32 r9);
		// setupAndSpawnSprites
		// setupAndSpawnSpritesAlternate
		// doStuffForWholeArea
		// doStuffForCurrentZone
		// checkSpriteAgainstLookupTable
		// doGroupIDStuff
		bool doesSpriteNotHaveGroupID(u32 settings);
		// doStuff()
		void everyFrame();
		void createActorsIfNeedBe(spawnContext_s *context, bool what);
		dStageActor_c *createOneActor(
				dSprite_s *sprite, dSpriteInfo_s *spriteInfo,
				u8 *bytePtr, u16 *shortPtr, u8 zoneID);
};

#endif /* STAGEACTORMGR_H */
