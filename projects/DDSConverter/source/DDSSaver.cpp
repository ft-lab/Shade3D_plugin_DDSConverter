/**
 * @brief  DDSの保存.
 */

#include "DDSSaver.h"

namespace {
	/**
	 * floatからfp16(floatの半分、16bits)の浮動小数点に変換.
	 */
	unsigned int convFloatToFP16 (const float fVal) {
		unsigned int iVal = *(unsigned int *)(&fVal);
		if (!iVal) return 0;

		unsigned int e = ((iVal & 0x7f800000) >> 23) - 127 + 15;
		if (e < 0) {
			return  0;
		}
		else if (e > 31) {
			e = 31;
		}
		unsigned int s = iVal & 0x80000000;
		unsigned int f = iVal & 0x007fffff;
		return ((s >> 16) & 0x8000) | ((e << 10) & 0x7c00) | ((f >> 13) & 0x03ff);
	}
}

CDDSSaver::CDDSSaver (sxsdk::shade_interface& shade) : shade(shade)
{
}

/**
 * DDSの保存.
 */
bool CDDSSaver::saveDDS (sxsdk::stream_interface* stream, sxsdk::image_interface* image)
{
	if (!stream) return false;

	// ヘッダ部.
	memset(&m_header, 0, sizeof(m_header));
	m_width  = image->get_size().x;
	m_height = image->get_size().y;
	m_header.dwMagic  = 0x20534444;
	m_header.dwSize   = 124;
	m_header.dwFlags  = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	m_header.dwDepth  = 1;
	m_header.dwWidth  = (unsigned int)m_width;
	m_header.dwHeight = (unsigned int)m_height;
	m_header.dwMipMapCount = 1;

	m_header.dwPfFlags |= DDPF_FOURCC;

	// 非圧縮のF16のABGRを格納とする.
	const int bit_count = 0;
    m_header.dwPitchOrLinearSize = (m_width * bit_count) >> 3;
	m_header.dwFourCC = 0x00000071;		// DDS_FORMAT_A16B16G16R16F.

	try {
		stream->set_pointer(0);
		stream->set_size(0);
		stream->write(sizeof(m_header), &m_header);

		// ビットデータを出力.
		std::vector<sxsdk::rgba_class> colLines;
		std::vector< sx::vec<unsigned short, 4> > colLines2;

		const int wWidthSize = m_width * sizeof(sx::vec<unsigned short, 4>);
		colLines.resize(m_width);
		colLines2.resize(m_width);
		for (int y = 0; y < m_height; y++) {
			image->get_pixels_rgba_float(0, y, m_width, 1, &colLines[0]);

			for (int x = 0; x < m_width; x++) {
				colLines2[x][0] = (unsigned short)::convFloatToFP16(colLines[x].red);
				colLines2[x][1] = (unsigned short)::convFloatToFP16(colLines[x].green);
				colLines2[x][2] = (unsigned short)::convFloatToFP16(colLines[x].blue);
				colLines2[x][3] = (unsigned short)::convFloatToFP16(colLines[x].alpha);
			}
			stream->write(wWidthSize, &colLines2[0]);
		}

		return true;
	}
	catch (...) {}

	return true;
}

/**
 * @brief  ビットデータを出力.
 */
bool m_saveDDSBits (sxsdk::stream_interface* stream, sxsdk::image_interface* image)
{
	return false;
}

/**
 * @brief  ビットデータを出力 (float RGBAで格納).
 */
bool m_saveDDSBitsFloat (sxsdk::stream_interface* stream, sxsdk::image_interface* image)
{
	return false;
}
