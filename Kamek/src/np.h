#ifndef NP_H
#define NP_H 
#include "../Kamek/include/commonX.h"

namespace nw4r { namespace math {


// Uppercase is used so there is no conflict with DolphinSDK structures and names.
/* =======================================================================
        Structure definitions
   ======================================================================== */
struct _VEC2
{
    f32 x;
    f32 y;
};

struct _VEC3
{
    f32 x;
    f32 y;
    f32 z;
};

struct _VEC4
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

struct _QUAT
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

struct _QUAT16
{
    s16 x;
    s16 y;
    s16 z;
    s16 w;
};

struct _MTX22
{
    union
    {
        struct
        {
            f32 _00, _01;
            f32 _10, _11;
        };
        f32 m[2][2];
        f32 a[4];
    };
};

struct _MTX33
{
    union
    {
        struct
        {
            f32 _00, _01, _02;
            f32 _10, _11, _12;
            f32 _20, _21, _22;
        };
        f32 m[3][3];
        f32 a[9];
    };
};

struct _MTX34
{
    union
    {
        struct
        {
            f32 _00, _01, _02, _03;
            f32 _10, _11, _12, _13;
            f32 _20, _21, _22, _23;
        };
        f32 m[3][4];
        f32 a[12];
        Mtx mtx;
    };
};

struct _MTX44
{
    union
    {
        struct
        {
            f32 _00, _01, _02, _03;
            f32 _10, _11, _12, _13;
            f32 _20, _21, _22, _23;
            f32 _30, _31, _32, _33;
        };
        f32 m[4][4];
        f32 a[16];
        Mtx44 mtx;
    };
};



/* =======================================================================
        Class definitions
   ======================================================================== */
struct VEC2;
struct VEC3;
struct VEC4;
struct QUAT;
struct QUAT16;
struct MTX22;
struct MTX34;
struct MTX44;


/* ------------------------------------------------------------------------
    VEC2
   ------------------------------------------------------------------------ */
struct VEC2 : public _VEC2
{
public:
    typedef VEC2 self_type;
    typedef f32  value_type;
public:
    VEC2() {}
    VEC2(const f32* p) { x = p[0]; y = p[1]; }
    VEC2(f32 fx, f32 fy) { x = fx; y = fy; }
    VEC2(const _VEC2& v) { x = v.x; y = v.y; }

    operator f32*() { return &x; }
    operator const f32*() const { return &x; }

    self_type& operator+=(const self_type& rhs) { x += rhs.x; y += rhs.y; return *this; }
    self_type& operator-=(const self_type& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    self_type& operator*=(f32 f) { x *= f; y *= f; return *this; }
    self_type& operator/=(f32 f) { f32 r = 1.f / f; x *= r; y *= r; return *this; }

    self_type operator+() const { return *this; }
    self_type operator-() const { return self_type(-x, -y); }

    self_type operator+(const self_type& rhs) const { return self_type(x + rhs.x, y + rhs.y); }
    self_type operator-(const self_type& rhs) const { return self_type(x - rhs.x, y - rhs.y); }
    self_type operator*(f32 f) const { return self_type(f * x, f * y); }
    self_type operator/(f32 f) const { f32 r = 1.f / f; return self_type(r * x, r * y); }
    bool operator==(const self_type& rhs) const { return x == rhs.x && y == rhs.y; }
    bool operator!=(const self_type& rhs) const { return x != rhs.x || y != rhs.y; }

    f32 LenSq() const { return x * x + y * y; }

    void Report(bool bNewline = true, const char* name = NULL) const;
};



/* ------------------------------------------------------------------------
    VEC3
   ------------------------------------------------------------------------ */
struct VEC3 : public _VEC3
{
public:
    typedef VEC3 self_type;
    typedef f32  value_type;
public:
    VEC3() {}
    VEC3(const f32* p) { x = p[0]; y = p[1]; z = p[2]; }
    VEC3(f32 fx, f32 fy, f32 fz) { x = fx; y = fy; z = fz; }
    VEC3(const _VEC3& v) { x = v.x; y = v.y; z = v.z; }
    VEC3(const Vec& v) { x = v.x; y = v.y; z = v.z; }

    operator f32*() { return &x; }
    operator const f32*() const { return &x; }
#if 1
    operator Vec*() { return (Vec*)&x; }
    operator const Vec*() const { return (const Vec*)&x; }
#endif

    self_type& operator+=(const self_type& rhs);
    self_type& operator-=(const self_type& rhs);
    self_type& operator*=(f32 f);
    self_type& operator/=(f32 f) { return operator*=(1.f / f); }

    self_type operator+() const { return *this; }
    self_type operator-() const { return self_type(-x, -y, -z); }

    // The optimal implementation of binary operators depends on whether the return value is optimized.
    self_type operator+(const self_type& rhs) const;
    self_type operator-(const self_type& rhs) const;
    self_type operator*(f32 f) const;
    self_type operator/(f32 f) const { f32 r = 1.f / f; return operator*(r); }

    bool operator==(const self_type& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const self_type& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

    f32 LenSq() const { return x * x + y * y + z * z; }

    void Report(bool bNewline = true, const char* name = NULL) const;
};



/* ------------------------------------------------------------------------
    VEC4
   ------------------------------------------------------------------------ */
struct VEC4 : public _VEC4
{
public:
    typedef VEC4 self_type;
    typedef f32  value_type;
public:
    VEC4() {}
    VEC4(const f32* p) { x = p[0]; y = p[1]; z = p[2]; w = p[3]; }
    VEC4(f32 fx, f32 fy, f32 fz, f32 fw) { x = fx; y = fy; z = fz; w = fw; }
    VEC4(const _VEC4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }

    operator f32*() { return &x; }
    operator const f32*() const { return &x; }

    self_type& operator+=(const self_type& rhs);
    self_type& operator-=(const self_type& rhs);
    self_type& operator*=(f32 f);
    self_type& operator/=(f32 f);

    self_type operator+() const { return *this; }
    self_type operator-() const { return self_type(-x, -y, -z, -w); }

    self_type operator+(const self_type& rhs) const;
    self_type operator-(const self_type& rhs) const;
    self_type operator*(f32 f) const;
    self_type operator/(f32 f) const { f32 r = 1.f / f; return operator*(r); }

    bool operator==(const self_type& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
    bool operator!=(const self_type& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }

    f32 LenSq() const { return x * x + y * y + z * z + w * w; }

    void Report(bool bNewline = true, const char* name = NULL) const;
};



/* ------------------------------------------------------------------------
    QUAT
   ------------------------------------------------------------------------ */
struct QUAT : public _QUAT
{
public:
    typedef QUAT self_type;
    typedef f32  value_type;
public:
    QUAT() {}
    QUAT(const f32* p) { x = p[0]; y = p[1]; z = p[2]; w = p[3]; }
    QUAT(const _QUAT& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; }
    QUAT(f32 fx, f32 fy, f32 fz, f32 fw) { x = fx; y = fy; z = fz; w = fw; }

    operator f32*() { return &x; }
    operator const f32*() const { return &x; }
#if 0
    // We've refrained from using the cast operators because the function names would be the same as DolphinSDK.
    operator Quaternion*() { return (Quaternion*)&x; }
    operator const Quaternion*() const { return (const Quaternion*)&x; }
#endif

    self_type& operator+=(const self_type& rhs);
    self_type& operator-=(const self_type& rhs);
    self_type& operator*=(f32 f);
    self_type& operator/=(f32 f) { return operator*=(1.f / f); }

    self_type operator+() const { return *this; }
    self_type operator-() const { return self_type(-x, -y, -z, -w); }

    self_type operator+(const self_type& rhs) const;
    self_type operator-(const self_type& rhs) const;
    self_type operator*(f32 f) const;
    self_type operator/(f32 f) const { return operator*(1.f / f); }

    bool operator==(const self_type& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
    bool operator!=(const self_type& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }

    void Report(bool bNewline = true, const char* name = NULL) const;
};



/* ------------------------------------------------------------------------
    MTX33
    Exists primarily for normal matrices.
    For now, implement only those that are definitely required.
   ------------------------------------------------------------------------ */
struct MTX33 : public _MTX33
{
public:
    typedef MTX33 self_type;
    typedef f32   value_type;
public:
    MTX33() {}
    MTX33(const f32* p);
    MTX33(const MTX34& rhs);
    MTX33(f32 x00, f32 x01, f32 x02,
          f32 x10, f32 x11, f32 x12,
          f32 x20, f32 x21, f32 x22)
    {
        _00 = x00; _01 = x01; _02 = x02;
        _10 = x10; _11 = x11; _12 = x12;
        _20 = x20; _21 = x21; _22 = x22;
    }

    // cast operators
    operator f32*() { return &_00; }
    operator const f32*() const { return &_00; }

    bool operator==(const self_type& rhs) const { return memcmp(this, &rhs, sizeof(MTX33)) == 0; }
    bool operator!=(const self_type& rhs) const { return memcmp(this, &rhs, sizeof(MTX33)) != 0; }

    void Report(bool bNewline = true, const char* name = NULL) const;
};



/* ------------------------------------------------------------------------
    MTX34
   ------------------------------------------------------------------------ */
struct MTX34 : public _MTX34
{
public:
    typedef MTX34 self_type;
    typedef f32   value_type;
    typedef const f32 (*ConstMtxPtr)[4];
public:
    MTX34() {}
    MTX34(const f32* p);
    MTX34(const Mtx& rhs);
    MTX34(const MTX33& rhs);
    MTX34(f32 x00, f32 x01, f32 x02, f32 x03,
          f32 x10, f32 x11, f32 x12, f32 x13,
          f32 x20, f32 x21, f32 x22, f32 x23)
    {
        _00 = x00; _01 = x01; _02 = x02; _03 = x03;
        _10 = x10; _11 = x11; _12 = x12; _13 = x13;
        _20 = x20; _21 = x21; _22 = x22; _23 = x23;
    }

    // cast operators
    operator f32*() { return &_00; }
    operator const f32*() const { return &_00; }
#if 1
    operator MtxPtr() { return (MtxPtr)&_00; }
    operator ConstMtxPtr() const { return (ConstMtxPtr)&_00; }
#endif

    self_type& operator+=(const self_type& rhs);
    self_type& operator-=(const self_type& rhs);
    
    self_type& operator*=(f32 f);
    self_type& operator/=(f32 f) { return operator*=(1.f / f); }

    self_type operator+() const { return *this; }
    self_type operator-() const
    {
        return MTX34(-_00, -_01, -_02, -_03,
                     -_10, -_11, -_12, -_13,
                     -_20, -_21, -_22, -_23);
    }

    //
    self_type operator+(const self_type& rhs) const;
    self_type operator-(const self_type& rhs) const;

    self_type operator*(f32 f) const;
    self_type operator/(f32 f) const { return *this * (1.f / f); }

    bool operator==(const self_type& rhs) const { return memcmp(this, &rhs, sizeof(MTX34)) == 0; }
    bool operator!=(const self_type& rhs) const { return memcmp(this, &rhs, sizeof(MTX34)) != 0; }

    void Report(bool bNewline = true, const char* name = NULL) const;
};



/* ------------------------------------------------------------------------
    MTX44
   ------------------------------------------------------------------------ */
struct MTX44 : public _MTX44
{
public:
    typedef MTX44 self_type;
    typedef f32   value_type;
    typedef const f32 (*ConstMtx44Ptr)[4];
public:
    MTX44() {}
    MTX44(const f32* p);
    MTX44(const MTX44& rhs);
    MTX44(const MTX34& rhs);
    MTX44(const Mtx44& rhs);
    MTX44(f32 x00, f32 x01, f32 x02, f32 x03,
          f32 x10, f32 x11, f32 x12, f32 x13,
          f32 x20, f32 x21, f32 x22, f32 x23,
          f32 x30, f32 x31, f32 x32, f32 x33)
    {
        _00 = x00; _01 = x01; _02 = x02; _03 = x03;
        _10 = x10; _11 = x11; _12 = x12; _13 = x13;
        _20 = x20; _21 = x21; _22 = x22; _23 = x23;
        _30 = x30; _31 = x31; _32 = x32; _33 = x33;
    }

    //
    operator f32*() { return &_00; }
    operator const f32*() const { return &_00; }
#if 1
    operator Mtx44Ptr() { return (Mtx44Ptr)&_00; }
    operator ConstMtx44Ptr() const { return (ConstMtx44Ptr)&_00; }
#endif

#if 0
    // Shouldn't be a problem to not implement this for now.
    self_type& operator+=(const self_type& rhs);
    self_type& operator-=(const self_type& rhs);
    self_type& operator*=(const self_type& rhs);
    
    self_type& operator*=(f32 f);
    self_type& operator/=(f32 f);

    self_type operator+() const { return *this; }
    self_type operator-() const;

    //
    self_type operator+(const self_type& rhs) const;
    self_type operator-(const self_type& rhs) const;
    self_type operator*(const self_type& rhs) const;

    self_type operator*(f32 f) const;
    self_type operator/(f32 f) const;
#endif

    bool operator==(const self_type& rhs) const { return memcmp(this, &rhs, sizeof(MTX44)) == 0; }
    bool operator!=(const self_type& rhs) const { return memcmp(this, &rhs, sizeof(MTX44)) != 0; }

    void Report(bool bNewline = true, const char* name = NULL) const;
};



}}  // nw4r::math
using namespace nw4r::math;
#include "../Kamek/include/gameX.h"
#include "../Kamek/include/sfx.h"

enum Direction {
	RIGHT = 0,
	LEFT = 1,
	UP = 2,
	DOWN = 3,
};

float cM_atan2s(float one, float two);
float sqrtf(float value);
float ConvertXPosForWrapping(float value);
float NormalizeVec2(VEC2 *vec);

void sub_80060250(int playerID);
void AddScore(int amount, int playerID);
void sub_80094370(VEC3 *v);
void sub_80094390(VEC3 *v);
bool CheckShit(dStageActor_c *player, int flagnum);
void *sub_800D99F0(void *pc2dc);
u32 BasicColliderListProcessor1(void *pc2dc, u32 something, u32 anotherThing, u32 lastThing);
bool sub_80056900(dStageActor_c *player);
bool sub_80076630(u32 unk, float x, float y, u8 layer);
bool sub_800D9B20(void *pc2dc, u32 things, u32 yetAnotherVar, u32 what);

VEC3 ConvertStagePositionToScreenPosition(const VEC3 &source);

int GetSoundSourceIDForPlayer(int playerID);

class SoundPlayer {
	public:
		void playSoundAtPosition(
				SFX soundID, const VEC3 &pos, int sourceID);
};
extern SoundPlayer *SoundPlayer2;

class dBlockMgr_c {
	public:
		static dBlockMgr_c *instance;

		struct for_coinColl {
			int x, y;
			u8 unk1, unk2;
		};
		void/*?*/ coinCollected(for_coinColl *info);

		struct for_883E0 {
			u32 _0, _4, _8;
			u8 _C, _D, _E, _F;
		};
		bool/*?*/ s_800883E0(for_883E0 *info);

		struct for_88970 {
			u32 _0, _4, _8, _C;
			u8 _10, _11, _12;
		};
		void/*?*/ s_80088970(for_88970 *info);

		void/*?*/ donutLiftTouched(VEC2 *pos);
};

class dBgGm_cX {
	public:
		static dBgGm_cX *instance;

		void/*?*/ addTimedSomething(int unk, int x, int y, u8 layer);
		u32 getTileBehaviour1At(int x, int y, u8 layer);
		u32 getTileBehaviour2At(int x, int y, u8 layer);
		float s_800789A0();
		u8 crap[0x8FE70];
		float float_8FE70;
		u8 crap2[0x2C];
		float float_8FEA0;
		u8 crap3[0x20];
		float liquidHeightMaybe;
		float float_8FEC8;
		float wavyLavaHeights[80];
		float zoomDivisor;
		float float_90010, float_90014;
		u8 crap4[0x14];
		u8 liquidTypeMaybe;
};

class dScStage_cX {
	public:
		u8 crap[0x120C];
		u8 curWorld;
		u8 curLevel;
		u8 curArea;
		u8 curZone;
		u8 curEntrance;
		static dScStage_cX *instance;
};

class ClassCFC {
	public:
		static ClassCFC *instance;
		bool findExitAtPosition(int curArea, int *pOutEntranceID, float x, float y);
};

class CWCI {
	public:
		static CWCI *instance;
};

float Similar_to_cLib_chaseF(float *pValue, float one, float two);

inline float abs(float value) {
	return __fabs(value);
}
inline double abs(double value) {
	return __fabs(value);
}
extern "C" int abs(int value);

float FastS16toFloat(s16 value) {
	register s16 *pValue = &value;
	register float output;
	asm { psq_l output, 0(pValue), 1, 3 }
	return output;
}

namespace nw4r { namespace math {
	struct SEGMENT3 {
		VEC3 one, two;

		SEGMENT3(const VEC3 &one, const VEC3 &two) {
			this->one = one;
			this->two = two;
		}
	};

	float DistSqSegment3ToSegment3(const SEGMENT3 *one, const SEGMENT3 *two, float *pFloatOne, float *pFloatTwo);

	float FrSqrt(float v);
	s16 Atan2Idx(float one, float two);
} }

#endif /* NP_H */
