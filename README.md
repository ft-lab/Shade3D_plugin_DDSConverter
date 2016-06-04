# DDS Converter
DDS Converterプラグインは、Shade 3DでDDS形式の画像をインポート/エクスポートするプラグインです。

## 動作環境

* Windows 7/8/10以降のOS
* Mac 10.9以降のOS
* Shade 3D ver.14/15以降で、Standard/Professional版（Basic版では動作しません）

## ソースのビルド

WindowsはVisual Studio 2013を使用。  
MacはXcode 6.4を使用。  
Shade 3D Plugin SDK ver.15.1が必要です ( https://shade3d.jp/community/sdn/sdk.html )。  

## ビルド方法

Shade 3D Plugin SDK ver.15.1をダウンロード。  
Shade3D_plugin_DDSConverterでダウンロードしたソースのDDSConverterディレクトリを、Shade 3D Plugin SDKのplugin_projectsディレクトリ内に配置。  

### Windows

Visual Studio 2013で、DDSConverter/win/Template.slnを読み込みビルドします。  

### Mac

Xcode6.xで、DDSConverter/mac/plugins/DDSConverter.xcodeprojを読み込みビルドします。  

## 実行方法

### Windows

ビルドされた DDSConverter64.dll (32bits版の場合はDDSConverter.dll)をShade 3Dのpluginsディレクトリに格納してShade 3Dを起動。  
イメージの読み込みや書き出しに「DirectDrawSurface」があるのを確認します。  

### Mac

ビルドされた DDSConverter.shdpluginをShade 3Dのpluginsディレクトリに格納してShade 3Dを起動。  
イメージの読み込みや書き出しに「DirectDrawSurface」があるのを確認します。  

## 動作仕様

### DDSインポート

DDSファイルでfloat型のビット深度の場合は、Shade 3Dの64bits深度(16bits x 4)で格納されます。  
それ以外は、Shade 3Dの32bits深度(8bits x 4)で格納されます。  
DDSファイルに複数レイヤがある場合は、横に並べて読み込まれます。

### DDSエクスポート

float型のDDS_FORMAT_A16B16G16R16F (Shade 3Dでの64bits深度相当)で出力されます。  


## 更新履歴

### ver.1.0.0.0 (2016/06/04)

* 初回版
