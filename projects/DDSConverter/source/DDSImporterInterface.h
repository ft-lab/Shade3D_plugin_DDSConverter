/**
 * @brief  DDS Importer.
 */
#ifndef _DDS_IMPORTER_INTERFACE_H
#define _DDS_IMPORTER_INTERFACE_H

#include "GlobalHeader.h"

class CDDSImporterInterface : public sxsdk::image_importer_interface {

private:
	sxsdk::shade_interface& shade;

private:
	virtual sx::uuid_class get_uuid (void *) { return DDS_IMPORTER_PLUGIN_ID; }
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
	 * @brief  前処理(ダイアログを出す場合など).
	 */
	virtual bool do_pre_import (int index, void*);

	/**
	 * @brief インポート処理を行う関数.
	 */
	virtual sxsdk::image_interface* do_import (int index, sxsdk::stream_interface *stream, void *);

public:
	CDDSImporterInterface (sxsdk::shade_interface &shade);

	/**
	 * プラグイン名.
	 */
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("dds_importer_title"); }

};

#endif
