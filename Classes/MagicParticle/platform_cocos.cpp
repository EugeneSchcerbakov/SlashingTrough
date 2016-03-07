//-----------------------------------------------------------------------------
// File: platform_cocos.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

/*CHECK_ME*/
//#include <CCFileUtils.h>
#include "cocos/platform/CCFileUtils.h"
#include "platform_cocos.h"

// -------------------------------------------------------------------------------------------

#ifdef __ANDROID__
#include <CCFileUtils-android.h>
#endif

MP_Platform_COCOS::MP_Platform_COCOS() : MP_Platform_WIN_POSIX()
{
	#ifdef __ANDROID__
	asset_dir=NULL;
	#endif
}

MP_Platform_COCOS::~MP_Platform_COCOS()
{
}

// eng: Returns path to folder with emitters
// rus: Возвращает путь к папке с ptc-файлами
const char* MP_Platform_COCOS::GetPathToPTC()
{
	if (ptc_path.empty())
	{
		std::string ptc_path_temp=cocos2d::FileUtils::getInstance()->fullPathForFilename("resources_marker.txt");
		int len=ptc_path_temp.size();
		const char* s=ptc_path_temp.c_str();
		char v;
		for (int i=len-1;i>=0;i--)
		{
			v=s[i];
			if (v=='\\' || v=='/')
			{
				for (int j=0;j<=i;j++)
					ptc_path+=ptc_path_temp[j];
				break;
			}
		}

		#ifndef __ANDROID__
		std::string ptc_path_asset;
		#endif

		#ifdef MAGIC_3D
		ptc_path_asset="particles3d";
		#else
		ptc_path_asset="particles2d";
		#endif

		ptc_path+=ptc_path_asset;
		ptc_path+=v;
	}

	return ptc_path.c_str();
}

// eng: Returns path to folder which could be used by wrapper to store temporary files. If NULL is returned the temporary files are saved in RAM.
// rus: Возвращает путь к временной папке. Если вернуть NULL, то временные файлы будут сохраняться в ОЗУ.
const char* MP_Platform_COCOS::GetPathToTemp()
{
	#ifdef _WINDOWS
	return MP_Platform_WIN_POSIX::GetPathToTemp();
	#else
	return NULL;
	#endif
}

#ifdef __ANDROID__
// eng: Finds the first ptc-file in emitters folder
// rus: Поиск первого ptc-файла в папке с эмиттерами
const char* MP_Platform_COCOS::GetFirstFile()
{
	std::string ptc_path=GetPathToPTC();

	asset_dir=AAssetManager_openDir(cocos2d::FileUtilsAndroid::getAssetManager(), ptc_path_asset.c_str());

	if (asset_dir)
	{
		const char* file=GetNextFile();
		return file;
	}

	return NULL;
}

// eng: Finds the next ptc-file in emitters folder
// rus: Поиск очередного ptc-файла в папке с эмиттерами
const char* MP_Platform_COCOS::GetNextFile()
{
	const char* result=AAssetDir_getNextFileName(asset_dir);
	while(result)
	{
		int len=strlen(result);
		if (len>4)
		{
			const char* ext=&(result[len-4]);
			if (strcmp(ext, ".ptc")==0)
				return result;
		}
		result=AAssetDir_getNextFileName(asset_dir);
	}
	AAssetDir_close(asset_dir);
	asset_dir=NULL;
	return NULL;
}
#endif

// eng: Loading of file
// rus: Чтение файла
bool MP_Platform_COCOS::LoadFile(const char* file, MP_FILE_DATA* file_data)
{
	if (cocos2d::FileUtils::getInstance()->isFileExist(file))
	{
		cocos2d::Data data=cocos2d::FileUtils::getInstance()->getDataFromFile(file);
		int len=data.getSize();
		file_data->Create(len);

		memcpy(file_data->data, data.getBytes(), len);

		return true;
	}
	return false;
}

// eng: Saving of file
// rus: Сохранение файла
bool MP_Platform_COCOS::SaveFile(const char* file, MP_FILE_DATA* file_data)
{
	#ifdef _WINDOWS
	return MP_Platform_WIN_POSIX::SaveFile(file, file_data);
	#else
	return false;
	#endif
}