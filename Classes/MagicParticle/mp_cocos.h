//-----------------------------------------------------------------------------
// File: mp_cocos.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_COCOS
#define MAGIC_PARTICLES_COCOS

#include "mp_wrap.h"

class MagicEmitter;

class MP_Emitter_Cocos : public MP_Emitter
{
protected:
	MagicEmitter* node;

public:
	MP_Emitter_Cocos(HM_EMITTER emitter, MP_Manager* owner);
	virtual ~MP_Emitter_Cocos();

	void SetNode(MagicEmitter* node);
	MagicEmitter* GetNode();
	void ResetNode();

	// eng: Position of emitter
	// rus: Позиция эмиттера
	virtual void SetPosition(MP_POSITION& position);
};

class MP_Device_Cocos : public MP_Device_WRAP
{
public:
	MP_Device_Cocos(int width, int height);
	virtual ~MP_Device_Cocos();

	virtual MP_Emitter* NewEmitter(HM_EMITTER emitter, MP_Manager* owner);
};

#endif