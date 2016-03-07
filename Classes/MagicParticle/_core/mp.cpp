//-----------------------------------------------------------------------------
// File: mp.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "mp_wrap.h"

#include <math.h>
#include <errno.h>

#if defined(_WINDOWS) || defined(WINAPI_FAMILY)
#pragma warning ( disable : 4996)
#endif

#ifdef MAGIC_3D

// ru: Инверсия кватерниона
// en: Quaternion inversion
void MagicQuaternionInversion(MAGIC_DIRECTION* q)
{
	float N=(float)(sqrt(q->x*q->x+q->y*q->y+q->z*q->z+q->w*q->w));
	q->x=-q->x/N;
	q->y=-q->y/N;
	q->z=-q->z/N;
	q->w=q->w/N;
}

// ru: Перемножение кватернионов
// en: Quaternion multiplication
void MagicQuaternionMul(MAGIC_DIRECTION *res, const MAGIC_DIRECTION *q1, const MAGIC_DIRECTION *q2)
{
	float A, B, C, D, E, F, G, H;

	A = (q1->w + q1->x) * (q2->w + q2->x);
	B = (q1->z - q1->y) * (q2->y - q2->z);
	C = (q1->x - q1->w) * (q2->y + q2->z);
	D = (q1->y + q1->z) * (q2->x - q2->w);
	E = (q1->x + q1->z) * (q2->x + q2->y);
	F = (q1->x - q1->z) * (q2->x - q2->y);
	G = (q1->w + q1->y) * (q2->w - q2->z);
	H = (q1->w - q1->y) * (q2->w + q2->z);

	res->w = B + (-E - F + G + H) * 0.5f;
	res->x = A - ( E + F + G + H) * 0.5f; 
	res->y =-C + ( E - F + G - H) * 0.5f;
	res->z =-D + ( E - F - G + H) * 0.5f;
}

#endif

// ------------------------------------------------------------------------------------------

// ru: Событие "Создание частицы"
// en: Event "Creating of particle"
void EventCreation(MAGIC_EVENT* evt)
{
	// ...
	// ...
	// ...
}

// ru: Событие "Уничтожение частицы"
// en: Event "Destruction of particle"
void EventDestruction(MAGIC_EVENT* evt)
{
	// ...
	// ...
	// ...
}

// ru: Событие "Существование частицы"
// en: Event "Existence of particle"
void EventExistence(MAGIC_EVENT* evt)
{
	// ...
	// ...
	// ...
}

// ru: Событие "Столкновение с препятствием"
// en: Event "Collision of particle with an obstacle"
void EventCollision(MAGIC_EVENT* evt)
{
	// ...
	// ...
	// ...
}

// ru: Событие "Примагничивание частицы"
// en: Event "Attraction of particle to the magnet"
void EventMagnetism(MAGIC_EVENT* evt)
{
	// ...
	// ...
	// ...
}

void (*function_event[5])(MAGIC_EVENT* evt)={EventCreation, EventDestruction, EventExistence, EventCollision, EventMagnetism};

MP_FILE_DATA::MP_FILE_DATA()
{
	DetachData();
}

MP_FILE_DATA::~MP_FILE_DATA()
{
	Clear();
}

void MP_FILE_DATA::Clear()
{
	if (data)
	{
		delete []data;
		data=NULL;
	}
	length=0;
}

void MP_FILE_DATA::Create(int len)
{
	Clear();
	data=new unsigned char[len];
	length=len;
}

void MP_FILE_DATA::DetachData()
{
	data=NULL;
	length=0;
}


MP_Manager* MP_Platform::MP=NULL;

MP_Platform::MP_Platform()
{
}

MP_Platform::~MP_Platform()
{
}

// ru: Конвертация wchar_t в utf8
// en: Conversion of wchar_t in utf8
const char* MP_Platform::wchar_to_utf8(const wchar_t* str)
{
	const char* str8=NULL;

	int size=sizeof(wchar_t);
	switch (size)
	{
	case 1:
		wchar_t8=(const char*)str;
		str8=wchar_t8.c_str();
		break;

	case 2:
		str8=(const char*)Magic_UTF16to8((const unsigned short*)str);
		break;

	case 4:
		str8=(const char*)Magic_UTF32to8((const unsigned int*)str);
		break;
	}
	return str8;
}

// ru: Конвертация utf8 в wchar_t
// en: Conversion of utf8 in wchar_t
const wchar_t* MP_Platform::utf8_to_wchar(const char* str)
{
	wchar_t* strw=NULL;

	int size=sizeof(wchar_t);
	switch (size)
	{
	case 1:
		wchar_t8=str;
		strw=(wchar_t*)wchar_t8.c_str();
		break;

	case 2:
		strw=(wchar_t*)Magic_UTF8to16((const unsigned char*)str);
		break;

	case 4:
		strw=(wchar_t*)Magic_UTF8to32((const unsigned char*)str);
		break;
	}

	return strw;
}

// eng: Opens ptc-file
// rus: Открывает ptc-файл
HM_FILE MP_Platform::OpenPTC(const char* ptc_file)
{
	MP_FILE_DATA fd;
	HM_FILE hmFile=0;
	if (LoadFile(ptc_file, &fd))
	{
		HM_STREAM hmStream=Magic_StreamOpenMemory((char*)fd.data, (unsigned int)fd.length, MAGIC_STREAM_READ);
		hmFile=Magic_OpenStream(hmStream);
		if (hmFile>0)
		{
			fd.DetachData();
			MP_PTC* ptc=new MP_PTC;
			ptc->hmFile=hmFile;
			ptc->hmStream=hmStream;
			MP->AddPTC(ptc);
		}
		else
		{
			Magic_StreamClose(hmStream);
		}
	}
	return hmFile;
}

// Loading of file
// Чтение файла
bool MP_Platform::LoadFile(const char* file, MP_FILE_DATA* file_data)
{
	#ifdef _WINDOWS

	std::string mode="rb";
	const wchar_t* pmode=utf8_to_wchar(mode.c_str());
	wchar_t wmode[3];
	wmode[0]=pmode[0];
	wmode[1]=pmode[1];
	wmode[2]=0;

	const wchar_t* wfile=utf8_to_wchar(file);

	FILE* f=NULL;
	_wfopen_s(&f, wfile, wmode);

	#else

	FILE* f=fopen(file, "rb");

	#endif

	if (f)
	{
		fseek(f,0,SEEK_END);
		long len=ftell(f);
		fseek(f,0,SEEK_SET);

		file_data->Create(len);

		fread(file_data->data, 1, len, f);
		fclose(f);

		return  true;
	}

	return false;
}

// Saving of file
// Сохранение файла
bool MP_Platform::SaveFile(const char* file, MP_FILE_DATA* file_data)
{
	#ifdef _WINDOWS

	std::string mode="wb";
	const wchar_t* pmode=utf8_to_wchar(mode.c_str());
	wchar_t wmode[3];
	wmode[0]=pmode[0];
	wmode[1]=pmode[1];
	wmode[2]=0;

	const wchar_t* wfile=utf8_to_wchar(file);

	FILE* f=NULL;
	_wfopen_s(&f, wfile, wmode);

	#else

	FILE* f=fopen(file, "wb");

	#endif

	if (f)
	{
		fwrite(file_data->data, 1, file_data->length, f);
		fclose(f);

		return  true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------

MP_POSITION::MP_POSITION()
{
	x=0.f;
	y=0.f;
	z=0.f;
}

MP_POSITION::MP_POSITION(float x,float y,float z)
{
	this->x=x;
	this->y=y;
	this->z=z;
}

MP_POSITION::MP_POSITION(float x,float y)
{
	this->x=x;
	this->y=y;
	z=0.f;
}

MP_POSITION MP_POSITION::operator+(const MP_POSITION& pos)
{
	MP_POSITION temp(x,y,z);
	temp.x+=pos.x;
	temp.y+=pos.y;
	temp.z+=pos.z;
	return temp;
}

MP_POSITION MP_POSITION::operator+=(const MP_POSITION& pos)
{
	*this=*this+pos;
	return *this;
}

MP_POSITION MP_POSITION::operator-(const MP_POSITION& pos)
{
	MP_POSITION temp(x,y,z);
	temp.x-=pos.x;
	temp.y-=pos.y;
	temp.z-=pos.z;
	return temp;
}

MP_POSITION MP_POSITION::operator-=(const MP_POSITION& pos)
{
	*this=*this-pos;
	return *this;
}

float MP_POSITION::Length()
{
	float d=sqrtf(x*x+y*y+z*z);
	return d;
}

void MP_POSITION::Normalize()
{
	float d=Length();
	x/=d;
	y/=d;
	z/=d;
}

// rus: Векторное произведение
void MP_POSITION::Cross(const MP_POSITION* pV1, const MP_POSITION* pV2)
{
	x=pV1->y*pV2->z-pV1->z*pV2->y;
	y=pV1->z*pV2->x-pV1->x*pV2->z;
	z=pV1->x*pV2->y-pV1->y*pV2->x;
}

// rus: Скалярное произведение
float MP_POSITION::Dot(const MP_POSITION* p)
{
	return x*p->x+y*p->y+z*p->z;
}

// ------------------------------------------------------------------------------------------

MP_Device_WRAP* MP_Manager::device=NULL;

// eng: Class that is used as storage for Magic Particles emitters
// rus: Класс, который является хранилищем эмиттеров Magic Particles
MP_Manager::MP_Manager()
{
	MP_Device::MP=this;
	MP_Atlas::MP=this;
	MP_Copy::MP=this;
	MP_Platform::MP=this;

	platform=NULL;

	k_emitter=0;
	max_emitter=10;
	m_emitter=new MP_Emitter*[max_emitter];
	m_descriptor=new int[max_emitter];
	for (int i=0;i<max_emitter;i++)
	{
		m_emitter[i]=NULL;
		m_descriptor[i]=0;
	}

	k_atlas=0;
	m_atlas=NULL;

	k_copy=0;
	m_copy=NULL;

	k_ptc=0;
	m_ptc=NULL;

	interpolation=MAGIC_INTERPOLATION_ENABLE;
	position_mode=MAGIC_CHANGE_EMITTER_DEFAULT;

	atlas_width=atlas_height=1024;
	atlas_frame_step=1;
	atlas_scale_step=0.01f;

	next_descriptor=0;
	next_index=-1;

	is_new_atlas=false;
}

MP_Manager::~MP_Manager()
{
	Destroy();
}

MP_Manager& MP_Manager::GetInstance()
{
	static MP_Manager mp;
	return mp;
}

// eng: Cleaning up
// rus: Очистка
void MP_Manager::Destroy()
{
	int i;
	for (i=0;i<max_emitter;i++)
	{
		if (m_emitter[i])
		{
			delete m_emitter[i];
			m_emitter[i]=NULL;
		}
	}

	if (m_emitter)
	{
		delete []m_emitter;
		m_emitter=NULL;
	}

	if (m_descriptor)
	{
		delete []m_descriptor;
		m_descriptor=NULL;
	}

	k_emitter=0;
	max_emitter=0;

	for (i=0;i<k_atlas;i++)
	{
		m_atlas[i]->Destroy();
		delete m_atlas[i];
		m_atlas[i]=NULL;
	}

	if (m_atlas)
	{
		delete []m_atlas;
		m_atlas=NULL;
	}

	k_atlas=0;

	for (i=0;i<k_copy;i++)
	{
		delete m_copy[i];
		m_copy[i]=NULL;
	}

	if (m_copy)
	{
		delete []m_copy;
		m_copy=NULL;
	}

	k_copy=0;

	for (i=0;i<k_ptc;i++)
	{
		delete m_ptc[i];
		m_ptc[i]=NULL;
	}

	if (m_ptc)
	{
		delete []m_ptc;
		m_ptc=NULL;
	}

	k_ptc=0;

	MP_Device::MP=NULL;
	MP_Atlas::MP=NULL;
	MP_Copy::MP=NULL;

	if (platform)
	{
		delete platform;
		platform=NULL;
	}

	MP_Platform::MP=NULL;
}

// eng: Initialization
// rus: Инициализация
void MP_Manager::Initialization(bool* render_states_filters, bool render_states_optimization, MAGIC_AXIS_ENUM axis, MP_Platform* platform, int interpolation, int position_mode, int atlas_width, int atlas_height, int atlas_frame_step, float atlas_starting_scale, float atlas_scale_step, bool copy_mode)
{
	this->platform=platform;

	Magic_SetRenderStateFilter(render_states_filters, render_states_optimization);

	Magic_SetAxis(axis);

	Magic_SetStartingScaleForAtlas(atlas_starting_scale);

	this->interpolation=interpolation;
	this->position_mode=position_mode;

	this->atlas_width=atlas_width;
	this->atlas_height=atlas_height;
	this->atlas_frame_step=atlas_frame_step;
	this->atlas_scale_step=atlas_scale_step;

	this->copy_mode=copy_mode;
}

// eng: Returning descriptor of first emitter. 
// rus: Получение дескриптора первого эмиттера
HM_EMITTER MP_Manager::GetFirstEmitter()
{
	next_descriptor=0;
	next_index=-1;

	if (k_emitter)
	{
		next_descriptor=m_descriptor[0];
		next_index=0;
	}

	return next_descriptor;
}

/// eng: Returning descriptor of next emitter. 
// rus: Получение дескриптора следующего эмиттера
HM_EMITTER MP_Manager::GetNextEmitter(HM_EMITTER hmEmitter)
{
	if (next_index==-1 || hmEmitter!=next_descriptor)
	{
		next_index=-1;
		for (int i=0;i<k_emitter;i++)
		{
			if (m_descriptor[i]==hmEmitter)
			{
				next_index=i;
				break;
			}
		}
	}

	next_descriptor=0;

	if (next_index!=-1)
	{
		next_index++;
		if (next_index<k_emitter)
		{
			next_descriptor=m_descriptor[next_index];
		}
		else
			next_index=-1;
	}

	return next_descriptor;
}

// eng: Returning the emitter by its descriptor
// rus: Возвращение эмиттера по дескриптору
MP_Emitter* MP_Manager::GetEmitter(HM_EMITTER hmEmitter)
{
	if (hmEmitter>=0 && hmEmitter<max_emitter)
		return m_emitter[hmEmitter];
	return NULL;
}

// eng: Returning the emitter by name
// rus: Возвращание эмиттера по имени
MP_Emitter* MP_Manager::GetEmitterByName(const char* name)
{
	HM_EMITTER hmEmitter=GetFirstEmitter();
	while (hmEmitter)
	{
		MP_Emitter* emitter=GetEmitter(hmEmitter);
		const char* emitter_name=emitter->GetEmitterName();
		if (!strcmp(name,emitter_name))
		{
			// eng: name coincides
			// rus: имя совпадает
			return emitter;
		}
		hmEmitter=GetNextEmitter(hmEmitter);
	}

	return NULL;
}

// eng: Loading all emitters from emitters folder
// rus: Загрузка всех эмиттеров из всех файлов
void MP_Manager::LoadAllEmitters()
{
	const char* file=platform->GetFirstFile();
	while (file)
	{
		LoadEmittersFromFile(file);
		file=platform->GetNextFile();
	}
	RefreshAtlas();
}

// eng: Loading all the emitters and animated folders from the file specified
// rus: Загрузка всех эмиттеров из указанного файла. Загружаются эмиттеры и анимированные папки
HM_FILE MP_Manager::LoadEmittersFromFile(const char* file)
{
	std::string ptc_file=GetPathToPTC();
	ptc_file+=file;

	HM_FILE mf=OpenPTC(ptc_file.c_str());
	if (mf>0)
	{
		// eng: file was opened
		// rus: файл успешно открыт
		LoadFolder(mf,"");
		return mf;
	}

	return 0;
}

HM_FILE MP_Manager::LoadEmittersFromFileInMemory(const char* address)
{
	HM_STREAM hmStream=Magic_StreamOpenMemory(address, 0, MAGIC_STREAM_READ);

	HM_FILE hmFile=Magic_OpenStream(hmStream);
	if (hmFile>0)
	{
		// eng: file was opened
		// rus: файл успешно открыт
		MP_PTC* ptc=new MP_PTC;
		ptc->hmFile=hmFile;
		ptc->hmStream=hmStream;
		ptc->data=address;
		AddPTC(ptc);

		LoadFolder(hmFile,"");
	}
	else
	{
		Magic_StreamClose(hmStream);
		delete []address;
		address=NULL;
	}
	return hmFile;
}

// eng: Closing file
// rus: Выгрузка одного файла
int MP_Manager::CloseFile(HM_FILE hmFile)
{
	RefreshAtlas();
	return DeletePTC(hmFile);
}

// eng: Closing all files
// rus: Выгрузка всех файлов
void MP_Manager::CloseFiles()
{
	RefreshAtlas();

	if (k_ptc)
	{
		int i=0;
		do 
		{
			delete m_ptc[i];
			m_ptc[i]=NULL;
			i++;
		}
		while (i<k_ptc);
		delete []m_ptc;
		m_ptc=NULL;
		k_ptc=0;
	}
}

// eng: Duplicating specified emitter
// rus: Дублирование указанного эмиттера
HM_EMITTER MP_Manager::DuplicateEmitter(HM_EMITTER hmEmitter)
{
	MP_Emitter* from=GetEmitter(hmEmitter);
	if (from)
	{
		if (Magic_IsInterval1(hmEmitter) && (!from->copy))
		{
			// eng: it is necessary firstly to create particles copy
			// rus: необходимо сначала создать копию частиц
			from->Restart();
		}

		MP_Emitter* emitter=device->NewEmitter(0,this);
		*emitter=*from;
		AddEmitter(emitter);
		return emitter->GetEmitter();
	}
	return 0;
}

// eng: Updating emitters taking into account the passed time
// rus: Обновление эмиттеров по таймеру
int MP_Manager::UpdateByTimer()
{
	static int fps=0;
	static int fps_counter=0;
	static unsigned long old_time=0;

	static unsigned long last_time=0;
	double rate=0.01;
	unsigned long new_time=GetTick();
	if (new_time>last_time)
	{
		rate=new_time-last_time;
		last_time=new_time;
		if (rate>500)
			rate=0.01;
	}

	unsigned long rtime=new_time-old_time;
	if (rtime>1000)
	{
		// eng: more than 1 second passed
		// rus: прошло больше секунды
		double percent=((double)rtime)/10.f;
		fps=(int)(percent*fps_counter/100);
		fps_counter=0;
		old_time=new_time;
	}

	fps_counter++;

	Update(rate);

	return fps;
}

// eng: Updating emitters
// rus: Обновление эмиттеров
void MP_Manager::Update(double time)
{
	HM_EMITTER hmEmitter=GetFirstEmitter();

	while (hmEmitter)
	{
		MP_Emitter* emitter=GetEmitter(hmEmitter);
		int state=emitter->GetState();
		if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
		{
			emitter->Update(time);

			MAGIC_EVENT evt;
			while (Magic_GetNextEvent(&evt)==MAGIC_SUCCESS)
			{
				function_event[evt.event](&evt);	// обработка события
			}
		}
		
		hmEmitter=GetNextEmitter(hmEmitter);
	}
}

// eng: Rendering all emitters
// rus: Рисование эмиттеров
int MP_Manager::Render()
{
	RefreshAtlas();
	device->BeginScene();

	// eng: visualisation of all emitters
	// rus: визуализация всех эмиттеров
	int k_particles=0;

	HM_EMITTER hmEmitter=GetFirstEmitter();

	while (hmEmitter)
	{
		MP_Emitter* emitter=GetEmitter(hmEmitter);
		k_particles+=emitter->Render();
		hmEmitter=GetNextEmitter(hmEmitter);
	}

	device->EndScene();

	return k_particles;
}

// eng: Stopping all the emitters
// rus: Остановка эмиттеров
void MP_Manager::Stop()
{
	HM_EMITTER hmEmitter=GetFirstEmitter();

	while (hmEmitter)
	{
		MP_Emitter* emitter=GetEmitter(hmEmitter);
		emitter->SetState(MAGIC_STATE_STOP);
		hmEmitter=GetNextEmitter(hmEmitter);
	}
}

// eng: Loading folder
// rus: Загрузка папки
void MP_Manager::LoadFolder(HM_FILE file, const char* path)
{
	Magic_SetCurrentFolder(file, path);

	MAGIC_FIND_DATA find;
	const char* name=Magic_FindFirst(file,&find,MAGIC_FOLDER | MAGIC_EMITTER);
	while (name)
	{
		if (find.animate)
		{
			LoadEmitter(file,name);
		}
		else
			LoadFolder(file,name);

		name=Magic_FindNext(file,&find);
	}

	Magic_SetCurrentFolder(file,"..");
}

// eng: Loading emitter
// rus: Загрузка конкретного эмиттера
MP_Emitter* MP_Manager::LoadEmitter(HM_FILE file, const char* path)
{
	// eng: it is necessary to load emitter from file
	// rus: нужно извлечь эмиттер из файла
	MP_Emitter* em=NULL;
	HM_EMITTER emitter=Magic_LoadEmitter(file,path);
	if (emitter)
	{
		em=device->NewEmitter(emitter,this);
		const char* ptc=Magic_GetFileName(file);
		if (ptc && Magic_HasTextures(file))
			em->restore_file=ptc;

		AddEmitter(em);
		// eng: initialization of emitter by default values
		// rus: инициализация эмиттера значениями по умолчанию
		if (interpolation!=MAGIC_INTERPOLATION_DEFAULT)
		{
			bool _interpolation=false;
			if (interpolation==MAGIC_INTERPOLATION_ENABLE)
				_interpolation=true;
			Magic_SetInterpolationMode(emitter,_interpolation);
		}

		switch (position_mode)
		{
		case MAGIC_CHANGE_EMITTER_ONLY:
			Magic_SetEmitterPositionMode(emitter,false);
			Magic_SetEmitterDirectionMode(emitter,false);
			break;

		case MAGIC_CHANGE_EMITTER_AND_PARTICLES:
			Magic_SetEmitterPositionMode(emitter,true);
			Magic_SetEmitterDirectionMode(emitter,true);
			break;
		}

		if (Magic_GetStaticAtlasCount(file))
			em->is_atlas=true;
		else
			is_new_atlas=true;
	}
	return em;
}

// eng: Adding new emitter into array
// rus: Добавление нового эмиттера в массив
void MP_Manager::AddEmitter(MP_Emitter* emitter)
{
	int i;

	next_descriptor=0;
	next_index=-1;

	int index=(int)emitter->GetEmitter();

	while (index>=max_emitter)
	{
		int new_max_emitter=max_emitter+10;

		MP_Emitter** vm_emitter=new MP_Emitter*[new_max_emitter];
		for (i=0;i<max_emitter;i++)
			vm_emitter[i]=m_emitter[i];
		delete []m_emitter;
		m_emitter=vm_emitter;
		
		int* vm_descriptor=new int[new_max_emitter];
		for (i=0;i<max_emitter;i++)
			vm_descriptor[i]=m_descriptor[i];
		delete []m_descriptor;
		m_descriptor=vm_descriptor;

		for (i=max_emitter;i<new_max_emitter;i++)
		{
			m_emitter[i]=NULL;
			m_descriptor[i]=0;
		}

		max_emitter=new_max_emitter;
	}

	m_emitter[index]=emitter;
	m_descriptor[k_emitter]=index;
	k_emitter++;
}

// eng: Refreshing textural atlases
// rus: Построение текстурного атласа
void MP_Manager::RefreshAtlas()
{
	int i;

	if (is_new_atlas)
	{
		// eng: new emitters were added, it is necessary to create new atlases for them
		// rus: были добавлены новые эмиттеры, необходимо создать для них атласы
		is_new_atlas=false;

		int k=GetEmitterCount();
		if (k)
		{
			HM_EMITTER* hm_emitter=new HM_EMITTER[k];

			k=0;

			HM_EMITTER hmEmitter=GetFirstEmitter();
			while (hmEmitter)
			{
				MP_Emitter* emitter=GetEmitter(hmEmitter);
				if (!emitter->is_atlas)
				{
					emitter->is_atlas=true;
					hm_emitter[k]=hmEmitter;
					k++;
				}
				hmEmitter=GetNextEmitter(hmEmitter);
			}

			if (k)
				Magic_CreateAtlasesForEmitters(atlas_width,atlas_height,k,hm_emitter,atlas_frame_step,atlas_scale_step);

			delete []hm_emitter;
			hm_emitter=NULL;
		}
	}

	MAGIC_CHANGE_ATLAS c;
	while (Magic_GetNextAtlasChange(&c)==MAGIC_SUCCESS)
	{
		int type=c.type;
		switch (type)
		{
		case MAGIC_CHANGE_ATLAS_LOAD:
			// eng: loading of frame in atlas
			// rus: загрузка кадра в атлас
			m_atlas[c.index]->LoadTexture(&c);
			break;

		case MAGIC_CHANGE_ATLAS_CLEAN:
			// eng: cleaning up of rectangle in atlas
			// rus: очистка прямоугольника в атласе
			m_atlas[c.index]->CleanRectangle(&c);
			break;

		case MAGIC_CHANGE_ATLAS_CREATE:
			// eng: creating of atlas
			// rus: создание атласа
			if (m_atlas)
			{
				// eng: broadening of atlas array
				// rus: расширение массив атласов
				MP_Atlas** vm_atlas=new MP_Atlas*[k_atlas+1];
				for (i=0;i<k_atlas;i++)
					vm_atlas[i]=m_atlas[i];
				delete []m_atlas;
				m_atlas=vm_atlas;
			}
			else
			{
				m_atlas=new MP_Atlas*[1];
			}

			m_atlas[k_atlas]=device->NewAtlas(c.width, c.height, c.file);
			k_atlas++;

			break;

		case MAGIC_CHANGE_ATLAS_DELETE:
			// eng: Deleting of atlas
			// rus: удаление атласа
			m_atlas[c.index]->Destroy();
			delete m_atlas[c.index];

			if (k_atlas==1)
			{
				delete []m_atlas;
				m_atlas=NULL;
			}
			else
			{
				MP_Atlas** vm_atlas=new MP_Atlas*[k_atlas-1];
				for (i=0;i<c.index;i++)
					vm_atlas[i]=m_atlas[i];
				for (i=c.index+1;i<k_atlas;i++)
					vm_atlas[i-1]=m_atlas[i];
				delete []m_atlas;
				m_atlas=vm_atlas;
			}
			k_atlas--;
		}
	}

	#ifdef SHADER_WRAP
	device->RefreshMaterials();
	#endif
}

// eng: Restoring textural atlas in cases of loosing textures
// rus: Восстановление текстурного атласа в случае потери текстур
void MP_Manager::RestoreAtlas()
{
	if (k_emitter)
	{
		int i;

		// eng: Recreating of static atlas
		// rus: Перестраиваем статические атласы
		for (i=0;i<k_atlas;i++)
		{
			MP_Atlas* atlas=m_atlas[i];
			std::string file=atlas->GetFile();
			if (!(file.empty()))
			{
				// eng: it is necessary to reload texture
				// rus: необходимо перегрузить текстуру
				int width, height;
				atlas->GetSize(width, height);

				atlas->Destroy();
				delete atlas;
				m_atlas[i]=device->NewAtlas(width,height,file.c_str());
			}
		}

		// eng: Recreating of dynamic atlas
		// rus: Перестраиваем динамические атласы
		int k_restore_file=0;
		std::string** m_restore_file=new std::string*[k_emitter];

		HM_EMITTER hmEmitter=GetFirstEmitter();
		while (hmEmitter)
		{
			MP_Emitter* emitter=GetEmitter(hmEmitter);

			bool add=true;

			if (emitter->restore_file.empty())
				add=false;
			else
			{
				for (int j=0;j<k_restore_file;j++)
				{
					if (*(m_restore_file[j])==emitter->restore_file)
					{
						add=false;
						break;
					}
				}
			}

			if (add)
			{
				m_restore_file[k_restore_file]=&(emitter->restore_file);
				k_restore_file++;
			}

			hmEmitter=GetNextEmitter(hmEmitter);
		}

		HM_FILE* m_opened_file=NULL;
		if (k_restore_file)
		{
			// rus: loading all found files with textures
			// rus: загружаем все отобранные файлы с текстурами
			m_opened_file=new HM_FILE[k_restore_file];
			for (i=0;i<k_restore_file;i++)
			{
				std::string path=GetPathToPTC();
				path+=*(m_restore_file[i]);
				m_opened_file[i]=OpenPTC(path.c_str());
			}
		}

		// eng: Recreating of dynamic atlas
		// rus: Перестраиваем динамические атласы
		Magic_CreateAtlases(atlas_width,atlas_height,atlas_frame_step,atlas_scale_step);

		RefreshAtlas();

		if (k_restore_file)
		{
			// eng: Unload ptc-file with textures
			// rus: выгружаем ptc-файлы с текстурами
			for (i=0;i<k_restore_file;i++)
				CloseFile(m_opened_file[i]);

			delete []m_opened_file;
			m_opened_file=NULL;
		}

		delete []m_restore_file;
		m_restore_file=NULL;
		k_restore_file=0;
	}
}

// eng: Deleting specified emitter 
// rus: Удаление указанного эмиттера
int MP_Manager::DeleteEmitter(HM_EMITTER hmEmitter)
{
	int result=MAGIC_ERROR;
	next_descriptor=0;
	next_index=-1;

	for (int j=0;j<k_emitter;j++)
	{
		HM_EMITTER hme=m_descriptor[j];
		if (hme==hmEmitter)
		{
			// it is necessary to delete this element from index array
			// нужно удалить данный элемент из индексного массива
			for (int k=j+1;k<k_emitter;k++)
			{
				m_descriptor[k-1]=m_descriptor[k];
			}

			k_emitter--;

			m_descriptor[k_emitter]=0;

			delete m_emitter[hmEmitter];
			m_emitter[hmEmitter]=NULL;

			result=MAGIC_SUCCESS;

			break;
		}
	}

	return result;
}

// eng: Adding file with particles copy
// rus: Добавление файла с копией частиц
MP_Copy* MP_Manager::AddCopy(MP_Emitter* emitter)
{
	if (m_copy)
	{
		MP_Copy** vm_copy=new MP_Copy*[k_copy+1];
		for (int i=0;i<k_copy;i++)
			vm_copy[i]=m_copy[i];
		delete []m_copy;
		m_copy=vm_copy;
	}
	else
		m_copy=new MP_Copy*[1];

	MP_Copy* copy=new MP_Copy(emitter);
	m_copy[k_copy]=copy;
	k_copy++;

	return copy;
}

// eng: Deleting file with particles copy
// rus: Удаление файла с копией частиц
void MP_Manager::DeleteCopy(MP_Copy* copy)
{
	// eng: it is necessary to delete copy
	// rus: надо удалить копию
	int i;
	
	int index=-1;
	for (i=0;i<k_copy;i++)
	{
		if (m_copy[i]==copy)
		{
			index=i;
			break;
		}
	}

	delete m_copy[index];

	if (k_copy==1)
	{
		delete []m_copy;
		m_copy=NULL;
	}
	else
	{
		MP_Copy** vm_copy=new MP_Copy*[k_copy-1];
		for (i=0;i<index;i++)
			vm_copy[i]=m_copy[i];
		for (i=index+1;i<k_copy;i++)
			vm_copy[i-1]=m_copy[i];
		delete []m_copy;
		m_copy=vm_copy;
	}

	k_copy--;
}

// eng: Searching among files containing particle copies by specified emitter id
// rus: Поиск среди файлов копий частиц соответствующего указанному идентификатору эмиттера
MP_Copy* MP_Manager::FindCopy(unsigned int emitter_id)
{
	if (GetCopyMode())
	{
		for (int i=0;i<k_copy;i++)
		{
			MP_Copy* copy=m_copy[i];
			if (copy->GetEmitterID()==emitter_id)
				return copy;
		}
	}
	return NULL;
}

// eng: Adding open file
// rus: Добавление открытого файла
void MP_Manager::AddPTC(MP_PTC* ptc_file)
{
	if (m_ptc)
	{
		MP_PTC** vm_ptc=new MP_PTC*[k_ptc+1];
		for (int i=0;i<k_ptc;i++)
			vm_ptc[i]=m_ptc[i];
		delete []m_ptc;
		m_ptc=vm_ptc;
	}
	else
		m_ptc=new MP_PTC*[1];

	m_ptc[k_ptc]=ptc_file;
	k_ptc++;
}

// eng: Deleting open file
// rus: Удаление открытого файла
int MP_Manager::DeletePTC(HM_FILE hmFile)
{
	int result=MAGIC_ERROR;
	for (int i=0;i<k_ptc;i++)
	{
		if (m_ptc[i]->hmFile==hmFile)
		{
			result=m_ptc[i]->Close();
			delete m_ptc[i];

			if (k_ptc==1)
			{
				delete []m_ptc;
				m_ptc=NULL;
			}
			else
			{
				MP_PTC** vm_ptc=new MP_PTC*[k_ptc-1];
				int j;
				for (j=0;j<i;j++)
					vm_ptc[j]=m_ptc[j];
				for (j=i+1;j<k_ptc;j++)
					vm_ptc[j-1]=m_ptc[j];
				delete []m_ptc;
				m_ptc=vm_ptc;
			}

			k_ptc--;

			break;
		}
	}
	return result;
}

// ------------------------------------------------------------------------------------------------

// eng: Class, specialized for work with the emitters
// rus: Класс, который хранит загруженные эмиттеры
bool MP_Emitter::temporary_emitter_modes[32];
MP_Device_WRAP* MP_Emitter::device=NULL;
MP_Emitter::MP_Emitter(HM_EMITTER emitter, MP_Manager* owner)
{
	this->emitter=emitter;
	this->owner=owner;

	z=0.f;

	first_restart=true;

	copy=NULL;

	restore_file="";

	state=MAGIC_STATE_UPDATE;

	is_atlas=false;
}

MP_Emitter::~MP_Emitter()
{
	if (copy)
	{
		copy->DecReference();
		copy=NULL;
	}

	Magic_UnloadEmitter(emitter);
}

MP_Emitter& MP_Emitter::operator=(const MP_Emitter& from)
{
	if (copy)
	{
		copy->DecReference();
		copy=NULL;
	}

	state=from.state;

	is_atlas=from.is_atlas;

	z=from.z;

	if (from.copy)
	{
		copy=from.copy;
		copy->IncReference(this);
	}

	restore_file=from.restore_file;

	emitter=Magic_DuplicateEmitter(from.emitter);
	
	return *this;
}

// eng: Returning the name of the emitter
// rus: Возвращение имени эмиттера
const char* MP_Emitter::GetEmitterName()
{
	return Magic_GetEmitterName(emitter);
}

// eng: Restarting of emitter
// rus: Установка эмиттера на стартовую позицию
void MP_Emitter::Restart()
{
	if (Magic_IsInterval1(emitter))
	{
		// eng: animation starts not from beginning
		// rus: анимация начинается не с начала
		if (!copy)
		{
			copy=owner->FindCopy(Magic_GetEmitterID(emitter));
			if (!copy)
				copy=owner->AddCopy(this);
		}

		copy->LoadParticles(this);
	}
	else
	{
		Magic_Restart(emitter);
	}

	first_restart=false;
}

// eng: Position of emitter
// rus: Позиция эмиттера
void MP_Emitter::GetPosition(MP_POSITION& position)
{
	MAGIC_POSITION pos;
	Magic_GetEmitterPosition(emitter,&pos);
	position.x=pos.x;
	position.y=pos.y;
	#ifdef MAGIC_3D
	position.z=pos.z;
	#else
	position.z=z;
	#endif
}

void MP_Emitter::SetPosition(MP_POSITION& position)
{
	MAGIC_POSITION pos;
	pos.x=position.x;
	pos.y=position.y;

	#ifdef MAGIC_3D
	pos.z=position.z;
	#else
	z=position.z;
	#endif
	Magic_SetEmitterPosition(emitter,&pos);
}

// eng: Moving the emitter to the position specified allowing restart. 
// rus: Перемещение эмиттера в указанную позицию с возможностью перезапуска. При перемещении все существующие частицы перемещаются вместе с эмиттером
void MP_Emitter::Move(MP_POSITION& position, bool restart)
{
	if (restart)
	{
		Restart();
		SetPosition(position);
	}
	else
	{
		// eng: temporary setting mode of movement together with particles
		// rus: временно устанавливаем режим перемещения вместе с частицами
		int i;
		int emitters=Magic_GetEmitterCount(emitter);
		for (i=0;i<emitters;i++)
		{
			HM_EMITTER child=Magic_GetEmitter(emitter, i);
			temporary_emitter_modes[i]=Magic_GetEmitterPositionMode(child);
		}
		Magic_SetEmitterPositionMode(emitter,MAGIC_CHANGE_EMITTER_AND_PARTICLES);

		SetPosition(position);

		// eng: restore previous mode of movement
		// rus: возвращаем на место старый режим перемещения
		for (i=0;i<emitters;i++)
		{
			HM_EMITTER child=Magic_GetEmitter(emitter, i);
			Magic_SetEmitterPositionMode(child, temporary_emitter_modes[i]);
		}
	}
}

// eng: Offsetting the current emitter position by the value specified
// rus: Смещение текущей позиции эмиттера на указанную величину. Будет использован текущий режим перемещения эмиттера
void MP_Emitter::Offset(MP_POSITION& offset)
{
	MP_POSITION pos;
	GetPosition(pos);
	pos+=offset;
	SetPosition(pos);
}

// eng: Direction of emitter
// rus: Направление эмиттера
void MP_Emitter::SetDirection(MAGIC_DIRECTION* direction)
{
	Magic_SetEmitterDirection(emitter, direction);
}

void MP_Emitter::GetDirection(MAGIC_DIRECTION* direction)
{
	Magic_GetEmitterDirection(emitter, direction);
}

// eng: Setting the emitter direction to the specified value with the restart ability
// rus: Поворот эмиттера в указанное направление с возможностью перезапуска. При повороте все существующие частицы поворачиваются вместе с эмиттером
void MP_Emitter::Direct(MAGIC_DIRECTION* direction, bool restart)
{
	if (restart)
	{
		Restart();
		SetDirection(direction);
	}
	else
	{
		// eng: temporary setting mode of rotation together with particles
		// rus: временно устанавливаем режим вращения вместе с частицами
		int i;
		int emitters=Magic_GetEmitterCount(emitter);
		for (i=0;i<emitters;i++)
		{
			HM_EMITTER child=Magic_GetEmitter(emitter, i);
			temporary_emitter_modes[i]=Magic_GetEmitterDirectionMode(child);
		}
		Magic_SetEmitterDirectionMode(emitter,MAGIC_CHANGE_EMITTER_AND_PARTICLES);

		SetDirection(direction);

		// eng: restore previous mode of movement
		// rus: возвращаем на место старый режим перемещения
		for (i=0;i<emitters;i++)
		{
			HM_EMITTER child=Magic_GetEmitter(emitter, i);
			Magic_SetEmitterDirectionMode(child, temporary_emitter_modes[i]);
		}
	}
}

// eng: Rotating of the emitter by the specified value
// rus: Вращение эмиттера на указанную величину. Будет использован текущий режим вращения эмиттера
void MP_Emitter::Rotate(MAGIC_DIRECTION* offset)
{
	MAGIC_DIRECTION q;
	GetDirection(&q);
	#ifdef MAGIC_3D
	MagicQuaternionInversion(&q);
	MagicQuaternionMul(&q,&q,offset);
	#else
	q.angle+=offset->angle;
	#endif
	SetDirection(&q);
}

// eng: Setting the scale of the emitter
// rus: Установка масштаба эмиттера
void MP_Emitter::SetScale(float scale)
{
	Magic_SetScale(emitter,scale);
}

float MP_Emitter::GetScale()
{
	return Magic_GetScale(emitter); 
}

// eng: Setting the state of the emitter
// rus: Установка статуса эмиттера
void MP_Emitter::SetState(int state)
{
	if (this->state!=state)
	{
		if (state==MAGIC_STATE_UPDATE && Magic_IsInterrupt(emitter))
		{
			// eng: it is necessary to turn off interrupting of emitter work
			// rus: необходимо отключить прерывание работы эмиттера
			Magic_SetInterrupt(emitter,false);
		}

		if (state==MAGIC_STATE_STOP && this->state!=MAGIC_STATE_INTERRUPT)
		{
			// eng: unload particles from memory
			// rus: выгружаем пространство частиц из памяти
			Magic_Stop(emitter);
		}
		else
		{
			if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
			{
				// eng: start emitter
				// rus: стартуем эмиттер
				if (!first_restart)
				{
					if (this->state==MAGIC_STATE_STOP || (!Magic_InInterval(emitter)))
					{
						// rus: позиция анимации эмиттера не входит в интервал видимости, необходимо осуществить установку на начало
						if (copy)
							copy->LoadParticles(this);
					}
				}

				if (state==MAGIC_STATE_INTERRUPT)
					Magic_SetInterrupt(emitter,true);
			}
		}

		this->state=state;
	}
}

// eng: Updating emitter
// rus: Обновление эмиттера
void MP_Emitter::Update(double time)
{
	if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
	{
		if (first_restart)
			Restart();

		if (!Magic_IsInterpolationMode(emitter))
		{
			// rus: without interpolation a fixing step is only possible
			// rus: без интерполяции возможен только фиксированный шаг
			time=Magic_GetUpdateTime(emitter);
		}

		if (!Magic_Update(emitter,time))
		{
			// eng: working of emitter is over
			// rus: выполнение эмиттера завершено
			SetState(MAGIC_STATE_STOP);
		}
	}
}

// eng: Emitter visualization
// rus: Отрисовка эмиттера. Возвращается количество нарисованных частиц
int MP_Emitter::Render()
{
	int count=0;
	if (state!=MAGIC_STATE_STOP)
	{
		HM_EMITTER emitter=GetEmitter();
		if (Magic_InInterval(emitter))
		{
			MAGIC_RENDERING_START start;

			MAGIC_ARGB_ENUM color_mode=device->GetColorMode();
			
			#ifdef SEPARATE_ATTRIBUTES_WRAP
			int max_array_streams=100;
			#else
			int max_array_streams=0;
			#endif

			#ifdef INDEX_BUFFER_32_WRAP
			bool index32=true;
			#else
			bool index32=false;
			#endif

			void* context=Magic_PrepareRenderArrays(emitter, &start, max_array_streams, color_mode, index32);
			if (start.arrays)
			{
				int i;

				count=start.particles;

				device->SaveAttributes(&start);

				device->BeginDrawEmitter(this);

				#ifdef SEPARATE_ATTRIBUTES_WRAP

				for (i=0;i<start.arrays;i++)
				{
					MP_ARRAY_INFO* array_info=device->GetArrayInfo(i);
					Magic_GetRenderArrayData(context, i, array_info);
				}
				device->SetArrayInfoCount(start.arrays);

				int max_vertices=device->MapBuffers(&(start.format));

				#else

				MAGIC_ARRAY_INFO vertex_info, index_info;

				Magic_GetRenderArrayData(context, 0, &vertex_info);
				Magic_GetRenderArrayData(context, 1, &index_info);

				int max_vertices=device->MapBuffers(&vertex_info, &index_info, &(start.format));

				#endif

				device->SetVertexFormat(&start);

				int k=device->GetArrayInfoCount();
				i=0;
				do 
				{
					MP_ARRAY_INFO* array_info=device->GetArrayInfo(i);
					Magic_SetRenderArrayData(context, array_info->stage, array_info->buffer, array_info->offset, array_info->stride);
					i++;
				}
				while (i<k);

				// заполнение буферов данными
				Magic_FillRenderArrays(context);

				device->UnmapBuffers();
				device->SetBuffers();

				MAGIC_RENDER_VERTICES vrts;
				MAGIC_RENDER_STATE state;
				while (Magic_GetVertices(context, &vrts)==MAGIC_SUCCESS)
				{
					while (Magic_GetNextRenderState(context, &state)==MAGIC_SUCCESS)
						device->SetRenderState(&state);

					#ifdef SHADER_WRAP
					device->SetRenderMaterial(vrts.material);
					#endif

					device->DrawVertices(vrts.starting_index, vrts.indexes_count, max_vertices);
				}

				device->EndDrawEmitter(this);

				device->RestoreAttributes(&start);
			}
		}
	}
	return count;
}

// ------------------------------------------------------------------------------------------

// eng: Class storing the files with particles copies
// rus: Класс, который хранит файлы с копиями частиц
int MP_Copy::file_count=0;
std::string MP_Copy::file_name="mp";
MP_Manager* MP_Copy::MP=NULL;

MP_Copy::MP_Copy(MP_Emitter* emitter)
{
	emitter_id=Magic_GetEmitterID(emitter->GetEmitter());
	reference=0;
	file="";
	ram=0;
	IncReference(emitter);
}

MP_Copy::~MP_Copy()
{
	Clear();
}

// eng: Cleaning up
// rus: Очистка
void MP_Copy::Clear()
{
	if (!file.empty())
	{
		MP->RemoveFile(file.c_str());
		file="";
	}

	if (ram)
	{
		Magic_StreamClose(ram);
		ram=0;
	}

	reference=0;
}

// eng: Increasing of reference count
// rus: Увеличение числа ссылок на файл
void MP_Copy::IncReference(MP_Emitter* emitter)
{
	if (!reference)
	{
		const char* temp_dir=MP->GetPathToTemp();
		if (temp_dir)
		{
			// eng: temporary folder exists
			// rus: временная папка существует
			srand(MP->GetTick());
			bool repeat_select;
			do
			{
				repeat_select=false;
				// сохранение в файл
				file=temp_dir;
				file+=file_name;
				static char s[20];
				sprintf(s,"%d",file_count);
				file+=s;
				if (MP->RemoveFile(file.c_str())==-1)
				{
					if (errno!=ENOENT)
					{
						// eng: temporary existing was not deleted, it is necessary to choose another one
						// rus: существующий временный файл не был удален, надо выбрать другой файл
						repeat_select=true;

						// eng: select random digit from 0 to 9
						// rus: выбираем случайное число от 0 до 9
						int r=rand();
						r=r % 10;
						// eng: convert it to symbol
						// rus: превращаем его в символ
						char v=(char)r;
						v+='0';
						file_name+=v;
					}
				}
			}
			while (repeat_select);
			file_count++;
		}
		else
		{
			// сохранение в ОЗУ
			ram=Magic_StreamOpenMemory(NULL, 0, MAGIC_STREAM_WRITE);
		}

		LoadParticles(emitter);

		reference++;
	}
	else
	{
		if (!MP->GetCopyMode())
			reference++;
	}
}

// eng: Decreasing of reference count
// rus: Уменьшение числа ссылок на файл
void MP_Copy::DecReference()
{
	if (!MP->GetCopyMode())
	{
		reference--;
		if (!reference)
			MP->DeleteCopy(this);
	}
}

// eng: Loading of particles from file to emitter
// rus: Загрузка частиц из файла в эмиттер
void MP_Copy::LoadParticles(MP_Emitter* emitter)
{
	if (ram)
	{
		Magic_StreamSetPosition(ram, 0);
		Magic_EmitterToInterval1_Stream(emitter->GetEmitter(), 1.f, ram);
	}
	else
	{
		const char* temp_file=NULL;
		if (!file.empty())
			temp_file=file.c_str();

		Magic_EmitterToInterval1(emitter->GetEmitter(), 1.f, temp_file);
	}
}

// ------------------------------------------------------------------------------------------

MP_PTC::MP_PTC()
{
	hmFile=0;
	hmStream=0;
	data=NULL;
}

MP_PTC::~MP_PTC()
{
	Close();
}

int MP_PTC::Close()
{
	int result=MAGIC_ERROR;

	if (hmFile)
	{
		result=Magic_CloseFile(hmFile);
		hmFile=0;
	}

	if (hmStream)
	{
		int result2=Magic_StreamClose(hmStream);
		if (result==MAGIC_SUCCESS)
			result=result2;
		hmStream=0;
	}

	if (data)
	{
		delete []data;
		data=NULL;
	}

	return result;
}

// ------------------------------------------------------------------------------------------

// eng: Class storing the textural atlas. This class will be abstract
// rus: Класс, который хранит текстурный атлас. Этот класс будет абстрактным

MP_Manager* MP_Atlas::MP=NULL;
MP_Device_WRAP* MP_Atlas::device=NULL;

MP_Atlas::MP_Atlas(int width, int height, const char* file)
{
	atlas_width=width;
	atlas_height=height;
	file_name="";
	if (file)
		file_name=file;
}

// ------------------------------------------------------------------------------------------

// eng: Class controlling drawing. This class will be abstract
// rus: Класс, который управляет рисованием. Этот класс будет абстрактным

MP_Manager* MP_Device::MP=NULL;

MP_Device::MP_Device(int width, int height)
{
	MP_Manager::device=(MP_Device_WRAP*)this;
	MP_Emitter::device=(MP_Device_WRAP*)this;
	MP_Atlas::device=(MP_Device_WRAP*)this;

	window_width=width;
	window_height=height;

	vertex_buffer=NULL;
	index_buffer=NULL;

	#ifdef SHADER_WRAP
	k_material=0;
	m_material=NULL;
	#endif
}

MP_Device::~MP_Device()
{
	MP_Manager::device=NULL;
	MP_Emitter::device=NULL;
	MP_Atlas::device=NULL;
}

// eng: Creating
// rus: Создание
bool MP_Device::Create()
{
	Destroy();
	vertex_buffer=NewVertexBuffer();
	index_buffer=NewIndexBuffer();
	return true;
}

// eng: Destroying
// rus: Уничтожение
void MP_Device::Destroy()
{
	int i;
	if (MP)
	{
		int k_atlas=MP->GetAtlasCount();
		for (i=0;i<k_atlas;i++)
			MP->GetAtlas(i)->Destroy();
	}

	if (vertex_buffer)
	{
		vertex_buffer->Destroy();
		delete vertex_buffer;
		vertex_buffer=NULL;
	}

	if (index_buffer)
	{
		index_buffer->Destroy();
		delete index_buffer;
		index_buffer=NULL;
	}

	#ifdef SHADER_WRAP
	for (i=0;i<k_material;i++)
	{
		delete m_material[i];
		m_material[i]=NULL;
	}

	if (m_material)
	{
		delete []m_material;
		m_material=NULL;
	}

	k_material=0;

	vshader_list.Clear();
	pshader_list.Clear();
	technique_list.Clear();
	#endif
}

// eng: Beginning of scene drawing
// rus: Начало отрисовки сцены
void MP_Device::BeginScene()
{
	if (IsLost())
	{
		Create();
		#ifdef MAGIC_3D
		// eng: setting view matrix
		// rus: устанавливаем матрицу вида
		SetScene3d();
		#endif
		MP->RestoreAtlas();
	}
}

// eng: Creating of atlas object
// rus: Создание объекта атласа
MP_Atlas* MP_Device::NewAtlas(int width, int height, const char* file)
{
	return new MP_Atlas_WRAP(width, height, file);
}

// eng: Creating of emitter object
// rus: Создание объекта эмиттера
MP_Emitter* MP_Device::NewEmitter(HM_EMITTER emitter, MP_Manager* owner)
{
	return new MP_Emitter(emitter, owner);
}

// eng: Creating of vertex buffer
// rus: Создание вершинного буфера
MP_BUFFER* MP_Device::NewVertexBuffer()
{
	return new MP_BUFFER_RAM();
}

// eng: Creating of index buffer
// rus: Создание индексного буфера
MP_BUFFER* MP_Device::NewIndexBuffer()
{
	return new MP_BUFFER_RAM();
}

#ifdef SHADER_WRAP
// eng: Creating of material
// rus: Создание материала
MP_MATERIAL* MP_Device::NewMaterial(MAGIC_MATERIAL* mat)
{
	return new MP_MATERIAL(mat, this);
}
#endif

#ifdef SEPARATE_ATTRIBUTES_WRAP
int MP_Device::MapBuffers(MAGIC_VERTEX_FORMAT* format)
{
	vertex_format=*format;

	int max_length=0;
	int stride=0;
	
	if (k_array_info)
	{
		#ifdef SEPARATE_STREAMS_WRAP
		SeparateStreams();
		#endif

		int k=k_array_info-1;

		for (int i=0;i<k;i++)
		{
			MP_ARRAY_INFO* attribute_info=&(m_array_info[i]);
			attribute_info->stage=i;
			attribute_info->offset=stride;
			stride+=attribute_info->bytes_per_one;
			#ifdef OFFSET_ATTRIBUTES_WRAP
			int additional_length=GetAdditionalAttributeLength(attribute_info->type, attribute_info->index);
			stride+=additional_length;
			#endif
			if (max_length<attribute_info->length)
				max_length=attribute_info->length;
		}
		int new_length=max_length*stride;
		vertex_buffer->SetLength(new_length, this);
		void* buffer_ptr=vertex_buffer->Map(stride, this);
		for (int i=0;i<k;i++)
		{
			MP_ARRAY_INFO* attribute_info=&(m_array_info[i]);
			#ifdef SEPARATE_STREAMS_WRAP
			attribute_info->stride=attribute_info->bytes_per_one;
			attribute_info->offset=GetStreamOffset(attribute_info->type, attribute_info->index);
			#else
			attribute_info->stride=stride;
			#endif
			attribute_info->buffer=buffer_ptr;
		}

		MP_ARRAY_INFO* index_info=&(m_array_info[k]);
		index_info->stage=k;
		new_length=index_info->length*index_info->bytes_per_one;
		index_buffer->SetLength(new_length, this);
		index_info->offset=0;
		index_info->stride=index_info->bytes_per_one;
		index_info->buffer=index_buffer->Map(index_info->stride, this);
		
	}
	return max_length;
}

#else

int MP_Device::MapBuffers(MAGIC_ARRAY_INFO* vertex_info, MAGIC_ARRAY_INFO* index_info, MAGIC_VERTEX_FORMAT* format)
{
	vertex_format=*format;

	int max_length=vertex_info->length;

	MP_ARRAY_INFO* info;
	int new_length;

	k_array_info=1;
	
	#ifdef OFFSET_ATTRIBUTES_WRAP
	int i;
	int stage=2;

	MP_ARRAY_INFO* attribute_info=&(m_array_info[0]);

	attribute_info->type=MAGIC_VERTEX_FORMAT_POSITION;
	attribute_info->index=0;
	attribute_info->stage=stage;
	stage++;
	#ifdef MAGIC_3D
	attribute_info->bytes_per_one=12;
	#else
	attribute_info->bytes_per_one=8;
	#endif
	int stride=0;
	attribute_info->offset=stride;
	stride+=attribute_info->bytes_per_one;
	int additional_length=GetAdditionalAttributeLength(attribute_info->type, attribute_info->index);
	stride+=additional_length;

	if (format->attributes & MAGIC_ATTRIBUTE_COLOR)
	{
		attribute_info=&(m_array_info[k_array_info]);
		k_array_info++;

		attribute_info->stage=stage;
		stage++;
		attribute_info->type=MAGIC_VERTEX_FORMAT_COLOR;
		attribute_info->index=0;
		attribute_info->bytes_per_one=4;
		attribute_info->offset=stride;
		stride+=attribute_info->bytes_per_one;
		additional_length=GetAdditionalAttributeLength(attribute_info->type, attribute_info->index);
		stride+=additional_length;
	}

	for (i=0;i<format->UVs;i++)
	{
		attribute_info=&(m_array_info[k_array_info]);
		k_array_info++;

		attribute_info->stage=stage;
		stage++;
		attribute_info->type=MAGIC_VERTEX_FORMAT_UV;
		attribute_info->index=i;
		attribute_info->bytes_per_one=8;
		attribute_info->offset=stride;
		stride+=attribute_info->bytes_per_one;
		additional_length=GetAdditionalAttributeLength(attribute_info->type, attribute_info->index);
		stride+=additional_length;
	}

	new_length=vertex_info->length*stride;
	vertex_buffer->SetLength(new_length, this);
	void* buffer_ptr=vertex_buffer->Map(stride, this);

	for (i=0;i<k_array_info;i++)
	{
		MP_ARRAY_INFO* attribute_info=&(m_array_info[i]);
		attribute_info->stride=stride;
		attribute_info->buffer=buffer_ptr;
		attribute_info->locked_start=vertex_info->locked_start;
		attribute_info->locked_length=vertex_info->locked_length;
	}

	#else

	int stage=0;

	*((MAGIC_ARRAY_INFO*)&(m_array_info[0]))=*vertex_info;

	info=&(m_array_info[0]);
	info->stage=stage;
	stage++;
	new_length=vertex_info->length*vertex_info->bytes_per_one;
	vertex_buffer->SetLength(new_length, this);
	info->offset=0;
	info->stride=vertex_info->bytes_per_one;
	info->buffer=vertex_buffer->Map(info->stride, this);

	#endif

	*((MAGIC_ARRAY_INFO*)&(m_array_info[k_array_info]))=*index_info;
	k_array_info++;
	info=&(m_array_info[k_array_info-1]);
	info->stage=stage;
	stage++;
	new_length=index_info->length*index_info->bytes_per_one;
	index_buffer->SetLength(new_length, this);
	info->offset=0;
	info->stride=index_info->bytes_per_one;
	info->buffer=index_buffer->Map(info->stride, this);
	
	return max_length;
}
#endif

void MP_Device::UnmapBuffers()
{
	vertex_buffer->Unmap(this);
	index_buffer->Unmap(this);
}

void MP_Device::SetBuffers()
{
	vertex_buffer->Set(&vertex_format, m_array_info[0].stride, this);
	index_buffer->Set(&vertex_format, m_array_info[k_array_info-1].stride, this);
}

#ifdef SHADER_WRAP
// eng: Refreshing materials
// rus: Построение материалов
void MP_Device::RefreshMaterials()
{
	int k_material_api=Magic_GetMaterialCount();
	if (k_material<k_material_api)
	{
		// необходимо расширить массив материалов
		int i;
		if (m_material)
		{
			// eng: broadening of material array
			// rus: расширение массива материалов
			MP_MATERIAL** vm_material=new MP_MATERIAL*[k_material_api];
			for (i=0;i<k_material;i++)
				vm_material[i]=m_material[i];
			delete []m_material;
			m_material=vm_material;
		}
		else
		{
			m_material=new MP_MATERIAL*[k_material_api];
		}

		MAGIC_MATERIAL mat;
		for (i=k_material;i<k_material_api;i++)
		{
			Magic_GetMaterial(i, &mat);
			m_material[i]=NewMaterial(&mat);
		}

		k_material=k_material_api;
	}
}
#endif


#ifdef SEPARATE_STREAMS_WRAP
// Разделение общего вершинного буфера на отдельные части
void MP_Device::SeparateStreams()
{
	int k=k_array_info-1;
	int stride=0;
	for (int i=0;i<k;i++)
	{
		MAGIC_ARRAY_INFO* array_info=&(m_array_info[i]);
		if (array_info->type==MAGIC_VERTEX_FORMAT_UV)
			offset_stream_uv[array_info->index]=stride;
		else
			offset_streams[array_info->type]=stride;
		stride+=array_info->length*array_info->bytes_per_one;
	}
}

// Получение смещения атрибута в вершинном буфере
int MP_Device::GetStreamOffset(int attribute, int index)
{
	if (attribute==MAGIC_VERTEX_FORMAT_UV)
		return offset_stream_uv[index];
	return offset_streams[attribute];
}
#endif

// ------------------------------------------------------------------------------------------

#ifdef MAGIC_3D
void QuaternionCreate(MAGIC_DIRECTION* quat, MP_POSITION* axis, float angle)
{
	axis->Normalize();
	angle*=0.5f;
	float sinus=sinf(angle);
	float cosinus=cosf(angle);
	quat->x=axis->x*sinus;
	quat->y=axis->y*sinus;
	quat->z=axis->z*sinus;
	quat->w=cosinus;
}
#endif

// eng: Getting identity matrix 
// rus: Возвращает единичную матрицу
void MP_MATRIX::MatrixIdentity()
{
	m[0][0]=1.f;
	m[0][1]=0.f;
	m[0][2]=0.f;
	m[0][3]=0.f;

	m[1][0]=0.f;
	m[1][1]=1.f;
	m[1][2]=0.f;
	m[1][3]=0.f;

	m[2][0]=0.f;
	m[2][1]=0.f;
	m[2][2]=1.f;
	m[2][3]=0.f;

	m[3][0]=0.f;
	m[3][1]=0.f;
	m[3][2]=0.f;
	m[3][3]=1.f;
}

// eng: Matrix multiplication
// rus: Умножение матриц
void MP_MATRIX::MatrixMul(const MP_MATRIX* a, const MP_MATRIX* b)
{
	MP_MATRIX* c=this;
	c->_11=a->_11*b->_11+a->_12*b->_21+a->_13*b->_31+a->_14*b->_41;
	c->_21=a->_21*b->_11+a->_22*b->_21+a->_23*b->_31+a->_24*b->_41;
	c->_31=a->_31*b->_11+a->_32*b->_21+a->_33*b->_31+a->_34*b->_41;
	c->_41=a->_41*b->_11+a->_42*b->_21+a->_43*b->_31+a->_44*b->_41;

	c->_12=a->_11*b->_12+a->_12*b->_22+a->_13*b->_32+a->_14*b->_42;
	c->_22=a->_21*b->_12+a->_22*b->_22+a->_23*b->_32+a->_24*b->_42;
	c->_32=a->_31*b->_12+a->_32*b->_22+a->_33*b->_32+a->_34*b->_42;
	c->_42=a->_41*b->_12+a->_42*b->_22+a->_43*b->_32+a->_44*b->_42;

	c->_13=a->_11*b->_13+a->_12*b->_23+a->_13*b->_33+a->_14*b->_43;
	c->_23=a->_21*b->_13+a->_22*b->_23+a->_23*b->_33+a->_24*b->_43;
	c->_33=a->_31*b->_13+a->_32*b->_23+a->_33*b->_33+a->_34*b->_43;
	c->_43=a->_41*b->_13+a->_42*b->_23+a->_43*b->_33+a->_44*b->_43;

	c->_14=a->_11*b->_14+a->_12*b->_24+a->_13*b->_34+a->_14*b->_44;
	c->_24=a->_21*b->_14+a->_22*b->_24+a->_23*b->_34+a->_24*b->_44;
	c->_34=a->_31*b->_14+a->_32*b->_24+a->_33*b->_34+a->_34*b->_44;
	c->_44=a->_41*b->_14+a->_42*b->_24+a->_43*b->_34+a->_44*b->_44;
}


// eng: Calculation of perspective projection matrix (analogue of D3DXMatrixPerspectiveFovLH from Direct3D) 
// rus: Расчет матрицы перспективной проекции (аналог D3DXMatrixPerspectiveFovLH из Direct3D)
void MP_MATRIX::MatrixPerspectiveFovLH(float fovy, float aspect, float zn, float zf)
{
	float yScale=(float)(1/(tan(fovy/2.f)));
	float xScale=yScale/aspect;

	m[0][0]=xScale;
	m[1][0]=0.f;
	m[2][0]=0.f;
	m[3][0]=0.f;

	m[0][1]=0.f;
	m[1][1]=yScale;
	m[2][1]=0.f;
	m[3][1]=0.f;

	m[0][2]=0.f;
	m[1][2]=0.f;
	m[2][2]=zf/(zf-zn);
	m[3][2]=-zn*zf/(zf-zn);

	m[0][3]=0.f;
	m[1][3]=0.f;
	m[2][3]=1.f;
	m[3][3]=0.f;
}

// eng: Calculation of orthogonal projection (analogue of D3DXMatrixOrthoLH from Direct3D) 
// rus: Расчет матрицы ортогональной проекции (аналог D3DXMatrixOrthoLH из Direct3D)
void MP_MATRIX::MatrixOrthoLH(float width, float height, float zn, float zf)
{
	m[0][0]=2.f/width;
	m[1][0]=0.f;
	m[2][0]=0.f;
	m[3][0]=0.f;

	m[0][1]=0.f;
	m[1][1]=2.f/height;
	m[2][1]=0.f;
	m[3][1]=0.f;

	m[0][2]=0.f;
	m[1][2]=0.f;
	m[2][2]=1.f/(zf-zn);
	m[3][2]=zn/(zn-zf);

	m[0][3]=0.f;
	m[1][3]=0.f;
	m[2][3]=0.f;
	m[3][3]=1.f;
}

// eng: Calculation of view matrix (analogue of D3DXMatrixLookAtLH from Direct3D) 
// rus: Расчет матрицы вида (аналог D3DXMatrixLookAtLH из Direct3D)
void MP_MATRIX::MatrixLookAtLH(MP_POSITION* peye, MP_POSITION* pat, MP_POSITION* pup)
{
	MP_POSITION right, rightn, up, upn, vec, vec2; 

	vec2=(*pat)-(*peye);
	
	vec=vec2;
	vec.Normalize();

	right.Cross(pup,&vec);
	up.Cross(&vec,&right);

	rightn=right;
	rightn.Normalize();

	upn=up;
	upn.Normalize();
    
	m[0][0] = rightn.x;
    m[1][0] = rightn.y;
    m[2][0] = rightn.z;
    m[3][0] = -peye->Dot(&rightn); 
    m[0][1] = upn.x;
    m[1][1] = upn.y;
    m[2][1] = upn.z;
    m[3][1] = -peye->Dot(&upn);
    m[0][2] = vec.x;
    m[1][2] = vec.y;
    m[2][2] = vec.z;
    m[3][2] = -peye->Dot(&vec);
    m[0][3] = 0.0f; 
    m[1][3] = 0.0f; 
    m[2][3] = 0.0f; 
    m[3][3] = 1.0f; 
}

// --------------------------------------------------------------------------------------------------

// eng: Calculation of perspective projection matrix (analogue of gluPerspective from OpenGL) 
// rus: Расчет матрицы перспективной проекции (аналог gluPerspective из OpenGL)
void MP_MATRIX::MatrixPerspective_OpenGL(float fovy, float aspect, float zn, float zf)
{
	float sine, cotangent, deltaZ;
	float radians=(float)(fovy/2*MAGIC_PI/180);

	deltaZ=zf-zn;
	sine=(float)sin(radians);
	if ((deltaZ==0) || (sine==0) || (aspect==0))
	{
		MatrixIdentity();
		return;
	}
	cotangent=cosf(radians)/sine;

	m[0][0]=cotangent/aspect;
	m[1][1]=cotangent;
	m[2][2]=-(zf+zn)/deltaZ;
	m[2][3]=-1;
	m[3][2]=-2*zn*zf/deltaZ;
	m[3][3]=0.f;

	m[0][1]=0.f;
	m[0][2]=0.f;
	m[0][3]=0.f;
	m[1][0]=0.f;
	m[1][2]=0.f;
	m[1][3]=0.f;
	m[2][0]=0.f;
	m[2][1]=0.f;
	m[3][0]=0.f;
	m[3][1]=0.f;
}

// eng: Calculation of orthogonal projection (analogue of gluOrtho from OpenGL) 
// rus: Расчет матрицы ортогональной проекции (аналог gluOrtho из OpenGL)
void MP_MATRIX::MatrixOrtho_OpenGL(float left, float right, float top, float bottom, float zn, float zf)
{
	float width=right-left;
	float height=bottom-top;
	float depth=zf-zn;

	m[0][0]=2.f/width;
	m[1][0]=0.f;
	m[2][0]=0.f;
	m[3][0]=-((left+right)/width);

	m[0][1]=0.f;
	m[1][1]=2.f/height;
	m[2][1]=0.f;
	m[3][1]=-((top+bottom)/height);

	m[0][2]=0.f;
	m[1][2]=0.f;
	m[2][2]=-2.f/(depth);
	m[3][2]=-((zn+zf)/depth);

	m[0][3]=0.f;
	m[1][3]=0.f;
	m[2][3]=0.f;
	m[3][3]=1.f;
}

// eng: Calculation of view matrix (analogue of gluLookAt from OpenGL) 
// rus: Расчет матрицы вида (аналог gluLookAt из OpenGL)
void MP_MATRIX::MatrixLookAt_OpenGL(MP_POSITION* peye, MP_POSITION* pat, MP_POSITION* pup)
{
	MP_POSITION side, forward, up; 

	forward=(*pat)-(*peye);
	forward.Normalize();

	up=*pup;

	side.Cross(&forward,&up);
	side.Normalize();

	up.Cross(&side, &forward);

	m[0][0]=side.x;
	m[1][0]=side.y;
	m[2][0]=side.z;
	m[3][0]=0.f; 
	m[0][1]=up.x;
	m[1][1]=up.y;
	m[2][1]=up.z;
	m[3][1]=0.f;
	m[0][2]=-forward.x;
	m[1][2]=-forward.y;
	m[2][2]=-forward.z;
	m[3][2]=0.f;
	m[0][3]=0.0f; 
	m[1][3]=0.0f; 
	m[2][3]=0.0f; 
	m[3][3]=1.0f;

	MP_MATRIX offset;
	offset.MatrixIdentity();
	offset.m[3][0]=-peye->x; 
	offset.m[3][1]=-peye->y; 
	offset.m[3][2]=-peye->z; 

	MatrixMul(&offset, this);
}

// eng: Transposing of matrix
// rus: Транспонирование матрицы (смена местами строк и столбцов)
void MP_MATRIX::Transposing()
{
	MP_MATRIX v=*this;

	_12=v._21;
	_13=v._31;
	_14=v._41;

	_21=v._12;
	_23=v._32;
	_24=v._42;

	_31=v._13;
	_32=v._23;
	_34=v._43;

	_41=v._14;
	_42=v._24;
	_43=v._34;
}

// --------------------------------------------------------------------------------------------------

bool MAGIC_VERTEX_FORMAT_IsEqual(MAGIC_VERTEX_FORMAT* f1, MAGIC_VERTEX_FORMAT* f2)
{
	return (f1->attributes==f2->attributes && f1->UVs==f2->UVs);
}

// --------------------------------------------------------------------------------------------------

MP_BUFFER::MP_BUFFER()
{
	max_length=0;
	length=0;
}

MP_BUFFER::~MP_BUFFER()
{
	Destroy();
}

void MP_BUFFER::Create(int new_length, MP_Device* device)
{
	Destroy();
	max_length=new_length;
}

void MP_BUFFER::Destroy()
{
	max_length=0;
	length=0;
}

// --------------------------------

MP_BUFFER_RAM::MP_BUFFER_RAM() : MP_BUFFER()
{
	buffer=NULL;
}

MP_BUFFER_RAM::~MP_BUFFER_RAM()
{
	Destroy();
}

void MP_BUFFER_RAM::Create(int new_length, MP_Device* device)
{
	MP_BUFFER::Create(new_length, device);
	buffer=new char[new_length];
}

void MP_BUFFER_RAM::Destroy()
{
	MP_BUFFER::Destroy();

	if (buffer)
	{
		delete []buffer;
		buffer=NULL;
	}
}

// --------------------------------------------------------------------------------------------------

#ifdef SHADER_WRAP

MP_MATERIAL::MP_MATERIAL(MAGIC_MATERIAL* m, MP_Device* device)
{
	material=*m;
	MP_TechniqueList* tech_list=device->GetTechniqueList();
	technique_index=tech_list->Select(&material, device);
}

void MP_ELEMENT_SHADER::Save(MP_Manager* MP, const char* path)
{
	MP_FILE_DATA fd;
	fd.data=(unsigned char*)shader_code.c_str();
	fd.length=shader_code.size();
	MP->SaveFile(path, &fd);
	fd.DetachData();
}

MP_VERTEX_SHADER::MP_VERTEX_SHADER() : MP_ELEMENT_SHADER()
{
}

bool MP_VERTEX_SHADER::IsEqual(MAGIC_MATERIAL* m)
{
	return (material->format.UVs==m->format.UVs && material->format.attributes==m->format.attributes);
}

MP_PIXEL_SHADER::MP_PIXEL_SHADER() : MP_ELEMENT_SHADER()
{
}

bool MP_PIXEL_SHADER::IsEqual(MAGIC_MATERIAL* m)
{
	#ifdef SHADER_ALPHATEST_WRAP
	if ((material->flags & MAGIC_MATERIAL_ALPHATEST)!=(m->flags & MAGIC_MATERIAL_ALPHATEST))
		return false;
	#endif

	if (material->textures==m->textures)
	{
		for (int i=0;i<m->textures;i++)
		{
			MAGIC_TEXTURE_STATES* s1=&(material->states[i]);
			MAGIC_TEXTURE_STATES* s2=&(m->states[i]);

			if (s1->operation_rgb!=s2->operation_rgb || s1->argument_rgb1!=s2->argument_rgb1 || s1->argument_rgb2!=s2->argument_rgb2 || s1->operation_alpha!=s2->operation_alpha || s1->argument_alpha1!=s2->argument_alpha1 || s1->argument_alpha2!=s2->argument_alpha2)
				return false;
		}
		return true;
	}

	return false;
}

MP_TECHNIQUE::MP_TECHNIQUE() : MP_ELEMENT()
{
	vertex_shader_index=pixel_shader_index=-1;
}


MP_ElementList::MP_ElementList()
{
	k_element=0;
	m_element=NULL;
}

MP_ElementList::~MP_ElementList()
{
	Clear();
}

void MP_ElementList::Clear()
{
	for (int i=0;i<k_element;i++)
	{
		delete m_element[i];
		m_element[i]=NULL;
	}

	if (m_element)
	{
		delete []m_element;
		m_element=NULL;
	}

	k_element=0;
}

int MP_ElementList::Add(MP_ELEMENT* element)
{
	if (k_element)
	{
		MP_ELEMENT** vm_element=new MP_ELEMENT*[k_element+1];

		for (int i=0;i<k_element;i++)
			vm_element[i]=m_element[i];

		delete []m_element;
		m_element=vm_element;
	}
	else
	{
		m_element=new MP_ELEMENT*[1];
	}

	m_element[k_element]=element;
	k_element++;
	return k_element-1;
}

int MP_ElementList::Select(MAGIC_MATERIAL* m, MP_Device* device)
{
	for (int i=0;i<k_element;i++)
	{
		if (m_element[i]->IsEqual(m))
			return i;
	}

	MP_ELEMENT* element=NewElement(device);
	element->Generate(m, device);

	return Add(element);
}


MP_ELEMENT* MP_VShaderList::NewElement(MP_Device* device)
{
	return device->NewVertexShader();
}

MP_ELEMENT* MP_PShaderList::NewElement(MP_Device* device)
{
	return device->NewPixelShader();
}


int MP_TechniqueList::Select(MAGIC_MATERIAL* m, MP_Device* device)
{
	MP_VShaderList* vs_list=device->GetVShaderList();
	int vs=vs_list->Select(m, device);

	MP_PShaderList* ps_list=device->GetPShaderList();
	int ps=ps_list->Select(m, device);

	MP_TECHNIQUE* tech;
	for (int i=0;i<k_element;i++)
	{
		tech=(MP_TECHNIQUE*)m_element[i];
		if (tech->vertex_shader_index==vs && tech->pixel_shader_index==ps)
			return i;
	}

	tech=(MP_TECHNIQUE*)NewElement(device);
	tech->vertex_shader_index=vs;
	tech->pixel_shader_index=ps;
	tech->Generate(m, device);

	return Add(tech);
}


MP_ELEMENT* MP_TechniqueList::NewElement(MP_Device* device)
{
	return device->NewTechnique();
}

#endif


#ifdef LAYOUT_WRAP

MP_LayoutList::MP_LayoutList()
{
	k_layout=0;
	m_layout=NULL;
}

MP_LayoutList::~MP_LayoutList()
{
	Clear();
}

void MP_LayoutList::Clear()
{
	for (int i=0;i<k_layout;i++)
	{
		if (m_layout[i])
		{
			delete m_layout[i];
			m_layout[i]=NULL;
		}
	}

	if (m_layout)
	{
		delete []m_layout;
		m_layout=NULL;
	}

	k_layout=0;
}

int MP_LayoutList::GetIndex(MAGIC_VERTEX_FORMAT* format)
{
	int index=format->UVs;
	if (!(format->attributes & MAGIC_ATTRIBUTE_COLOR))
		index+=32;
	return index;
}

int MP_LayoutList::Select(MAGIC_VERTEX_FORMAT* format, MP_Device* device)
{
	int i;

	int index=GetIndex(format);

	if (k_layout<=index)
	{
		// необходимо нарастить массив
		MP_LAYOUT** vm_layout=new MP_LAYOUT*[index+1];

		for (i=0;i<k_layout;i++)
			vm_layout[i]=m_layout[i];

		if (m_layout)
			delete []m_layout;
		m_layout=vm_layout;

		for (i=k_layout;i<=index;i++)
			m_layout[i]=NULL;

		k_layout=index+1;
	}

	if (!m_layout[index])
	{
		m_layout[index]=device->NewLayout();
		m_layout[index]->Generate(format, device);
	}

	return index;
}

#endif

// --------------------------------------------------------------------------------------------------

struct TargaFileHeader
{
	char idLength;
	char colourMapType;
	char dataTypeCode;
	short colourMapOrigin;
	short colourMapLength;
	char colourMapDepth;
	short xOrigin;
	short yOrigin;
	short width;
	short height;
	char bitsPerPixel;
	char imageDescriptor;
};

// eng: Saves TGA for testing
// rus: Сохраняет TGA для тестирования
void SaveTGA(int w, int h, unsigned char* pixel, const char* file)
{
	int components=4;
	TargaFileHeader fileHeader;
	memset(&fileHeader, 0, sizeof(TargaFileHeader));
	fileHeader.dataTypeCode=2;
	fileHeader.bitsPerPixel= components*8;
	fileHeader.width=w;
	fileHeader.height=h;
	fileHeader.imageDescriptor= components==4? 8: 0;

	FILE* f=fopen(file, "wb");
	fwrite(&fileHeader.idLength, sizeof(fileHeader.idLength), 1, f); 
	fwrite(&fileHeader.colourMapType, sizeof(fileHeader.colourMapType), 1, f);
	fwrite(&fileHeader.dataTypeCode, sizeof(fileHeader.dataTypeCode), 1, f);
	fwrite(&fileHeader.colourMapOrigin, sizeof(fileHeader.colourMapOrigin), 1, f);
	fwrite(&fileHeader.colourMapLength, sizeof(fileHeader.colourMapLength), 1, f);
	fwrite(&fileHeader.colourMapDepth, sizeof(fileHeader.colourMapDepth), 1, f);
	fwrite(&fileHeader.xOrigin, sizeof(fileHeader.xOrigin), 1, f);
	fwrite(&fileHeader.yOrigin, sizeof(fileHeader.yOrigin), 1, f);
	fwrite(&fileHeader.width, sizeof(fileHeader.width), 1, f);
	fwrite(&fileHeader.height, sizeof(fileHeader.height), 1, f);
	fwrite(&fileHeader.bitsPerPixel, sizeof(fileHeader.bitsPerPixel), 1, f);
	fwrite(&fileHeader.imageDescriptor, sizeof(fileHeader.imageDescriptor), 1, f);

	fwrite(pixel, 1, w*h*components, f);

	fclose(f);
}
