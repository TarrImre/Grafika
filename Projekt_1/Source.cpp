#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

/** Hagyományos tömb is használható a csúcspontok tárolásához. */
//0.16 az 50 pixel, 100pixel jobbra és balra kerekítve 0.33

GLfloat lineWidth = 0.33f;
static GLfloat verticalLine[] =
{
	-lineWidth, 0.0f, 0.00f,
	 lineWidth, 0.0f, 0.00f
};

/* Vektorok a szakasz vagy kör pontok tárolásához. */
std::vector<glm::vec3> Circle_vertic;
std::vector<glm::vec3> Circle_Color;
std::vector<glm::vec3> Circle_Color2;

/* Vertex buffer objektum és vertex array objektum az adattároláshoz. */
#define		numVBOs			3
#define		numVAOs			2
GLuint		VBO[numVBOs]; 
GLuint		VAO[numVAOs];

int			window_width = 600;
int			window_height = 600;
char		window_title[] = "Tarr Imre | Projekt 1";

/** A normál billentyűk a [0..255] tartományban vannak, a nyilak és a speciális billentyűk pedig a [256..511] tartományban helyezkednek el. */
GLboolean	keyboard[512] = { GL_FALSE };
GLFWwindow* window = nullptr;

//Vízszintes és függőleges mozgás
bool		xDir = true;
bool		yDir = false;

GLuint		Program_Line;
GLuint		Program_Circle;
GLfloat x = 0.0f;
GLfloat y = 0.0f;
GLfloat vx = 0.01f;
//2 = 600px 1 = 300 0.5 = 150 0.5/3=0.16 
GLfloat radius = 0.16f;

// Kör megrajzolása
void drawCircle()
{
	int numSegments = 360;
	for (int i = 0; i <= numSegments; ++i) {
		GLfloat theta = 2.0f * M_PI * i / numSegments;
		GLfloat cx = x + radius * cosf(theta);
		GLfloat cy = y + radius * sinf(theta);
		if (i % 2 == 0) {
			Circle_vertic.push_back(glm::vec3(cx, cy, 0));
			//Belső
			Circle_Color2.push_back(glm::vec3(1.0, 0.0, 0.0));
			//Külső
			Circle_Color.push_back(glm::vec3(0.0, 1.0, 0.0));
		}
		else {
			Circle_vertic.push_back(glm::vec3(0, 0, 0));
			//Külső
			Circle_Color2.push_back(glm::vec3(0.0, 1.0, 0.0));
			//Belső
			Circle_Color.push_back(glm::vec3(1.0, 0.0, 0.0));
		}

	}
}
void movementCircle() {
	if (xDir) {
		x += vx;
		//Jobb oldali fal
		if (x + radius >= 1.0f) {
			x = 1.0f - radius;
			vx = -vx;
		}
		//Bal oldal fal
		else if (x - radius <= -1.0f) {
			x = -1.0f + radius;
			vx = -vx;
		}
		//Y tengely
		y = 0.0f;
	}
	else if (yDir) {
		y += vx;
		if (y + radius >= 1.0f) {
			y = 1.0f - radius;
			vx = -vx;
		}
		else if (y - radius <= -1.0f) {
			y = -1.0f + radius;
			vx = -vx;
		}
		x = 0.0f;
	}
}

bool checkOpenGLError() {
	bool	foundError = false;
	int		glErr = glGetError();

	/** Vizsgáljuk meg, hogy van-e aktuálisan OpenGL hiba, és amennyiben igen, írassuk ki azokat a konzolra egyenként. */
	/** Check for OpenGL errors, and send them to the console ony by one. */
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;

		foundError = true;
		glErr = glGetError();
	}

	/** Ha van aktuálisan OpenGL hiba, a visszatérési érték true. */
	/** If there are OpenGL errors, the return value is true. */
	return foundError;
}
void printShaderLog(GLuint shader) {
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	/** Vizsgáljuk meg, hogy van-e valami a Shader Info Logban, és amennyiben igen, írassuk ki azt a konzolra soronként. */
	/** Check for Shader Info Log, and send it to the console by lines if it is created for the last compile. */
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		log = (char*)malloc(length);

		/** Olvassuk és írassuk ki a következő sort. */
		/** Read out and and send to the console the next line. */
		glGetShaderInfoLog(shader, length, &charsWritten, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	/** A file stream inicializálása olvasásra. */
	ifstream	fileStream(filePath, ios::in);
	string		content;
	string		line;

	/** A shader fájl sorainak beolvasása EOF jelzésig. (EOF = End Of File) */
	while (!fileStream.eof()) {
		getline(fileStream, line);
		/** A shader fájl sorainak összefűzése. */
		content.append(line + "\n");
	}

	/** A file stream lezárása. */
	fileStream.close();

	/** Visszatérés a shader fájl tartalmával. */
	return content;
}

GLuint createShaderLine() {
	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;

	/** Beolvassuk a shader fájlok tartalmát. */
	string		vertShaderStr = readShaderSource("vertexShader.glsl");
	string		fragShaderStr = readShaderSource("fragmentShader.glsl");

	/** Létrehozzuk a shader objektumokat és eltároljuk az ID-ket. */
	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);

	/** A shader fájlok tartalmát eltároló string objektum szöveggé konvertálásás is elvégezzük. */
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	/** Ekkor a betöltött kódot hozzárendelhetjük a shader objektumainkhoz. */
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	/** Fordítsuk le a vertex shader objektumhoz rendelt kódot. */
	glCompileShader(vShader);
	/** Hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	/** Error checking. Was the compile step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}

	/** Fordítsuk le a fragment shader objektumhoz rendelt kódot. */
	glCompileShader(fShader);
	/** Hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	/** Error checking. Was the compile step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}

	/** Shader program objektum létrehozása: ez fogja össze a shadereket. Eltároljuk az ID értéket. */
	GLuint		vfProgram = glCreateProgram();

	/** Csatoljuk a shadereket az előző lépésben létrehozott objektumhoz. */
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	/** Végül a GLSL szerkesztő ellenőrzi, hogy a csatolt shaderek kompatibilisek-e egymással. */
	glLinkProgram(vfProgram);

	/** Hibakeresési lépések. Például sikeres volt-e az összeszerkesztés? Ha nem, mi volt az oka? */
	/** Error checking. Was the link step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "Shader linking failed." << endl;
	}

	/** Ha minden rendben ment a linkelés során, a shader objektumok törölhetőek. */
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	/** Az elkészült program azonosítója a visszatérési értékünk. */
	return vfProgram;
}

GLuint createShaderCircle() {
	/* ==||== */

	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;

	string		vertShaderStr = readShaderSource("vertexShaderCircle.glsl");
	string		fragShaderStr = readShaderSource("fragmentShaderCircle.glsl");

	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);

	glCompileShader(fShader);

	GLuint		vfProgram = glCreateProgram();

	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	glLinkProgram(vfProgram);

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}

void init(GLFWwindow* window) {
	/** A rajzoláshoz használt shader programok betöltése. */
	Program_Line = createShaderLine();
	Program_Circle = createShaderCircle();

	/* Létrehozzuk a szükséges vertex buffer és vertex array objektumokat. */
	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);

	// Kör kirajzolása
	drawCircle();

	/* Csatoljuk a vertex array objektumunkat a shader programhoz. */
	//Kör
	glBindVertexArray(VAO[0]);

	/* A GL_ARRAY_BUFFER nevesített csatolóponthoz kapcsoljuk a vertex buffert (ide kerülnek a csúcs/körpont adatok). */
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	/* Másoljuk az adatokat a bufferbe! Megadjuk az aktuálisan csatolt buffert, azt hogy hány byte adatot másolunk,
	a másolandó adatot, majd a feldolgozás módját is meghatározzuk: most az adat nem változik a feltöltés után. */
	glBufferData(GL_ARRAY_BUFFER, Circle_vertic.size() * sizeof(glm::vec3), Circle_vertic.data(), GL_STATIC_DRAW);
	/* Ezen adatok szolgálják a location = 0 vertex attribútumot (itt: pozíció).
	   Elsőként megadjuk ezt az azonosítószámot (vertexShader.glsl).
	   Utána az attribútum méretét (vec3, láttuk a shaderben).
	   Harmadik az adat típusa.
	   Negyedik az adat normalizálása, ez maradhat FALSE jelen példában.
	   Az attribútum értékek hogyan következnek egymás után? Milyen lépésköz után találom a következő vertex adatait?
	   Végül megadom azt, hogy honnan kezdődnek az értékek a pufferben. Most rögtön, a legelejétől veszem őket. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// =||= Szín
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	// =||=
	glBufferData(GL_ARRAY_BUFFER, Circle_Color.size() * sizeof(glm::vec3), Circle_Color.data(), GL_STATIC_DRAW);
	// =||=
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	/* Engedélyezzük az imént definiált location = 0 attribútumot (vertexShader.glsl). */
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	/* Leválasztjuk a vertex array objektumot és a buffert is. */
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Vonal
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticalLine), verticalLine, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/** Aktiváljuk a shader-program objektumunkat az alapértelmezett fix csővezeték helyett. */
	glUseProgram(0);

	/** Ha pontokat vagy szakaszokat rajzolunk, kísérletezzünk a pont vagy szakasz méretének növelésével! */
	glLineWidth(3);

	/** Állítsuk be a törlési színt az áttetszőségi értékkel együtt! [0.0, 1.0] */
	glClearColor(1.0, 1.0, 0.0, 1.0);
}

void display(GLFWwindow* window, double currentTime) {
	/* Töröljük le a kiválasztott buffereket! Fontos lehet minden egyes alkalommal törölni! */
	glClear(GL_COLOR_BUFFER_BIT);

	/*!Program_Line!*/
	glUseProgram(Program_Line);
	/* Csatoljuk fel a vertex array objektumot a shader programhoz. */
	glBindVertexArray(VAO[1]);
	/** A megadott adatok segítségével törött szakaszt és azok végpontjait rajzoljuk meg. */
	glDrawArrays(GL_LINES, 0, sizeof(verticalLine) / sizeof(verticalLine[0]));
	/* Leválasztjuk a VAO-t, nehogy bármilyen érték felülíródjon. */
	glBindVertexArray(0);

	/*!Program_Circle!*/
	glUseProgram(Program_Circle);
	/* Csatoljuk fel a vertex array objektumot a shader programhoz. */
	glBindVertexArray(VAO[0]);
	GLuint offsetLoc3 = glGetUniformLocation(Program_Circle, "offsetX");
	glProgramUniform1f(Program_Circle, offsetLoc3, x);
	GLuint offsetLoc4 = glGetUniformLocation(Program_Circle, "offsetY");
	glProgramUniform1f(Program_Circle, offsetLoc4, y);
	/** A megadott adatok segítségével törött szakaszt és azok végpontjait rajzoljuk meg. */
	glDrawArrays(GL_TRIANGLE_STRIP, 0, Circle_vertic.size());
	/* Leválasztjuk a VAO-t, nehogy bármilyen érték felülíródjon. */
	glBindVertexArray(0);
}
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width = width;
	window_height = height;

	/** A kezelt képernyő beállítása a teljes (0, 0, width, height) területre. */
	glViewport(0, 0, width, height);
}
void cleanUpScene() {
	/** Töröljük a vertex array és a vertex buffer objektumokat. */
	glDeleteVertexArrays(numVAOs, VAO);
	glDeleteBuffers(numVBOs, VBO);

	/** Töröljük a shader programot. */
	glDeleteProgram(Program_Line);
	glDeleteProgram(Program_Circle);

	/** Töröljük a GLFW ablakot. */
	glfwDestroyWindow(window);

	/** Leállítjuk a GLFW-t. */
	glfwTerminate();

	/** Kilépés EXIT_SUCCESS kóddal. */
	exit(EXIT_SUCCESS);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/** ESC billentyűre kilépés. */
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		cleanUpScene();

	/** A billentyűk lenyomásának és felengedésének regisztrálása. Lehetővé teszi gombkombinációk használatát. */
	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;

	//Mozgás függőlegesen
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		yDir = true;
		xDir = false;
	}

	//Mozgás vízszintesen
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		xDir = true;
		yDir = false;
	}
}
int main(void) {
	/** Próbáljuk meg inicializálni a GLFW-t! */
	if (!glfwInit())
		exit(EXIT_FAILURE);

	/** A használni kívánt OpenGL verzió: 4.3. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	/** A képernyő átméretezés kezelése. */
	//glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	
	/** Próbáljuk meg létrehozni az ablakunkat. */
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);
	/** A window ablak minimum és maximum szélességének és magasságának beállítása. */
	glfwSetWindowSizeLimits(window, window_width, window_height, window_width, window_height);
	/** A window oldalarányának megadása a számláló és az osztó segítségével. */
	glfwSetWindowAspectRatio(window, 1, 1);
	/** Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	glfwMakeContextCurrent(window);

	/** Billentyűzethez köthető események kezelése. */
	glfwSetKeyCallback(window, keyCallback);

	/** Incializáljuk a GLEW-t, hogy elérhetővé váljanak az OpenGL függvények, probléma esetén kilépés EXIT_FAILURE értékkel. */
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	/** 0 = v-sync kikapcsolva, 1 = v-sync bekapcsolva, n = n db képkockányi időt várakozunk */
	glfwSwapInterval(1);

	/** Az alkalmazáshoz kapcsolódó előkészítő lépések, pl. a shader objektumok létrehozása. */
	init(window);
	if (!window) {
		std::cerr << "Sikertelen GLFW ablak létrehozása" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	/** A megadott window struktúra "close flag" vizsgálata. */
	while (!glfwWindowShouldClose(window)) {
		//Kör mozgása
		movementCircle();

		/** A kód, amellyel rajzolni tudunk a GLFWwindow objektumunkba. */
		display(window, glfwGetTime());

		/** Double buffered működés. */
		glfwSwapBuffers(window);

		/** Események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás. */
		glfwPollEvents();
	}

	/** Felesleges objektumok törlése. */
	cleanUpScene();

	/** Kilépés EXIT_SUCCESS kóddal. */
	return EXIT_SUCCESS;
}