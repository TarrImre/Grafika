#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

std::vector<glm::vec3> pointToDraw;

std::vector<glm::vec3> myControlPoints = {
			glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
};

GLuint		VBO;
GLuint		VAO;

int			window_width = 600;
int			window_height = 600;
char		window_title[] = "Horvath_Karoly_masodik_beadando";
int			kontrollpontok = 4;

GLint dragged = -1;
GLdouble updateFrequency = 0.01, lastUpdate;
GLboolean	keyboard[512] = { GL_FALSE };
GLFWwindow* window = nullptr;
GLuint		renderingProgram;
GLuint		renderingProgram_c;

void updateVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, myControlPoints.size() * sizeof(glm::vec3), myControlPoints.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool checkOpenGLError() {
	bool	foundError = false;
	int		glErr = glGetError();

	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;

		foundError = true;
		glErr = glGetError();
	}

	return foundError;
}

void printShaderLog(GLuint shader) {
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		log = (char*)malloc(length);

		glGetShaderInfoLog(shader, length, &charsWritten, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		log = (char*)malloc(length);

		glGetProgramInfoLog(prog, length, &charsWritten, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram() {
	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;
	string		vertShaderStr = readShaderSource("vertexShader.glsl");
	string		fragShaderStr = readShaderSource("fragmentShader.glsl");
	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "Vertex compilation failed." << endl;
		printShaderLog(vShader);
	}

	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}

	GLuint		vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "Shader linking failed." << endl;
		printProgramLog(vfProgram);
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}

GLuint createShaderProgram_c() {
	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;
	string		vertShaderStr = readShaderSource("vertexShader_c.glsl");
	string		fragShaderStr = readShaderSource("fragmentShader_c.glsl");
	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "Vertex compilation failed." << endl;
		printShaderLog(vShader);
	}

	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}

	GLuint		vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "Shader linking failed." << endl;
		printProgramLog(vfProgram);
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}

long double NCR(int n, int r) {

	if (r == 0) return 1;

	if (r > n / 2) return NCR(n, n - r);

	long double result = 1;

	for (int k = 1; k <= r; ++k) {
		result *= n - k + 1;
		result /= k;
	}

	return result;
}

int fact(int n) {
	if (n == 0)
		return 1;
	if (n == 0)
		return 1;
	return n * fact(n - 1);
}

GLfloat poly(int n, int i, GLfloat t)
{
	GLfloat B = (fact(n) / (fact(i) * fact(n - i)) * pow(t, i) * pow(1 - t, n - i));
	return B;
}

GLfloat dist2(glm::vec3 P1, glm::vec3 P2)
{
	GLfloat t1 = P1.x - P2.x;
	GLfloat t2 = P1.y - P2.y;

	return t1 * t1 + t2 * t2;
}

GLint getActivePoint(vector<glm::vec3> p, GLint size, GLfloat sens, GLfloat x, GLfloat y)
{

	GLint i;
	GLfloat s = sens * sens;

	float xNorm = x / (window_width / 2) - 1.0f;
	float yNorm = y / (window_height / 2) - 1.0f;
	glm::vec3 P = glm::vec3(xNorm, yNorm, 0.0f);

	for (i = 0; i < size; i++) {
		if (dist2(p[i], P) < s) {
			return i;
		}
	}
	return -1;

}

void drawBezierCurve() {
	glm::vec3 nextPoint;
	GLfloat t = 0.0f;
	GLfloat increment = 0.001f;

	while (t <= 1.0f)
	{
		nextPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < kontrollpontok; i++)
		{
			nextPoint.x = nextPoint.x + (poly(kontrollpontok - 1, i, t) * myControlPoints.at(0 + i * 2).x);
			nextPoint.y = nextPoint.y + (poly(kontrollpontok - 1, i, t) * myControlPoints.at(0 + i * 2).y);
			nextPoint.z = nextPoint.z + (poly(kontrollpontok - 1, i, t) * myControlPoints.at(0 + i * 2).z);
		}

		myControlPoints.push_back(glm::vec3(nextPoint.x, nextPoint.y, nextPoint.z));
		myControlPoints.push_back(glm::vec3(1.0f, 0.0f, 0.0f));


		t += increment;
	}
	updateVBO();
}

void init(GLFWwindow* window) {
	
	renderingProgram = createShaderProgram();
	renderingProgram_c = createShaderProgram_c();
	myControlPoints.insert(myControlPoints.begin() + 8, glm::vec3(myControlPoints[6].x + (myControlPoints[6].x - myControlPoints[4].x), myControlPoints[6].y + (myControlPoints[6].y - myControlPoints[4].y), 0.0f));
	myControlPoints.insert(myControlPoints.begin() + 9, glm::vec3(0.0f, 0.0f, 0.0f));
	updateVBO();
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, myControlPoints.size() * sizeof(glm::vec3), myControlPoints.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT); // fontos lehet minden egyes alkalommal t�r�lni!
	glUseProgram(renderingProgram);
	drawBezierCurve();
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, kontrollpontok);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(15.0f);

	
	glUseProgram(renderingProgram_c);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	glDrawArrays(GL_POINTS, 0, kontrollpontok);
	glPointSize(4.0f);
	glUseProgram(renderingProgram);
	glDrawArrays(GL_POINTS, kontrollpontok, myControlPoints.size());

	std::vector<glm::vec3> pts = { myControlPoints[0], myControlPoints[2], myControlPoints[4], myControlPoints[6], myControlPoints[8], myControlPoints[10], myControlPoints[12], myControlPoints[14], myControlPoints[16], myControlPoints[18], myControlPoints[20],
								   myControlPoints[22], myControlPoints[24], myControlPoints[26], myControlPoints[28], myControlPoints[30], myControlPoints[32], myControlPoints[34], myControlPoints[36], myControlPoints[38], myControlPoints[40], myControlPoints[42] };


	if (currentTime - lastUpdate >= updateFrequency) {
		drawBezierCurve();
		lastUpdate = currentTime;
		while (myControlPoints.size() > 44) {
			myControlPoints.pop_back();
		}
		updateVBO();
	}

	glBindVertexArray(0);
	
}


void cleanUpScene()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteProgram(renderingProgram);
	glDeleteProgram(renderingProgram_c);

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		cleanUpScene();

	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	GLint i;

	GLfloat xNorm = xPos / (window_width / 2) - 1.0f;
	GLfloat yNorm = (window_height - yPos) / (window_height / 2) - 1.0f;

	if (dragged >= 0)
	{

		myControlPoints.at(dragged).x = xNorm;
		myControlPoints.at(dragged).y = yNorm;
	/*	myControlPoints[dragged + 1] = { 1.0f, 1.0f, 0.0f };
		glBindBuffer(GL_ARRAY_BUFFER, VBO);*/
		glBufferData(GL_ARRAY_BUFFER, myControlPoints.size() * sizeof(glm::vec3), myControlPoints.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}

void addPoint(double xPos, double yPos) {
	GLfloat xNorm = xPos / (window_width / 2) - 1.0f;
	GLfloat yNorm = (window_height - yPos) / (window_height / 2) - 1.0f;
	//double yNorm = yPos;
	myControlPoints.insert((myControlPoints.begin() + (kontrollpontok * 2)), glm::vec3(xNorm, yNorm, 0.0f));
	myControlPoints.insert((myControlPoints.begin() + (kontrollpontok * 2 + 1)), glm::vec3(1.0f, 1.0f, 0.0f));
	kontrollpontok++;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, myControlPoints.size() * sizeof(glm::vec3), myControlPoints.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void removePoint(int i) {
	myControlPoints.erase((myControlPoints.begin() + (i + 1)));
	myControlPoints.erase((myControlPoints.begin() + (i)));
	kontrollpontok--;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GLint i;
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if ((i = getActivePoint(myControlPoints, myControlPoints.size(), 0.1f, x, window_height - y)) != -1 && i % 2 == 0 && i < 17)
		{
			dragged = i;
		}
		else {
			addPoint(x, y);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		/*if (dragged >= 0)
			myControlPoints[dragged + 1] = { 1.0f, 0.0f, 0.0f };*/

		dragged = -1;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if ((i = getActivePoint(myControlPoints, myControlPoints.size(), 0.1f, x, window_height - y)) != -1 && i % 2 == 0 && i < 17)
		{
			removePoint(i);
		}
	}

}

int main(void) {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);


	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glfwSwapInterval(1);

	glfwSetWindowSizeLimits(window, 400, 400, 800, 800);
	glfwSetWindowAspectRatio(window, 1, 1);

	init(window);
	lastUpdate = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUpScene();

	return EXIT_SUCCESS;
}