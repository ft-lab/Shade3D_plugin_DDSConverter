/**
 * @brief  DDS Exporter.
 */
#ifndef _DDS_EXPORTER_INTERFACE_H
#define _DDS_EXPORTER_INTERFACE_H

#include "GlobalHeader.h"

class CDDSExporterInterface : public sxsdk::image_exporter_interface {

private:
	sxsdk::shade_interface& shade;

private:
	virtual sx::uuid_class get_uuid (void *) { return DDS_EXPORTER_PLUGIN_ID; }
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	/**
	 * @brief ファイル拡張子.
	 */
	virtual const char *get_file_extension (int index, void *);

	/**
	 * @brief ファイル詳細.
	 */
	virtual const char *get_file_description (int index, void*);

	/**
	 * @brief エクスポート処理を行う関数.
	 */
	virtual bool do_export (int index, sxsdk::image_interface *image, sxsdk::stream_interface *stream, void*);

public:
	CDDSExporterInterface (sxsdk::shade_interface &shade);

	/**
	 * プラグイン名.
	 */
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("dds_exporter_title"); }

};

#endif
