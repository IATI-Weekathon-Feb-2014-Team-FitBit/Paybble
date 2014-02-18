
#include <pebble.h>

enum {
  KEY_CONFIRM,
  KEY_PLACE,
  KEY_AMOUNT,
  KEY_TRX
};

static Window *window;

static TextLayer *title_layer;
static TextLayer *place_layer;
static TextLayer *amount_layer;
    
static BitmapLayer *sidebar_layer;
static GBitmap *sidebar_image;

static char current_trx[32+1];

void in_received_handler(DictionaryIterator *received, void *context)
{
  Tuple *place_tuple = dict_find(received, KEY_PLACE);
  Tuple *amount_tuple = dict_find(received, KEY_AMOUNT);
  Tuple *trx_tuple = dict_find(received, KEY_TRX);

  if (place_tuple && amount_tuple && trx_tuple) {
    text_layer_set_text(place_layer, place_tuple->value->cstring);
    text_layer_set_text(amount_layer, amount_tuple->value->cstring);
    strncpy(current_trx, trx_tuple->value->cstring, 32);
  } else {
    text_layer_set_text(place_layer, ":-(");
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}

static void send_response(bool confirm)
{
  DictionaryIterator *iter;

  app_message_outbox_begin(&iter);
  if (iter != NULL) {
    const char *trx = current_trx;
    Tuplet trx_tupple = TupletCString(KEY_TRX, trx);
    Tuplet confirm_tuple = TupletInteger(KEY_CONFIRM, confirm);

    dict_write_tuplet(iter, &trx_tupple);
    dict_write_tuplet(iter, &confirm_tuple);
    dict_write_end(iter);

    app_message_outbox_send();
  }

  static const bool animated = true;
  window_stack_pop(animated);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
  send_response(false);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
  send_response(true);
}

static void click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
}

static void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  GRect text_rect = GRect(0, 0, bounds.size.w - 16, bounds.size.h / 3);

  title_layer = text_layer_create(text_rect);
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_text(title_layer, "Paybble");
  layer_add_child(window_layer, text_layer_get_layer(title_layer));

  text_rect.origin.y += bounds.size.h / 3;
  place_layer = text_layer_create(text_rect);
  text_layer_set_font(place_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(place_layer, GTextAlignmentLeft);
  text_layer_set_text(place_layer, "Star Bucks");
  layer_add_child(window_layer, text_layer_get_layer(place_layer));

  text_rect.origin.y += bounds.size.h / 3;
  amount_layer = text_layer_create(text_rect);
  text_layer_set_font(amount_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(amount_layer, GTextAlignmentRight);
  text_layer_set_text(amount_layer, "5.99 $");
  layer_add_child(window_layer, text_layer_get_layer(amount_layer));

  sidebar_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SIDE_BAR);
  sidebar_layer = bitmap_layer_create(GRect(bounds.size.w - 16, 0, 16, bounds.size.h));
  bitmap_layer_set_bitmap(sidebar_layer, sidebar_image);
  bitmap_layer_set_alignment(sidebar_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(sidebar_layer));

  vibes_short_pulse();
}

static void window_unload(Window *window)
{
  text_layer_destroy(title_layer);
  text_layer_destroy(place_layer);
  text_layer_destroy(amount_layer);
  
  bitmap_layer_destroy(sidebar_layer);
  gbitmap_destroy(sidebar_image);
}

static void init(void)
{
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  app_message_open(256, 64);

  const bool animated = true;
  window_stack_push(window, animated);
}

static void fini(void)
{
  window_destroy(window);
}

int main(void)
{
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  fini();
}

