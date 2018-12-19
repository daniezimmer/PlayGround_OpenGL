/* Using standard C++ output libraries */
#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "../common/shader_utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



int screen_width = 800, screen_height = 600;

GLuint program;
GLuint ibo_cube_elements;
GLuint vbo_cube_vertices, vbo_cube_colors;

GLint attribute_coord3d, attribute_v_color;
GLint uniform_mvp;

bool init_resources(void)
{
	GLfloat cube_vertices[] = {
		// front
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
	};
	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	

	GLfloat cube_colors[] = {
		// front colors
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		// back colors
		1.0, 1.0, 0.0,
		0.0, 1.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
	};
	glGenBuffers(1, &vbo_cube_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
	


	GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};
	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	GLint link_ok = GL_FALSE;

	GLuint vs, fs;
	if ((vs = create_shader("cube.v.glsl", GL_VERTEX_SHADER)) == 0)
		return false;
	if ((fs = create_shader("cube.f.glsl", GL_FRAGMENT_SHADER)) == 0)
		return false;

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cerr << "Error in glLinkProgram" << std::endl;
		print_log(program);
		return false;
	}

	const char *attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1)
	{
		std::cerr << "Could not bind attribute " << attribute_name << std::endl;
		return false;
	}

	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if (attribute_v_color == -1)
	{
		std::cerr << "Could not bind attribute " << attribute_name << std::endl;
		return false;
	}

	const char* uniform_name;
	uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if (uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return 0;
	}

	return true;
}

void render(SDL_Window *window)
{
	/* Clear the background as white */
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord3d);


	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glVertexAttribPointer(
		attribute_coord3d,			// attribute
		3,							// number of elements per vertex, here (x,y)
		GL_FLOAT,					// the type of each element
		GL_FALSE,					// take our values as-is
		0,							// no extra data between each position
		0							// offset of first element
	);
	glEnableVertexAttribArray(attribute_v_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
	glVertexAttribPointer(
		attribute_v_color,							   // attribute
		3,											   // number of elements per vertex, here (r,g,b)
		GL_FLOAT,									   // the tglm::value_ptr(m_transform)ype of each element
		GL_FALSE,									   // take our values as-is
		0,					   // next color appears every 5 floats
		0		 // offset of first element
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);

	/* Display the result */
	SDL_GL_SwapWindow(window);
}

void logic()
{


	float angle = SDL_GetTicks() / 1000.0 * 45;  // 45° per second
	glm::vec3 axis_y(0, 1, 0);
	glm::mat4 anim = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);


	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 10.0f);

	glm::mat4 mvp = projection * view * model * anim;

	glUseProgram(program);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

}


void onResize(int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}


void free_resouces()
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_cube_vertices);
	glDeleteBuffers(1, &vbo_cube_colors);
	glDeleteBuffers(1, &ibo_cube_elements);
}

void mainLoop(SDL_Window *window)
{
	while (true)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
				return;
			if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				onResize(ev.window.data1, ev.window.data2);
		}
		logic();
		render(window);
	}
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("My Rotating Cube",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (window == NULL)
	{
		std::cerr << "Error: can't create window: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
	if (SDL_GL_CreateContext(window) == NULL)
	{
		std::cerr << "Error: SDL_GL_CreateContext: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK)
	{
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
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mainLoop(window);

	free_resouces();
	return EXIT_SUCCESS;
}
