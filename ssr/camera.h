#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "inc.h"
#include <iostream>
using namespace std;
using namespace glm;
class Camera
{
public:
	static float PI;
	static float AngleVel_XZ;
	static float AngleVel_Y;
	static float Vel_XZ;
	static float Margin;
public:
	int WindowWidth,WindowHeight;
	vec3 CameraPos;
	vec3 Direction;
	vec3 DirectionXZ;
	vec3 HorizonXZ;
	vec3 Horizon;
	vec3 Up;
	int MousePos_x,MousePos_y;
	float Angle_XZ,Angle_Y;
	bool OnLeftEdge,OnRightEdge,OnTopEdge, OnButtonEdge;
	float fovy, aspect, nearz, farz;
public:
	Camera(int WindowWidth, int WindowHeight, vec3 CameraPos, float Angle_XZ, float Angle_Y, float fovy=45, float nearz=1.0f,float farz=1000.0) :
		WindowWidth(WindowWidth), WindowHeight(WindowHeight), CameraPos(CameraPos), Angle_XZ(Angle_XZ), Angle_Y(Angle_Y),
		fovy(fovy),nearz(nearz), farz(farz)
	{
		CalculateVector();
		MousePos_x = MousePos_y = -1;
		aspect = float(WindowWidth) / WindowHeight;
		glutWarpPointer(MousePos_x, MousePos_y);
	}
	void CalculateVector()
	{
		Direction=vec3(sin(Angle_Y)*cos(Angle_XZ),cos(Angle_Y),sin(Angle_Y)*sin(Angle_XZ));
		Direction = normalize(Direction);
		DirectionXZ=vec3(Direction.x,0.0,Direction.z);
		HorizonXZ=cross(vec3(Direction.x,fabs(Direction.y),Direction.z),DirectionXZ);
		Up=normalize(cross(Direction,HorizonXZ));
		Horizon = normalize(cross(Direction, Up));
	}
	void SetMousePos(int MousePosX, int MousePosY)
	{
		MousePos_x = MousePosX;
		MousePos_y = MousePosY;
	}
	void OnMouseMove(int x,int y)
	{
		if (MousePos_x == -1) return;
		int delta_x=x-MousePos_x;
		int delta_y=y-MousePos_y;
		MousePos_x=x;MousePos_y=y;
		Angle_XZ=Angle_XZ+delta_x*AngleVel_XZ;

		float Angle_Y_Next=Angle_Y+delta_y*AngleVel_Y;
		if (Angle_Y_Next>=0 && Angle_Y_Next<=PI)
			Angle_Y=Angle_Y_Next;

		OnLeftEdge=false;
		OnRightEdge=false;
		OnTopEdge=false;
		OnButtonEdge=false;
		if (delta_x==0)
		{
			if (x<Margin)
				OnLeftEdge=true;
			else if (x>WindowWidth-Margin)
				OnRightEdge=true;
		}
		if (delta_y==0)
		{
			if (y<Margin)
				OnTopEdge=true;
			else if (y>WindowHeight-Margin)
				OnButtonEdge=true;
		}

		CalculateVector();
	}
	void OnMoveEdge()
	{
		bool Moved=false;
		if (OnLeftEdge)
		{
			Angle_XZ-=AngleVel_XZ;
			Moved=true;
		}
		else if (OnRightEdge)
		{
			Angle_XZ+=AngleVel_XZ;
			Moved=true;
		}
		else if (OnTopEdge)
		{
			if (Angle_Y-AngleVel_Y>=0)
				Angle_Y-=AngleVel_Y;
			Moved=true;
		}
		else if (OnButtonEdge)
		{
			if (Angle_Y+AngleVel_Y<=PI)
				Angle_Y+=AngleVel_Y;
			Moved=true;
		}
		if (Moved)
			CalculateVector();
	}
	void Move(int step)
	{
		CameraPos += step*Vel_XZ*glm::normalize(Direction);
	}
	void MoveXZ(int step)
	{
		CameraPos+=step*Vel_XZ*glm::normalize(DirectionXZ);
	}
	void MoveXZ_LR(int step)
	{
		CameraPos+=step*Vel_XZ*glm::normalize(HorizonXZ);
	}
	~Camera()
	{
	}

	mat4 GetViewMatrix()
	{
		return lookAt(CameraPos, CameraPos + Direction, Up);
	}
	mat4 GetPerspectiveMatrix()
	{
		return perspective(fovy, aspect, nearz, farz);
	}
	vec3 GetCameraPos()
	{
		return CameraPos;
	}
	float GetScreenArea()
	{
		float height = nearz*tan(fovy / 2.0)*2.0;
		float width = height*aspect;
		return width*height;
	}
	void print()
	{
		cout << "camera pos=(" << CameraPos.x << "," << CameraPos.y << "," << CameraPos.z << ")"<< endl;
		cout << "camera dir=(" << Direction.x << "," << Direction.y << "," << Direction.z << ")" << endl;
		cout << "camera horiz=(" << Horizon.x << "," << Horizon.y << "," << Horizon.z << ")" << endl;
		cout << "camera up=(" << Up.x << "," << Up.y << "," << Up.z << ")" << endl;
	}
};

#endif