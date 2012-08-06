/* See LICENSE file for copyright and license details. */

typedef void (*ButtonCallback)(void);

/* TODO: fg_color, bg_color */
typedef struct {
  int id;
  V2i pos;
  V2i size;
  GLuint texture_id;
  char *text;
  TTF_Font *f;
  ButtonCallback callback;
} Button;

void init_widgets(void);
TTF_Font* open_font(char *font_name, int size);
Button* v2i_to_button(V2i pos);
int add_button(TTF_Font *f, const V2i *pos,
    char *text, ButtonCallback callback);
void draw_buttons(void);
void change_button_text(Button *b, char *text);
void change_button_text_by_id(int id, char *text);
