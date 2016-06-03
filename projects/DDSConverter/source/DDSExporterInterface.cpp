/**
 * @brief  DDS Exporter.
 */

#include "DDSExporterInterface.h"
#include "DDSsaver.h"

CDDSExporterInterface::CDDSExporterInterface (sxsdk::shade_interface &shade) : shade(shade)
{
}

/**
 * @brief  ファイル拡張子.
 */
const char *CDDSExporterInterface::get_file_extension (int index, void *)
{
	if (index == 0) return "dds";
	return 0;
}

/**
 * @brief ファイル詳細.
 */
const char *CDDSExporterInterface::get_file_description (int index, void*)
{
	if (index == 0) return "DirectDrawSurface";
	return 0;
}

/**
 * @brief  エクスポート処理を行う関数.
 */
bool CDDSExporterInterface::do_export (int index, sxsdk::image_interface *image, sxsdk::stream_interface *stream, void*)
{
	CDDSSaver ddsSaver(shade);
	return ddsSaver.saveDDS(stream, image);
}
