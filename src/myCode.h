#pragma once

#ifndef MY_CODE_H
#define MY_CODE_H

#include "includes.h"
#include "image.h"
#include "utils.h"
#include "game.h"

#include <stack>
//#include <string>

using namespace std;


enum STAGE_ID {
	INTRO = 0,
	TUTORIAL = 1,
	PLAY = 2,
	END = 3
};


struct sPlayer
{
	Vector2 pos;
	int health;
};  

class sEnemy {
public:
	int imgps[4];
	int type;
	Vector2 pos;
	int radius;

	sEnemy(int imgps[4] , int type, Vector2 pos, int ra);
};


class level
{	
public:
	int counter;
	vector<sEnemy*> enemies;
	
	level(int ctr, vector<sEnemy*> enemies);
};

class World {
public:
	Image font;
	Image minifont;
	Image sprite;
	Image bg;
	Image menu;
	Image end;
	Image btn;
	Image p1;
	Image misc;
	Image bullets;
	Image bossshield;
	Image hp;
	Image tuto;
	vector<Vector2> Pbullets;
	sPlayer player;
	


	vector<Vector2> Ebullets;

	vector<level*> levels;
	stack<Vector2> playerbullets;


	World();
	void CreateLevel(int ctr, int rad, int lvlnumb, int entype);

};

class Stage {
public:
	virtual STAGE_ID GetId() = 0;
	virtual void Render(Image& framebuffer) = 0;
	virtual void Update(float seconds_elapsed) = 0;

};


#endif