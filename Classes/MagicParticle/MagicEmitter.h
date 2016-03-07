#pragma once

#include "cocos2d.h"
#include "magic.h"

class MP_Emitter_Cocos;

class MagicEmitter : public cocos2d::Node
{
protected:
	MP_Emitter_Cocos* emitter;
	cocos2d::CustomCommand _customCommand;

public:
	static MagicEmitter* create(const char* emitterName);
	static MagicEmitter* create(HM_EMITTER hmEmitter);
	static MagicEmitter* create(MP_Emitter_Cocos* emitter);

	MP_Emitter_Cocos* GetEmitter(){return emitter;}
	void SetEmitter(MP_Emitter_Cocos* emitter){this->emitter=emitter;}
	void ResetEmitter();
	
	virtual void update(float delta);
	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);

	virtual void setPosition(const cocos2d::Vec2 &position){setPosition(position.x, position.y);}
	virtual void setPosition(float x, float y);

	#ifdef MAGIC_3D
	virtual void setPosition3D(const cocos2d::Vec3 &position){setPosition3D(position.x, position.y, position.z);}
	virtual void setPosition3D(float x, float y, float z);
	#endif

	virtual ~MagicEmitter();
protected:
	MagicEmitter();
};
