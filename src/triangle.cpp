/* Using standard C++ output libraries */
#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/shader_utils.h"


struct attributes {
	GLfloat coord2d[3];
	GLfloat v_color[3];
};


GLuint program;
GLint attribute_coord3d, attribute_v_color, uniform_fade; 
GLint uniform_m_transform;


GLuint ibo_cube_elements;					//t5
GLuint vbo_cube_vertices, vbo_cube_colors;	


bool init_resources(void) {
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

	GLfloat cube_colors[] = {
		// front colors
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
		// back colors
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
	};

	GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3,
	};
	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

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

	const char* attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1) {
		std::cerr << "Could not bind attribute " << attribute_name << std::endl;
		return false;
	}

	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if (attribute_v_color == -1) {
		std::cerr << "Could not bind attribute " << attribute_name << std::endl;
		return false;
	}

	const char * uniform_name;
	// uniform_name = "fade";
	// uniform_fade = glGetUniformLocation(program, uniform_name);
	// if (uniform_fade == -1) {
	// 	std::cerr << "Could not bind uniform_fade " << uniform_name << std::endl;
	// 	return false;
	// }

	uniform_name = "m_transform";
	uniform_m_transform = glGetUniformLocation(program, uniform_name);
	if (uniform_m_transform == -1) {
		std::cerr << "Could not bind uniform " << uniform_name << std::endl;
		return false;
	}
	

	return true;
}

void render(SDL_Window* window){

	/* Clear the background as white */
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);



	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glEnableVertexAttribArray(attribute_coord3d);
	glEnableVertexAttribArray(attribute_v_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

	glVertexAttribPointer(
		attribute_coord3d,	// attribute
		3,					// number of elements per vertex, here (x,y)
		GL_FLOAT,			// the type of each element
		GL_FALSE,			// take our values as-is
		sizeof(struct attributes),	// no extra data between each position
		0					// offset of first element
		);

	glVertexAttribPointer(
		attribute_v_color, // attribute
		3,                 // number of elements per vertex, here (r,g,b)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		sizeof(struct attributes),		// next color appears every 5 floats
		(GLvoid*) offsetof(struct attributes, v_color)	// offset of first element
	);


	glDrawArrays(GL_TRIANGLES, 0, 3);

	
	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);

	/* Display the result */
	SDL_GL_SwapWindow(window);
}

void logic()
{
	float move = sinf(SDL_GetTicks() / 1000.0 * (2*3.14) /5);

	float angle = SDL_GetTicks() / 1000.0 * 45;


	glm:: vec3 axis_z(0,0,1);
	glm::mat4 m_transform = 
		glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, 0.0))
		* glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_z);

	glUseProgram(program);
	glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, glm::value_ptr(m_transform));

	
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
		logic();
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
