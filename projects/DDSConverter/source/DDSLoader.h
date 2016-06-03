/**
 * @brief  DDSの読み込み.
 */
#ifndef _DDSLOADER_H
#define _DDSLOADER_H

#include "DDSHeader.h"

class CDDSLoader
{
private:
	sxsdk::shade_interface& shade;

	CDDSHeader m_header;			// DDSヘッダ情報.
	CDDSHeaderDX10 m_headerDX10;	// DX10ヘッダ情報.

	int m_width, m_height;			// 画像の幅と高さ.
	int m_depth;					// 複数画像を持つ場合のサイズ（Volume Texture）.
	int m_formatType;				// フォーマットの種類.

private:
	/**
	 * @brief  ビットデータを読み込み、画像作成.
	 */
	sxsdk::image_interface* m_loadDDSBits (sxsdk::stream_interface* stream);

	/**
	 * @brief  ビットデータを読み込み、画像作成 (float RGBAで格納).
	 */
	sxsdk::image_interface* m_loadDDSBitsFloat (sxsdk::stream_interface* stream);

public:
	CDDSLoader (sxsdk::shade_interface& shade);

	/**
	 * DDSの読み込み.
	 */
	sxsdk::image_interface* loadDDS (sxsdk::stream_interface* stream);

};

#endif
