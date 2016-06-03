/**
 * @brief  DDSの読み込み.
 */

#include "DDSLoader.h"

#include <algorithm>

/*
	DDSフォーマットについて.
	参考 : 
	http://dench.flatlib.jp/ddsformat

*/

namespace {
	/**
	 * fp16(floatの半分、16bits)の浮動小数点をfloatに変換.
	 */
	float convFP16ToFloat (const unsigned int iVal) {
		if (!iVal) return 0.0f;
		unsigned int s    = iVal & 0x8000;
		unsigned int e    = ((iVal & 0x7c00) >>10) - 15 + 127;
		unsigned int f    = iVal & 0x03ff;
		unsigned int fval = (s << 16) | ((e << 23) & 0x7f800000) | (f << 13);
		return  *(float*)(&fval);
	}
}

CDDSLoader::CDDSLoader (sxsdk::shade_interface& shade) : shade(shade)
{
}

/**
 * DDSの読み込み.
 */
sxsdk::image_interface* CDDSLoader::loadDDS (sxsdk::stream_interface* stream)
{
	if (!stream) return NULL;

	try {
		stream->set_pointer(0);

		// ヘッダ部を読み込み.
		stream->read(sizeof(m_header), &m_header);

		if (m_header.dwMagic != 0x20534444 || m_header.dwSize != 124) return NULL;
		
		// 画像のサイズと、Volume Textureの場合の保持レイヤ数.
		m_width  = (int)m_header.dwWidth;
		m_height = (int)m_header.dwHeight;
		m_depth  = (m_header.dwFlags & DDSD_DEPTH) ? std::max((int)m_header.dwDepth, 1) : 1;
		int array_count = 1;
		m_formatType = 0;

		if (m_header.dwPfFlags & DDPF_FOURCC) {
			const unsigned int fourCC = m_header.dwFourCC;
			if (fourCC == MAKE_FOURCC('D', 'X', '1', '0')) {		// DX10 拡張ヘッダあり.
				// DX10拡張ヘッダ情報を読み込み.
				stream->read(sizeof(m_headerDX10), &m_headerDX10);
				array_count = std::max((int)m_headerDX10.arraySize, 1);

				m_formatType = DDS_FORMAT_DX10;

			} else {
				switch (fourCC) {
					case 0x00000024:
						m_formatType = DDS_FORMAT_A16B16G16R16;
						break;

					case 0x0000006f:
						m_formatType = DDS_FORMAT_R16F;
						break;

					case 0x00000071:
						m_formatType = DDS_FORMAT_A16B16G16R16F;
						break;

					case 0x00000072:
						m_formatType = DDS_FORMAT_R32F;
						break;

					case 0x00000074:
						m_formatType = DDS_FORMAT_A32B32G32R32F;
						break;
				}
			}

		} else if (m_header.dwPfFlags & (DDPF_RGB | DDPF_ALPHAPIXELS | DDPF_ALPHA | DDPF_BUMPDUDV | DDPF_LUMINANCE)) {
			unsigned int  bit_count = m_header.dwRGBBitCount;
			unsigned int  r_mask = m_header.dwRBitMask;
			unsigned int  g_mask = m_header.dwGBitMask;
			unsigned int  b_mask = m_header.dwBBitMask;
			unsigned int  a_mask = m_header.dwABitMask;

			if (bit_count == 32) {
				if (a_mask == 0xff000000 && r_mask == 0x00ff0000 && g_mask == 0x0000ff00 && b_mask == 0x000000ff) {
					m_formatType = DDS_FORMAT_A8R8G8B8;
				}
				else if (a_mask == 0xff000000 && r_mask == 0x000000ff && g_mask == 0x0000ff00 && b_mask == 0x00ff0000) {
					m_formatType = DDS_FORMAT_A8B8G8R8;
				}
				else if (a_mask == 0x00000000 && r_mask == 0x00ff0000 && g_mask == 0x0000ff00 && b_mask == 0x000000ff) {
					m_formatType = DDS_FORMAT_X8R8G8B8;
				}

			} else if (bit_count == 24) {
				if (a_mask == 0x00000000 && r_mask == 0x00ff0000 && g_mask == 0x0000ff00 && b_mask == 0x000000ff) {
					m_formatType = DDS_FORMAT_R8G8B8;
				} else if (a_mask == 0x00000000 && r_mask == 0x000000ff && g_mask == 0x0000ff00 && b_mask == 0x00ff0000) {
					m_formatType = DDS_FORMAT_B8G8R8;
				}

			} else if (bit_count == 16) {
				if (a_mask == 0x00000000 && r_mask == 0x0000f800 && g_mask == 0x000007e0 && b_mask == 0x0000001f) {
					m_formatType = DDS_FORMAT_R5G6B5;
				}
			}
		}
		if (m_formatType == 0) return NULL;

		// ビットデータを読み込み.
		return m_loadDDSBits(stream);

	} catch (...) {}

	return NULL;
}

/**
 * @brief  ビットデータを読み込み、画像作成.
 */
sxsdk::image_interface* CDDSLoader::m_loadDDSBits (sxsdk::stream_interface* stream)
{
	if (!stream || m_formatType == 0) return NULL;

	// float RGBAでダイナミックレンジの画像を生成.
	if (m_formatType == DDS_FORMAT_A16B16G16R16 || m_formatType == DDS_FORMAT_A16B16G16R16F || m_formatType == DDS_FORMAT_A32B32G32R32F ||
		m_formatType == DDS_FORMAT_R16F || m_formatType == DDS_FORMAT_R32F) {
		return m_loadDDSBitsFloat(stream);
	}

	try {
		compointer<sxsdk::image_interface> image(shade.get_new_image_interface(sx::vec<int,2>(m_width * m_depth, m_height)));
		if (!image) return NULL;
		image->AddRef();

		std::vector< sx::vec<unsigned  char, 4> > byteLines;
		std::vector<sx::rgba8_class> colLines;
		colLines.resize(m_width);
		byteLines.resize(m_width);

		char szStr[256];
		sprintf(szStr, "DDS pixel format = %d", m_formatType);
		shade.message(szStr);

		// m_depthが１より大きい場合は、横にレイヤの画像を並べる.
		for (int loop = 0, posX = 0; loop < m_depth; loop++, posX += m_width) {
			if (m_formatType == DDS_FORMAT_A8R8G8B8) {
				for (int y = 0; y < m_height; y++) {
					stream->read(m_width * 4, &(byteLines[0]));
					for (int x = 0; x < m_width; x++) {
						colLines[x].red = byteLines[x][2];
						colLines[x].green = byteLines[x][1];
						colLines[x].blue = byteLines[x][0];
						colLines[x].alpha = byteLines[x][3];
					}
					image->set_pixels_rgba(posX, y, m_width, 1, &(colLines[0]));
				}

			}
			else if (m_formatType == DDS_FORMAT_A8B8G8R8) {
				for (int y = 0; y < m_height; y++) {
					stream->read(m_width * 4, &(byteLines[0]));
					for (int x = 0; x < m_width; x++) {
						colLines[x].red = byteLines[x][0];
						colLines[x].green = byteLines[x][1];
						colLines[x].blue = byteLines[x][2];
						colLines[x].alpha = byteLines[x][3];
					}
					image->set_pixels_rgba(posX, y, m_width, 1, &(colLines[0]));
				}

			}
			else if (m_formatType == DDS_FORMAT_X8R8G8B8) {
				for (int y = 0; y < m_height; y++) {
					stream->read(m_width * 4, &(byteLines[0]));
					for (int x = 0; x < m_width; x++) {
						colLines[x].red = byteLines[x][2];
						colLines[x].green = byteLines[x][1];
						colLines[x].blue = byteLines[x][0];
						colLines[x].alpha = 255;
					}
					image->set_pixels_rgba(posX, y, m_width, 1, &(colLines[0]));
				}

			}
			else if (m_formatType == DDS_FORMAT_R8G8B8) {
				std::vector< sx::vec<unsigned char, 3> > byteLines2;
				byteLines2.resize(m_width);
				for (int y = 0; y < m_height; y++) {
					stream->read(m_width * 3, &(byteLines2[0]));
					for (int x = 0; x < m_width; x++) {
						colLines[x].red = byteLines2[x][2];
						colLines[x].green = byteLines2[x][1];
						colLines[x].blue = byteLines2[x][0];
						colLines[x].alpha = 255;
					}
					image->set_pixels_rgba(posX, y, m_width, 1, &(colLines[0]));
				}

			}
			else if (m_formatType == DDS_FORMAT_B8G8R8) {
				std::vector< sx::vec<unsigned char, 3> > byteLines2;
				byteLines2.resize(m_width);
				for (int y = 0; y < m_height; y++) {
					stream->read(m_width * 3, &(byteLines2[0]));
					for (int x = 0; x < m_width; x++) {
						colLines[x].red = byteLines2[x][0];
						colLines[x].green = byteLines2[x][1];
						colLines[x].blue = byteLines2[x][2];
						colLines[x].alpha = 255;
					}
					image->set_pixels_rgba(posX, y, m_width, 1, &(colLines[0]));
				}
			}
			else if (m_formatType == DDS_FORMAT_R5G6B5) {
			}
		}

		image->update();

		return image;
	} catch (...) { }

	return NULL;
}

/**
 * @brief  ビットデータを読み込み、画像作成 (float RGBAで格納).
 */
sxsdk::image_interface* CDDSLoader::m_loadDDSBitsFloat (sxsdk::stream_interface* stream)
{
	try {
		compointer<sxsdk::image_interface> image(shade.create_image_interface(sx::vec<int, 2>(m_width * m_depth, m_height), 64));
		if (!image) return NULL;
		image->AddRef();

		std::vector<sxsdk::rgba_class> colLines;
		colLines.resize(m_width);

		char szStr[256];
		sprintf(szStr, "DDS pixel format = %d", m_formatType);
		shade.message(szStr);

		// m_depthが１より大きい場合は、横にレイヤの画像を並べる.
		for (int loop = 0, posX = 0; loop < m_depth; loop++, posX += m_width) {
			if (m_formatType == DDS_FORMAT_A16B16G16R16) {
				std::vector< sx::vec<unsigned short, 4> > byteLines2;
				byteLines2.resize(m_width);
				const float divV = 1.0f / 65535.0f;
				for (int y = 0; y < m_height; y++) {
					stream->read(m_width * sizeof(sx::vec<unsigned short, 4>), &(byteLines2[0]));
					for (int x = 0; x < m_width; x++) {
						colLines[x].red = (float)byteLines2[x][0] * divV;
						colLines[x].green = (float)byteLines2[x][1] * divV;
						colLines[x].blue = (float)byteLines2[x][2] * divV;
						colLines[x].alpha = (float)byteLines2[x][3] * divV;
					}
					image->set_pixels_rgba_float(posX, y, m_width, 1, &(colLines[0]));
				}
			}
			else if (m_formatType == DDS_FORMAT_A16B16G16R16F) {
				std::vector< sx::vec<unsigned short, 4> > byteLines2;
				byteLines2.resize(m_width);
				for (int y = 0; y < m_height; y++) {
					stream->read(m_width * sizeof(sx::vec<unsigned short, 4>), &(byteLines2[0]));
					for (int x = 0; x < m_width; x++) {
						colLines[x].red = ::convFP16ToFloat(byteLines2[x][0]);
						colLines[x].green = ::convFP16ToFloat(byteLines2[x][1]);
						colLines[x].blue = ::convFP16ToFloat(byteLines2[x][2]);
						colLines[x].alpha = ::convFP16ToFloat(byteLines2[x][3]);
					}
					image->set_pixels_rgba_float(posX, y, m_width, 1, &(colLines[0]));
				}
			}
			else if (m_formatType == DDS_FORMAT_A32B32G32R32F) {
				std::vector< sx::vec<float, 4> > byteLines2;
				byteLines2.resize(m_width);
				for (int y = 0; y < m_height; y++) {
					stream->read(m_width * sizeof(sx::vec<float, 4>), &(byteLines2[0]));
					for (int x = 0; x < m_width; x++) {
						colLines[x].red = byteLines2[x][0];
						colLines[x].green = byteLines2[x][1];
						colLines[x].blue = byteLines2[x][2];
						colLines[x].alpha = byteLines2[x][3];
					}
					image->set_pixels_rgba_float(posX, y, m_width, 1, &(colLines[0]));
				}
			}
		}

		image->update();
		return image;

	} catch (...) { }

	return NULL;
}
