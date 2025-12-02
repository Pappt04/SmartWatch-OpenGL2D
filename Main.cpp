#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <iostream>
#include "Util.h"

#define NUM_SLICES 40

enum ScreenStates {
	SCREEN_CLOCK,
	SCREEN_HEART_RATE,
};

// ALL GLOBAL VARIABLES DECLARED HERE
ScreenStates currentScreen = SCREEN_CLOCK;
int hours = 0, minutes = 0, seconds = 0;
int heartRate = 72;
bool isRunning = false;
float ecgScrollOffset = 0.0f;
float ecgScale = 1.0f;
int batteryPercent = 100;

double lastHeartUpdate = 0, lastBatteryUpdate = 0, lastEcgUpdate = 0;


float projection[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

//END OF GLOBAL VARIABLES
int endProgram(std::string message) {
	std::cout << message << std::endl;
	glfwTerminate();
	return -1;
}

void preprocessTexture(unsigned& texture, const char* filepath) {
	texture = loadImageToTexture(filepath); // Učitavanje teksture
	glBindTexture(GL_TEXTURE_2D, texture); // Vezujemo se za teksturu kako bismo je podesili

	// Generisanje mipmapa - predefinisani različiti formati za lakše skaliranje po potrebi (npr. da postoji 32 x 32 verzija slike, ali i 16 x 16, 256 x 256...)
	glGenerateMipmap(GL_TEXTURE_2D);

	// Podešavanje strategija za wrap-ovanje - šta da radi kada se dimenzije teksture i poligona ne poklapaju
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S - tekseli po x-osi
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T - tekseli po y-osi

	// Podešavanje algoritma za smanjivanje i povećavanje rezolucije: nearest - bira najbliži piksel, linear - usrednjava okolne piksele
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void formVAOs(float* verticesRect, size_t rectSize, float* verticesCirc, size_t circSize, unsigned int& VAOrect, unsigned int& VAOcirc) {
	unsigned int VBOrect;
	glGenVertexArrays(1, &VAOrect);
	glGenBuffers(1, &VBOrect);

	glBindVertexArray(VAOrect);
	glBindBuffer(GL_ARRAY_BUFFER, VBOrect);
	glBufferData(GL_ARRAY_BUFFER, rectSize, verticesRect, GL_STATIC_DRAW);

	// Atribut 0 (pozicija):
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Atribut 1 (boja):
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Krug

	unsigned int VBOcirc;
	glGenVertexArrays(1, &VAOcirc);
	glGenBuffers(1, &VBOcirc);

	glBindVertexArray(VAOcirc);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcirc);
	glBufferData(GL_ARRAY_BUFFER, circSize, verticesCirc, GL_STATIC_DRAW);

	// Atribut 0 (pozicija):
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Atribut 1 (boja):
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void drawRect(unsigned int rectShader, unsigned int VAOrect) {
	glUseProgram(rectShader); // Podešavanje da se crta koristeći šejder za četvorougao

	glBindVertexArray(VAOrect); // Podešavanje da se crta koristeći vertekse četvorougla
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Crtaju se trouglovi tako da formiraju četvorougao
}

void drawCirc(unsigned int colorShader, unsigned int VAOcirc) {
	glUseProgram(colorShader);
	float pulsingSpeed = 1.0f;
	// Brzina pulsiranja se podešava pomoću pulsingSpeed
	// Sinus preslikava proteklo vreme (pomnoženo brzinom) na opseg (-1, 1), potrebno ga je transformisati na (0, 1) jer se plava komponenta kreće u tom opsegu
	// Menjamo samo plavu komponentu, jer je ona jedina razlika između žute (R=1, G=1, B=0) i bele (R=1, G=1, B=1) boje
	glUniform1f(glGetUniformLocation(colorShader, "uB"), sin(glfwGetTime() * pulsingSpeed) / 2 + 0.5f);
	// Sa slike zadatka 9, centar kružnice koja opisuje putanju je otprilike u (0, -1) i poluprečnik 1,5
	// xcPath i yPath su podešeni tako da se krug sa svoje originalne pozicije (0, 0.5) pomera na centar putanje i odatle započinje pokretanje
	float xcPath = 0.0f;
	float ycPath = -1.5f;
	float rPath = 1.5f;
	float rotationSpeed = 0.2f;
	// Pređeni ugao zavisi od proteklog vremena i podešene brzine kretanja
	// Sabira se sa pi/2, odnosno 90 stepeni, kako bi kretanje počelo na gornjoj tački kružne putanje
	float anglePath = glfwGetTime() * rotationSpeed + M_PI / 2;
	// Formula za x i y pomeraj je ista kao za formiranje kruga u verticesCirc
	float uXCirc = cos(anglePath) * rPath + xcPath;
	float uYCirc = sin(anglePath) * rPath + ycPath;
	glUniform2f(glGetUniformLocation(colorShader, "uPos"), uXCirc, uYCirc); // Zadatak 4

	glBindVertexArray(VAOcirc); // Podešavanje da se crta koristeći vertekse kruga
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SLICES + 2);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "SmartWatch2D", monitor, NULL);
	if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
	glfwMakeContextCurrent(window);

	GLFWcursor* heartCursor = loadImageToCursor("./res/red_heart.svg.png");
	if (heartCursor != nullptr) {
		glfwSetCursor(window, heartCursor);
	}
	else {
		std::cout << "WARNING: CURSOR NOT SET, DEFAULTING TO BASE POINTER" << std::endl;
	}

	if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//Set everything after here
	unsigned rectShader = createShader("rect.vert", "rect.frag");
	glUseProgram(rectShader);


	glClearColor(0.5f, 0.6f, 1.0f, 1.0f);

	//Start of main loop
	while (!glfwWindowShouldClose(window))
	{
		double initFrameTime = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
		while (glfwGetTime() - initFrameTime < 1 / 75.0) {}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}