#include "game.h"
#include "myCode.h"
#include "utils.h"
#include "input.h"
#include "image.h"

#include <stack>
#include <cmath>
#include <random>
#include <stdlib.h>
using namespace std;

class IntroStage : public Stage {
public:
	STAGE_ID GetId() { return STAGE_ID::INTRO; };
	void Render(Image& framebuffer) {
		float t = getTime() * 0.001;
		int stg = ((int)(t)) % 2 * 4;
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		framebuffer.fill(Color::GREEN);
		framebuffer.drawText("You won!", 53, 30+stg, InsWorld->font);

		framebuffer.drawText("X to restart", 40, 100+stg, InsWorld->font);
	}
	void Update(float seconds_elapsed) {
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		if (Input::wasKeyPressed(SDL_SCANCODE_X)) {
			Insgame->Restart();
			Insgame->curr_stg = 0;
		}   
	}
};

class TutorialStage : public Stage {
public:
	int xokas = 0;
	STAGE_ID GetId() { return STAGE_ID::TUTORIAL; };
	void Render(Image& framebuffer) {
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		framebuffer.drawImage(InsWorld->bg, 0, 0);
		//framebuffer.drawImage(InsWorld->xd, 0, 0, framebuffer.width, framebuffer.height); ;
		framebuffer.drawImage(InsWorld->btn, 30, 30);
		framebuffer.drawImage(InsWorld->btn, 30, 50);
		framebuffer.drawImage(InsWorld->btn, 30, 70);
		framebuffer.drawText("Tutorial", 53, 30, InsWorld->font);
		framebuffer.drawText("Play", 65, 50, InsWorld->font);
		framebuffer.drawText("Exit", 65, 70, InsWorld->font);
		framebuffer.drawText("X to select!", 40, 100, InsWorld->font);
		framebuffer.drawText("Spaceshot!", 45, 15, InsWorld->font);
		framebuffer.drawImage(InsWorld->p1, 20, 32+(20*xokas), Area(8, 32, 8, 39));

	}

	void Update(float seconds_elapsed) {
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		if (Input::wasKeyPressed(SDL_SCANCODE_DOWN)) {
			xokas = (xokas + 1) % 3 ;
			
			Insgame->synth.playSample("data/music/sfx_menu.wav", 0.5, false);
		}else if (Input::wasKeyPressed(SDL_SCANCODE_UP)){
			xokas = (xokas +2) % 3;
			Insgame->synth.playSample("data/music/sfx_menu.wav", 0.5, false);
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_X)){
			Insgame->synth.playSample("data/music/sfx_enter.wav", 0.5, false);
			//Game::ChangeScene(3);
			if(xokas ==0)//tutorial
			{
				
				Insgame->curr_stg = 4;
			
			}
			else if (xokas==1)//play
			{
				
				
				Insgame->curr_stg = 2;
			}
			else if (xokas == 2)// exit
			{
				Insgame->must_exit = true; //ESC key, kill the app
			}
			
		}
	}
};

class PlayStage : public Stage {
public:
	int current_level = 1;
	int enemiesImgPos[3][4] = { {40,17,8,6}, {7,0,0,0}, {48,64,16,16} };
	int startingbullet = 0;
	int startingEbullet = 0;
	int side = 0;
	bool boosters;

	int enemieskilled = 0;
	
	const float invincibilityDuration = 2.0f;
	float invincibilityLeft = 0.0f;
	bool isInv = true;

	//boss
	vec2 pos;
	int boss_hp=40;
	int boss_phase=0;
	int boss_shield = 50;
	int boss_flag = 0;;
	float boss_shooting_temp = 0.0f;
	vector<Vector2> staticbullets;
	//int increment = 1;

	STAGE_ID GetId() { return STAGE_ID::PLAY; };

	
	void Render(Image& framebuffer) {
		float t = getTime() * 0.001;
		//time = 0.0f;
		int stg = ((int)(t*4.0f)) % 4 * 8;
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		//framebuffer.fill(Color::CYAN);
		framebuffer.drawImage(InsWorld->bg, 0, 0, Area(0+ InsWorld->player.pos.x, 0+ InsWorld->player.pos.y, 160+ InsWorld->player.pos.x, 120+ InsWorld->player.pos.y));
		framebuffer.drawImage(InsWorld->p1, InsWorld->player.pos.x, InsWorld->player.pos.y, Area(8,32,8, 39));//ctr
		//framebuffer.drawImage(InsWorld->p1, InsWorld->player.pos.x, InsWorld->player.pos.y, Area(40, 17, 8, 6));//ctr
		if (side == 1) { framebuffer.drawImage(InsWorld->p1, InsWorld->player.pos.x, InsWorld->player.pos.y, Area(0, 32, 8, 39)); }//left}
		if (side == 2) { framebuffer.drawImage(InsWorld->p1, InsWorld->player.pos.x, InsWorld->player.pos.y, Area(16, 32, 16, 39)); }//right
		if (boosters) {
			framebuffer.drawImage(InsWorld->misc, InsWorld->player.pos.x + 2, InsWorld->player.pos.y + 8, Area(44 + stg, 0, 1, 3));
			framebuffer.drawImage(InsWorld->misc, InsWorld->player.pos.x + 5, InsWorld->player.pos.y + 8, Area(44 + stg, 0, 1, 3));
		}

		//objectives
		if (current_level < 2) {
			framebuffer.drawText(to_string(enemieskilled), 128,3, InsWorld->font);
			framebuffer.drawText("/30", 138, 3, InsWorld->font);
		}
		else {
			if (boss_phase == 0) {
				framebuffer.drawText("Shield", 115, 3, InsWorld->font);
				framebuffer.drawText(to_string(boss_shield), 140, 12, InsWorld->font);	
			}
			else {
				framebuffer.drawText("HP:", 140, 3, InsWorld->font);
				framebuffer.drawText(to_string(boss_hp), 140, 12, InsWorld->font);
			}
			
		}
		
		//player bullets
		for (int i = startingbullet; i < InsWorld->Pbullets.size(); i++)
		{
			int bx = InsWorld->Pbullets[i].x;
			int by = InsWorld->Pbullets[i].y;
			//bullet collision		
			for (int j = 0; j < InsWorld->levels[current_level]->enemies.size(); j++)
			{
				int ex= InsWorld->levels[current_level]->enemies[j]->pos.x+3;
				int ey = InsWorld->levels[current_level]->enemies[j]->pos.y+2;
				if ((sqrt(pow(ex - bx, 2) +pow(ey - by, 2) * 1.0))<7) {
					InsWorld->Pbullets[i].x=300;
					InsWorld->Pbullets[i].y=300;
					if (current_level == 2) {
						if (boss_phase == 0) {
							boss_shield--;
						}
						else {
							boss_hp--;
						}
					}
					else {
						
						InsWorld->levels[current_level]->enemies[j]->pos.x = -40;
						InsWorld->levels[current_level]->enemies[j]->pos.y = 20;
						enemieskilled++;
					}
				}	
			}

			if (InsWorld->Pbullets[i].y < -10) {
				startingbullet++;
			}
			InsWorld->Pbullets[i].y -=4 ;
			framebuffer.drawImage(InsWorld->bullets, InsWorld->Pbullets[i].x, InsWorld->Pbullets[i].y, Area(19, 3, 1, 3));
		}

		//enemies2
		if (current_level < 2) {
			for (int i = 0; i < InsWorld->levels[current_level]->enemies.size(); i++)
			{

				int rnd = rand() % 500;

				sEnemy* temp = InsWorld->levels[current_level]->enemies[i];
				int type = InsWorld->levels[current_level]->enemies[i]->type;

				framebuffer.drawImage(InsWorld->p1, temp->pos.x, temp->pos.y, Area(enemiesImgPos[type][0], enemiesImgPos[type][1], enemiesImgPos[type][2], enemiesImgPos[type][3]));
				
				if (rnd == 55 || rnd == 69) {
					Vector2 etmp;
					etmp.x = temp->pos.x;
					etmp.y = temp->pos.y;
					InsWorld->Ebullets.push_back(etmp);
					
				}
				else if (rnd == 19)
				{
					int rnd2 = (rand() % 150) + 5;
					if (temp->pos.x < rnd2) { while (temp->pos.x < rnd2) { temp->pos.x += 3; } }
					else { while (temp->pos.x > rnd2) { temp->pos.x -= 3; } }
				}

				else if (rnd == 42)
				{
					int rnd2 = (rand() % 100) - 35;
					if (rnd2 < 0) { rnd2 *= -1; }
					if (temp->pos.y < rnd2) { while (temp->pos.y < rnd2) { temp->pos.y += 3; } }
					else { while (temp->pos.y > rnd2) { temp->pos.y -= 3; } }
				}
			}
		}
		else {
			
			sEnemy* temp = InsWorld->levels[current_level]->enemies[0];
			//InsWorld->levels[current_level]->enemies[0]->pos.y = 50;
			
			framebuffer.drawImage(InsWorld->p1, InsWorld->levels[current_level]->enemies[0]->pos.x, InsWorld->levels[current_level]->enemies[0]->pos.y, Area(48, 64, 16, 16));
			if(boss_phase==0) framebuffer.drawImage(InsWorld->bossshield, InsWorld->levels[current_level]->enemies[0]->pos.x-2, InsWorld->levels[current_level]->enemies[0]->pos.y);
			if (boss_phase == 0) {
				
				InsWorld->levels[current_level]->enemies[0]->pos.y = 10;
				if (boss_flag == 0) {
					InsWorld->levels[current_level]->enemies[0]->pos.x += 0.5;
					if (InsWorld->levels[current_level]->enemies[0]->pos.x > 140) { boss_flag = 1; }
				}
				else {
					InsWorld->levels[current_level]->enemies[0]->pos.x -= 0.5;
					if (InsWorld->levels[current_level]->enemies[0]->pos.x < 10) { boss_flag = 0; }
				}
				//shooting
				if (boss_shooting_temp <= 0.0f) {
					Vector2 etmp;
					
					etmp.x = temp->pos.x+5;
					etmp.y = temp->pos.y;
					InsWorld->Ebullets.push_back(etmp);
					
					boss_shooting_temp = 0.5f;
				}
			}
			else if (boss_phase==1)
			{
				temp->pos.x = 70;
				temp->pos.y = 30;
				
				if (boss_shooting_temp <= 0.0f) {
					InsWorld->Ebullets.clear();
						startingEbullet = 0;
						for (int i = 0; i < 12; i++)
						{
							Vector2 etmp;
							etmp.x = (rand() % 150) + 5;
							etmp.y = (rand() % 115) +5;
							InsWorld->Ebullets.push_back(etmp);
							//circle[i].push_back(etmp);
							staticbullets.push_back(etmp);
							
							boss_shooting_temp = 4.0f;
						}
				}
				
			}

		}
		//enemy bullets
			
		for (int i = startingEbullet; i < InsWorld->Ebullets.size(); i++)
		{
			int bx = InsWorld->Ebullets[i].x;
			int by = InsWorld->Ebullets[i].y;


			//player bullet collision		
			if ((sqrt(pow(InsWorld->player.pos.x + 4 - bx, 2) + pow(InsWorld->player.pos.y - by, 2) * 1.0)) < 10 && !(invincibilityLeft > 0.0f)) {
				invincibilityLeft = invincibilityDuration;
				std::cout << "player hit" << std::endl;
				InsWorld->player.health--;
				Insgame->synth.playSample("data/music/sfx_hit.wav", 0.5, false);
				InsWorld->Ebullets[i].x=300;
				InsWorld->Ebullets[i].y=300;
			}

			if (InsWorld->Ebullets[i].y > 125) {
				startingEbullet++;
			}
			if (boss_phase == 1) {
				framebuffer.drawImage(InsWorld->misc, InsWorld->Ebullets[i].x + 2, InsWorld->Ebullets[i].y, Area(40+stg, 48, 8, 8));
				//framebuffer.drawImage(InsWorld->bullets, InsWorld->Ebullets[i].x + 2, InsWorld->Ebullets[i].y, Area(16, 8, 8, 8));
			}
			else {
				InsWorld->Ebullets[i].y += 2;
				framebuffer.drawImage(InsWorld->bullets, InsWorld->Ebullets[i].x + 2, InsWorld->Ebullets[i].y, Area(19, 3, 1, 3));
			}
		}
	
		framebuffer.drawImage(InsWorld->hp, 0, 3);//hp
		framebuffer.drawText(to_string(InsWorld->player.health), 7, 3 , InsWorld->font);

	}	


	void Update(float seconds_elapsed) {
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		const float moveSpeed = 50.0f;
		Vector2 movement;
		side = 0;
		boosters = false;
		float px = InsWorld->player.pos.x;
		float py = InsWorld->player.pos.y;

		//inv
		invincibilityLeft = max(0.0f, invincibilityLeft -seconds_elapsed);
		boss_shooting_temp = max(0.0f, boss_shooting_temp - seconds_elapsed);
		if (Input::isKeyPressed(SDL_SCANCODE_UP)) //if key up
		{	
			boosters = true;
			if (py > 3) { movement.y -= moveSpeed; }
		}
		if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) //if key down
		{
			boosters = true;
			if (py <113)movement.y += moveSpeed;
		}
		if (Input::isKeyPressed(SDL_SCANCODE_LEFT)) //if key Left
		{	
			boosters = true;
			side = 1;
			if (px > 3)movement.x -= moveSpeed;
		}
		if (Input::isKeyPressed(SDL_SCANCODE_RIGHT)) //if key Right
		{
			boosters = true;
			side = 2;
			if (px <153)movement.x += moveSpeed;
		}

		if (Input::wasKeyPressed(SDL_SCANCODE_X)) //if key Right
		{	
			Insgame->synth.playSample("data/music/snd_disparo.wav", 0.5, false);
			Vector2 temp;
			temp.x = InsWorld->player.pos.x+3;
			temp.y = InsWorld->player.pos.y;
			InsWorld->Pbullets.push_back(temp);
			

		}
		InsWorld->player.pos += movement * seconds_elapsed;

		//win 
		{
			if (enemieskilled >= 30) {
			//Insgame->curr_stg = 0;
				current_level = 2;

			}

		}
		//death
		if (InsWorld->player.health <= 0) {
			Insgame->synth.playSample("data/music/sfx_dead.wav", 0.5, false);
			Insgame->curr_stg = 3;

		}

		//boss shield
		if (boss_shield <= 0) {
			boss_phase=1;
		}
		if(boss_hp<=0){
			Insgame->curr_stg = 1;
			Insgame->synth.playSample("data/music/sfx_youwin.wav", 0.5, false);
		}
	}
};

class EndStage : public Stage {
public:
	STAGE_ID GetId() { return STAGE_ID::END; }
	void Render(Image& framebuffer) {
		
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		float t = getTime() * 0.001;
		//time = 0.0f;
		int stg = ((int)(t )) % 2 * 4;
		framebuffer.fill(Color::BLACK);
		framebuffer.drawText("Game", 65, 50+stg, InsWorld->font);
		framebuffer.drawText("Over", 70, 60+stg, InsWorld->font);

		framebuffer.drawText("X to restart", 40, 100 + stg, InsWorld->font);
	}

	void Update(float seconds_elapsed) {
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		if (Input::wasKeyPressed(SDL_SCANCODE_X)) {
			Insgame->Restart();
			Insgame->curr_stg = 0;

		}


	}
};


class Tuto : public Stage {
public:
	STAGE_ID GetId() { return STAGE_ID::END; }
	void Render(Image& framebuffer) {

		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		float t = getTime() * 0.001;
		//time = 0.0f;
		framebuffer.drawImage(InsWorld->bg, 0, 0);
		framebuffer.drawImage(InsWorld->tuto, 0, 0);
		framebuffer.drawText("Move!", 35, 90 , InsWorld->font);
		framebuffer.drawText("Shoot!", 95, 90, InsWorld->font);
		framebuffer.drawText("<-X to go back", 5, 5, InsWorld->font);
	}

	void Update(float seconds_elapsed) {
		Game* Insgame = Game::instance; //singelton	
		World* InsWorld = Game::instance->my_world;
		if (Input::wasKeyPressed(SDL_SCANCODE_X)) {
			Insgame->Restart();
			Insgame->curr_stg = 0;
		}
	}
};

