#include "stdafx.h"

Game::Game() {}

bool Game::Initialize()
{
	// initialize sdl
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		SDL_Log("Failed initializing SDL: %s\n", SDL_GetError());
		return 0;
	}
	// use core opengl profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// color buffer with 8 bits per channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	// create sdl window
	mWindow = SDL_CreateWindow("Carl's Fantastic Ball Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed creating window: %s\n", SDL_GetError());
		return 0;
	}
	// opengl context
	mContext = SDL_GL_CreateContext(mWindow);
	// initialize glew
	glewExperimental = GL_TRUE; // prevents unnecessary errors on some platforms
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed initializing GLEW");
		return 0;
	}
	glGetError(); // clear insignificant error codes
	// load shaders
	if (!LoadShaders()) return 0;
	// allow program to run
	mIsRunning = 1;
	
	float v[] = {
		-0.5f,  0.5f, 0.f, 0.f, 0.f, // top left
		 0.5f,  0.5f, 0.f, 1.f, 0.f, // top right
		 0.5f, -0.5f, 0.f, 1.f, 1.f, // bottom right
		-0.5f, -0.5f, 0.f, 0.f, 1.f  // bottom left
	};
	unsigned int i[] = {
		0, 1, 2,
		2, 3, 0
	};
	mSquare = new VertexArray(v, 4, i, 6);

	AddActor(new Player(this));

	return 1;
}

void Game::RunLoop()
{
	// game loop
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::ProcessInput()
{
	// poll for events
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}
	// update keyboard state
	mKeyboardState = SDL_GetKeyboardState(NULL);

	mActorsUpdating = 0;
	for (auto a : mActors) a->ProcessInput(mKeyboardState);
	mActorsUpdating = false;
}

void Game::UpdateGame()
{
	// get delta 
	float delta = (SDL_GetTicks() - mTickCount) / 1000.f;
	// update tick count
	mTickCount = SDL_GetTicks();
	// limit framerate (15 fps)
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTickCount + 64));
	// clamp delta  value
	if (delta > 0.5f) delta = 0.5f;
	// update actors
	mActorsUpdating = 1;
	for (auto a : mActors) a->Update(delta);
	mActorsUpdating = 0;
	for (auto a : mPending) mActors.emplace_back(a);
	mPending.clear();
}

void Game::GenerateOutput()
{
	// set clear color to black
	glClearColor(0.f, 0.f, 0.f, 1.f);
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	// activate vertex array
	mShader->SetActive();
	mSquare->SetActive();
	for (auto a : mActors) a->Draw();
	// swap buffers
	SDL_GL_SwapWindow(mWindow);
}

void Game::AddActor(class Actor* a)
{
	if (mActorsUpdating) mPending.push_back(a);
	else mActors.push_back(a);
}

void Game::RemoveActor(class Actor* a)
{
	auto i = std::find(mPending.begin(), mPending.end(), a);
	if (i != mPending.end())
	{
		std::iter_swap(i, mPending.end() - 1);
		mPending.pop_back();
	}

	i = std::find(mActors.begin(), mActors.end(), a);
	if (i != mActors.end())
	{
		std::iter_swap(i, mActors.end() - 1);
		mActors.pop_back();
	}
}

bool Game::LoadShaders()
{
	mShader = new Shader();
	if (!mShader->Load("Basic.vert", "Basic.frag")) return 0;
	mShader->SetActive();
	mShader->SetMatrixUniform("uViewTransform", Matrix4::CreateSimpleViewProj(1024.f, 768.f));
	return 1;
}