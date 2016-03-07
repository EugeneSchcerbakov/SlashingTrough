
#include "MagicEmitter.h"
#include "mp_cocos.h"

using namespace cocos2d;

MagicEmitter::MagicEmitter() : cocos2d::Node()
{
	emitter=NULL;
}

MagicEmitter::~MagicEmitter()
{
	if (emitter)
	{
		emitter->ResetNode();
		MP_Manager& MP=MP_Manager::GetInstance();
		HM_EMITTER hmEmitter=emitter->GetEmitter();
		MP.DeleteEmitter(hmEmitter);
	}
}

MagicEmitter* MagicEmitter::create(const char* emitterName)
{
	MP_Manager& MP=MP_Manager::GetInstance();
	MP_Emitter_Cocos* e=(MP_Emitter_Cocos*)MP.GetEmitterByName(emitterName);
	if (e)
		return create(e);
	return NULL;
}

MagicEmitter* MagicEmitter::create(HM_EMITTER hmEmitter)
{
	MP_Manager& MP=MP_Manager::GetInstance();
	MP_Emitter_Cocos* e=(MP_Emitter_Cocos*)MP.GetEmitter(hmEmitter);
	if (e)
		return create(e);
	return NULL;
}

MagicEmitter* MagicEmitter::create(MP_Emitter_Cocos* emitter)
{
	MagicEmitter* node=new MagicEmitter();

	MP_Manager& MP=MP_Manager::GetInstance();
	HM_EMITTER hmEmitter=emitter->GetEmitter();
	HM_EMITTER copy=MP.DuplicateEmitter(hmEmitter);
	emitter=(MP_Emitter_Cocos*)MP.GetEmitter(copy);

	node->emitter=emitter;
	emitter->SetNode(node);
	emitter->SetState(MAGIC_STATE_UPDATE);

	MP_POSITION pos;
	emitter->GetPosition(pos);
	node->cocos2d::Node::setPosition(pos.x, pos.y);
	#ifdef MAGIC_3D
	node->cocos2d::Node::setPosition3D(cocos2d::Vec3(pos.x, pos.y, pos.z));
	#endif

	node->scheduleUpdate();

	return node;
}

void MagicEmitter::ResetEmitter()
{
	emitter=NULL;
}

void MagicEmitter::update(float delta) 
{
	Node::update(delta);

	if (emitter)
	{
		double rate=delta*1000;
		if (rate>500)
			rate=0.01;
		emitter->Update(rate);
	}
}

void MagicEmitter::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	Node::draw(renderer, transform, flags);

	if (emitter)
	{
		//#ifdef MAGIC_3D
		//const Camera* camera=Camera::getVisitingCamera();
		//Mat4 mat=camera->getViewProjectionMatrix();

		//Mat4 mat_view=camera->getViewMatrix();
		//MAGIC_CAMERA cam;
		//cam.mode=MAGIC_CAMERA_PERSPECTIVE;
		//cam.dir.x=mat_view.m[8];
		//cam.dir.y=mat_view.m[9];
		//cam.dir.z=mat_view.m[10];
		//Vec3 pos=camera->getPosition3D();
		//cam.pos.x=pos.x;
		//cam.pos.y=pos.y;
		//cam.pos.z=pos.z;
		//#endif
				
		_customCommand.init(_globalZOrder);
		//#ifdef MAGIC_3D
		//_customCommand.func = [this, mat, cam]()
		//#else
		_customCommand.func = [this]()
		//#endif
		{
			//#ifdef MAGIC_3D
			//Magic_SetCamera((MAGIC_CAMERA*)&cam);
			//emitter->device->SetProjectionMatrix((MAGIC_MATRIX*)mat.m);
			//#endif

			emitter->Render();
		};
		renderer->addCommand(&_customCommand);
	}
}

void MagicEmitter::setPosition(float x, float y)
{
	if (emitter)
	{
		MP_POSITION pos;
		pos.x=x;
		pos.y=y;
		#ifdef MAGIC_3D
		pos.z=0.f;
		#endif
		emitter->ResetNode();
		emitter->SetPosition(pos);
		emitter->SetNode(this);
	}
	else
		Node::setPosition(x, y);
}

void MagicEmitter::setScale(float scale)
{
    emitter->SetScale(scale);
}

#ifdef MAGIC_3D
void MagicEmitter::setPosition3D(float x, float y, float z)
{
	if (emitter)
	{
		MP_POSITION pos;
		pos.x=x;
		pos.y=y;
		pos.z=z;
		emitter->ResetNode();
		emitter->SetPosition(pos);
		emitter->SetNode(this);
	}
	else
		Node::setPosition3D(cocos2d::Vec3(x, y, z));
}
#endif
