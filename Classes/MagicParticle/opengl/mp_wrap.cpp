//-----------------------------------------------------------------------------
// File: mp_wrap.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "mp_wrap.h"

#ifdef COCOS_2DX
	using namespace cocos2d;
#else
	#ifdef _WINDOWS
	#pragma comment(lib, "opengl32.lib")
	#endif
#endif

// eng: loading from memory
// rus: загрузка из памяти
extern unsigned char* LoadTextureFromMemory(const unsigned char* buffer, int len, int* width, int* height);

// eng: loading from file
// rus: загрузка из файла
extern unsigned char* LoadTextureFromFile(MP_Manager* MP, const char* file_name, int* width, int* height);

// deleting
// удаление
extern void FreeTexture(unsigned char* pixels);

#ifdef _WINDOWS
#pragma warning ( disable : 4996)
#endif



#ifdef SHADER_WRAP
// attribute index
enum ATTRIBUTES_INDEX {
	ATTRIB_VERTEX,
	ATTRIB_COLOR,
	ATTRIB_TEXTUREPOSITION,
	NUM_ATTRIBUTES
};

#endif

#ifdef OPENGL_WIN

PFNGLACTIVETEXTUREARBPROC glActiveTexture = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = NULL;
PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribiv = NULL;

#ifdef SHADER_WRAP
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;

PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;

PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;

PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocation = NULL;

PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLUNIFORM2FARBPROC glUniform2f = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv=NULL;

#endif

#endif

int multitextures=0;
STAGE stages[16];

int InitGL_multitexture(void)
{
	//const GLubyte* version=glGetString(GL_VERSION);
	//const GLubyte* ext=glGetString(GL_EXTENSIONS);
	int units;

	#ifdef SHADER_WRAP
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &units);
	#else
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &units);
	#endif

	#ifdef OPENGL_WIN

	glActiveTexture = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTexture");
	glClientActiveTexture= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTexture");

	glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)wglGetProcAddress("glGetVertexAttribPointerv");
	glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVARBPROC)wglGetProcAddress("glGetVertexAttribiv");
	
	#ifdef SHADER_WRAP
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");

	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");

	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");

	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");

	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glUniform2f = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2f");

	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	#endif

	#endif

	return units;
}

#ifdef SHADER_WRAP
void ShaderErrorDetect(GLuint shader, MP_ELEMENT_SHADER* s, MP_Device* device)
{
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		const char* path_to_temp=device->MP->GetPathToTemp();
		std::string error_path;
		if (path_to_temp)
			error_path+=path_to_temp;
		error_path+="bad_shader.txt";
		s->Save(device->MP, error_path.c_str());

		MP_FILE_DATA fd;
		if (device->MP->LoadFile(error_path.c_str(), &fd))
		{
			MP_FILE_DATA fd2;
			fd2.Create(fd.length+length);
			memcpy(fd2.data, fd.data, fd.length);
			GLchar* buffer=(GLchar*)&(fd2.data[fd.length]);
			glGetShaderInfoLog(shader, length, NULL, buffer);
			printf("%s", buffer);
			device->MP->SaveFile(error_path.c_str(), &fd2);
		}

		exit(1);
	}
}
#endif

// eng: Class storing the textural atlas
// rus: Класс, который хранит текстурный атлас
MP_Atlas_WRAP::MP_Atlas_WRAP(int width, int height, const char* file) : MP_Atlas(width, height, file)
{
	address_u=address_v=-1;

	int prev_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_texture);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    #ifndef SHADER_WRAP
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    #endif

	if (file_name.empty())
	{
		int size=width*height*4;
		unsigned char* null_data=new unsigned char[size];

		for (int i=0;i<size;i++)
			null_data[i]=0;

		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, null_data);

		delete []null_data;
		null_data=NULL;
	}
	else
	{
		int wi, he;
		unsigned char* pixels=LoadTextureFromFile(MP, file_name.c_str(), &wi, &he);
		if (pixels)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			FreeTexture(pixels);
		}
	}
	glBindTexture(GL_TEXTURE_2D, prev_texture);
}

// eng: Destroy atlas texture
// rus: Уничтожить текстуру атласа
void MP_Atlas_WRAP::Destroy()
{
	if (texture)
	{
		glDeleteTextures(1, &texture);
		texture=0;
	}
}

// eng: Loading of frame texture
// rus: Загрузка текстурного кадра
void MP_Atlas_WRAP::LoadTexture(const MAGIC_CHANGE_ATLAS* c)
{
	address_u=address_v=-1;

	int prev_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_texture);

	unsigned char* pixels=NULL;
	int wi_frame, he_frame;
	if (c->data)
	{
		// eng: loading from memory
		// rus: загрузка из памяти
		pixels=LoadTextureFromMemory((const unsigned char*)c->data, c->length, &wi_frame, &he_frame);
		if (!pixels)
		{
            #ifdef OPENGL_WIN
			const char* error="ERROR of TEXTURE LOADING";
			MessageBox(NULL, error, "Magic Particles", MB_OK | MB_ICONERROR);
			#endif
		}
	}
	else
	{
		// eng: loading from file
		// rus: загрузка из файла
		pixels=LoadTextureFromFile(MP, c->file, &wi_frame, &he_frame);
	}

	bool is_scaled=(wi_frame!=c->width || he_frame!=c->height);
	if (is_scaled)
	{
		// масштабируем текстуру
		int size=c->width*c->height*4;
		unsigned int* to=new unsigned int[size];
		unsigned int* from=(unsigned int*)pixels;

		float scale_x=((float)c->width)/wi_frame;
		float scale_y=((float)c->height)/he_frame;

		int pitch_to=c->width;
		int pitch_from=wi_frame;
		
		for (int i=0;i<c->width;i++)
		{
			for (int j=0;j<c->height;j++)
			{
				int i2=(int)(((float)i)/scale_x);
				int j2=(int)(((float)j)/scale_y);

				unsigned int color=from[j2*pitch_from+i2];
				to[j*pitch_to+i]=color;
			}
		}

		FreeTexture(pixels);
		pixels=(unsigned char*)to;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	GLint unpack_alignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D,0,c->x,c->y,c->width,c->height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
	glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);

	if (is_scaled)
		delete []pixels;
	else
		FreeTexture(pixels);

	glBindTexture(GL_TEXTURE_2D, prev_texture);
}

// eng: Cleaning up of rectangle
// rus: Очистка прямоугольника
void MP_Atlas_WRAP::CleanRectangle(const MAGIC_CHANGE_ATLAS* c)
{
	address_u=address_v=-1;

	int prev_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	int size=c->width*c->height*4;
	unsigned char* null_data=new unsigned char[size];
	for (int i=0;i<size;i++)
		null_data[i]=0;

	GLint unpack_alignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D,0,c->x,c->y,c->width,c->height,GL_RGBA,GL_UNSIGNED_BYTE,null_data);
	glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);

	delete []null_data;
	null_data=NULL;

	glBindTexture(GL_TEXTURE_2D, prev_texture);
}

// --------------------------------------------------------------------------------

GLint tex_stage[]={GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8, GL_TEXTURE9, GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14, GL_TEXTURE15, GL_TEXTURE16, GL_TEXTURE17, GL_TEXTURE18, GL_TEXTURE19, GL_TEXTURE20, GL_TEXTURE21, GL_TEXTURE22, GL_TEXTURE23, GL_TEXTURE24, GL_TEXTURE25, GL_TEXTURE26, GL_TEXTURE27, GL_TEXTURE28, GL_TEXTURE29, GL_TEXTURE30, GL_TEXTURE31};

MP_VERTEX_BUFFER_WRAP::MP_VERTEX_BUFFER_WRAP() : MP_BUFFER_RAM()
{
}

void MP_VERTEX_BUFFER_WRAP::Set(MAGIC_VERTEX_FORMAT* format, int stride, MP_Device* device)
{
	int n=0;

	#ifdef SHADER_WRAP
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	#ifdef MAGIC_3D
	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, stride, buffer);
	n+=12;
	#else
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, stride, buffer);
	n+=8;
	#endif

	if (format->attributes & MAGIC_ATTRIBUTE_COLOR)
	{
		glEnableVertexAttribArray(ATTRIB_COLOR);
		glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, &(buffer[n]));
		n+=4;
	}

	for (int i=0;i<format->UVs;i++)
	{
		glEnableVertexAttribArray(ATTRIB_TEXTUREPOSITION+i);
		glVertexAttribPointer(ATTRIB_TEXTUREPOSITION+i, 2, GL_FLOAT, GL_FALSE, stride,  &(buffer[n]));
		n+=8;
	}
	#else
	glEnableClientState(GL_VERTEX_ARRAY);
	#ifdef MAGIC_3D
	glVertexPointer(3, GL_FLOAT, stride, buffer);
	n+=12;
	#else
	glVertexPointer(2, GL_FLOAT, stride, buffer);
	n+=8;
	#endif

	if (format->attributes & MAGIC_ATTRIBUTE_COLOR)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, stride, &(buffer[n]));
		n+=4;
	}

	for (int i=0;i<format->UVs;i++)
	{
		glClientActiveTexture(tex_stage[i]);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, stride, &(buffer[n]));
		n+=8;
	}
	#endif
}

// --------------------------------------------------------------------------------

#ifdef SHADER_WRAP

OPENGL_SHADER::OPENGL_SHADER()
{
	shader=0;
}

OPENGL_SHADER::~OPENGL_SHADER()
{
	if (shader)
	{
		glDeleteShader(shader);
		shader=0;
	}
}

MP_VERTEX_SHADER_WRAP::MP_VERTEX_SHADER_WRAP() : MP_VERTEX_SHADER(), OPENGL_SHADER()
{
}

void MP_VERTEX_SHADER_WRAP::Generate(MAGIC_MATERIAL* m, MP_Device* device)
{
	MP_VERTEX_SHADER::Generate(m, device);

	int i;
	MAGIC_VERTEX_FORMAT* format=&(m->format);
	bool is_color=(format->attributes & MAGIC_ATTRIBUTE_COLOR);
	int UVs=format->UVs;

	shader=glCreateShader(GL_VERTEX_SHADER);

	#ifdef MAGIC_3D
	shader_code+="attribute vec3 position;\r\n";
	#else
	shader_code+="attribute vec2 position;\r\n";
	#endif
	if (is_color)
		shader_code+="attribute vec4 color;\r\n";
	for (i=0;i<UVs;i++)
	{
		shader_code+="attribute vec2 inputTextureCoordinate";
		shader_code+=i+'0';
		shader_code+=";\r\n";
	}
	#ifdef MAGIC_3D
	shader_code+="uniform mat4 matrix;\r\n";
	#else
	shader_code+="uniform vec2 screenScale;\r\n";
	#endif
	if (is_color)
		shader_code+="varying vec4 colorVarying;\r\n";
	for (i=0;i<UVs;i++)
	{
		shader_code+="varying vec2 textureCoordinate";
		shader_code+=i+'0';
		shader_code+=";\r\n";
	}
	shader_code+="void main()\r\n";
	shader_code+="{\r\n";
	#ifdef MAGIC_3D
	shader_code+="gl_Position = matrix * vec4(position, 1.0);\r\n";
	#else
	shader_code+="gl_Position=vec4((position*screenScale-1.0), 0.0, 1.0);\r\n";
	#endif
	if (is_color)
		shader_code+="colorVarying = color;\r\n";
	for (i=0;i<UVs;i++)
	{
		shader_code+="textureCoordinate";
		shader_code+=i+'0';
		shader_code+=" = inputTextureCoordinate";
		shader_code+=i+'0';
		shader_code+=";\r\n";
	}
	shader_code+="}\r\n";

	const char* code=shader_code.c_str();
	const GLchar** code_ptr=&code;

	glShaderSource(shader, 1, code_ptr, 0);
	glCompileShader(shader);

	ShaderErrorDetect(shader, this, device);
}


MP_PIXEL_SHADER_WRAP::MP_PIXEL_SHADER_WRAP() : MP_PIXEL_SHADER(), OPENGL_SHADER()
{
}

void MP_PIXEL_SHADER_WRAP::Generate(MAGIC_MATERIAL* m, MP_Device* device)
{
	MP_PIXEL_SHADER::Generate(m, device);

	int i;

	int texs=m->textures;

	bool is_color=(m->format.attributes & MAGIC_ATTRIBUTE_COLOR);

	shader=glCreateShader(GL_FRAGMENT_SHADER);

	shader_code+="#ifdef GL_ES\r\n";
	shader_code+="// define default precision for float, vec, mat.\r\n";
	shader_code+="precision lowp float;\r\n";
	shader_code+="#endif\r\n";
	if (is_color)
		shader_code+="varying vec4 colorVarying;\r\n";
	shader_code+="#ifdef GL_ES\r\n";
	for (i=0;i<texs;i++)
	{
		shader_code+="varying lowp vec2 textureCoordinate";
		shader_code+=i+'0';
		shader_code+=";\r\n";
	}
	shader_code+="#else\r\n";
	for (i=0;i<texs;i++)
	{
		shader_code+="varying vec2 textureCoordinate";
		shader_code+=i+'0';
		shader_code+=";\r\n";
	}
	shader_code+="#endif\r\n";
	for (i=0;i<texs;i++)
	{
		shader_code+="uniform sampler2D texture";
		shader_code+=i+'0';
		shader_code+=";\r\n";
	}

	shader_code+="void main()\r\n";
	shader_code+="{\r\n";

	if (texs)
	{
		shader_code+="vec4 color;\r\n";
		shader_code+="vec4 arg1;\r\n";
		shader_code+="vec4 arg2;\r\n";
		shader_code+="vec4 colorTex;\r\n";
		for (i=0;i<texs;i++)
		{
			shader_code+="\r\n";
			MAGIC_TEXTURE_STATES* s=&(m->states[i]);

			if (s->argument_rgb1==MAGIC_TEXARG_TEXTURE || s->argument_alpha1==MAGIC_TEXARG_TEXTURE || (s->operation_rgb!=MAGIC_TEXOP_ARGUMENT1 && s->argument_rgb2==MAGIC_TEXARG_TEXTURE) || (s->operation_alpha!=MAGIC_TEXOP_ARGUMENT1 && s->argument_alpha2==MAGIC_TEXARG_TEXTURE))
			{
				shader_code+="colorTex = texture2D(texture";
				shader_code+=i+'0';
				shader_code+=", textureCoordinate";
				shader_code+=i+'0';
				shader_code+=");\r\n";
			}

			if (s->argument_rgb1==s->argument_alpha1)
			{
				switch (s->argument_rgb1)
				{
				case MAGIC_TEXARG_CURRENT:
					shader_code+="arg1 = color;\r\n";
					break;
				case MAGIC_TEXARG_DIFFUSE:
					shader_code+="arg1 = colorVarying;\r\n";
					break;
				default:
					shader_code+="arg1 = colorTex;\r\n";
					break;
				}
			}
			else
			{
				switch (s->argument_rgb1)
				{
				case MAGIC_TEXARG_CURRENT:
					shader_code+="arg1.xyz = color.xyz;\r\n";
					break;
				case MAGIC_TEXARG_DIFFUSE:
					shader_code+="arg1.xyz = colorVarying.xyz;\r\n";
					break;
				default:
					shader_code+="arg1.xyz = colorTex.xyz;\r\n";
					break;
				}
				switch (s->argument_alpha1)
				{
				case MAGIC_TEXARG_CURRENT:
					shader_code+="arg1.w = color.w;\r\n";
					break;
				case MAGIC_TEXARG_DIFFUSE:
					shader_code+="arg1.w = colorVarying.w;\r\n";
					break;
				default:
					shader_code+="arg1.w = colorTex.w;\r\n";
					break;
				}
			}

			if (s->argument_rgb2==s->argument_alpha2 && s->operation_rgb!=MAGIC_TEXOP_ARGUMENT1 && s->operation_alpha!=MAGIC_TEXOP_ARGUMENT1)
			{
				switch (s->argument_rgb2)
				{
				case MAGIC_TEXARG_CURRENT:
					shader_code+="arg2 = color;\r\n";
					break;
				case MAGIC_TEXARG_DIFFUSE:
					shader_code+="arg2 = colorVarying;\r\n";
					break;
				default:
					shader_code+="arg2 = colorTex;\r\n";
					break;
				}
			}
			else
			{
				if (s->operation_rgb!=MAGIC_TEXOP_ARGUMENT1)
				{
					switch (s->argument_rgb2)
					{
					case MAGIC_TEXARG_CURRENT:
						shader_code+="arg2.xyz = color.xyz;\r\n";
						break;
					case MAGIC_TEXARG_DIFFUSE:
						shader_code+="arg2.xyz = colorVarying.xyz;\r\n";
						break;
					default:
						shader_code+="arg2.xyz = colorTex.xyz;\r\n";
						break;
					}
				}
				if (s->operation_alpha!=MAGIC_TEXOP_ARGUMENT1)
				{
					switch (s->argument_alpha2)
					{
					case MAGIC_TEXARG_CURRENT:
						shader_code+="arg2.w = color.w;\r\n";
						break;
					case MAGIC_TEXARG_DIFFUSE:
						shader_code+="arg2.w = colorVarying.w;\r\n";
						break;
					default:
						shader_code+="arg2.w = colorTex.w;\r\n";
						break;
					}
				}
			}

			if (s->operation_rgb==s->operation_alpha)
			{
				switch (s->operation_rgb)
				{
				case MAGIC_TEXOP_ARGUMENT1:
					shader_code+="color = arg1;\r\n";
					break;
				case MAGIC_TEXOP_ADD:
					shader_code+="color = arg1 + arg2;\r\n";
					break;
				case MAGIC_TEXOP_SUBTRACT:
					shader_code+="color = arg1 - arg2;\r\n";
					break;
				case MAGIC_TEXOP_MODULATE:
					shader_code+="color = arg1 * arg2;\r\n";
					break;
				case MAGIC_TEXOP_MODULATE2X:
					shader_code+="color = arg1 * arg2;\r\n";
					shader_code+="color = color + color;\r\n";
					break;
				default:
					shader_code+="color = arg1 * arg2;\r\n";
					shader_code+="color = color * 4.0;\r\n";
					break;
				}
			}
			else
			{
				switch (s->operation_rgb)
				{
				case MAGIC_TEXOP_ARGUMENT1:
					shader_code+="color.xyz = arg1.xyz;\r\n";
					break;
				case MAGIC_TEXOP_ADD:
					shader_code+="color.xyz = arg1.xyz + arg2.xyz;\r\n";
					break;
				case MAGIC_TEXOP_SUBTRACT:
					shader_code+="color.xyz = arg1.xyz - arg2.xyz;\r\n";
					break;
				case MAGIC_TEXOP_MODULATE:
					shader_code+="color.xyz = arg1.xyz * arg2.xyz;\r\n";
					break;
				case MAGIC_TEXOP_MODULATE2X:
					shader_code+="color.xyz = arg1.xyz * arg2.xyz;\r\n";
					shader_code+="color.xyz = color.xyz + color.xyz;\r\n";
					break;
				default:
					shader_code+="color.xyz = arg1.xyz * arg2.xyz;\r\n";
					shader_code+="color.xyz = color.xyz * 4.0;\r\n";
					break;
				}

				switch (s->operation_alpha)
				{
				case MAGIC_TEXOP_ARGUMENT1:
					shader_code+="color.w = arg1.w;\r\n";
					break;
				case MAGIC_TEXOP_ADD:
					shader_code+="color.w = arg1.w + arg2.w;\r\n";
					break;
				case MAGIC_TEXOP_SUBTRACT:
					shader_code+="color.w = arg1.w - arg2.w;\r\n";
					break;
				case MAGIC_TEXOP_MODULATE:
					shader_code+="color.w = arg1.w * arg2.w;\r\n";
					break;
				case MAGIC_TEXOP_MODULATE2X:
					shader_code+="color.w = arg1.w * arg2.w;\r\n";
					shader_code+="color.w = color.w + color.w;\r\n";
					break;
				default:
					shader_code+="color.w = arg1.w * arg2.w;\r\n";
					shader_code+="color.w = color.w * 4.0;\r\n";
					break;
				}
			}
		}
		shader_code+="gl_FragColor = color;\r\n";
	}
	else
		shader_code+="gl_FragColor = colorVarying;\r\n";

	//shader_code+="gl_FragColor = vec4(frac(1.0+frac(textureCoordinate1)), 0.0 ,1.0);\r\n";
	//shader_code+="gl_FragColor = vec4(frac(1.0+frac(textureCoordinate0)), 0.0 ,1.0);\r\n";

	//shader_code+="gl_FragColor = texture2D(texture1, textureCoordinate0);\r\n";
	//shader_code+="gl_FragColor = texture2D(texture0, textureCoordinate1);\r\n";

	//shader_code+="gl_FragColor = vec4(1.0, 1.0, 1.0 ,1.0);\r\n";

	#ifdef SHADER_ALPHATEST_WRAP
	if (m->flags & MAGIC_MATERIAL_ALPHATEST)
	{
		// учитываем альфа-тест
		shader_code+="#ifndef GL_ES\r\n";
		//shader_code+="clip(gl_FragColor.w-1.0/255.0);\r\n";
		//shader_code+="#else\r\n";
		shader_code+="if (gl_FragColor.w==0.0)\r\n";
		shader_code+="discard;\r\n";
		shader_code+="#endif\r\n";
	}
	#endif

	shader_code+="}\r\n";

	const char* code=shader_code.c_str();
	const GLchar** code_ptr=&code;

	glShaderSource(shader, 1, code_ptr, 0);
	glCompileShader(shader);

	ShaderErrorDetect(shader, this, device);
}

MP_TECHNIQUE_WRAP::MP_TECHNIQUE_WRAP() : MP_TECHNIQUE()
{
	technique=0;
	#ifdef MAGIC_3D
	uniform_matrix=0;
	#else
	uniform_screenscale=0;
	#endif
	uniform_sampler=NULL;
}

MP_TECHNIQUE_WRAP::~MP_TECHNIQUE_WRAP()
{
	if (uniform_sampler)
	{
		delete []uniform_sampler;
		uniform_sampler=NULL;
	}

	if (technique)
	{
		glDeleteProgram(technique);
		technique=0;
	}
}

void MP_TECHNIQUE_WRAP::Generate(MAGIC_MATERIAL* m, MP_Device* device)
{
	MP_TECHNIQUE::Generate(m, device);
	int i;

	technique=glCreateProgram();

	MP_VERTEX_SHADER_WRAP* vs=(MP_VERTEX_SHADER_WRAP*)device->GetVShaderList()->Get(vertex_shader_index);
	GLuint vertex_shader=vs->shader;
	MP_PIXEL_SHADER_WRAP* ps=(MP_PIXEL_SHADER_WRAP*)device->GetPShaderList()->Get(pixel_shader_index);
	GLuint pixel_shader=ps->shader;

	glAttachShader(technique, vertex_shader);
	glAttachShader(technique, pixel_shader);

	glBindAttribLocation(technique, ATTRIB_VERTEX, "position");
	if (m->format.attributes & MAGIC_ATTRIBUTE_COLOR)
		glBindAttribLocation(technique, ATTRIB_COLOR, "color");
	int textures=m->format.UVs;
	for (i=0;i<textures;i++)
	{
		std::string texattr="inputTextureCoordinate";
		texattr+=i+'0';
		glBindAttribLocation(technique, ATTRIB_TEXTUREPOSITION+i, texattr.c_str());
	}

	glLinkProgram(technique);

	#ifdef MAGIC_3D
	uniform_matrix=glGetUniformLocation(technique, "matrix");
	#else
	uniform_screenscale=glGetUniformLocation(technique, "screenScale");
	#endif

	if (textures)
	{
		uniform_sampler=new GLuint[textures];
		for (i=0;i<textures;i++)
		{
			std::string texsampler="texture";
			texsampler+=i+'0';
			uniform_sampler[i]=glGetUniformLocation(technique, texsampler.c_str());
		}
	}
}

#endif

// --------------------------------------------------------------------------------

#ifdef OPENGL_SAVE_ATTRIBUTES

void SAVED_ATTRIBUTES_TEX::Save(int index)
{
	glActiveTexture(tex_stage[index]);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &address_u);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &address_v);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &mag_filter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);
	
	#ifndef SHADER_WRAP
	enabled=glIsEnabled(GL_TEXTURE_2D);

	glGetTexEnviv(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, &operation_rgb);
	glGetTexEnvfv(GL_TEXTURE_ENV, GL_RGB_SCALE, &operation_rgb_scale);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, &argument1_rgb);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, &argument2_rgb);

	glGetTexEnviv(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, &operation_alpha);
	glGetTexEnvfv(GL_TEXTURE_ENV, GL_ALPHA_SCALE, &operation_alpha_scale);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, &argument1_alpha);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, &argument2_alpha);

	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &env_mode);
	#endif
}

void SAVED_ATTRIBUTES_TEX::Restore(int index)
{
	glActiveTexture(tex_stage[index]);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, address_u);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, address_v);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	
	#ifndef SHADER_WRAP
	if (enabled)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, operation_rgb);
	glTexEnvfv(GL_TEXTURE_ENV, GL_RGB_SCALE, &operation_rgb_scale);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, argument1_rgb);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, argument2_rgb);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, operation_alpha);
	glTexEnvfv(GL_TEXTURE_ENV, GL_ALPHA_SCALE, &operation_alpha_scale);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, argument1_alpha);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, argument2_alpha);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, env_mode);
	
	#endif
}

// -----------------------------------

void SAVED_BUFFER_ATTRIBUTES::Save(GLuint index)
{
	this->index=index;
	glGetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);
	glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
	glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
	glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
	glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);
	glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
}

void SAVED_BUFFER_ATTRIBUTES::Restore()
{
	if (enabled)
		glEnableVertexAttribArray(index);
	else
		glDisableVertexAttribArray(index);

	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

// -----------------------------------

void SAVED_ATTRIBUTES::Save(MAGIC_VERTEX_FORMAT* format, int textures)
{
	#ifndef COCOS_2DX
	glGetIntegerv(GL_BLEND, &attr_GL_BLEND);

	glGetIntegerv(GL_BLEND_SRC_ALPHA, &attr_GL_BLEND_SRC_ALPHA);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &attr_GL_BLEND_DST_ALPHA);
	#endif

	glGetBooleanv(GL_DEPTH_WRITEMASK, &attr_GL_DEPTH_WRITEMASK);
	glGetBooleanv(GL_DEPTH_TEST, &attr_GL_DEPTH_TEST);

	#ifndef SHADER_ALPHATEST_WRAP
	glGetBooleanv(GL_ALPHA_TEST, &attr_GL_ALPHA_TEST);
	glGetIntegerv(GL_ALPHA_TEST_FUNC, &attr_glAlphaFunc);
	glGetFloatv(GL_ALPHA_TEST_REF, &attr_glAlphaFunc_param);
	#endif

	glGetIntegerv(GL_ACTIVE_TEXTURE, &attr_GL_ACTIVE_TEXTURE);

	k_buffer=1;
	if (format->attributes & MAGIC_ATTRIBUTE_COLOR)
		k_buffer++;
	k_buffer+=format->UVs;

	int i;
	#ifndef SHADER_WRAP
	m_buffer_enabled[0]=glIsEnabled(GL_VERTEX_ARRAY);
	m_buffer_enabled[1]=glIsEnabled(GL_COLOR_ARRAY);
	for (i=0;i<format->UVs;i++)
	{
		glActiveTexture(tex_stage[i]);
		m_buffer_enabled[i+2]=glIsEnabled(GL_TEXTURE_COORD_ARRAY);
	}
	#else
	for (i=0;i<k_buffer;i++)
	{
		m_buffer[i].Save(i);
		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &(m_buffer_enabled[i]));
	}
	#endif

	k_tex=textures;
	for (i=0;i<k_tex;i++)
		m_tex[i].Save(i);

	#ifdef SHADER_WRAP
	glGetIntegerv(GL_CURRENT_PROGRAM, &attr_program);
	#else
	glGetBooleanv(GL_TEXTURE_2D, &attr_GL_TEXTURE_2D);
	glGetBooleanv(GL_COLOR_MATERIAL, &attr_GL_COLOR_MATERIAL);
	glGetBooleanv(GL_LIGHTING, &attr_GL_LIGHTING);
	#endif
}

void SAVED_ATTRIBUTES::Restore()
{
	#ifndef COCOS_2DX
	if (attr_GL_BLEND)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glBlendFunc(attr_GL_BLEND_SRC_ALPHA, attr_GL_BLEND_DST_ALPHA);
	#endif

	if (attr_GL_DEPTH_WRITEMASK)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);

	if (attr_GL_DEPTH_TEST)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	#ifndef SHADER_ALPHATEST_WRAP
	if (attr_GL_ALPHA_TEST)
		glEnable(GL_ALPHA_TEST);
	else
		glDisable(GL_ALPHA_TEST);
	glAlphaFunc(attr_glAlphaFunc, attr_glAlphaFunc_param);
	#endif

	int i;

	#ifndef SHADER_WRAP

	if (m_buffer_enabled[0])
		glEnableClientState(GL_VERTEX_ARRAY);
	else
		glDisableClientState(GL_VERTEX_ARRAY);

	if (m_buffer_enabled[1])
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	else
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	for (i=2;i<k_buffer;i++)
	{
		glActiveTexture(tex_stage[i-2]);
		if (m_buffer_enabled[i])
			glEnableClientState(GL_COLOR_ARRAY);
		else
			glDisableClientState(GL_COLOR_ARRAY);
	}
	#else
	for (i=0;i<k_buffer;i++)
	{
		m_buffer[i].Restore();
		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &(m_buffer_enabled[i]));
	}
	#endif

	for (i=0;i<k_tex;i++)
		m_tex[i].Restore(i);

	glActiveTexture(attr_GL_ACTIVE_TEXTURE);

	#ifdef SHADER_WRAP
	glUseProgram(attr_program);
	#else
	if (attr_GL_TEXTURE_2D)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	if (attr_GL_COLOR_MATERIAL)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);

	if (attr_GL_LIGHTING)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
	#endif
}

#endif

// --------------------------------------------------------------------------------

// eng: Class controlling drawing
// rus: Класс, который управляет рисованием
#ifndef OPENGL_WIN
MP_Device_WRAP::MP_Device_WRAP(int width, int height) : MP_Device(width, height)
#else
MP_Device_WRAP::MP_Device_WRAP(int width, int height, HWND hwnd, int bits) : MP_Device(width, height)
#endif
{
	#ifdef OPENGL_WIN
	this->hwnd=hwnd;
	hDC=NULL;
	hRC=NULL;
	this->bits=bits;
    #endif

	view.MatrixIdentity();
	projection=view;
	
	max_texture_units=0;
}

bool MP_Device_WRAP::Create()
{
	MP_Device::Create();

	#ifdef OPENGL_WIN
	GLuint		PixelFormat;

	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cRedBits = 0;
	pfd.cRedShift = 0;
	pfd.cGreenBits = 0;
	pfd.cGreenShift = 0;
	pfd.cBlueBits = 0;
	pfd.cBlueShift = 0;
	pfd.cAlphaBits = 0;
	pfd.cAlphaShift = 0;
	pfd.cAccumBits = 0;
	pfd.cAccumRedBits = 0;
	pfd.cAccumGreenBits = 0;
	pfd.cAccumBlueBits = 0;
	pfd.cAccumAlphaBits = 0;
	pfd.cDepthBits = 0;
	pfd.cStencilBits = 0;
	pfd.cAuxBuffers = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.bReserved = 0;
	pfd.dwLayerMask = 0;
	pfd.dwVisibleMask = 0;
	pfd.dwDamageMask = 0;

	#ifdef MAGIC_3D
	if (Magic_IsZBufferEnabled())
		pfd.cDepthBits=32;
	#endif

	
	if (!(hDC=GetDC(hwnd)))							// eng: Did We Get A Device Context?
	{
		Destroy();
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// eng: Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// eng: Did Windows Find A Matching Pixel Format?
	{
		Destroy();
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// eng: eturn FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// eng: Are We Able To Set The Pixel Format?
	{
		Destroy();
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// eng: Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// eng: Are We Able To Get A Rendering Context?
	{
		Destroy();
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// eng: Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// eng: Try To Activate The Rendering Context
	{
		Destroy();
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// eng: Return FALSE
	}

	RECT client_rect;
	GetClientRect(hwnd,&client_rect);
	window_width=client_rect.right;
	window_height=client_rect.bottom;
	#endif

	max_texture_units=InitGL_multitexture();

	return true;
}

// eng: Destroying
// rus: Уничтожение
void MP_Device_WRAP::Destroy()
{
	MP_Device::Destroy();

	#ifdef OPENGL_WIN
	if (hRC)											// eng: Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// eng: Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// eng: Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// eng: Set RC To NULL
	}

	if (hDC && !ReleaseDC(hwnd,hDC))					// eng: Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// eng: Set DC To NULL
	}
	#endif
}

// eng: Setting of scene
// rus: Настройка сцены
void MP_Device_WRAP::SetScene3d()
{
    #ifdef OPENGL_WIN
	// eng: viewport
	// rus: вьюпорт
	MP_VIEWPORT view_port;
	view_port.x=0;
	view_port.y=0;
	view_port.width=(unsigned int)window_width;
	view_port.height=(unsigned int)window_height;
	view_port.min_z=0.f;
	view_port.max_z=1.f;
	SetViewport(&view_port);
    #endif

	// eng: projection matrix
	// rus: проектная матрица
	MP_MATRIX proj;
	#ifdef MAGIC_3D
	proj.MatrixPerspective_OpenGL(27, ((float)window_width)/((float)window_height), 0.25f, 1000000.f);
	#else
	proj.MatrixOrtho_OpenGL(0.f, (float)window_width, 0.f, (float)window_height, -1.f, 1.f);
	#endif
	SetProjectionMatrix(&proj);

	// eng: view matrix
	// rus: матрица вида
	MP_MATRIX view;

    #ifdef MAGIC_3D

	float dist=-(DIST_CAMERA_3D);
	MP_POSITION eye(0.f,300.f,dist);
	MP_POSITION at(0.f,0.f,0.f);
	MP_POSITION up(0.f,1.f,0.f);

	view.MatrixLookAt_OpenGL(&eye, & at, &up);

	MAGIC_CAMERA camera;
	camera.mode=MAGIC_CAMERA_PERSPECTIVE;
	camera.pos.x=eye.x;
	camera.pos.y=eye.y;
	camera.pos.z=eye.z;
	camera.dir.x=at.x-camera.pos.x;
	camera.dir.y=at.y-camera.pos.y;
	camera.dir.z=at.z-camera.pos.z;
	Magic_SetCamera(&camera);

	#else

	view.MatrixIdentity();
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);

	#endif

	glDisable(GL_CULL_FACE);

	SetViewMatrix(&view);
}

// eng: Beginning of scene drawing
// rus: Начало отрисовки сцены
void MP_Device_WRAP::BeginScene()
{
	MP_Device::BeginScene();

	#ifndef SHADER_WRAP
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	#endif

	#ifdef OPENGL_WIN
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	int mode_clear=GL_COLOR_BUFFER_BIT;
	#ifdef MAGIC_3D
	if (Magic_IsZBufferEnabled())
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);
		glClearDepth(1.f);
		mode_clear=mode_clear | GL_DEPTH_BUFFER_BIT;
	}
	#endif

	glClear(mode_clear);			// eng: Clear Screen
	#endif
}

// eng: End of scene drawing
// rus: Конец отрисовки сцены
void MP_Device_WRAP::EndScene()
{
	#ifdef OPENGL_WIN
	SwapBuffers(hDC);						// eng: Swap Buffers (Double Buffering)
	#endif
}

// eng: Beginning of emitter drawing
// rus: Начало отрисовки эмиттера
void MP_Device_WRAP::BeginDrawEmitter(MP_Emitter* emitter)
{
	for (int i=0;i<16;i++)
	{
		STAGE* s=&(stages[i]);
		s->texture=-1;
		s->address_u=-1;
		s->address_v=-1;
	}
}

// eng: Indicates that device is lost
// rus: Проверка на потерю устройства рисования
bool MP_Device_WRAP::IsLost()
{
	return false;
}

void MP_Device_WRAP::SetViewMatrix(MP_MATRIX* m)
{
	view=*m;

	#ifndef SHADER_WRAP
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((const float*)m);
	#endif
}

void MP_Device_WRAP::SetProjectionMatrix(MP_MATRIX* m)
{
	projection=*m;

    #ifndef SHADER_WRAP
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((const float*)&projection);
	#endif
}

void MP_Device_WRAP::SetViewport(MP_VIEWPORT* viewport)
{
	glViewport(viewport->x,viewport->y,viewport->width,viewport->height);
}


#ifdef OPENGL_SAVE_ATTRIBUTES

void MP_Device_WRAP::SaveAttributes(MAGIC_RENDERING_START* start)
{
	saved_attributes.Save(&(start->format), start->textures);
}

void MP_Device_WRAP::RestoreAttributes(MAGIC_RENDERING_START* start)
{
	saved_attributes.Restore();
}

#endif

// -------------------------------------------------------------------------------------------

void MP_Device_WRAP::SetVertexFormat(MAGIC_RENDERING_START* start)
{
	for (int i=0;i<start->textures;i++)
	{
		glActiveTexture(tex_stage[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		#ifndef SHADER_WRAP
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		#endif
	}

	#ifdef SHADER_WRAP
	glDisableVertexAttribArray(ATTRIB_VERTEX);
	glDisableVertexAttribArray(ATTRIB_COLOR);
	#else
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	#endif

	multitextures=0;
}

// eng: Creating of vertex buffer
// rus: Создание вершинного буфера
MP_BUFFER* MP_Device_WRAP::NewVertexBuffer()
{
	return new MP_VERTEX_BUFFER_WRAP();
}

#ifdef SHADER_WRAP

// eng: Creating of vertex shader
// rus: Создание вершинного шейдера
MP_ELEMENT* MP_Device_WRAP::NewVertexShader()
{
	return new MP_VERTEX_SHADER_WRAP;
}

// eng: Creating of pixel shader
// rus: Создание пиксельного шейдера
MP_ELEMENT* MP_Device_WRAP::NewPixelShader()
{
	return new MP_PIXEL_SHADER_WRAP;
}

// eng: Creating of technique
// rus: Создание техники
MP_ELEMENT* MP_Device_WRAP::NewTechnique()
{
	return new MP_TECHNIQUE_WRAP;
}

#endif

// -------------------------------------------------------------------------------------------

// Обычное смешивание
void SetBlendingNormal()
{
	#ifdef COCOS_2DX
	GL::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	#else
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	#endif
}

// Аддитивное смешивание
void SetBlendingAdd()
{
	#ifdef COCOS_2DX
	GL::blendFunc(GL_SRC_ALPHA, GL_ONE);
	#else
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	#endif
}

// Непрозрачность
void SetBlendingOpacity()
{
	#ifdef COCOS_2DX
	GL::blendFunc(GL_ONE, GL_ZERO);
	#else
	glDisable(GL_BLEND);
	#endif
}

// массив с функциями, которые служат для установки блендинга
void (*render_blending[MAGIC_BLENDING_SHADER])()={SetBlendingNormal, SetBlendingAdd, SetBlendingOpacity};

// Установка текущего смешивания
void SetRenderBlending(MAGIC_RENDER_STATE* s)
{
	render_blending[s->value]();
}

// Установка количества используемых текстур
void SetRenderTextureCount(MAGIC_RENDER_STATE* s)
{
	if (multitextures!=s->value)
	{
		#ifndef SHADER_WRAP
		if (multitextures<s->value)
		{
			// необходимо увеличить количество текстур
			for (int i=multitextures;i<s->value;i++)
			{
				glActiveTexture(tex_stage[i]);
				glEnable(GL_TEXTURE_2D);
			}
		}
		else
		{
			// необходимо сократить количество текстур
			for (int i=s->value;i<=multitextures;i++)
			{
				glActiveTexture(tex_stage[i]);
				glDisable(GL_TEXTURE_2D);
			}
		}
		#endif
		multitextures=s->value;
	}
}

#ifdef GL_CLAMP_TO_BORDER
GLuint ogl_wrap[]={GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER};
#else
GLuint ogl_wrap[]={GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};
#endif

// Установка текущей текстуры
void SetRenderTexture(MAGIC_RENDER_STATE* s)
{
	MP_Atlas_WRAP* atlas=(MP_Atlas_WRAP*)MP_Manager::GetInstance().GetAtlas(s->value);

	GLuint tex=atlas->GetTexture();

	STAGE* stage=&(stages[s->index]);
	stage->texture=s->value;

	glActiveTexture(tex_stage[s->index]);
	glBindTexture(GL_TEXTURE_2D, tex);

	if (atlas->address_u!=stage->address_u)
	{
		atlas->address_u=stage->address_u;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ogl_wrap[stage->address_u]);
	}

	if (atlas->address_v!=stage->address_v)
	{
		atlas->address_v=stage->address_v;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ogl_wrap[stage->address_v]);
	}
}

// ----------------------------------------------------------------

// Установка текущей адресации по U
void SetRenderAddressU(MAGIC_RENDER_STATE* s)
{
	STAGE* stage=&(stages[s->index]);
	if (stage->texture!=-1)
	{
		MP_Atlas_WRAP* atlas=(MP_Atlas_WRAP*)MP_Manager::GetInstance().GetAtlas(stage->texture);
		if (atlas->address_u!=s->value)
		{
			atlas->address_u=s->value;
			GLuint tex=atlas->GetTexture();
			glActiveTexture(tex_stage[s->index]);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ogl_wrap[s->value]);
		}
	}
	stage->address_u=s->value;
}

// Установка текущей адресации по V
void SetRenderAddressV(MAGIC_RENDER_STATE* s)
{
	STAGE* stage=&(stages[s->index]);
	if (stage->texture!=-1)
	{
		MP_Atlas_WRAP* atlas=(MP_Atlas_WRAP*)MP_Manager::GetInstance().GetAtlas(stage->texture);
		if (atlas->address_v!=s->value)
		{
			atlas->address_v=s->value;
			GLuint tex=atlas->GetTexture();
			glActiveTexture(tex_stage[s->index]);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ogl_wrap[s->value]);
		}
	}
	stage->address_v=s->value;
}

// ----------------------------------------------------------------

#ifndef SHADER_WRAP
GLuint ogl_combine[]={GL_REPLACE, GL_ADD, GL_SUBTRACT, GL_MODULATE, GL_MODULATE, GL_MODULATE};
GLfloat ogl_scale[]={1.f, 1.f, 1.f, 1.f, 2.f, 4.f};

// Установка текстурной операции по RGB
void SetRenderOperationRGB(MAGIC_RENDER_STATE* s)
{
	glActiveTexture(tex_stage[s->index]);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, ogl_combine[s->value]);
	glTexEnvfv(GL_TEXTURE_ENV, GL_RGB_SCALE, &(ogl_scale[s->value]));
}

// ----------------------------------------------------------------

GLuint ogl_arg[]={GL_PREVIOUS, GL_PRIMARY_COLOR, GL_TEXTURE};

// Установка аргумента 1 для текстурной операции по RGB
void SetRenderArgument1RGB(MAGIC_RENDER_STATE* s)
{
	glActiveTexture(tex_stage[s->index]);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  ogl_arg[s->value]);
}

// ----------------------------------------------------------------

// Установка аргумента 2 для текстурной операции по RGB
void SetRenderArgument2RGB(MAGIC_RENDER_STATE* s)
{
	glActiveTexture(tex_stage[s->index]);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,  ogl_arg[s->value]);
}

// ----------------------------------------------------------------

// Установка текстурной операции по A
void SetRenderOperationA(MAGIC_RENDER_STATE* s)
{
	glActiveTexture(tex_stage[s->index]);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, ogl_combine[s->value]);
	glTexEnvfv(GL_TEXTURE_ENV, GL_ALPHA_SCALE, &(ogl_scale[s->value]));
}

// ----------------------------------------------------------------

// Установка аргумента 1 для текстурной операции по A
void SetRenderArgument1A(MAGIC_RENDER_STATE* s)
{
	glActiveTexture(tex_stage[s->index]);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB,  ogl_arg[s->value]);
}

// ----------------------------------------------------------------

// Установка аргумента 2 для текстурной операции по A
void SetRenderArgument2A(MAGIC_RENDER_STATE* s)
{
	glActiveTexture(tex_stage[s->index]);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB,  ogl_arg[s->value]);
}

#endif

// ----------------------------------------------------------------

// Включение/отключение проверки на Z-буфера
void SetRenderZEnable(MAGIC_RENDER_STATE* s)
{
	if (s->value)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

// ----------------------------------------------------------------

// Настройка Z-буфера на перезапись
void SetRenderZWrite(MAGIC_RENDER_STATE* s)
{
	GLboolean zwrtie_enabled=(GLboolean)s->value;
	glDepthMask(zwrtie_enabled);
}

// ----------------------------------------------------------------

#ifndef SHADER_ALPHATEST_WRAP
// Инициализация альфа-теста
void SetRenderAlphaTestInit(MAGIC_RENDER_STATE* s)
{
	glAlphaFunc(GL_GREATER, 0.f);
}
#endif

// ----------------------------------------------------------------

#ifndef SHADER_ALPHATEST_WRAP
// Включение/отключение альфа-теста
void SetRenderAlphaTest(MAGIC_RENDER_STATE* s)
{
	if (s->value)
	{
		glEnable(GL_ALPHA_TEST);
	}
	else
	{
		glDisable(GL_ALPHA_TEST);
	}
}
#endif

// ----------------------------------------------------------------

// массив с функциями, которые служат для установки состояния видеокарты
#ifdef SHADER_WRAP
	#ifdef SHADER_ALPHATEST_WRAP
		void (*render_states[])(MAGIC_RENDER_STATE* s)={SetRenderBlending, SetRenderTextureCount, SetRenderTexture, SetRenderAddressU, SetRenderAddressV, NULL, NULL, NULL, NULL, NULL, NULL, SetRenderZEnable, SetRenderZWrite, NULL, NULL};
	#else
		void (*render_states[])(MAGIC_RENDER_STATE* s)={SetRenderBlending, SetRenderTextureCount, SetRenderTexture, SetRenderAddressU, SetRenderAddressV, NULL, NULL, NULL, NULL, NULL, NULL, SetRenderZEnable, SetRenderZWrite, SetRenderAlphaTestInit, SetRenderAlphaTest};
	#endif
#else
	void (*render_states[])(MAGIC_RENDER_STATE* s)={SetRenderBlending, SetRenderTextureCount, SetRenderTexture, SetRenderAddressU, SetRenderAddressV, SetRenderOperationRGB, SetRenderArgument1RGB, SetRenderArgument2RGB, SetRenderOperationA, SetRenderArgument1A, SetRenderArgument2A, SetRenderZEnable, SetRenderZWrite, SetRenderAlphaTestInit, SetRenderAlphaTest};
#endif

void MP_Device_WRAP::SetRenderState(MAGIC_RENDER_STATE* state)
{
	render_states[state->state](state);
}

#ifdef SHADER_WRAP
void MP_Device_WRAP::SetRenderMaterial(int material)
{
	MP_MATERIAL* m=m_material[material];
	MP_TECHNIQUE_WRAP* tech=(MP_TECHNIQUE_WRAP*)technique_list.Get(m->technique_index);

	glUseProgram(tech->technique);
	#ifdef MAGIC_3D
	MP_MATRIX matrix;
	matrix.MatrixMul(&view, &projection);
	glUniformMatrix4fv(tech->uniform_matrix, 1, false, (float*)&matrix);
	#else
	float screenscale_x=2.f/(float)window_width;
	float screenscale_y=2.f/(float)window_height;
	glUniform2f(tech->uniform_screenscale, screenscale_x, screenscale_y);

	#endif

	int textures=m->material.textures;
	for (int i=0;i<textures;i++)
		glUniform1i(tech->uniform_sampler[i], i);
}
#endif

void MP_Device_WRAP::DrawVertices(int starting_index, int indexes_count, int max_vertices)
{
	MP_BUFFER_RAM* index_buffer_ram=(MP_BUFFER_RAM*)index_buffer;

	#ifdef INDEX_BUFFER_32_WRAP
	glDrawElements(GL_TRIANGLES, indexes_count, GL_UNSIGNED_INT, &(index_buffer_ram->buffer[starting_index*4]));
	#else
	glDrawElements(GL_TRIANGLES, indexes_count, GL_UNSIGNED_SHORT, &(index_buffer_ram->buffer[starting_index*2]));
	#endif
}

