//-----------------------------------------------------------------------------
// File: mp_config.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_CONFIG
#define MAGIC_PARTICLES_CONFIG

#define SHADER_WRAP

#ifdef SHADER_WRAP
	#define SHADER_ALPHATEST_WRAP
#endif

#if defined(_WINDOWS) && !defined(COCOS_2DX) && !defined(__S3E__)
#define OPENGL_WIN
#endif

#endif
