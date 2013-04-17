#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "num2words.h"

#define MY_UUID { 0xD4, 0xED, 0x05, 0xFF, 0x75, 0x8B, 0x46, 0x6F, 0x80, 0xBF, 0x55, 0xFA, 0xD9, 0xCB, 0xAA, 0x3C }
PBL_APP_INFO(MY_UUID,
             "Spanish Fuzzy",
             "jmdelaroca@gmail.com",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define BUFFER_SIZE 86

static struct CommonWordsData {
  TextLayer lblDate;
  TextLayer label;
  Window window;
  char buffer[BUFFER_SIZE];
} s_data;


//Layer line_layer;


static char date_text[] = "dd/mm/YYYY";

/*
// dibuja linea bajo texto de fecha
void line_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_draw_line(ctx, GPoint(0, 20), GPoint(144, 20));
 
}
*/

static void update_time(PblTm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, BUFFER_SIZE);
  text_layer_set_text(&s_data.label, s_data.buffer);

  // colocar fecha
  //string_format_time(date_text, sizeof(date_text), "%B %e", t);
  string_format_time(date_text, sizeof(date_text), "%d/%m/%Y", t);
  text_layer_set_text(&s_data.lblDate, date_text);
}

static void handle_minute_tick(AppContextRef app_ctx, PebbleTickEvent* e) {
  update_time(e->tick_time);
}

static void handle_init(AppContextRef ctx) {
  (void) ctx;

  window_init(&s_data.window, "Spanish Fuzzy");
  const bool animated = true;
  window_stack_push(&s_data.window, animated);

  window_set_background_color(&s_data.window, GColorBlack);
  GFont gotham = fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD);
  GFont gothic = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  // label para Fecha
  text_layer_init(&s_data.lblDate, GRect(0, 0, s_data.window.layer.frame.size.w, 20));
  text_layer_set_background_color(&s_data.lblDate, GColorBlack);
  text_layer_set_text_color(&s_data.lblDate, GColorWhite);
  text_layer_set_font(&s_data.lblDate, gothic);
  text_layer_set_text_alignment(&s_data.lblDate, GTextAlignmentCenter);
  layer_add_child(&s_data.window.layer, &s_data.lblDate.layer);

  // label para Fuzzy Text
  text_layer_init(&s_data.label, GRect(0, 20, s_data.window.layer.frame.size.w, s_data.window.layer.frame.size.h - 20));
  text_layer_set_background_color(&s_data.label, GColorBlack);
  text_layer_set_text_color(&s_data.label, GColorWhite);
  text_layer_set_font(&s_data.label, gotham);
  layer_add_child(&s_data.window.layer, &s_data.label.layer);

  /*
  // linea debajo de texto de fecha
  layer_init(&line_layer, s_data.window.layer.frame);
  line_layer.update_proc = &line_layer_update_callback;
  layer_add_child(&s_data.window.layer, &line_layer);
  */

  PblTm t;
  get_time(&t);
  update_time(&t);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
