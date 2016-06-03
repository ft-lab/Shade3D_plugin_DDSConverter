/**
 * DDSファイルの情報.
 */
#ifndef _DDS_HEADER_H
#define _DDS_HEADER_H

#include "GlobalHeader.h"

/*
	＜＜ テクスチャの種類 ＞＞
	- 2D Texture     ... 普通のテクスチャ.
	- Cube Map       ... 6面の立方体状のマッピングを行うテクスチャ.
	- Volume Texture ... 複数のレイヤ(depth)を持つテクスチャ.

	RGB Bitmask では 32bpp までの非圧縮ピクセルの表現しかできない。圧縮ありもしくはfloat表現などを行う場合は dwFourCCを使用.

*/
 //--------------------------------------------------------------.
#define	MAKE_FOURCC( x, y, z, w )  (((w)<<24)|((z)<<16)|((y)<<8)|(x))

/**
 * @brief  DDSのフラグ情報.
 */
enum DDS_Flags {
	DDSD_CAPS        = 0x00000001,			// dwCaps/dwCpas2 が有効.
	DDSD_HEIGHT      = 0x00000002,			// dwHeight が有効.
	DDSD_WIDTH       = 0x00000004,			// dwWidth が有効.
	DDSD_PITCH       = 0x00000008,			// dwPitchOrLinearSize が Pitch を表す.
	DDSD_PIXELFORMAT = 0x00001000,			// dwPfSize/dwPfFlags/dwRGB/dwFourCC～ 等の Pixel 定義が有効。.
	DDSD_MIPMAPCOUNT = 0x00020000,			// dwMipMapCount が有効。mipmap を格納する場合は必須。.
	DDSD_LINEARSIZE  = 0x00080000,			// dwPitchOrLinearSize が LinearSize を表す.
	DDSD_DEPTH       = 0x00800000,			// dwDepth が有効。3D (Volume) Texture 時に設定.
};

/*
 *  @brief  DDS DX10での情報.
 */
enum DDS_DX10_DIMENSION
{
	DDS10_DIMENSION_1D = 2,
	DDS10_DIMENSION_2D = 3,
	DDS10_DIMENSION_3D = 4,
};

/**
 * dwPfFlagsに格納するフラグ情報.
 *  「*」が付いた項目はレガシーでDX10以降は利用しない.
 */
enum DDS_PF_Flags
{
	DDPF_ALPHAPIXELS     = 0x00000001,			// RGB 以外に alpha が含まれている。つまり dwRGBAlphaBitMask が有効。.
	DDPF_ALPHA           = 0x00000002,			// pixel は Alpha 成分のみ含まれている。.
	DDPF_FOURCC          = 0x00000004,			// dwFourCC が有効。.
	DDPF_PALETTEINDEXED4 = 0x00000008,			// * Palet 16 colors (DX9 以降はたぶん使用されない) .
	DDPF_PALETTEINDEXED8 = 0x00000020,			// * Palet 256 colors (DX10 以降は使用されない) .
	DDPF_RGB             = 0x00000040,			// dwRGBBitCount/dwRBitMask/dwGBitMask/dwBBitMask/dwRGBAlphaBitMask によってフォーマットが定義されていることを示す.
	DDPF_LUMINANCE       = 0x00020000,			// * 1ch のデータが RGB すべてに展開される.
	DDPF_BUMPDUDV        = 0x00080000,			// * pixel が符号付であることを示す (本来は bump 用) DX10以降は使用しない.
};

/**
 * dwCapsに格納する情報.
 */
enum DDS_CAPS
{
	DDSCAPS_ALPHA    = 0x00000002,				// Alpha が含まれている場合 (あまり参照されない).
	DDSCAPS_COMPLEX  = 0x00000008,				// 複数のデータが含まれている場合。Palette/Mipmap/Cubemap/VolumeTexture では On にする。 .
	DDSCAPS_TEXTURE  = 0x00001000,				// 常に On.
	DDSCAPS_MIPMAP   = 0x00400000,				// 	MipMap が存在する場合。(dwFlags の DDSD_MIPMAPCOUNT が On でかつ dwMipMapCount が 2以上の場合に On).
};

/**
* dwCaps2に格納する情報.
*/
enum DDS_CAPS2
{
	DDSCAPS2_CUBEMAP            = 0x00000200,				// CubeMapの場合.
	DDSCAPS2_CUBEMAP_POSITIVEX  = 0x00000400,				// Cubemap +X が含まれている.
	DDSCAPS2_CUBEMAP_NEGATIVEX  = 0x00000800,				// Cubemap -X が含まれている.
	DDSCAPS2_CUBEMAP_POSITIVEY  = 0x00001000,				// Cubemap +Y が含まれている.
	DDSCAPS2_CUBEMAP_NEGATIVEY  = 0x00002000,				// Cubemap -Y が含まれている.
	DDSCAPS2_CUBEMAP_POSITIVEZ  = 0x00004000,				// Cubemap +Z が含まれている.
	DDSCAPS2_CUBEMAP_NEGATIVEZ  = 0x00008000,				// Cubemap -Z が含まれている.
	DDSCAPS2_VOLUME             = 0x00400000,				// 	VolumeTexture の場合.
};

/**
 * フォーマットの種類 (dwFourCCで判断).
 */
enum DDS_FORMAT_Type
{
	// 非圧縮のフォーマット.
	DDS_FORMAT_A8R8G8B8 = 1,					// ARGB 8 x 4 bits ( 32 bits/pixel ).
	DDS_FORMAT_A8B8G8R8,						// ABGR 8 x 4 bits ( 32 bits/pixel ).
	DDS_FORMAT_X8R8G8B8,						// xRGB 8 x 4 bits (Alpha未使用) ( 32 bits/pixel ). 
	DDS_FORMAT_R8G8B8,							// RGB 8 x 3 bits ( 24 bits/pixel ).
	DDS_FORMAT_B8G8R8,							// BGR 8 x 3 bits ( 24 bits/pixel ).
	DDS_FORMAT_R5G6B5,							// RGB 16 bits ( 16 bits/pixel ).

	// 圧縮も可、ダイナミックレンジを持てるフォーマット.
	DDS_FORMAT_DX10,							// DX10.
	DDS_FORMAT_A16B16G16R16,					// RGBA 16 x 4 bits  ( 64 bits/pixel ).
	DDS_FORMAT_A16B16G16R16F,					// RGBA F16 x 4 bits ( 64 bits/pixel ).
	DDS_FORMAT_A32B32G32R32F,					// RGBA float x 4 bits ( 128 bits/pixel ).
	DDS_FORMAT_R16F,							// R F16 bits ( 16 bits/pixel ).
	DDS_FORMAT_R32F,							// R float bits ( 32 bits/pixel ).
};

//--------------------------------------------------------------.
/**
 * @brief  DDSヘッダ情報.
 */
class CDDSHeader
{
public:
	unsigned int dwMagic;				// 常に0x20534444 ' SDD'.
	unsigned int dwSize;				// 常に 124.
	unsigned int dwFlags;				// ヘッダ内の有効な情報 DDSD_* の組み合わせ.
	unsigned int dwHeight;				// 画像の高さ.
	unsigned int dwWidth;				// 画像の幅.
	unsigned int dwPitchOrLinearSize;	// 横 1ラインのbyte数.
	unsigned int dwDepth;				// 画像の奥行き z size (Volume Texture 用).
	unsigned int dwMipMapCount;			// 含まれている mipmapレベル数.
	unsigned int dwReserved1[11];
	unsigned int dwPfSize;				// 常に32.
	unsigned int dwPfFlags;				// Pixelフォーマットをあらわす DDPF_* の組み合わせ.
	unsigned int dwFourCC;				// フォーマットが FourCC で表現される場合に使用する.
										// DX10 拡張ヘッダが存在する場合は 'DX10' (0x30315844) が入る。.
	unsigned int dwRGBBitCount;			// 1 pixel のbit数.
	unsigned int dwRBitMask;			// RGB Format時のmaskR (0x00FF0000 など).
	unsigned int dwGBitMask;			// RGB Format時のmaskG (0x0000FF00 など).
	unsigned int dwBBitMask;			// RGB Format時のmaskB (0x000000FF など).
	unsigned int dwABitMask;			// RGB Format時のmaskA (0xFF000000 など).
	unsigned int dwCaps;				// mipmapなどのフラグ指定用.
	unsigned int dwCaps2;				// cube/volume textureなどのフラグ指定用.
	unsigned int dwReservedCaps[2];
	unsigned int dwReserved2;

public:
	CDDSHeader () { }
};

//--------------------------------------------------------------.
/**
 * @brief  DX10拡張ヘッダ.
 */
class CDDSHeaderDX10 {
public:
	unsigned int format;		// DXGI_FORMAT.
	unsigned int dimension;		// D3D10_RESOURCE_DIMENSION が入る。(1D=2, 2D=3, 3D=4).
	unsigned int miscFlag;		// 0.
	unsigned int arraySize;		// Texture Array を格納する場合に必要.
	unsigned int miscFlag2;		// 0.

public:
	CDDSHeaderDX10 () { }
};

#endif

