#if defined(__APPLE__)
#  define GL_SILENCE_DEPRECATION
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  if defined(_WIN32)
//#    pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#    define _USE_MATH_DEFINES
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#  include <GL/glut.h>
#  include <GL/glext.h>
#  if defined(_WIN32)
PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd;
#  endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
** 光源
*/
static const GLfloat lightpos[] = { 0.0f, 0.0f, 1.0f, 0.0f }; /* 位置　　　 */
static const GLfloat lightcol[] = { 1.0f, 1.0f, 1.0f, 1.0f }; /* 直接光強度 */
static const GLfloat lightamb[] = { 0.1f, 0.1f, 0.1f, 1.0f }; /* 環境光強度 */

/*
** テクスチャ
*/
#define TEXWIDTH  128                               /* テクスチャの幅　　　 */
#define TEXHEIGHT 128                               /* テクスチャの高さ　　 */
static const int target[] = {                   /* テクスチャのターゲット名 */
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
};

/*
** マテリアル
*/
static const GLfloat kdiff[] = { 0.0f, 0.1f, 0.3f, 1.0f };  /* 拡散反射係数 */
static const GLfloat kspec[] = { 0.6f, 0.6f, 0.6f, 1.0f };  /* 鏡面反射係数 */
static const GLfloat knone[] = { 0.0f, 0.0f, 0.0f, 1.0f };  /* 鏡面反射無効 */
static const GLfloat kshi = 20.0;                           /* 輝き係数　　 */

/*
** 鏡面反射光強度
*/
static void specular(float fx, float fy, float fz, const float *l, GLubyte *t)
{
  /* 光線ベクトルと反射ベクトルの内積を求める */
  float lf = l[0] * fx + l[1] * fy + l[2] * fz;

  if (lf > 0.0) {
    /* 鏡面反射率×255を求める */
    float rs = powf(lf, kshi) * 255.0f;

    /* 鏡面反射光強度を求める */
    t[0] = (GLubyte)(kspec[0] * rs * lightcol[0]);
    t[1] = (GLubyte)(kspec[1] * rs * lightcol[1]);
    t[2] = (GLubyte)(kspec[2] * rs * lightcol[2]);
  }
  else {
    t[0] = t[1] = t[2] = 0;
  }
}

/*
** テクスチャの作成
*/
static void makeTexture(GLubyte *tex[], int width, int height)
{
#if 0
  /* 光線ベクトルと視線ベクトルの中間ベクトル h を求める */
  float l2 = lightpos[0] * lightpos[0]
           + lightpos[1] * lightpos[1] 
           + lightpos[2] * lightpos[2];
  float l = sqrt(l2);
  float h[] = { lightpos[0], lightpos[1], lightpos[2] + l };
  int i = 0;

  l2 += lightpos[2] * l;
  if (l2 > 0.0) {
    double m = sqrt(l2 + l2);
    h[0] /= m;
    h[1] /= m;
    h[2] /= m;
  }

  /* 中間ベクトルと法線ベクトルの内積値でテクスチャを作る */
  for (int v = 0; v < height; ++v) {
    float y = (float)(v + v - height) / (float)height;
    float y2 = y * y;

    for (int tx = 0; tx < width; ++tx) {
      float x = (float)(tx + tx - width) / (float)width;
      float x2 = x * x;

      /* 法線ベクトル */
      float r = 1.0 / sqrt(x2 + y2 + 1.0);
      float s = x * r;
      float t = y * r;

      /* ６面のテクスチャについてそれぞれ鏡面反射強度を求める */
      specular(-r,  t,  s, h, tex[0] + i);  /* negative x */
      specular( s, -r,  t, h, tex[1] + i);  /* negative y */
      specular(-s,  t, -r, h, tex[2] + i);  /* negative z */
      specular( r,  t, -s, h, tex[3] + i);  /* positive x */
      specular( s,  r, -t, h, tex[4] + i);  /* positive y */
      specular( s,  t,  r, h, tex[5] + i);  /* positive z */

      i += 3;
    }
  }
#else
  int i = 0;

  /* 反射ベクトルと光線ベクトルとの内積値でテクスチャを作る */
  for (int v = 0; v < height; ++v) {
    float y = (float)(v + v - height) / (float)height;
    float y2 = y * y;

    for (int u = 0; u < width; ++u) {
      float x = (float)(u + u - width) / (float)width;
      float x2 = x * x;

      /* 反射ベクトル */
      float r = 1.0f / sqrtf(x2 + y2 + 1.0f);
      float s = x * r;
      float t = y * r;

      /* ６面のテクスチャについてそれぞれ鏡面反射強度を求める */
      specular(-r,  t,  s, lightpos, tex[0] + i);  /* negative x */
      specular( s, -r,  t, lightpos, tex[1] + i);  /* negative y */
      specular(-s,  t, -r, lightpos, tex[2] + i);  /* negative z */
      specular( r,  t, -s, lightpos, tex[3] + i);  /* positive x */
      specular( s,  r, -t, lightpos, tex[4] + i);  /* positive y */
      specular( s,  t,  r, lightpos, tex[5] + i);  /* positive z */

      i += 3;
    }
  }
#endif

#if 0
  FILE *fp;
  static char tname[] = "aX.raw";
  for (i = 0; i < 6; ++i) {
    tname[1] = i + '0';
    if (fp = fopen(tname, "wb")) {
      fwrite(t[i], sizeof(unsigned char) * 3, width * height, fp);
      fclose(fp);
    }
  }
#endif
}

/*
** 初期化
*/
static void init(void)
{
  /* テクスチャ画像はバイト単位に詰め込まれている */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  /* テクスチャの読み込みに使う配列 */
  static GLubyte t[6][TEXHEIGHT * TEXWIDTH * 3];
  static GLubyte* textures[]{ t[0], t[1], t[2], t[3], t[4], t[5] };

  /* テクスチャの作成 */
  makeTexture(textures, TEXWIDTH, TEXHEIGHT);

  /* テクスチャの割り当て */
  for (int i = 0; i < 6; ++i) {
    glTexImage2D(target[i], 0, GL_RGB, TEXWIDTH, TEXHEIGHT, 0,
      GL_RGB, GL_UNSIGNED_BYTE, textures[i]);
  }

  /* テクスチャを拡大・縮小する方法の指定 */
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  /* テクスチャの繰り返し方法の指定 */
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  /* テクスチャ環境 */
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

#if 0
  /* 混合する色の設定 */
  static const GLfloat blend[] = { 0.0f, 1.0f, 0.0f, 1.0f };
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, blend);
#endif

  /* キューブマッピング用のテクスチャ座標を生成する */
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

  /* 初期設定 */
  glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  /* 光源の初期設定 */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);

#if defined(WIN32)
  glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC)wglGetProcAddress("glMultTransposeMatrixd");
#endif
}

/* 箱を描く関数の宣言 */
#include "box.h"

/*
** シーンの描画
*/
static void scene(void)
{
  /* 材質の設定 */
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, kdiff);
  glMaterialfv(GL_FRONT, GL_SPECULAR, knone);

  /* テクスチャマッピング開始 */
  glEnable(GL_TEXTURE_CUBE_MAP);

  /* テクスチャ座標の自動生成を有効にする */
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);

  /* ティーポットを描く */
  glutSolidTeapot(1.0);

  /* テクスチャ座標の自動生成を無効にする */
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);

  /* テクスチャマッピング終了 */
  glDisable(GL_TEXTURE_CUBE_MAP);
}

/****************************
** GLUT のコールバック関数 **
****************************/

/* トラックボール処理用関数の宣言 */
#include "trackball.h"

static void display(void)
{
  /* モデルビュー変換行列の設定 */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* 光源の位置を設定 */
  glPushMatrix();
  glMultMatrixd(trackballRotation());
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  glPopMatrix();

  /* 視点の移動（物体の方を奥に移動）*/
  glTranslated(0.0, 0.0, -3.0);
  //gluLookAt(1.5, 2.0, 2.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  /* トラックボール処理で図形を回転 */
  //glMultMatrixd(trackballRotation());

  /* テクスチャ行列の設定 */
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();

  /* トラックボール処理でテクスチャを回転 */
  glMultTransposeMatrixd(trackballRotation());

  /* 画面クリア */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* シーンの描画 */
  scene();

  /* ダブルバッファリング */
  glutSwapBuffers();
}

static void resize(int w, int h)
{
  /* トラックボールする範囲 */
  trackballRegion(w, h);

  /* ウィンドウ全体をビューポートにする */
  glViewport(0, 0, w, h);

  /* 透視変換行列の指定 */
  glMatrixMode(GL_PROJECTION);

  /* 透視変換行列の初期化 */
  glLoadIdentity();
  gluPerspective(60.0, (double)w / (double)h, 0.1, 10.0);
}

static void idle(void)
{
  /* 画面の描き替え */
  glutPostRedisplay();
}

static void mouse(int button, int state, int x, int y)
{
  switch (button) {
  case GLUT_LEFT_BUTTON:
    switch (state) {
    case GLUT_DOWN:
      /* トラックボール開始 */
      trackballStart(x, y);
      glutIdleFunc(idle);
      break;
    case GLUT_UP:
      /* トラックボール停止 */
      trackballStop(x, y);
      glutIdleFunc(0);
      break;
    default:
      break;
    }
    break;
    default:
      break;
  }
}

static void motion(int x, int y)
{
  /* トラックボール移動 */
  trackballMotion(x, y);
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
    /* ESC か q か Q をタイプしたら終了 */
    exit(0);
  default:
    break;
  }
}

/*
** メインプログラム
*/
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  init();
  glutMainLoop();
  return 0;
}
