#include <pebble.h>


static Window *window;
static GFont s_res_gothic_14;
static GFont s_res_gothic_18_bold;

static ActionBarLayer *actionbarlayer;

static TextLayer *text_layer_modifier;
static TextLayer *text_layer_select;
static TextLayer *text_layer_function;
static TextLayer *text_layer;
static TextLayer *text_layer_updated;
static TextLayer *text_layer_info;


static GBitmap* icon_top;
static GBitmap* icon_bot;
static GBitmap* icon_mid;


static char buffer[40];

static int connections=0;
static char functions[5][20];

static bool local_robot_move = false;
static bool local_mouse = false;
static bool local_keyboare = false;
static int  local_function =false;
enum Sync_Data {
  x = 0x0,         // TUPLE_INT
  y = 0x1,       // TUPLE_INT
  z = 0x2,       // TUPLE_INT
  robot_move = 0x4, // TUPLE_BOOL
  mouse = 0x5, // TUPLE_BOOL
  keyboard = 0x6, // TUPLE_BOOL
  select = 0x7, // TUPLE_BOOL
  mod = 0x7, // TUPLE_BOOL
  function = 0x8 // TUPLE_INT
};

//Accelerometer

void accel_data_handler(AccelData *data, uint32_t num_samples) {
  // Process 10 events - every 1 second
    int16_t x_co = 0, y_co=0, z_co=0;
    uint32_t a=0;
    int16_t num= 0;
    for (a =0; a<num_samples; a++){
        if (data[a].did_vibrate==false){
            num++;
            x_co+=data[a].x;
            y_co+=data[a].y;
            z_co+=data[a].z;
        }
    }

    x_co=(int16_t) x_co/num;
    y_co=(int16_t) y_co/num;
    z_co=(int16_t) z_co/num;
    if(num > 0){
        Tuplet values[] = {
            TupletInteger(x, x_co),
            TupletInteger(y, y_co),
            TupletInteger(z, z_co)
        };

        snprintf(buffer, sizeof(buffer), "x: %i\ny: %i\nz:%i", x_co,y_co,z_co);

        // Byte array + key:
        static const uint32_t SOME_DATA_KEY = 0xb00bf00b;
        static const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        // CString + key:
        static const uint32_t SOME_STRING_KEY = 0xabbababe;
        static const char *string = "Hello World";
        // Calculate the buffer size that is needed for the final Dictionary:
        const uint8_t key_count = 2;
        const uint32_t size = dict_calc_buffer_size(key_count, sizeof(data),
                                                    strlen(string) + 1);
        // Stack-allocated buffer in which to create the Dictionary:
        uint8_t buffer[size];
        // Iterator variable, keeps the state of the creation serialization process:
        DictionaryIterator iter;
        // Begin:
        dict_write_begin(&iter, buffer, sizeof(buffer));
        // Write the Data:
        dict_write_data(&iter, SOME_DATA_KEY, data, sizeof(data));
        // Write the CString:
        dict_write_cstring(&iter, SOME_STRING_KEY, string);
        // End:
        const uint32_t final_size = dict_write_end(&iter);
        // buffer now contains the serialized information



        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        int key=1, value=5;
        dict_write_int(iter, key, &value, sizeof(int), true);
        app_message_outbox_send();



        text_layer_set_text(text_layer, buffer);
    }
}

//Clicks

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    text_layer_set_text(text_layer, "Select");

}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

//Appmessage

void out_sent_handler(DictionaryIterator *sent, void *context) {
    text_layer_set_text(text_layer_updated, "Updated!");
    APP_LOG(APP_LOG_LEVEL_DEBUG, "out_sent_handler");

 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
     text_layer_set_text(text_layer_updated, "Failed :(");
     APP_LOG(APP_LOG_LEVEL_DEBUG, "out_failed_handler");

 }


 void in_received_handler(DictionaryIterator *received, void *context) {
   // incoming message received
   APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler");

 }



 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
   APP_LOG(APP_LOG_LEVEL_DEBUG, "in_dropped_handlers");

 }

//UI
static void initialise_ui(void) {
  window_set_fullscreen(window, false);

  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  icon_top = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ARROW_UP);
  icon_mid = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SQUARE);
  icon_bot = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ARROW_DOWN);
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  // text_layer_modifier
  text_layer_modifier = text_layer_create(GRect(62, 22, 60, 15));
  text_layer_set_background_color(text_layer_modifier, GColorClear);
  text_layer_set_text(text_layer_modifier, "Text layer");
  text_layer_set_text_alignment(text_layer_modifier, GTextAlignmentRight);
  text_layer_set_font(text_layer_modifier, s_res_gothic_14);
  layer_add_child(window_get_root_layer(window), (Layer *)text_layer_modifier);

  // actionbarlayer
  actionbarlayer = action_bar_layer_create();
  action_bar_layer_add_to_window(actionbarlayer, window);
  action_bar_layer_set_background_color(actionbarlayer, GColorBlack);
  action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_UP, icon_top);
  action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_SELECT, icon_mid);
  action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_DOWN, icon_bot);
  layer_add_child(window_get_root_layer(window), (Layer *)actionbarlayer);

  // text_layer_select
  text_layer_select = text_layer_create(GRect(57, 68, 64, 15));
  text_layer_set_background_color(text_layer_select, GColorClear);
  text_layer_set_text(text_layer_select, "Text layer");
  text_layer_set_text_alignment(text_layer_select, GTextAlignmentRight);
  text_layer_set_font(text_layer_select, s_res_gothic_14);
  layer_add_child(window_get_root_layer(window), (Layer *)text_layer_select);

  // text_layer_function
  text_layer_function = text_layer_create(GRect(5, 3, 79, 20));
  text_layer_set_text(text_layer_function, "Text layer");
  text_layer_set_font(text_layer_function, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(window), (Layer *)text_layer_function);

  // text_layer
  text_layer = text_layer_create(GRect(5, 39, 59, 81));
  text_layer_set_text(text_layer, "Text layer");
  text_layer_set_font(text_layer, s_res_gothic_14);
  layer_add_child(window_get_root_layer(window), (Layer *)text_layer);

  // text_layer_updated
  text_layer_updated = text_layer_create(GRect(8, 131, 100, 20));
  text_layer_set_text(text_layer_updated, "Text layer");
  text_layer_set_text_alignment(text_layer_updated, GTextAlignmentCenter);
  text_layer_set_font(text_layer_updated, s_res_gothic_14);
  layer_add_child(window_get_root_layer(window), (Layer *)text_layer_updated);

  // text_layer_info
  text_layer_info = text_layer_create(GRect(12, 88, 100, 39));
  text_layer_set_text(text_layer_info, "Text layer");
  text_layer_set_text_alignment(text_layer_info, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), (Layer *)text_layer_info);
}

static void destroy_ui(void) {
  text_layer_destroy(text_layer_modifier);
  action_bar_layer_destroy(actionbarlayer);
  text_layer_destroy(text_layer_select);
  text_layer_destroy(text_layer_function);
  text_layer_destroy(text_layer);
  text_layer_destroy(text_layer_updated);
  text_layer_destroy(text_layer_info);

  gbitmap_destroy(icon_top);
  gbitmap_destroy(icon_mid);
  gbitmap_destroy(icon_bot);
}

//Loading
static void window_unload(Window *window) {
    destroy_ui();
}

static void window_load(Window *window) {
    initialise_ui();

    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .unload = window_unload,
    });
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);


    Tuplet initial_values[] = {
        TupletInteger(x      , (uint8_t) 0),
        TupletInteger(y      , (uint8_t) 0),
        TupletInteger(z      , (uint8_t) 0),
        TupletInteger(robot_move, (bool) false),
        TupletInteger(mouse     , (bool) false),
        TupletInteger(keyboard  , (bool) false),
        TupletInteger(select    , (bool) false),
        TupletInteger(mod       , (bool) false),
        TupletInteger(function  , (uint8_t) 0)

    };

    text_layer_set_text(text_layer, "Move!");
    text_layer_set_text(text_layer_updated, "Not connected...");
}


static void init(void) {

    //UI
    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(window, animated);

    //AppMessage
    app_message_register_inbox_received(in_received_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_sent(out_sent_handler);
    app_message_register_outbox_failed(out_failed_handler);

    const int inbound_size = 64;
    const int outbound_size = 30;
    app_message_open(inbound_size, outbound_size);

    //Accelerometer
    accel_data_service_subscribe(40, accel_data_handler);
    accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);

}

static void deinit(void) {
    //UI
    window_destroy(window);
    //Accelerometer
    accel_data_service_unsubscribe();


}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}
