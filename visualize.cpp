#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <random>
using namespace std;

#define WINDOW_X (900)
#define WINDOW_Y (900)
#define WINDOW_NAME "virtual room tour"
#define TEXTURE_HEIGHT (1000)
#define TEXTURE_WIDTH (1000)
#define ROTATE "rotate.wav"
#define MOVE "move.wav"
#define WALK "/usr/share/sounds/ubuntu/stereo/message.ogg"


vector<vector<int>> room;
vector<string> split(string str, char del);
void init_GL(int argc, char *argv[]);
void init();
void set_callback_functions();
void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);
void glut_idle();
void set_texture();
void timer_r(int value);
void timer_l(int value);
void detect_wall(vector<vector<int>> room);
void draw_plane(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3,double x4,double y4,double z4,double R,double G,double B);
void draw_plane2(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3,double x4,double y4,double z4,double R,double G,double B);
void draw_wall(vector<vector<vector<int>>> wall_points);
void draw_rectangle(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3,double x4,double y4,double z4,double x5,double y5,double z5,double x6,double y6,double z6,double x7,double y7,double z7,double x8,double y8,double z8,double R,double G,double B);
void draw_bed(double x, double z, double x_len, double z_len, double R, double G, double B);
void draw_shelf(double x, double z, double x_len, double y_len, double z_len, double R, double G, double B);
void draw_table(double x, double z, double x_len, double y_len, double z_len, double R, double G, double B);

// グローバル変数
int flag;
double g_angle1 = 4.4;
double g_angle2 = -2.64;
double g_distance = 90.0;
double g_distance1 = 5;
double camera_x = 16;
double camera_z = 11.3;
double stand_x = 5;
double stand_y = 11;
bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;
// bed, shelf, desk
// 引数に渡されるtxtファイルに記載順で回転角の配列を作成
double rotate_angle[3] = {-90, 0, 0};
int kaguindex = -1; // -1のときは人間モード
int count_rotate = 0; // 回転回数の変数
int vision_mode = 0;
GLuint g_TextureHandles[3] = {0,0,0};

vector<vector<vector<int>>> wall_points;
vector<string> kagu_name;
vector<int> kagu_x;
vector<int> kagu_y;
vector<int> kagu_z;
vector<int> place_x{30,11,20};
vector<int> place_z{17,0,0};
vector<double> kagu_R;
vector<double> kagu_G;
vector<double> kagu_B;

// yとzはtxtファイルと逆転していることに注意


std::vector<std::string> split2(std::string &input, char delimiter) {
  std::istringstream stream(input);
  std::string field;
  std::vector<std::string> result;
  while (getline(stream, field, delimiter)) result.push_back(field);
  return result;
}


// csvファイルの読み取り
void read() {
  std::string path = "./room.csv"; // 読み取り元
  std::ifstream ifs(path);                       // 読み取り用ストリーム
  if (ifs) {
    std::string line;

    // 一行目がラベルの場合
    // getline(ifs, line);
    // std::vector<std::string> strvec = split(line, ',');

    while (getline(ifs, line)) {
      std::vector<int> datvec;
      std::vector<std::string> strvec = split2(line, ',');
      for (auto &&s : strvec) datvec.push_back(std::stoi(s)); // セルの文字列を数値に変換
      room.push_back(datvec);
    }
  }
//   for(int j = 0; j < 27;j++){
//         for(int i = 0; i < 35; i++){
//             cout << data.at(j).at(i);
//         }
//         cout << endl;
//   }
}


void PlaySound(char *file){
    static char cmd[256];
    if (strlen(file) > 200) return;
    sprintf(cmd, "paplay %s &> /dev/null &", file);
    system(cmd);
}


int main(int argc, char *argv[]){
    // BGM
    PlaySound("BGM.wav");

    // 部屋の壁位置の２次配列をmadori.pyで作成したcsvから読み取り
    read();

    // 壁検出
    detect_wall(room);
    // for(int i = 0; i < 12; i++){
    //     cout << "{{" << wall_points.at(i).at(0).at(0) << wall_points.at(i).at(0).at(1) << "}{" << wall_points.at(i).at(1).at(0) << wall_points.at(i).at(1).at(1) << "}}" << endl;
    // }

    // txtファイル読み込み
    ifstream reading_file;
    string filename = argv[1];
    reading_file.open(filename, ios::in);
    string reading_line_buffer;
    char del = ',';

    while(getline(reading_file, reading_line_buffer)){
        kagu_name.push_back(split(reading_line_buffer, del).at(0));
        kagu_x.push_back(stoi(split(reading_line_buffer, del).at(1)));
        kagu_z.push_back(stoi(split(reading_line_buffer, del).at(2)));
        kagu_y.push_back(stoi(split(reading_line_buffer, del).at(3)));
        // place_x.push_back(stoi(split(reading_line_buffer, del).at(4)));
        // place_z.push_back(stoi(split(reading_line_buffer, del).at(5)));
        kagu_R.push_back(stod(split(reading_line_buffer, del).at(4)));
        kagu_G.push_back(stod(split(reading_line_buffer, del).at(5)));
        kagu_B.push_back(stod(split(reading_line_buffer, del).at(6)));
    }

    int kagu_n = kagu_name.size();
    // cout << kagu_n << endl;

	/* OpenGLの初期化 */
	init_GL(argc,argv);

	/* このプログラム特有の初期化 */
	init();

	/* コールバック関数の登録 */
	set_callback_functions();

	/* メインループ */
	glutMainLoop();

	return 0;
}


vector<string> split(string str, char del) {
    int first = 0;
    int last = str.find_first_of(del);
 
    vector<string> result;
 
    while (first < str.size()) {
        string subStr(str, first, last - first);
 
        result.push_back(subStr);
 
        first = last + 1;
        last = str.find_first_of(del, first);
 
        if (last == string::npos) {
            last = str.size();
        }
    }
    return result;
}


void init_GL(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_X,WINDOW_Y);
	glutCreateWindow(WINDOW_NAME);
}


void init(){
	glClearColor(0.4, 0.8, 1.0, 0.0);         // 背景の塗りつぶし色を指定

    glGenTextures(3, g_TextureHandles);
    for(int i = 0; i < 3; i++){
        glBindTexture(GL_TEXTURE_2D, g_TextureHandles[i]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    set_texture();
}


void set_callback_functions(){
	glutDisplayFunc(glut_display);
	glutKeyboardFunc(glut_keyboard);
	glutMouseFunc(glut_mouse);
	glutMotionFunc(glut_motion);
	glutPassiveMotionFunc(glut_motion);
    glutIdleFunc(glut_idle);
}


void glut_keyboard(unsigned char key, int x, int y){
	switch(key){
    // TODO: 家具の移動時に音が出るようにする（あつ森風）
	case 'q':
	case 'Q':
	case 0x1B:
        // cout << place_x[0] << "," << place_x[1] << "," << place_x[2] << endl;
        // cout << place_z[0] << "," << place_z[1] << "," << place_z[2] << endl;
        // cout << g_distance << endl;
        // cout << camera_x << "," << camera_z << endl;
        // cout << rotate_angle[0] << rotate_angle[1] << rotate_angle[2] << endl;
        // cout << stand_x << "," << stand_y << "," << g_angle1 << "," << g_angle2 << endl;
		exit(0);

    case 'r':
        if(kaguindex != -1){
            count_rotate = 0;
            PlaySound(ROTATE);
            glutTimerFunc(0, timer_r, 0);
        }
        break;
    case 'l':
        if(kaguindex != -1){
            count_rotate = 0;
            PlaySound(ROTATE);
            glutTimerFunc(0, timer_l, 0);
        }
        break;
    case 'i':
        if(kaguindex != -1){
            PlaySound(MOVE);
            place_z[kaguindex] -= 1;
        }
        else{
            PlaySound(WALK);
            stand_y -= 3;
        }
        break;
    case 'm':
        if(kaguindex != -1){
            place_z[kaguindex] += 1;
            PlaySound(MOVE);
        }
        else{
            PlaySound(WALK);
            stand_y += 3;
        }
        break;
    case 'j':
        if(kaguindex != -1){
            place_x[kaguindex] -= 1;
            PlaySound(MOVE);
        }
        else{
            PlaySound(WALK);
            stand_x -= 3;
        }
        break;
    case 'k':
        if(kaguindex != -1){
            place_x[kaguindex] += 1;
            PlaySound(MOVE);
        }
        else{
            PlaySound(WALK);
            stand_x += 3;
        }
        break;
    case 'v':
        vision_mode = !vision_mode;
        break;
    case '1':
        kaguindex = 0;
        cout << kagu_name[kaguindex] << " selected" << endl;
        break;
    case '2':
        kaguindex = 1;
        cout << kagu_name[kaguindex] << " selected" << endl;
        break;
    case '3':
        kaguindex = 2;
        cout << kagu_name[kaguindex] << " selected" << endl;
        break;
    case '0':
        //  人間が動く
        kaguindex = -1;
        cout << "no furniture selected" << endl;
        break;
    }
	glutPostRedisplay();
}


void glut_mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_UP){
			g_isLeftButtonOn = false;
		}else if(state == GLUT_DOWN){
			g_isLeftButtonOn = true;
		}
	}

	if(button == GLUT_RIGHT_BUTTON){
		if(state == GLUT_UP){
			g_isRightButtonOn = false;
		}else if(state == GLUT_DOWN){
			g_isRightButtonOn = true;
		}
	}
}


void glut_motion(int x, int y){
	static int px = -1, py = -1;
	if(g_isLeftButtonOn == true){
		if(px >= 0 && py >= 0){
			g_angle1 += (double)-(x - px)/300;
			g_angle2 += (double)(y - py)/300;
            camera_z -= (double)(y - py)/20;
            camera_x -= (double)(x - px)/20;
		}
		px = x;
		py = y;
	}else if(g_isRightButtonOn == true){
		if(px >= 0 && py >= 0){
			g_distance += (double)(y - py)/20;
		}
		px = x;
		py = y;
	}else{
		px = -1;
		py = -1;
	}
	glutPostRedisplay();
}


void glut_display(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1.0, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// if (cos(g_angle2)>0){
	// gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
	// 	g_distance * sin(g_angle2),
	// 	g_distance * cos(g_angle2) * cos(g_angle1),
	// 	0.0, 0.0, 0.0, 0.0, 1.0, 0.0);}
	// else{
	// gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
    //             g_distance * sin(g_angle2),
    //             g_distance * cos(g_angle2) * cos(g_angle1),
    //             0.0, 0.0, 0.0, 0.0, -1.0, 0.0);}
    
    if(vision_mode == 0){
        gluLookAt(camera_x, g_distance, camera_z, camera_x, 20, camera_z, 0.0, 0.0, -1.0);
    }

    if(vision_mode == 1){
        gluLookAt(stand_x, 17, stand_y, stand_x + g_distance1 * cos(g_angle2) * sin(g_angle1), 17 + g_distance1 * sin(g_angle2), stand_y + g_distance1 * cos(g_angle2) * cos(g_angle1), 0, 1, 0);
    }

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    //  壁描画
    glPushMatrix();
    draw_wall(wall_points);
    glPopMatrix();

    // 床描画
    glPushMatrix();
    draw_plane(0,0,0, 36,0,0, 0,0,27, 36,0,27, 0.6,0.3,0.3);
    glPopMatrix();
   
    glPushMatrix();
    glTranslatef(place_x[0], 0.0, place_z[0]);
    glRotatef(rotate_angle[0], 0.0, 1.0, 0.0);
    glTranslatef(-place_x[0], 0.0, -place_z[0]);
    draw_bed(place_x[0], place_z[0], kagu_x[0], kagu_z[0], kagu_R[0], kagu_G[0], kagu_B[0]);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(place_x[1], 0.0, place_z[1]);
    glRotatef(rotate_angle[1], 0, 1.0, 0.0);
    glTranslatef(-place_x[1], 0.0, -place_z[1]);
    draw_shelf(place_x[1], place_z[1], kagu_x[1], kagu_z[1], kagu_y[1], kagu_R[1], kagu_G[1], kagu_B[1]);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(place_x[2], 0.0, -place_z[2]);
    glRotatef(rotate_angle[2], 0, 1.0, 0.0);
    glTranslatef(-place_x[2], 0.0, -place_z[2]);
    draw_table(place_x[2], place_z[2], kagu_x[2], kagu_z[2], kagu_y[2], kagu_R[2], kagu_G[2], kagu_B[2]);
    glPopMatrix();

	glFlush();
	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}


void glut_idle(){
	static int counter = 0;

	if(counter == 0){
		// glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
        flag = 0;
	}else if(counter == 100){
		// glBindTexture(GL_TEXTURE_2D, g_TextureHandles[1]);
        flag = 1;
	}else if(counter == 200){
		// glBindTexture(GL_TEXTURE_2D, g_TextureHandles[2]);
        flag = 2;
	}

	counter++;
	if(counter > 300) counter = 0;

	glutPostRedisplay();
}


void draw_plane(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3,double x4,double y4,double z4,double R,double G,double B){
    GLdouble pointA[] = {x1, y1, z1};
	GLdouble pointB[] = {x2, y2, z2};
	GLdouble pointC[] = {x3, y3, z3};
	GLdouble pointD[] = {x4, y4, z4};

    glColor3d(R, G, B);
    glBegin(GL_POLYGON);
    glVertex3dv(pointA);
	glVertex3dv(pointB);
	glVertex3dv(pointD);
	glVertex3dv(pointC);
	glEnd();
}


void draw_plane2(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3,double x4,double y4,double z4,double R,double G,double B){
    GLdouble pointA[] = {x1, y1, z1};
	GLdouble pointB[] = {x2, y2, z2};
	GLdouble pointC[] = {x3, y3, z3};
	GLdouble pointD[] = {x4, y4, z4};

    glColor3d(R, G, B);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_TextureHandles[flag]);

    glBegin(GL_POLYGON);
    glTexCoord2d(1, 0.0);
    glVertex3dv(pointA);
    glTexCoord2d(0.0, 0.0);
	glVertex3dv(pointB);
    glTexCoord2d(0.0, 1);
	glVertex3dv(pointD);
    glTexCoord2d(1, 1);
	glVertex3dv(pointC);
	glEnd();

    glDisable(GL_TEXTURE_2D);
    // cout << "disabled" << endl;
}


void draw_rectangle(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3,double x4,double y4,double z4,double x5,double y5,double z5,double x6,double y6,double z6,double x7,double y7,double z7,double x8,double y8,double z8,double R,double G,double B){
	GLdouble pointA[] = {x1, y1, z1};
	GLdouble pointB[] = {x2, y2, z2};
	GLdouble pointC[] = {x3, y3, z3};
	GLdouble pointD[] = {x4, y4, z4};
    GLdouble pointE[] = {x5, y5, z5};
	GLdouble pointF[] = {x6, y6, z6};
	GLdouble pointG[] = {x7, y7, z7};
	GLdouble pointH[] = {x8, y8, z8};


    glColor3d(R, G, B);
	glBegin(GL_POLYGON);
	glVertex3dv(pointA);
	glVertex3dv(pointB);
	glVertex3dv(pointD);
	glVertex3dv(pointC);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3dv(pointA);
	glVertex3dv(pointB);
	glVertex3dv(pointF);
	glVertex3dv(pointE);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3dv(pointA);
	glVertex3dv(pointC);
	glVertex3dv(pointG);
	glVertex3dv(pointE);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3dv(pointB);
	glVertex3dv(pointD);
	glVertex3dv(pointH);
	glVertex3dv(pointF);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3dv(pointC);
	glVertex3dv(pointD);
	glVertex3dv(pointH);
	glVertex3dv(pointG);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3dv(pointF);
	glVertex3dv(pointH);
	glVertex3dv(pointG);
	glVertex3dv(pointE);
	glEnd();
}


void draw_bed(double x, double z, double x_len, double z_len, double R, double G, double B){
    draw_rectangle(x+x_len,2.5,z, x,2.5,z, x+x_len,2.5,z+z_len, x,2.5,z+z_len, x+x_len,1,z, x,1,z, x+x_len,1,z+z_len, x,1,z+z_len, R,G,B);
    draw_rectangle(x+x_len,5,z+1, x,5,z+1, x+x_len,5,z+z_len, x,5,z+z_len, x+x_len,2.5,z+1, x,2.5,z+1, x+x_len,2.5,z+z_len, x,2.5,z+z_len, 1.0,1.0,1.0);
    draw_rectangle(x+x_len,8,z, x,8,z, x+x_len,8,z+1, x,8,z+1, x+x_len,1,z, x,1,z, x+x_len,1,z+1, x,1,z+1, R,G,B);
    draw_rectangle(x+7,9,z, x+3,9,z, x+7,9,z+1, x+3,9,z+1, x+8,8,z, x+2,8,z, x+8,8,z+1, x+2,8,z+1, R,G,B);
    draw_rectangle(x+x_len,7,z+z_len, x,7,z+z_len, x+x_len,7,z+z_len+1, x,7,z+z_len+1, x+x_len,1,z+z_len, x,1,z+z_len, x+x_len,1,z+z_len+1, x,1,z+z_len+1, R,G,B);
    draw_rectangle(x+7,8,z+z_len, x+3,8,z+z_len, x+7,8,z+z_len+1, x+3,8,z+z_len+1, x+8,7,z+z_len, x+2,7,z+z_len, x+8,7,z+z_len+1, x+2,7,z+z_len+1, R,G,B);
    draw_rectangle(x+x_len,1,z, x+x_len-1,1,z, x+x_len,1,z+1, x+x_len-1,1,z+1, x+x_len,0,z, x+x_len-1,0,z, x+x_len,0,z+1, x+x_len-1,0,z+1, R,G,B);
    draw_rectangle(x+x_len,1,z+z_len, x+x_len-1,1,z+z_len, x+x_len,1,z+z_len+1, x+x_len-1,1,z+z_len+1, x+x_len,0,z+z_len, x+x_len-1,0,z+z_len, x+x_len,0,z+z_len+1, x+x_len-1,0,z+z_len+1, R,G,B);
    draw_rectangle(x+1,1,z, x,1,z, x+1,1,z+1, x,1,z+1, x+1,0,z, x,0,z, x+1,0,z+1, x,0,z+1, R,G,B);
    draw_rectangle(x+1,1,z+z_len, x,1,z+z_len, x+1,1,z+z_len+1, x,1,z+z_len+1, x+1,0,z+z_len, x,0,z+z_len, x+1,0,z+z_len+1, x,0,z+z_len+1, R,G,B);
    draw_rectangle(x+8,6,z+1.5, x+2,6,z+1.5, x+8,6,z+5, x+2,6,z+5, x+8,5,z+1.5, x+2,5,z+1.5, x+8,5,z+5, x+2,5,z+5, 0.6,1.0,1.0);

    // glPushMatrix();
    // glColor3d(1.0,0.2,0.3);
    // glTranslatef(35,5.5,21);
    // glScalef(1.0,0.5,1.0);
    // glutSolidSphere(2,16,16);
    // glPopMatrix();

    // glPushMatrix();
    // glColor3d(1.0,0.2,0.3);
    // glTranslatef(37,6,23);
    // // glRotatef(-60,1,0,1);
    // glScalef(1.0,0.5,1.0);
    // glutSolidSphere(2,16,16);
    // glPopMatrix();
}


void draw_shelf(double x, double z, double x_len, double z_len, double y_len, double R, double G, double B){
    draw_rectangle(x+x_len,1.5,z, x,1.5,z, x+x_len,1.5,z+z_len, x,1.5,z+z_len, x+x_len,1,z, x,1,z, x+x_len,1,z+z_len, x,1,z+z_len, R,G,B);
    draw_rectangle(x+x_len,6.5,z, x,6.5,z, x+x_len,6.5,z+z_len, x,6.5,z+z_len, x+x_len,6,z, x,6,z, x+x_len,6,z+z_len, x,6,z+z_len, R,G,B);
    draw_rectangle(x+x_len,11.5,z, x,11.5,z, x+x_len,11.5,z+z_len, x,11.5,z+z_len, x+x_len,11,z, x,11,z, x+x_len,11,z+z_len, x,11,z+z_len, R,G,B);
    draw_rectangle(x+x_len,16.5,z, x,16.5,z, x+x_len,16.5,z+z_len, x,16.5,z+z_len, x+x_len,16,z, x,16,z, x+x_len,16,z+z_len, x,16,z+z_len, R,G,B);
    draw_rectangle(x+x_len,19,z, x+x_len,19,z+z_len, x+x_len,0,z, x+x_len,0,z+z_len, x+x_len-1,19,z, x+x_len-1,19,z+z_len, x+x_len-1,0,z, x+x_len-1,0,z+z_len, R,G,B);
    draw_rectangle(x,19,z, x,19,z+z_len, x,0,z, x,0,z+z_len, x+1,19,z, x+1,19,z+z_len, x+1,0,z, x+1,0,z+z_len, R,G,B);

    // 本
    for(int i = 1; i < 5; i++){
        draw_rectangle(x+i,14,z, x+i,14,z+1, x+i,11.5,z, x+i,11.5,z+1, x+i+0.5,14,z, x+i+0.5,14,z+1, x+i+0.5,11.5,z, x+i+0.5,11.5,z+1, 0.0,0.0,0.0);
        draw_rectangle(x+i+0.5,14,z, x+i+0.5,14,z+1, x+i+0.5,11.5,z, x+i+0.5,11.5,z+1, x+i+0.5+0.5,14,z, x+i+0.5+0.5,14,z+1, x+i+0.5+0.5,11.5,z, x+i+0.5+0.5,11.5,z+1, 0.0,0.0,1.0);
        i += 3;
        draw_rectangle(x+i,9,z, x+i,9,z+1, x+i,6.5,z, x+i,6.5,z+1, x+i+0.5,9,z, x+i+0.5,9,z+1, x+i+0.5,6.5,z, x+i+0.5,6.5,z+1, 1.0,0.0,0.0);
        draw_rectangle(x+i+0.5,9,z, x+i+0.5,9,z+1, x+i+0.5,6.5,z, x+i+0.5,6.5,z+1, x+i+0.5+0.5,9,z, x+i+0.5+0.5,9,z+1, x+i+0.5+0.5,6.5,z, x+i+0.5+0.5,6.5,z+1, 1.0,0.0,1.0);
        i -= 3;
    }
}


void draw_table(double x, double z, double x_len, double y_len, double z_len, double R, double G, double B){
    draw_rectangle(x+x_len,y_len+1,z, x,y_len+1,z, x+x_len,y_len+1,z+z_len, x,y_len+1,z+z_len, x+x_len,y_len,z, x,y_len,z, x+x_len,y_len,z+z_len, x,y_len,z+z_len, R,G,B);
    draw_rectangle(x+x_len,y_len+1,z, x+x_len-0.7,y_len+1,z, x+x_len,y_len+1,z+0.7, x+x_len-0.7,y_len+1,z+0.7, x+x_len,0,z, x+x_len-0.7,0,z, x+x_len,0,z+0.7, x+x_len-0.7,0,z+0.7, R,G,B);
    draw_rectangle(x+0.7,y_len+1,z, x,y_len+1,z, x+0.7,y_len+1,z+0.7, x,y_len+1,z+0.7, x+0.7,0,z, x,0,z, x+0.7,0,z+0.7, x,0,z+0.7, R,G,B);
    draw_rectangle(x+x_len,y_len+1,z+z_len-0.7, x+x_len-0.7,y_len+1,z+z_len-0.7, x+x_len,y_len+1,z+z_len, x+x_len-0.7,y_len+1,z+z_len, x+x_len,0,z+z_len-0.7, x+x_len-0.7,0,z+z_len-0.7, x+x_len,0,z+z_len, x+x_len-0.7,0,z+z_len, R,G,B);
    draw_rectangle(x+0.7,y_len+1,z+z_len-0.7, x,y_len+1,z+z_len-0.7, x+0.7,y_len+1,z+z_len, x,y_len+1,z+z_len, x+0.7,0,z+z_len-0.7, x,0,z+z_len-0.7, x+0.7,0,z+z_len, x,0,z+z_len, R,G,B);

    draw_plane2(x+13,y_len+5,z+3, x+7,y_len+5,z, x+13,y_len+1,z+3, x+7,y_len+1,z+0, 1.0,1.0,1.0);
    draw_plane(x+9,y_len+1.1,z+1, x+11,y_len+1.1,z+2, x+8,y_len+1.1,z+3, x+10,y_len+1.1,z+4, 0,0,0);
}


void detect_wall(vector<vector<int>> room){
    for(int j = 0; j < room.size(); j++){
        for(int i = 0; i < room.at(0).size(); i++){
            if(room.at(j).at(i) == 1){
                int plus = 1;
                while(i+plus < room.at(0).size() && room.at(j).at(i+plus) == 1){
                    room.at(j).at(i+plus-1) = 0;
                    plus++;
                }
                wall_points.push_back(vector<vector<int>>{{j,i}, {j,i+plus-1}});

                plus = 1;
                while(j+plus < room.size() && room.at(j+plus).at(i) == 1){
                    room.at(j+plus-1).at(i) = 0;
                    plus++;
                }
                wall_points.push_back(vector<vector<int>>{{j,i}, {j+plus-1,i}});
            }
        }
    }
}


void draw_wall(vector<vector<vector<int>>> wall_points){
    int n = wall_points.size();
    for(int i = 0; i < n; i++){
        draw_plane(wall_points.at(i).at(0).at(0),25,wall_points.at(i).at(0).at(1), wall_points.at(i).at(1).at(0),25,wall_points.at(i).at(1).at(1), wall_points.at(i).at(0).at(0),0,wall_points.at(i).at(0).at(1), wall_points.at(i).at(1).at(0),0,wall_points.at(i).at(1).at(1), 0.9,0.9,0.9);
    }
}


void timer_r(int value){
    rotate_angle[kaguindex] -= 10;
    
    count_rotate++;
    
    glutPostRedisplay();

    if(count_rotate != 9){
        glutTimerFunc(30, timer_r, 0);
    }
}


void timer_l(int value){
    rotate_angle[kaguindex] += 10;
    
    count_rotate++;
    
    glutPostRedisplay();

    if(count_rotate != 9){
        glutTimerFunc(30, timer_l, 0);
    }
}


void set_texture(){
    const char* inputFileNames[3] = {"tanuki2.jpeg","tanuki.jpeg","tanuki1.jpeg"};
    for(int i = 0; i < 3; i++){
        cv::Mat input = cv::imread(inputFileNames[i], 1);
        // BGR -> RGBの変換
        cv::cvtColor(input, input, cv::COLOR_BGR2RGB);

        glBindTexture(GL_TEXTURE_2D, g_TextureHandles[i]);
        glTexSubImage2D(GL_TEXTURE_2D, 0,
            (TEXTURE_WIDTH - input.cols)/2,
            (TEXTURE_HEIGHT - input.rows)/2,
            input.cols, input.rows,
            GL_RGB, GL_UNSIGNED_BYTE, input.data);
    }
}


// 床をroom配列に対応させて描画するようにすると部屋からはみ出ることはない