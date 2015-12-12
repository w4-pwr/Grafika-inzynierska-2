/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj¹ osi
//  uk³adu wspó³rzêdnych dla rzutowania perspektywicznego

/*************************************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>
#include <math.h>
using namespace std;
typedef float point3[3];


static GLfloat viewer[] = { 0.0, 0.0, 10.0 };


GLfloat light_positionBlue[] = { -30.0, 0.0, -10.0, 1.0 }; // po³o¿enie Ÿród³a
GLfloat light_position[] = { 30.0, 0.0, -10.0, 1.0 }; // po³o¿enie Ÿród³a

// inicjalizacja po?o?enia obserwatora
/*************************************************************************************/
// Funkcja rysuj?ca osie uk?adu wspó?rz?dnych

static GLfloat theta = 0.0;   // k?t obrotu obiektu
static GLfloat phi = 0.0;		// kat obrotu wokol x
static GLfloat alfa = 0.0;
static GLfloat pix2angle;     // przelicznik pikseli na stopnie

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naciœniêto ¿adnego klawisza
							   // 1 - naciœniêty zostaæ lewy klawisz
							   // 2 - wcisniety prawy klawisz

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int y_pos_old = 0;
static int scroll_old = 0;
static int delta_x = 0;        // ró¿nica pomiêdzy pozycj¹ bie¿¹c¹
static int delta_y = 0;
static int delta_scroll = 0;

static int n = 80;
void Egg() {
	point3 **vectors3d = new point3*[n];
	point3 **vektorNorm = new point3*[n];
	for (int i = 0; i < n; i++) {
		vectors3d[i] = new point3[n];
		vektorNorm[i] = new point3[n];
	}


	double odstep = 1 / (double)(n - 1);
	double ***punkty2D = new double**[n];
	for (int i = 0; i < n; i++) {
		punkty2D[i] = new double*[n];
		for (int j = 0; j < n; j++) {
			punkty2D[i][j] = new double[2];
			punkty2D[i][j][0] = i*odstep;
			punkty2D[i][j][1] = j*odstep;
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			double u = punkty2D[i][j][0];
			double v = punkty2D[i][j][1];
			vectors3d[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u)*cos(3.14 *v);
			vectors3d[i][j][1] = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5;
			vectors3d[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u)*sin(3.14 *v);

			float ux, uz, uy, vz, vy, vx, length;

			// obliczenie wartosci wektorow normalnych
			ux = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(3.14*v);
			uy = (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
			uz = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(3.14*v);

			vx = 3.14*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(3.14*v);
			vy = 0;
			vz = -3.14*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(3.14*v);

			vektorNorm[i][j][0] = uy*vz - uz*vy;
			vektorNorm[i][j][1] = uz*vx - ux*vz;
			vektorNorm[i][j][2] = ux*vy - uy*vx;

			length = sqrt(vektorNorm[i][j][0] * vektorNorm[i][j][0] + vektorNorm[i][j][1] * vektorNorm[i][j][1] + vektorNorm[i][j][2] * vektorNorm[i][j][2]);
			if (i == 0 || i == n)
			{
				vektorNorm[i][j][0] = 0;
				vektorNorm[i][j][1] = -1;
				vektorNorm[i][j][2] = 0;
			}
			else if (i < n / 2)
			{
				vektorNorm[i][j][0] = (uy*vz - uz*vy) / length;
				vektorNorm[i][j][1] = (uz*vx - ux*vz) / length;
				vektorNorm[i][j][2] = (ux*vy - uy*vx) / length;
			}
			else if (i > n / 2)
			{
				vektorNorm[i][j][0] = -1 * (uy*vz - uz*vy) / length;
				vektorNorm[i][j][1] = -1 * (uz*vx - ux*vz) / length;
				vektorNorm[i][j][2] = -1 * (ux*vy - uy*vx) / length;
			}
			else
			{
				vektorNorm[i][j][0] = 0;
				vektorNorm[i][j][1] = 1;
				vektorNorm[i][j][2] = 0;
			}


		}
	}
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - 1; j++) {

			glColor3f(255.0f, 255.0f, 255.0f);
			glBegin(GL_TRIANGLES);
			glNormal3fv(vektorNorm[i][j + 1]);
			glVertex3fv(vectors3d[i][j + 1]);

			glNormal3fv(vektorNorm[i + 1][j]);
			glVertex3fv(vectors3d[i + 1][j]);

			glNormal3fv(vektorNorm[i + 1][j + 1]);
			glVertex3fv(vectors3d[i + 1][j + 1]);
			glEnd();


			glBegin(GL_TRIANGLES);
			glNormal3fv(vektorNorm[i][j]);
			glVertex3fv(vectors3d[i][j]);

			glNormal3fv(vektorNorm[i + 1][j]);
			glVertex3fv(vectors3d[i + 1][j]);

			glNormal3fv(vektorNorm[i][j + 1]);
			glVertex3fv(vectors3d[i][j + 1]);
			glEnd();

		}
	}
}


void Axes(void)
{
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz¹tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz¹tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz¹tek i koniec obrazu osi y

	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora
		y_pos_old = y;
		// jako pozycji poprzedniej
		status = 1;          // wciêniêty zosta³ lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		scroll_old = y;
		// jako pozycji poprzedniej
		status = 2;          // wciêniêty zosta³ prawy klawisz myszy
	}
	else
		status = 0;          // nie zosta³ wciêniêty ¿aden klawisz
}


void Motion(GLsizei x, GLsizei y)
{
	if (status == 1) {
		delta_x = x - x_pos_old;     // obliczenie ró¿nicy po³o¿enia kursora myszy
		delta_y = y - y_pos_old;
		x_pos_old = x;            // podstawienie bie¿¹cego po³o¿enia jako poprzednie
		y_pos_old = y;
	}
	else if (status == 2) {
		delta_scroll = y - scroll_old;
		scroll_old = y;
	}
	glutPostRedisplay();     // przerysowanie obrazu sceny
}

static int zoom = 30;
void calculateLightPosition() {
	light_positionBlue[0] = zoom*cos(theta) * cos(phi);
	light_positionBlue[1] = zoom*sin(phi);
	light_positionBlue[2] = zoom * sin(theta) * cos(phi);

	
	light_position[0] = zoom * cos(theta) * cos(phi) +50;
	light_position[1] = zoom * sin(phi);
	light_position[2] = zoom * sin(theta) * cos(phi);

}

void setupLight() {
	// Definicja materia³u z jakiego zrobiony jest czajnik

	GLfloat mat_ambient[] = { 1.0,1.0, 1.0, 1.0 };
	// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego               

	GLfloat mat_shininess = { 50.0 };
	// wspó³czynnik n opisuj¹cy po³ysk powierzchni

	/*************************************************************************************/
	// Definicja Ÿród³a œwiat³a numer 1 - red

	GLfloat light_ambient[] = { 1.0, 0.0, 0.0, 1.0 };	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
														// Ia = [Iar,Iag,Iab]
	GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 }; // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
													  // odbicie dyfuzyjne Id = [Idr,Idg,Idb]
	GLfloat light_specular[] = { 1.0, 0.0, 0.0, 1.0 };	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
														// odbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat att_constant = { 1.0 }; // sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
									// odleg³oœci od Ÿród³a
	GLfloat att_linear = { (GLfloat) 0.05 }; // sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
											 // odleg³oœci od Ÿród³a
	GLfloat att_quadratic = { (GLfloat) 0.001 }; // sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
												 // odleg³oœci od Ÿród³a


												 /*************************************************************************************/
												 // Definicja Ÿród³a œwiat³a numer 2 - blue

	GLfloat light_ambientBlue[] = { 0.0, 0.0, 1.0, 1.0 };	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
															// Ia = [Iar,Iag,Iab]
	GLfloat light_diffuseBlue[] = { 0.0, 0.0,1.0, 1.0 }; // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
														 // odbicie dyfuzyjne Id = [Idr,Idg,Idb]
	GLfloat light_specularBlue[] = { 0.0, 0.0,1.0, 1.0 };	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
															// odbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat att_constantBlue = { 1.0 }; // sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
										// odleg³oœci od Ÿród³a
	GLfloat att_linearBlue = { (GLfloat) 0.05 }; // sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
												 // odleg³oœci od Ÿród³a
	GLfloat att_quadraticBlue = { (GLfloat) 0.001 }; // sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
													 // odleg³oœci od Ÿród³a

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	/*************************************************************************************/
	// Ustawienie parametrów Ÿród³a

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambientBlue);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuseBlue);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specularBlue);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constantBlue);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linearBlue);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadraticBlue);


	/*************************************************************************************/
	// Ustawienie opcji systemu oœwietlania sceny

	glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
	glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny
	glEnable(GL_LIGHT0);     // w³¹czenie Ÿród³a o numerze 0
	glEnable(GL_LIGHT1);     // w³¹czenie Ÿród³a o numerze 0
	glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym

	glLoadIdentity();
	// Czyszczenie macierzy bie??cej
	float scroll = delta_scroll*pix2angle;
	if (status == 2 && ((viewer[2] <= 6 && scroll > 0) || (scroll < 0 && viewer[2] >= 20) || (viewer[2]>6 && viewer[2] < 20))) {
		viewer[2] += scroll;
	}
	gluLookAt(0,0, viewer[2], 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);	
	theta += delta_x*pix2angle;    // modyfikacja k¹ta obrotu o kat proporcjonalny
	phi += delta_y*pix2angle;

	float pi = 3.14;
	if (phi > 2 * pi) phi = 2 * pi;
	if (theta > 2 * pi) theta = 2 * pi;
	if (phi < 0) phi = 0;
	if (theta < 0) theta = 0;


	calculateLightPosition();

	glLightfv(GL_LIGHT1, GL_POSITION, light_positionBlue);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glColor3f(1.0f, 23.0f, 1.0f);
	// Ustawienie koloru rysowania na bia³y
	Axes();
	Egg();
	// Narysowanie czajnika

	glFlush();
	// Przekazanie poleceñ rysuj¹cych do wykonania

	glutSwapBuffers();
}

void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	setupLight();
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;
	pix2angle *= 0.009;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)horizontal / vertical;

	gluPerspective(70, (float)aspect, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego


	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci
	// relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna

	glMatrixMode(GL_MODELVIEW);
	// Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu 

	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej

}

void main(void)
{
	char *myargv[1];
	int myargc = 1;
	myargv[0] = _strdup("Myappname");
	glutInit(&myargc, myargv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(1000, 1000);

	glutCreateWindow("Oœwietlenie jajka");

	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹
	// (callback function).  Bêdzie ona wywo³ywana za ka¿dym razem
	// gdy zajdzie potrzeba przerysowania okna


	glutMouseFunc(Mouse);

	glutMotionFunc(Motion);

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹
	// za zmiany rozmiaru okna                      


	MyInit();
	// Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst¹pieniem do renderowania

	glEnable(GL_DEPTH_TEST);
	// W³¹czenie mechanizmu usuwania niewidocznych elementów sceny

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

}
