/*************************************************
Author: Stan
Created: 2019-6-4
Note: showInfo common headers.
************************************************/

#pragma once

#ifdef __GLES2__ 

#ifdef __APPLE__
#include "OpenGLES/ES2/gl.h"
#include "OpenGLES/ES2/glext.h"
#else
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#endif

#else

#include <glad/glad.h>
#include <glm/glm.hpp>
#endif

#include "common/log.h"
