#ifndef _INC_H_
#define _INC_H_

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

//#ifndef FREEGLUT_STATIC
//#define FREEGLUT_STATIC
//#endif

#pragma comment (lib, "lib/glew32s.lib")
#pragma comment (lib, "lib/assimp.lib")
#pragma comment (lib, "lib/FreeImage.lib")
#pragma comment (lib, "lib/freeglut.lib")
#pragma comment (lib, "lib/AntTweakBar64.lib")

#include "include\gl\glew.h"
#include "include\gl\freeglut.h"
#include "include\glm\glm.hpp"
#include "include\glm\gtc\matrix_transform.hpp"
#include "include\assimp\Importer.hpp"
#include "include\assimp\scene.h"
#include "include\assimp\postprocess.h"
#include "include\freeimage\FreeImage.h"
#include "include\ATB\AntTweakBar.h"

#endif