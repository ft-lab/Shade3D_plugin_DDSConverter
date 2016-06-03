/**
 * @brief  DDS Importer.
 */

#include "DDSImporterInterface.h"
#include "DDSLoader.h"

CDDSImporterInterface::CDDSImporterInterface (sxsdk::shade_interface &shade) : shade(shade)
{
}

/**
 * @brief  ファイル拡張子.
 */
const char *CDDSImporterInterface::get_file_extension (int index, void *)
{
	if (index == 0) return "dds";
	return 0;
}

/**
 * @brief ファイル詳細.
 */
const char *CDDSImporterInterface::get_file_description (int index, void*)
{
	if (index == 0) return "DirectDrawSurface";
	return 0;
}

/**
 * @brief  前処理(ダイアログを出す場合など).
 */
bool CDDSImporterInterface::do_pre_import (int index, void*)
{
	return true;
}

/**
 * @brief  インポート処理を行う関数.
 */
sxsdk::image_interface* CDDSImporterInterface::do_import (int index, sxsdk::stream_interface *stream, void *)
{
	CDDSLoader ddsLoader(shade);
	sxsdk::image_interface* image = ddsLoader.loadDDS(stream);

	return image;
}
