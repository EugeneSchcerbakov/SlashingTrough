//-----------------------------------------------------------------------------
// File: platform_cocos.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_PLATFORM_COCOS
#define MAGIC_PARTICLES_PLATFORM_COCOS

#include "platform_win_posix.h"

#ifdef __ANDROID__
struct AAssetDir;
#endif

class MP_Platform_COCOS : public MP_Platform_WIN_POSIX
{
	#ifdef __ANDROID__
	AAssetDir* asset_dir;
	std::string ptc_path_asset;
	#endif
public:
	MP_Platform_COCOS();
	virtual ~MP_Platform_COCOS();

	// eng: Returns path to folder with emitters
	// rus: Возвращает путь к папке с ptc-файлами
	virtual const char* GetPathToPTC();

	// eng: Returns path to folder which could be used by wrapper to store temporary files. If NULL is returned the temporary files are saved in RAM.
	// rus: Возвращает путь к временной папке. Если вернуть NULL, то временные файлы будут сохраняться в ОЗУ.
	virtual const char* GetPathToTemp();

	#ifdef __ANDROID__
	// eng: Finds the first ptc-file in emitters folder
	// rus: Поиск первого ptc-файла в папке с эмиттерами
	virtual const char* GetFirstFile();

	// eng: Finds the next ptc-file in emitters folder
	// rus: Поиск очередного ptc-файла в папке с эмиттерами
	virtual const char* GetNextFile();
	#endif

	// eng: Loading of file
	// rus: Чтение файла
	virtual bool LoadFile(const char* file, MP_FILE_DATA* file_data);

	// Saving of file
	// Сохранение файла
	virtual bool SaveFile(const char* file, MP_FILE_DATA* file_data);
};

#endif