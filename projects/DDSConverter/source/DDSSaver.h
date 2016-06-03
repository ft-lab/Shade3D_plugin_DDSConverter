/**
 * @brief  DDSの保存.
 */
#ifndef _DDS_SAVER_H
#define _DDS_SAVER_H

#include "GlobalHeader.h"
#include "DDSHeader.h"

class CDDSSaver
{
private:
	sxsdk::shade_interface& shade;

	CDDSHeader m_header;			// DDSヘッダ情報.

	int m_width, m_height;			// 画像の幅と高さ.
	int m_depth;					// 複数画像を持つ場合のサイズ（Volume Texture）.
	int m_formatType;				// フォーマットの種類.

private:
	/**
	 * @brief  ビットデータを出力.
	 */
	bool m_saveDDSBits (sxsdk::stream_interface* stream, sxsdk::image_interface* image);

	/**
	 * @brief  ビットデータを出力 (float RGBAで格納).
	 */
	bool m_saveDDSBitsFloat (sxsdk::stream_interface* stream, sxsdk::image_interface* image);

public:
	CDDSSaver (sxsdk::shade_interface& shade);

	/**
	 * DDSの保存.
	 */
	bool saveDDS (sxsdk::stream_interface* stream, sxsdk::image_interface* image);

};

#endif
