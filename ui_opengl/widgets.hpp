// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_WIDGETS_H
#define UI_OPENGL_WIDGETS_H

typedef void (*ButtonCallback)();

// TODO: fg_color, bg_color
struct Button {
  int id;
  V2i pos;
  V2i size;
  GLuint textureID;
  char* text;
  TTF_Font* f;
  ButtonCallback callback;
};

void initWidgets();
TTF_Font* openFont(const char* fontName, int size);
Button* v2iToButton(V2i pos);
int addButton(TTF_Font* f, const V2i* pos,
    const char* text, ButtonCallback callback);
void drawButtons();
void changeButton_text(Button* b, char* text);
void changeButtonTextByID(int id, char* text);

#endif
