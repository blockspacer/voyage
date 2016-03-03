#include "SDLGraphicsEngine.hpp"
#include "ImguiInterface.hpp"
#include "TextureAtlas.hpp"
#include <graphics/Gui/imgui.h>
#include <SDL_image.h>

GLuint SDLGraphicsEngine::g_FontTexture = 0;
double SDLGraphicsEngine::g_Time = 0.0;

void SDLGraphicsEngine::init() {

	// Setup SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
	}

	// Setup SDL_Image
	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
	}

	// Create Gui object. Only loads low-level data (i.e window size)
	_gui = std::make_shared<Gui>();
	_gui->init();

	//create tilemap
	_tilemap = std::make_shared<TileMap>();
	_tilemap->init();

	// Setup window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	_window = SDL_CreateWindow("Voyage", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _gui->getWindowWidth(), _gui->getWindowHeight(), SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	_glcontext = SDL_GL_CreateContext(_window);

	// Setup input and ImGui binding
	_input.init(_window);

	_tilemap->loadTextureAtlas();
	_tilemap->getAtlas()->createSDLRenderer(_window);
	
	ImGuiIO& io = ImGui::GetIO();
	io.RenderDrawListsFn = ImGuiRenderDrawLists;
	io.SetClipboardTextFn = ImGuiSetClipboardText;
	io.GetClipboardTextFn = ImGuiGetClipboardText;

	//load texture atlas after all the imgui initialization!
//	_gui->displayMap(_tilemap);
	
	//DEBUG STUFF AS WELL!	
	_show_test_window = true;
	_show_another_window = false;
	
}

void SDLGraphicsEngine::newFrame() {

	if (!g_FontTexture)
		createDeviceObjects();
	
	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);

	// Setup time step
	Uint32	time = SDL_GetTicks();
	double current_time = time / 1000.0;
	io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
	g_Time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
	int mx, my;
	Uint32 mouseMask = SDL_GetMouseState(&mx, &my);	
	if (SDL_GetWindowFlags(_window) & SDL_WINDOW_MOUSE_FOCUS)
		io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
	else
		io.MousePos = ImVec2(-1,-1);
   
	io.MouseDown[0] = SDLInputEngine::g_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = SDLInputEngine::g_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	io.MouseDown[2] = SDLInputEngine::g_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
	SDLInputEngine::g_MousePressed[0] = SDLInputEngine::g_MousePressed[1] = SDLInputEngine::g_MousePressed[2] = false;

	io.MouseWheel = SDLInputEngine::g_MouseWheel;
	SDLInputEngine::g_MouseWheel = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

	 // Start the frame
	ImGui::NewFrame();

}

bool SDLGraphicsEngine::createDeviceObjects() {
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);
	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);	

	return true;
}

void SDLGraphicsEngine::run() {

	SDL_Event event;
        while (SDL_PollEvent(&event))
        {
		_input.processEvent(&event);
		if (event.type == SDL_QUIT)
			_gameClient->requestQuit();		
        }
		
	//setup new frame
	newFrame();

	//displayGUI
	//_gui->displayMain();
	//_gui->displayConsole();

	//display map
	_gui->displayMap(_tilemap);
	
	// Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	_gui->clear();

        ImGui::Render();
        SDL_GL_SwapWindow(_window);
}


void SDLGraphicsEngine::stop() {

	_gameClient->requestQuit();
	
	// Cleanup
	if (g_FontTexture)
	{
		glDeleteTextures(1, &g_FontTexture);
		ImGui::GetIO().Fonts->TexID = 0;
		g_FontTexture = 0;
	}
	ImGui::Shutdown();
	SDL_GL_DeleteContext(_glcontext);  
	SDL_DestroyWindow(_window);
	SDL_Quit();
}