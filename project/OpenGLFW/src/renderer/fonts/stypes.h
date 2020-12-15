/*************************************************
Author: Stan
Created: 2019-6-4
Note: showInfo basic types.
************************************************/

#pragma once
#include "ShowComman.h"

namespace show {

struct Size
{
	Size() {};
	Size(float w, float h)
		:width(w), height(h)
	{};

	float width = 0.0f;
	float height = 0.0f;
};

struct Vec2
{
	Vec2() {};
	Vec2(float xx, float yy)
		:x(xx), y(yy)
	{};
	
	float x = 0.f;
	float y = 0.f;
};

struct Vec3
{
	Vec3() {};
	Vec3(float xx, float yy, float zz) 
		:x(xx), y(yy), z(zz)
	{
	};
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};

struct Tex2F
{
	Tex2F() {};
	Tex2F(GLfloat uu, GLfloat vv)
		: u(uu), v(vv)
	{};
	GLfloat u = 0.0f;
	GLfloat v = 0.0f;
};

struct V2F_T2F
{
	V2F_T2F(){};
	V2F_T2F(const Vec2 && v, const Tex2F && t)
		: vertices(v), texCoords(t)
	{};

	Vec2 vertices;
	Tex2F texCoords;
};

struct V2F_T2F_Quad
{
	// top left
	V2F_T2F tl;
	// bottom left
	V2F_T2F bl;
	// top right
	V2F_T2F tr;
	// bottom right
	V2F_T2F br;
};



}