/**
 *  @file   GlobalHeader.h
 *  @brief  共通して使用する変数など.
 */

#ifndef _GLOBALHEADER_H
#define _GLOBALHEADER_H

#include "sxsdk.cxx"

/**
 * プラグインインターフェイス派生クラスのプラグインID.
 */
// DDS Importer.
#define DDS_IMPORTER_PLUGIN_ID  sx::uuid_class("0B419C51-4F9B-4E3C-AD0D-4C5534AA94B3")

// DDS Exporter.
#define DDS_EXPORTER_PLUGIN_ID  sx::uuid_class("08606CA7-C3D1-43CE-9A3B-936EABAAEC1A")

#endif
