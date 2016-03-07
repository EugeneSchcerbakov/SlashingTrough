//-----------------------------------------------------------------------------
// File: mp_cocos.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "mp_cocos.h"
#include "MagicEmitter.h"

// -------------------------------------------------------------------------------------------

MP_Emitter_Cocos::MP_Emitter_Cocos(HM_EMITTER emitter, MP_Manager* owner) : MP_Emitter(emitter, owner)
{
	node=NULL;
}

MP_Emitter_Cocos::~MP_Emitter_Cocos()
{
	if (node)
	{
		node->ResetEmitter();
	}
}

void MP_Emitter_Cocos::SetNode(MagicEmitter* node)
{
	this->node=node;
}

MagicEmitter* MP_Emitter_Cocos::GetNode()
{
	return node;
}

void MP_Emitter_Cocos::ResetNode()
{
	node=NULL;
}

// eng: Position of emitter
// rus: Позиция эмиттера
void MP_Emitter_Cocos::SetPosition(MP_POSITION& position)
{
	if (node)
	{
		node->ResetEmitter();
		#ifdef MAGIC_3D
		node->cocos2d::Node::setPosition3D(cocos2d::Vec3(position.x, position.y, position.z));
		#else
		node->cocos2d::Node::setPosition(position.x, position.y);
		#endif
		node->SetEmitter(this);
	}
	MP_Emitter::SetPosition(position);
}

// -------------------------------------------------------------------------------------------

MP_Device_Cocos::MP_Device_Cocos(int width, int height) : MP_Device_WRAP(width, height)
{
}

MP_Device_Cocos::~MP_Device_Cocos()
{
}

MP_Emitter* MP_Device_Cocos::NewEmitter(HM_EMITTER emitter, MP_Manager* owner)
{
	return new MP_Emitter_Cocos(emitter, owner);
}

