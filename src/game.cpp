#include "game.h"
#include "utils.h"
#include "input.h"
#include "image.h"

#include "mycode.h"
#include "mycode.cpp"
#include <cmath>
#include <random>
#include <stdlib.h>

int enemiesImgPos[3][4] = { {40,17,8,6}, {7,0,0,0}, {4,0,0,1} };

Game* Game::instance = NULL;

Image font;
Image minifont;
Image sprite;
Image bg;
Color bgcolor(130, 80, 100);

Image test;

std::vector<Stage*> stages;

STAGE_ID currentStage = STAGE_ID::INTRO;

Stage* GetStage(STAGE_ID id) {
	return stages[id];
}

Stage* GetCurrentStage() {
	return GetStage(currentStage);
}

void SetStage(STAGE_ID id) {
	currentStage = id;
}

sEnemy::sEnemy(int imgps[4], int typ, Vector2 s, int rad)
{
	type = typ;
	pos = s;
	radius = rad;

}

level::level(int ctr, vector<sEnemy*> enemies) {
	counter = ctr;
	enemies = enemies;

};

void World::CreateLevel(int ctr,int rad,int lvlnumb,int entype) {
	Game* Insgame = Game::instance; //singelton	
	vector<sEnemy*> temp;
	level* a = new level(ctr, temp);
	for (int i = 0; i < ctr; i++)
	{
		Vector2 tmp;
		tmp.x = (rand() % 200) + 5;
		tmp.y = (rand() % 100) - 35;
		//sEnemy tenemy= new sEnemy(a,1,tmp);
		//temp.push_back(new sEnemy(enemiesImgPos[0], 0, tmp));
		a->enemies.push_back(new sEnemy(enemiesImgPos[0], entype, tmp,rad));
	}
	levels.push_back(a);
	
}


World::World()
{
	bg.loadTGA("data/bgcool.tga");
	btn.loadTGA("data/menubu.tga");
	font.loadTGA("data/bitmap-font-white.tga");
	p1.loadTGA("data/assetpck.tga");
	misc.loadTGA("data/misc.tga");
	bullets.loadTGA("data/projectiles.tga");
	bossshield.loadTGA("data/bossshield.tga");
	hp.loadTGA("data/vida.tga");
	tuto.loadTGA("data/tuto.tga");
	player.pos = Vector2(50, 50);
	player.health = 10;
	vector<int> tmp;
	tmp.push_back(40);
	tmp.push_back(17);
	tmp.push_back(8);
	tmp.push_back(6);

	World::CreateLevel(4,7,1,0);
	World::CreateLevel(8, 10, 1, 0);
	World::CreateLevel(1, 30, 3, 3);

}

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	//current_stage = intro_stage;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	
	stages.reserve(5);
	stages.push_back(new TutorialStage());
	stages.push_back(new IntroStage());
	stages.push_back(new PlayStage());
	stages.push_back(new EndStage());
	stages.push_back(new Tuto());

	curr_stg = 0;
	
	//stages.size(); //4*/


	//pause_stage = new PauseLevel1to2();
	//explic_stage = new Explication();
	//Stage* curr_stage = intro_stage;
	


	my_world = new World();

	font.loadTGA("data/bitmap-font-white.tga"); //load bitmap-font image
	minifont.loadTGA("data/mini-font-white-4x6.tga"); //load bitmap-font image
	//sprite.loadTGA("data/spritesheet.tga"); //example to load an sprite
	//bg.loadTGA("data/bg_new.tga");
	test.loadTGA("data/misc.tga");

	enableAudio(); //enable this line if you plan to add audio to your application
	synth.playSample("data/music/snd_gameoverscreen.wav", 0.2, true);
	//synth.playSample("data/coin.wav",1,true);
	//synth.osc1.amplitude = 0.5;
}

void Game::ChangeScene(int nsi)
{
	SetStage((STAGE_ID)nsi);
}
void Game::Restart()
{
	my_world = new World();
	stages.clear();
	stages.reserve(5);
	stages.push_back(new TutorialStage());
	stages.push_back(new IntroStage());
	stages.push_back(new PlayStage());
	stages.push_back(new EndStage());
	stages.push_back(new Tuto());
	curr_stg = 0;


}


//what to do when the image has to be draw
void Game::render(void)
{
	//Create a new Image (or we could create a global one if we want to keep the previous frame)
	Image framebuffer(160, 120); //do not change framebuffer size

	//add your code here to fill the framebuffer
	//...

	//some new useful functions
		//framebuffer.fill( bgcolor );								//fills the image with one color
		//framebuffer.drawLine( 0, 0, 100,100, Color::RED );		//draws a line
		//framebuffer.drawImage( sprite, 0, 0 );					//draws full image
		//framebuffer.drawImage(bg, 0, 0, framebuffer.width, framebuffer.height);
		//framebuffer.drawImage( sprite, 0, 0, framebuffer.width, framebuffer.height );			//draws a scaled image
		//framebuffer.drawImage( sprite, 0, 0, Area(0,0,14,18) );	//draws only a part of an image
		//framebuffer.drawText( "Hello World", 0, 0, font );				//draws some text using a bitmap font in an image (assuming every char is 7x9)
		//framebuffer.drawText( toString(time), 1, 10, minifont,4,6);	//draws some text using a bitmap font in an image (assuming every char is 4x6)
		//framebuffer.drawImage( bg, 0, 0 ,160,120);	
	GetCurrentStage()->Render(framebuffer);

	//framebuffer.drawImage(test, 50, 50, Area(40, 48, 8, 8));
	//framebuffer.drawImage(test, 0, 0, 40, 40);
	//send image to screen
	showFramebuffer(&framebuffer);
}

void Game::update(double seconds_elapsed)
{
	//Add here your update method
	//...
	//stages2[0]->update(seconds_elapsed);
	//curr_stage->Update(seconds_elapsed);
	if (curr_stg != (int)currentStage) {
		currentStage = (STAGE_ID)curr_stg;
	}
	GetCurrentStage()->Update(seconds_elapsed);
	if (Input::wasKeyPressed(SDL_SCANCODE_E)) {
		int nsi = (((int)currentStage + 1) % stages.size());
		curr_stg = nsi;
		SetStage((STAGE_ID)nsi);

	}
	if (Input::wasKeyPressed(SDL_SCANCODE_R)) {
		Game::Restart();

	}
	//Read the keyboard state, to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (Input::isKeyPressed(SDL_SCANCODE_UP)) //if key up
	{
	}
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) //if key down
	{
	}

	//example of 'was pressed'
	if (Input::wasKeyPressed(SDL_SCANCODE_A)) //if key A was pressed
	{
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_Z)) //if key Z was pressed
	{
	}

	//to read the gamepad state
	if (Input::gamepads[0].isButtonPressed(A_BUTTON)) //if the A button is pressed
	{
	}

	if (Input::gamepads[0].direction & PAD_UP) //left stick pointing up
	{
		bgcolor.set(0, 255, 0);
	}
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseMove(SDL_MouseMotionEvent event)
{
}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	window_width = width;
	window_height = height;
}

//sends the image to the framebuffer of the GPU
void Game::showFramebuffer(Image* img)
{
	static GLuint texture_id = -1;
	static GLuint shader_id = -1;
	if (!texture_id)
		glGenTextures(1, &texture_id);

	//upload as texture
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);

	glDisable(GL_CULL_FACE); glDisable(GL_DEPTH_TEST); glEnable(GL_TEXTURE_2D);
	float startx = -1.0; float starty = -1.0;
	float width = 2.0; float height = 2.0;

	//center in window
	float real_aspect = window_width / (float)window_height;
	float desired_aspect = img->width / (float)img->height;
	float diff = desired_aspect / real_aspect;
	width *= diff;
	startx = -diff;

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex2f(startx, starty + height);
	glTexCoord2f(1.0, 0.0); glVertex2f(startx + width, starty + height);
	glTexCoord2f(1.0, 1.0); glVertex2f(startx + width, starty);
	glTexCoord2f(0.0, 1.0); glVertex2f(startx, starty);
	glEnd();

	/* this version resizes the image which is slower
	Image resized = *img;
	//resized.quantize(1); //change this line to have a more retro look
	resized.scale(window_width, window_height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (1) //flip
	{
	glRasterPos2f(-1, 1);
	glPixelZoom(1, -1);
	}
	glDrawPixels( resized.width, resized.height, GL_RGBA, GL_UNSIGNED_BYTE, resized.pixels );
	*/
}

//AUDIO STUFF ********************

SDL_AudioSpec audio_spec;

void AudioCallback(void*  userdata,
	Uint8* stream,
	int    len)
{
	static double audio_time = 0;

	memset(stream, 0, len);//clear
	if (!Game::instance)
		return;

	Game::instance->onAudio((float*)stream, len / sizeof(float), audio_time, audio_spec);
	audio_time += len / (double)audio_spec.freq;
}

void Game::enableAudio()
{
	SDL_memset(&audio_spec, 0, sizeof(audio_spec)); /* or SDL_zero(want) */
	audio_spec.freq = 48000;
	audio_spec.format = AUDIO_F32;
	audio_spec.channels = 1;
	audio_spec.samples = 1024;
	audio_spec.callback = AudioCallback; /* you wrote this function elsewhere. */
	if (SDL_OpenAudio(&audio_spec, &audio_spec) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}
	SDL_PauseAudio(0);
}

void Game::onAudio(float *buffer, unsigned int len, double time, SDL_AudioSpec& audio_spec)
{
	//fill the audio buffer using our custom retro synth
	synth.generateAudio(buffer, len, audio_spec);
}
