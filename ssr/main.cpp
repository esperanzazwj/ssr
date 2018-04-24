#include <iostream>
#include "inc.h"
#include "compiler.h"
#include "skybox.h"
#include "camera.h"
#include "scene.h"
#include "frameBuffer.h"
#include "raytracer.h"
#include "light.h"

#define PI 3.1415926
using namespace std;
const int wndWidth = 1280, wndHeight = 720;
const float cameraX = 0.0, cameraY = 2.0, cameraZ = 100.0;
const float angleXZ = -PI / 2, angleY = PI / 2;

GLsync syn;

Camera *camera = NULL;
vector<Skybox*> skyboxs;
vector<Scene*> scenes;
FrameBuffer *framebuffer0 = NULL;
FrameBuffer *framebuffer1 = NULL;
GLuint envProgram;
GLuint ssrProgram;
GLuint backProgram;
GLuint raytracingProgram;
GLuint raytracingProgram2;
Raytracer *raytracer = NULL;
Light *light = NULL;
//parameters
float index = 1.333;
typedef enum { SIMPLE, SSR, RAYTRACING2, RAYTRACING } MODE_TYPE;
MODE_TYPE mode = SIMPLE;
typedef enum { BUDDHA, DRAGON, HEAD, MITSUBA,TEAPOT } MESH_TYPE;
MESH_TYPE scene = BUDDHA;
typedef enum { SKYBOX0, SKYBOX1, SKYBOX2, SKYBOX3 } SKYBOX_TYPE;
SKYBOX_TYPE skybox = SKYBOX0;
bool test = false;
bool hasLocalShading = false;

void init()
{
	Compiler::glGenProgramFromFile("env","shader/env_vs.glsl", NULL, NULL, NULL, "shader/env_fs.glsl");
	Compiler::glGenProgramFromFile("back", "shader/back_vs.glsl", NULL, NULL, NULL, "shader/back_fs.glsl");
	Compiler::glGenProgramFromFile("ssr", "shader/ssr_vs.glsl", NULL, NULL, NULL, "shader/ssr_fs.glsl");
	Compiler::glGenProgramFromFile("raytracer2", "shader/raytracer_vs.glsl", NULL, NULL, NULL, "shader/raytracer_fs2.glsl");

	raytracer = new Raytracer();
	raytracingProgram = raytracer->getProgram();
	raytracingProgram2 = Compiler::container["raytracer2"];

	const char *filename0[] = { "skybox/left.jpg", "skybox/right.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/back.jpg", "skybox/front.jpg" };
	const char *filename1[] = { "skybox/bloody-heresy_bk.tga", "skybox/bloody-heresy_ft.tga" , "skybox/bloody-heresy_up.tga", "skybox/bloody-heresy_dn.tga", "skybox/bloody-heresy_rt.tga", "skybox/bloody-heresy_lf.tga" };
	const char *filename2[] = { "skybox/violentdays_bk.tga", "skybox/violentdays_ft.tga" , "skybox/violentdays_up.tga","skybox/violentdays_dn.tga", "skybox/violentdays_rt.tga", "skybox/violentdays_lf.tga" };
	const char *filename3[] = { "skybox/lmcity_bk.tga", "skybox/lmcity_ft.tga", "skybox/lmcity_up.tga", "skybox/lmcity_dn.tga", "skybox/lmcity_rt.tga", "skybox/lmcity_lf.tga" };

	skyboxs.push_back(new Skybox(filename0,2048));
	skyboxs.push_back(new Skybox(filename1, 512));
	skyboxs.push_back(new Skybox(filename2, 1024));
	skyboxs.push_back(new Skybox(filename3, 512));

	scenes.push_back(new Scene("budda/buddha_5k.obj"));
	scenes.push_back(new Scene("dragon/dragon_x.obj"));
	scenes.push_back(new Scene("head/head.obj"));
	scenes.push_back(new Scene("mitsuba/mitsuba-sphere.obj"));
	scenes.push_back(new Scene("teapot/teapot.obj"));
	//scenes.push_back(new Scene("sphere/sphere.obj"));

	camera = new Camera(wndWidth, wndHeight, vec3(cameraX, cameraY, cameraZ), angleXZ, angleY);

	framebuffer0 = new FrameBuffer(wndWidth, wndHeight, 2);
	framebuffer1 = new FrameBuffer(wndWidth, wndHeight, 1);

	envProgram = Compiler::container["env"];
	ssrProgram = Compiler::container["ssr"];
	backProgram = Compiler::container["back"];

	light = new Light();
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void renderBack(GLuint program)
{
	glUseProgram(program);
	mat4 gWorld = mat4(1.0);
	mat4 gViewProjectMatrix = camera->GetPerspectiveMatrix()*camera->GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program, "gWorld"), 1, GL_FALSE, &gWorld[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "gViewProjectMatrix"), 1, GL_FALSE, &gViewProjectMatrix[0][0]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxs[skybox]->getSkybox());
	glUniform1i(glGetUniformLocation(program, "skybox"), 3);
	scenes[scene]->RenderScene(program);
}

void render(GLuint program)
{
	glUseProgram(program);
	mat4 gWorld = mat4(1.0);
	mat4 gViewProjectMatrix = camera->GetPerspectiveMatrix()*camera->GetViewMatrix();
	vec3 EyePosition = camera->GetCameraPos();
	glUniformMatrix4fv(glGetUniformLocation(program, "gWorld"), 1, GL_FALSE, &gWorld[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "gViewProjectMatrix"), 1, GL_FALSE, &gViewProjectMatrix[0][0]);
	glUniform3fv(glGetUniformLocation(program, "EyePosition"), 1, &EyePosition[0]);
	glUniform1f(glGetUniformLocation(program, "index"), index);
	glUniform1i(glGetUniformLocation(program, "hasLocalShading"), hasLocalShading);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer0->GetColorTexture(0));
	glUniform1i(glGetUniformLocation(program, "backWorldPos"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, framebuffer0->GetColorTexture(1));
	glUniform1i(glGetUniformLocation(program, "backWorldNorm"), 1);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxs[skybox]->getSkybox());
	glUniform1i(glGetUniformLocation(program, "skybox"), 3);

	light->setLightProperties(program);

	scenes[scene]->RenderScene(program);
}

void raytracing(GLuint raytracingProgram)
{
	glUseProgram(raytracingProgram);
	vec3 pos = camera->CameraPos;
	float nearz = 1.0f / tan(58.5f / 180.0f * 3.1415926 / 2.0f);
	float aspect = camera->aspect;
	vec3 direction = camera->Direction;
	vec3 horizontal = camera->Horizon;
	vec3 up = camera->Up;
	glUniform1f(glGetUniformLocation(raytracingProgram, "index"), index);
	glUniform3fv(glGetUniformLocation(raytracingProgram, "camera.pos"), 1, &pos[0]);
	glUniform1f(glGetUniformLocation(raytracingProgram, "camera.nearz"), nearz);
	glUniform1f(glGetUniformLocation(raytracingProgram, "camera.aspect"), aspect);
	glUniform3fv(glGetUniformLocation(raytracingProgram, "camera.direction"), 1, &direction[0]);
	glUniform3fv(glGetUniformLocation(raytracingProgram, "camera.horizontal"), 1, &horizontal[0]);
	glUniform3fv(glGetUniformLocation(raytracingProgram, "camera.up"), 1, &up[0]);
	glUniform1i(glGetUniformLocation(raytracingProgram, "hasLocalShading"), hasLocalShading);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxs[skybox]->getSkybox());
	glUniform1i(glGetUniformLocation(raytracingProgram, "skybox"), 3);
	light->setLightProperties(raytracingProgram);
	int idx = 0;
	if (scene == MITSUBA) idx = 1;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scenes[scene]->m_Meshes[idx].GPUVBO[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, scenes[scene]->m_Meshes[idx].GPUVBO[0]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scenes[scene]->m_Meshes[idx].GPUVBO[1]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scenes[scene]->m_Meshes[idx].GPUVBO[1]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scenes[scene]->m_Meshes[idx].GPUVBO[2]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, scenes[scene]->m_Meshes[idx].GPUVBO[2]);
	raytracer->render();
}
void display()
{
	//camera->OnMoveEdge();
	
	//äÖÈ¾±³Ãæµ½Ö¡»º´æ
	glDepthFunc(GL_GREATER);
	glClearDepth(0);
	framebuffer0->Begin();
	renderBack(backProgram);
	framebuffer0->End();
	glClearDepth(1);
	glDepthFunc(GL_LESS);

	//Êµ¼ÊÆÁÄ»
	framebuffer1->Begin();
	if (mode == SIMPLE)
	{
		render(envProgram);
		skyboxs[skybox]->Render(camera);
	}
	else if (mode == SSR)
	{
		render(ssrProgram);
		skyboxs[skybox]->Render(camera);
	}
	else if (mode==RAYTRACING2)
	{
		raytracing(raytracingProgram2);
	}
	else
	{
		raytracing(raytracingProgram);
	}
	framebuffer1->End();

	if (mode == RAYTRACING2 || mode == RAYTRACING)
	{

		syn = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		while (true)
		{
			GLenum state = glClientWaitSync(syn, 0, 1e9);
			cout << "gain state" << endl;
			if (state == GL_ALREADY_SIGNALED)
			{
				break;
			}
			else
			{
				cout << "waiting" << endl;
			}
		}
		glDeleteSync(syn);
	}

	if (test)
	{
		test = false;
		cout << "---------------begin test--------------" << endl;
		cout << "index: " << index << endl;
		camera->print();
		Texture2D *result = framebuffer1->GetColorBufferData(0);
		result->SaveAsPfmFile("Output/result.pfm", false, 2.2);
		Texture2D *backPos= framebuffer0->GetColorBufferData(0);
		backPos->SaveAsPfmFile("Output/BackPos.pfm", false, 2.2);
		Texture2D *backnorm = framebuffer0->GetColorBufferData(1);
		backnorm->SaveAsPfmFile("Output/BackNorm.pfm", false, 2.2);
		delete backnorm;
		delete backPos;
		delete result;
		cout << "---------------end test--------------" << endl;
	}

	framebuffer1->Bilt(0);

	TwDraw();

	glutSwapBuffers();

}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
}

void kbdown(unsigned char key, int x, int y)
{
	if (!TwEventKeyboardGLUT(key, x, y))
	{
		switch (key)
		{
		case '1':mode = SIMPLE; 
			break;
		case '2':mode = SSR;
			break;
		case '3':mode = RAYTRACING2; 
			break;
		case '4':mode = RAYTRACING;
			break;
		case 'w': camera->Move(1);
			break;
		case 'a': camera->MoveXZ_LR(1);
			break;
		case 's': camera->Move(-1);
			break;
		case 'd': camera->MoveXZ_LR(-1);
			break;
		case 'z': index += 0.01;
			break;
		case 'x': index -= 0.01;
			break;
		case 'p':
			test = true;
			break;
		case 'c':hasLocalShading = !hasLocalShading; break;
		case 'e':
			glutExit();
			break;
		}
	}
}

void mouseButtonDown(int button, int state, int mouseX, int mouseY)
{
	if (!TwEventMouseButtonGLUT(button, state, mouseX, mouseY))
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			camera->SetMousePos(mouseX, mouseY);
		}
		else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		{
			camera->SetMousePos(-1, -1);
		}
	}
	
}
void mouseActiveMove(int mouseX, int mouseY)
{
	if (!TwEventMouseMotionGLUT(mouseX,mouseY))
	{
		camera->OnMouseMove(mouseX, mouseY);
	}
	
}
void TW_CALL localShadingCB(void *p)
{
	hasLocalShading = !hasLocalShading;
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wndWidth, wndHeight);
	glutInitContextVersion(4, 4);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	glutCreateMenu(NULL);
	glewExperimental = true;

	glutGameModeString("1280x720:32");

	if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
		glutEnterGameMode();

	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		exit(0);
	}

	printf("OpenGL version supported by this platform (%s): \n", \
		glGetString(GL_VERSION));

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(kbdown);
	glutMouseFunc(mouseButtonDown);
	glutMotionFunc(mouseActiveMove);
	glutIdleFunc(display);

	if (TwInit(TW_OPENGL_CORE, NULL) == 0)
	{
		std::cerr << "Failed to initialize ATB" << std::endl;
	}
	
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);

	// Create a tweak bar
	TwBar *bar = TwNewBar("Options");
	TwAddVarRW(bar, "index", TW_TYPE_FLOAT, &index,	" min=1.0 max=4.0 step=0.01");
	TwEnumVal modes[] = { { SIMPLE, "Simple" }, { SSR, "SSR" }, { RAYTRACING2, "Raytracing2" },{ RAYTRACING, "Raytracing" } };
	TwType modeTwType = TwDefineEnum("ModeType", modes, 4);
	TwAddVarRW(bar, "Mode", modeTwType, &mode, NULL);
	TwAddButton(bar, "localShading", localShadingCB, NULL, " label='localShading' ");
	TwEnumVal meshes[] = { { BUDDHA, "Buddha" }, { DRAGON, "Dragon" }, { HEAD, "Head" }, { MITSUBA, "Mitsuba" }, { TEAPOT, "Teapot" } };
	TwType meshTwType = TwDefineEnum("MeshType", meshes, 5);
	TwAddVarRW(bar, "Mesh", meshTwType, &scene, NULL);
	TwEnumVal skyboxs_[] = { { SKYBOX0, "Skybox0" }, { SKYBOX1, "Skybox1" }, { SKYBOX2, "Skybox2" }, { SKYBOX3, "Skybox3" } };
	TwType skyboxTwType = TwDefineEnum("skyboxType", skyboxs_, 4);
	TwAddVarRW(bar, "Skybox", skyboxTwType, &skybox, NULL);
	glutMainLoop();
	
	return 0;
}