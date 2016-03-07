//-----------------------------------------------------------------------------
// File: loader_image.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "mp_wrap.h"


// -------------------------------------------------------------------------------------------------------------------

#ifdef LOADER_OPENIL
#include "IL/ilu.h"
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")
bool first_loading_openil=true;
#endif

#ifdef LOADER_STDIMAGE
#include "stb_image.h"
#endif

#ifdef LOADER_LIBPNGJPG
#include "png.h"

extern "C"
{
	#include "jpeglib.h"
}

struct PNGBuffer
{
	png_bytep   data;
	png_uint_32 position;
};

void PNGRead(png_structp pngStruct, png_bytep data, png_size_t length)
{
	PNGBuffer* pngBuffer = (PNGBuffer*)(png_get_io_ptr(pngStruct));

	memcpy(data, pngBuffer->data + pngBuffer->position, length);
	pngBuffer->position += length;
}


// JPEG source manager
struct jpeg_source_t
{
	jpeg_source_mgr _pub;

	const char *_begin;
	const char *_end;
};


// Translate JPEG error to C++ exception
static void error_jpeg_exit(j_common_ptr cinfo)
{
	#ifdef ENABLE_JPG_EXCEPTION
	throw cinfo->err->msg_code;
	#else
	exit(2);
	#endif
}


// Initialize JPEG source
METHODDEF(void) init_source(j_decompress_ptr)
{
}

// Fill image buffer
METHODDEF(boolean) fill_input_buffer(j_decompress_ptr cinfo)
{
	jpeg_source_t *src=(jpeg_source_t *)cinfo->src;

	src->_pub.next_input_byte=(const JOCTET *)src->_begin;
	src->_pub.bytes_in_buffer=src->_end-src->_begin;

	return (boolean)true;
}

// Skip data in buffer
METHODDEF(void)skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	jpeg_source_t *src=(jpeg_source_t *)cinfo->src;

	src->_pub.next_input_byte+=(size_t)num_bytes;
	src->_pub.bytes_in_buffer-=(size_t)num_bytes;
}

// Close buffer
METHODDEF(void)term_source(j_decompress_ptr)
{
}

// Initialize memory source
static void jpeg_memory_src(j_decompress_ptr cinfo, const char *begin, const char *end)
{
	cinfo->src=(jpeg_source_mgr *)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(jpeg_source_t));
	jpeg_source_t *src=(jpeg_source_t *)cinfo->src;

	src->_pub.init_source=init_source;
	src->_pub.fill_input_buffer=fill_input_buffer;
	src->_pub.skip_input_data=skip_input_data;
	src->_pub.resync_to_restart=jpeg_resync_to_restart;
	src->_pub.term_source=term_source;
	src->_pub.bytes_in_buffer=0;
	src->_pub.next_input_byte=NULL;
	src->_begin=begin;
	src->_end=end;
}

// Load JPG from memory
unsigned char* LoadJPGWithAlphaFromMemory(const unsigned char* begin, int len, int* width, int* height)
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;
	
	cinfo.err=jpeg_std_error(&jerr);
	jerr.error_exit=error_jpeg_exit;

	#ifdef ENABLE_JPG_EXCEPTION
	try
	#endif
	{
		// Init decompressor
		jpeg_create_decompress(&cinfo);
		jpeg_memory_src(&cinfo, (const char*)begin, (const char*)(begin+len));
		jpeg_read_header(&cinfo,TRUE);
		jpeg_start_decompress(&cinfo);

		int wi=cinfo.image_width;
		int he=cinfo.image_height;

		int bytes_per_line=cinfo.output_width*cinfo.output_components;
		int bytes_per_line4=cinfo.output_width*4;

		unsigned char* data=new unsigned char[bytes_per_line4*he];
		unsigned char* data_temp=new unsigned char[bytes_per_line];

		//построчно читаем данные
		unsigned char* line = data;
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, &data_temp, 1);

			int temp_index=0;
			for (unsigned int i=0;i<cinfo.output_width;i++)
			{
				*line=data_temp[temp_index];
				temp_index++;
				line++;
				*line=data_temp[temp_index];
				temp_index++;
				line++;
				*line=data_temp[temp_index];
				temp_index++;
				line++;
				*line=255;
				line++;
			}

		}
		delete []data_temp;

		//подчищаем
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		*width=wi;
		*height=he;

		return data;
	}
	#ifdef ENABLE_JPG_EXCEPTION
	catch (...)
	{
		jpeg_destroy((j_common_ptr)&cinfo);
		return NULL;
	}
	#endif
}

unsigned char* LoadPNGJPGFromMemory(const unsigned char* buffer, int len, int* width, int* height)
{
	const int number=8;
	// проверяем сигнатуру файла (первые number байт)
	if ( !png_check_sig((png_bytep)buffer, number) )
	{
		// неизвестный формат
		return LoadJPGWithAlphaFromMemory(buffer, len, width, height);
	}

	// создаем внутреннюю структуру png для работы с файлом
	// последние параметры - структура, для функции обработки ошибок и варнинга (последн. 2 параметра)
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	// создаем структуру с информацией о файле
	png_infop info_ptr = png_create_info_struct(png_ptr);

	PNGBuffer pngBuffer;
	pngBuffer.data = (png_bytep)buffer;
	pngBuffer.position = 8;

	png_set_read_fn(png_ptr, (void*)&pngBuffer, PNGRead);

	// говорим библиотеке, что мы уже прочли number байт, когда проверяли сигнатуру
	png_set_sig_bytes(png_ptr, number);
	// читаем всю информацию о файле
	png_read_info(png_ptr, info_ptr);
	// Эта функция возвращает инфу из info_ptr
	png_uint_32 w = 0, h = 0;  // размер картинки в пикселях
	int bit_depth = 0;      // глубина цвета (одного из каналов, может быть 1, 2, 4, 8, 16)
	int color_type = 0;      // описывает какие каналы присутствуют:
	// PNG_COLOR_TYPE_GRAY, PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE,
	// PNG_COLOR_TYPE_RGB, PNG_COLOR_TYPE_RGB_ALPHA...
	// последние 3 параметра могут быть нулями и обозначают: тип фильтра, тип компрессии и тип смещения
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);

	// png формат может содержать 16 бит на канал, но нам нужно только 8, поэтому сужаем канал
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	// преобразуем файл если он содержит палитру в нормальный RGB
	if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8) png_set_palette_to_rgb(png_ptr);
	// если в грэйскейле меньше бит на канал чем 8, то конвертим к нормальному 8-битному
	//if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	// и добавляем полный альфа-канал
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);
    

	double gamma = 0.0f;
	// если есть информация о гамме в файле, то устанавливаем на 2.2
	if ( png_get_gAMA(png_ptr, info_ptr, &gamma) ) png_set_gamma(png_ptr, 2.2, gamma);
	// иначе ставим дефолтную гамму для файла в 0.45455 (good guess for GIF images on PCs)
	else png_set_gamma(png_ptr, 2.2, 0.45455);

	// после всех трансформаций, апдейтим информацию в библиотеке
	png_read_update_info(png_ptr, info_ptr);
	// опять получаем все размеры и параметры обновленной картинки
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);

	// определяем кол-во байт нужных для того чтобы вместить строку
	png_uint_32 row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	// теперь, мы можем выделить память чтобы вместить картинку
	png_byte* data = new png_byte[row_bytes * h];
	// выделяем память, для указателей на каждую строку
	png_byte **row_pointers = new png_byte * [h];
	// сопоставляем массив указателей на строчки, с выделенными в памяти (res)
	// т.к. изображение перевернутое, то указатели идут снизу вверх
	for (unsigned int i = 0; i < h; i++)
		row_pointers[i] = data + i * row_bytes; 
	// все, читаем картинку
	png_read_image(png_ptr, row_pointers);

	// освобождаем память от указателей на строки
	delete []row_pointers;
	// освобождаем память выделенную для библиотеки libpng
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	*width=w;
	*height=h;

	return data;
}
#endif

// eng: loading from memory
// rus: загрузка из памяти
unsigned char* LoadTextureFromMemory(const unsigned char* buffer, int len, int* width, int* height)
{
	#ifdef LOADER_OPENIL

	if (first_loading_openil)
	{
		first_loading_openil=false;
		ilInit();
	}

	ILuint image;
	ilGenImages(1,&image);
	ilBindImage(image);

	ilLoadL(IL_TYPE_UNKNOWN, buffer, len);

	ilConvertImage(IL_RGBA,IL_UNSIGNED_BYTE);

	*width=ilGetInteger(IL_IMAGE_WIDTH);
	*height=ilGetInteger(IL_IMAGE_HEIGHT);
	int len4=(*width)*(*height); 

	int* pixels=(int*)ilGetData();
	int* data=new int[len4];
	for (int i=0;i<len4;i++)
		data[i]=pixels[i];

	ilDeleteImages(1, &image);

	return (unsigned char*)data;

	#else
	
		#ifdef LOADER_STDIMAGE

			int comp;
			return stbi_load_from_memory(buffer, len, width, height, &comp, 4);

		#else

			#ifdef LOADER_LIBPNGJPG
				return LoadPNGJPGFromMemory(buffer, len, width, height);
			#endif

		#endif

	#endif
}

// eng: loading from file
// rus: загрузка из файла
unsigned char* LoadTextureFromFile(MP_Manager* MP, const char* file_name, int* width, int* height)
{
	MP_FILE_DATA fd;
	std::string texture_file=MP->GetPathToTexture();
	texture_file+=file_name;
	MP->LoadFile(texture_file.c_str(), &fd);

	unsigned char* pixels=LoadTextureFromMemory(fd.data, fd.length, width, height);
	return pixels;
}

// deleting
// удаление
void FreeTexture(unsigned char* pixels)
{
	#ifdef LOADER_STDIMAGE
	stbi_image_free(pixels);
	#else
	delete(pixels);
	#endif
}
