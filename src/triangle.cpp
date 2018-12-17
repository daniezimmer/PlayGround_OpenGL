/* Using standard C++ output libraries */
#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "../common/shader_utils.h"


GLuint vbo_triangle;
GLuint program;
GLint attribute_coord2d;

bool init_resources(void) {
	GLfloat triangle_vertices[] = {
		0.0, 0.8,
		-0.8, -0.8,
		0.8, -0.8,
	};
	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

	GLint link_ok = GL_FALSE;

	GLuint vs, fs;
	if ((vs = create_shader("triangle.v.glsl", GL_VERTEX_SHADER)) == 0) return false;
	if ((fs = create_shader("triangle.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;


	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if(!link_ok){
		std::cerr << "Error in glLinkProgram" << std::endl;
		print_log(program);
		return false;
	}

	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1) {
		std::cerr << "Could not bind attribute " << attribute_name << std::endl;
		return false;
	}

	return true;
}

void render(SDL_Window* window){

	/* Clear the background as white */
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);


	glEnableVertexAttribArray(attribute_coord2d);

	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */

	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

	glVertexAttribPointer(
		attribute_coord2d,	// attribute
		2,					// number of elements per vertex, here (x,y)
		GL_FLOAT,			// the type of each element
		GL_FALSE,			// take our values as-is
		0,					// no extra data between each position
		0					// offset of first element
						  );

	glDrawArrays(GL_TRIANGLES, 3, 6);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glDisableVertexAttribArray(attribute_coord2d);

	/* Display the result */
	SDL_GL_SwapWindow(window);
}

void free_resouces(){
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_triangle);
}

void mainLoop(SDL_Window* window){
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				return;
		}
		render(window);	
	}
}

int main(int argc, char * argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("My Second Triangle",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (window == NULL){
		std::cerr << "Error: can't create window: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
	if(SDL_GL_CreateContext(window) == NULL) {
		std::cerr << "Error: SDL_GL_CreateContext: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}



	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK){
		std::cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << std::endl;
		return EXIT_FAILURE;
	}

	if (!GLEW_VERSION_2_0)
	{
		std::cerr << "Error: your graphic card does not support OpenGL 2.0" << std::endl;
		return EXIT_FAILURE;
	}

	if (!init_resources())
		return EXIT_FAILURE;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mainLoop(window);

	free_resouces();
	return EXIT_SUCCESS;
}
