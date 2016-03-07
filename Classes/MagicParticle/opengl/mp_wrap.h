//-----------------------------------------------------------------------------
// File: mp_wrap.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_OPENGL
#define MAGIC_PARTICLES_OPENGL

#include <stdlib.h>
#include "mp.h"

#ifdef COCOS_2DX
	#define OPENGL_SAVE_ATTRIBUTES
	#define DIST_CAMERA_3D  -2520
	#include "cocos2d.h"
	#include "platform/CCPlatformConfig.h"
	#include "platform/CCGL.h"
	
	#if (CC_TARGET_PLATFORM==CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
		#define internalformat GL_RGBA
	#else
		#define internalformat GL_RGBA8
	#endif

	#if (CC_TARGET_PLATFORM!=CC_PLATFORM_WIN32)
		#define LOADER_LIBPNGJPG
	#endif
#else
	#ifdef __S3E__
		#include <IwGL.h>
		#define DIST_CAMERA_3D  -840
		#define internalformat GL_RGBA
		#define LOADER_LIBPNGJPG

	#else

	#ifdef _WINDOWS
		#include <windows.h>
		#include <gl/gl.h>
		#include "gl/glext.h"
		#include "gl/wglext.h"
		#ifdef SHADER_WRAP
		#include "gl/glcorearb.h"
		#endif
		#define internalformat GL_RGBA8
		#define DIST_CAMERA_3D  -840
	#endif

	#ifdef __APPLE__
		#include "TargetConditionals.h"
		#if (TARGET_OS_IPHONE==1) || (TARGET_IPHONE_SIMULATOR==1)
			#include <stdlib.h>
			#ifdef SHADER_WRAP
				#import <OpenGLES/ES2/gl.h>
				#import <OpenGLES/ES2/glext.h>
			#else
				#import <OpenGLES/ES1/gl.h>
				#import <OpenGLES/ES1/glext.h>
			#endif
			#define internalformat GL_RGBA
			#define DIST_CAMERA_3D  -2520
		#else
			#if (TARGET_OS_MAC==1)
				#include <GLUT/glut.h>
				#define internalformat GL_RGBA8
				#define DIST_CAMERA_3D  -840
			#endif
		#endif
	#endif

	#ifdef __ANDROID__
		#include <jni.h>
		#include <android/asset_manager.h>
		#include <android/log.h>

		#define  LOG_TAG    "libwpapper_android"
		#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
		#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

		#include <GLES2/gl2.h>
		#include <GLES2/gl2ext.h>

		#define internalformat GL_RGBA
		#define DIST_CAMERA_3D  -2520
	#endif

	#endif

#endif

#if !defined(LOADER_OPENIL) && !defined(LOADER_LIBPNGJPG)
#define LOADER_STDIMAGE
#endif

#if !defined(GL_ALPHA_TEST) && !defined(SHADER_ALPHATEST_WRAP)
#define SHADER_ALPHATEST_WRAP
#endif

//#ifdef __ANDROID__
//#include <android/log.h>
//#define  LOG_TAG    "magic"
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
//#endif

// eng: Class storing the textural atlas
// rus: Класс, который хранит текстурный атлас
class MP_Atlas_WRAP : public MP_Atlas
{
protected:
	GLuint texture;

public:
	int address_u, address_v;

public:
	MP_Atlas_WRAP(int width, int height, const char* file);

	GLuint GetTexture(){return texture;}

	// eng: Destroy atlas texture
	// rus: Уничтожить текстуру атласа
	virtual void Destroy();

	// eng: Loading of frame texture
	// rus: Загрузка текстурного кадра
	virtual void LoadTexture(const MAGIC_CHANGE_ATLAS* c);

	// eng: Cleaning up of rectangle
	// rus: Очистка прямоугольника
	virtual void CleanRectangle(const MAGIC_CHANGE_ATLAS* c);
};

struct MP_VERTEX_BUFFER_WRAP : public MP_BUFFER_RAM
{
	MP_VERTEX_BUFFER_WRAP();

	virtual void Set(MAGIC_VERTEX_FORMAT* format, int stride, MP_Device* device);
};

#ifdef SHADER_WRAP

struct OPENGL_SHADER
{
	GLuint shader;
	
	OPENGL_SHADER();
	virtual ~OPENGL_SHADER();
};

struct MP_VERTEX_SHADER_WRAP : public MP_VERTEX_SHADER, public OPENGL_SHADER
{
	MP_VERTEX_SHADER_WRAP();
	
	virtual void Generate(MAGIC_MATERIAL* m, MP_Device* device);
};

struct MP_PIXEL_SHADER_WRAP : public MP_PIXEL_SHADER, public OPENGL_SHADER
{
	MP_PIXEL_SHADER_WRAP();
	
	virtual void Generate(MAGIC_MATERIAL* m, MP_Device* device);
};

struct MP_TECHNIQUE_WRAP : public MP_TECHNIQUE
{
	GLuint technique;

	#ifdef MAGIC_3D
	GLuint uniform_matrix;
	#else
	GLuint uniform_screenscale;
	#endif
	GLuint* uniform_sampler;

	MP_TECHNIQUE_WRAP();
	virtual ~MP_TECHNIQUE_WRAP();

	virtual void Generate(MAGIC_MATERIAL* m, MP_Device* device);
};

#endif


#ifdef OPENGL_SAVE_ATTRIBUTES

struct SAVED_ATTRIBUTES_TEX
{
	int texture;

	GLint address_u;
	GLint address_v;
	GLint mag_filter;
	GLint min_filter;

	#ifndef SHADER_WRAP
	GLboolean enabled;

	GLint operation_rgb;
	GLfloat operation_rgb_scale;
	GLint argument1_rgb;
	GLint argument2_rgb;

	GLint operation_alpha;
	GLfloat operation_alpha_scale;
	GLint argument1_alpha;
	GLint argument2_alpha;

	GLint env_mode;
	#endif

	void Save(int index);
	void Restore(int index);
};


struct SAVED_BUFFER_ATTRIBUTES
{
	int index;

	void* pointer;
	GLint enabled;
	GLint size;
	GLint type;
	GLint normalized;
	GLint stride;
	
	void Save(GLuint index);
	void Restore();
};


struct SAVED_ATTRIBUTES
{
	GLint attr_GL_BLEND;
	GLint attr_GL_BLEND_SRC_ALPHA;
	GLint attr_GL_BLEND_DST_ALPHA;

	GLboolean attr_GL_DEPTH_WRITEMASK;
	GLboolean attr_GL_DEPTH_TEST;

	#ifndef SHADER_ALPHATEST_WRAP
	GLboolean attr_GL_ALPHA_TEST;
	GLint attr_glAlphaFunc;
	GLfloat attr_glAlphaFunc_param;
	#endif

	GLint attr_GL_ACTIVE_TEXTURE;

	int k_tex;
	SAVED_ATTRIBUTES_TEX m_tex[32];

	int k_buffer;
	#ifdef SHADER_WRAP
	GLint m_buffer_enabled[32];
	SAVED_BUFFER_ATTRIBUTES m_buffer[32];
	#else
	GLboolean m_buffer_enabled[32];
	#endif

	#ifdef SHADER_WRAP
	GLint attr_program;
	#else
	GLboolean attr_GL_TEXTURE_2D;
	GLboolean attr_GL_COLOR_MATERIAL;
	GLboolean attr_GL_LIGHTING;
	#endif

	void Save(MAGIC_VERTEX_FORMAT* format, int textures);
	void Restore();
};

#endif

struct STAGE
{
	int texture;
	int address_u, address_v;
};

// eng: Class controlling drawing
// rus: Класс, который управляет рисованием
class MP_Device_WRAP : public MP_Device
{
protected:
	#ifdef OPENGL_WIN
	HWND hwnd;
	HDC	hDC;		// eng: Private GDI Device Context
	HGLRC hRC;		// eng: Permanent Rendering Context
	int bits;
	#endif

	MP_MATRIX view, projection;

	#ifdef OPENGL_SAVE_ATTRIBUTES
	SAVED_ATTRIBUTES saved_attributes;
	#endif

	int max_texture_units;

public:

    #ifdef OPENGL_WIN
	MP_Device_WRAP(int width, int height, HWND hwnd, int bits);
	#else
    MP_Device_WRAP(int width, int height);
    #endif
    
	// eng: Creating
	// rus: Создание
	virtual bool Create();

	// eng: Destroying
	// rus: Уничтожение
	virtual void Destroy();

	// eng: Setting of scene
	// rus: Настройка сцены
	virtual void SetScene3d();

	// eng: Beginning of scene drawing
	// rus: Начало отрисовки сцены
	virtual void BeginScene();

	// eng: End of scene drawing
	// rus: Конец отрисовки сцены
	virtual void EndScene();

	// eng: Beginning of emitter drawing
	// rus: Начало отрисовки эмиттера
	virtual void BeginDrawEmitter(MP_Emitter* emitter);

	// eng: End of emitter drawing
	// rus: Конец отрисовки эмиттера
	virtual void EndDrawEmitter(MP_Emitter* emitter){}

	// eng: Indicates that device is lost
	// rus: Проверка на потерю устройства рисования
	virtual bool IsLost();

	virtual void SetWorldMatrix(MP_MATRIX* m){}
	virtual void SetViewMatrix(MP_MATRIX* m);
	virtual void SetProjectionMatrix(MP_MATRIX* m);

	virtual void SetViewport(MP_VIEWPORT* viewport);

 	#ifdef OPENGL_SAVE_ATTRIBUTES
	virtual void SaveAttributes(MAGIC_RENDERING_START* start);
	virtual void RestoreAttributes(MAGIC_RENDERING_START* start);
	#endif

	virtual void SetVertexFormat(MAGIC_RENDERING_START* start);

	virtual void SetRenderState(MAGIC_RENDER_STATE* state);
	#ifdef SHADER_WRAP
	virtual void SetRenderMaterial(int material);
	#endif
	
	virtual void DrawVertices(int starting_index, int indexes_count, int max_vertices);

	virtual MAGIC_ARGB_ENUM GetColorMode(){return MAGIC_ARGB;}


	// eng: Creating of vertex buffer
	// rus: Создание вершинного буфера
	MP_BUFFER* NewVertexBuffer();
	
	#ifdef SHADER_WRAP
	
	// eng: Creating of vertex shader
	// rus: Создание вершинного шейдера
	virtual MP_ELEMENT* NewVertexShader();

	// eng: Creating of pixel shader
	// rus: Создание пиксельного шейдера
	virtual MP_ELEMENT* NewPixelShader();

	// eng: Creating of technique
	// rus: Создание техники
	virtual MP_ELEMENT* NewTechnique();

	#endif
};

#endif