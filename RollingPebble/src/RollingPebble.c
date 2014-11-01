#include <pebble.h>

static Window *window;
static ActionBarLayer *actionbarlayer;

static TextLayer *text_layer;
static TextLayer *text_layer_updated;
static TextLayer *text_layer_function;
static TextLayer *text_layer_select;
static TextLayer *text_layer_modifier;


static GBitmap* icon_up;
static GBitmap* icon_down;
static GBitmap* icon_square;


static char buffer[40];
static AppSync sync;
static uint8_t sync_buffer[32];

static int connections=0;
static char functions[5][20];

static bool robot_move = false;
static bool mouse = false;
static bool keyboare = false;
static int  function =false;
enum Sync_Data {
  _x_co = 0x0,         // TUPLE_INT
  _y_co = 0x1,       // TUPLE_INT
  _z_co = 0x2,       // TUPLE_INT
  _robot_move = 0x4, // TUPLE_BOOL
  _mouse = 0x5, // TUPLE_BOOL
  _keyboard = 0x6, // TUPLE_BOOL
  _select = 0x7, // TUPLE_BOOL
  _mod = 0x7, // TUPLE_BOOL
  _function = 0x8 // TUPLE_INT
};

//Accelerometer

void accel_data_handler(AccelData *data, uint32_t num_samples) {
  // Process 10 events - every 1 second
    int16_t x = 0, y=0, z=0;
    uint32_t a=0;
    int16_t num= 0;
    for (a =0; a<num_samples; a++){
        if (data[a].did_vibrate==false){
            num++;
            x+=data[a].x;
            y+=data[a].y;
            z+=data[a].z;
        }
    }

    x=(int16_t) x/num;
    y=(int16_t) y/num;
    z=(int16_t) z/num;
    if(num > 0){
        Tuplet values[] = {
            TupletInteger(_x_co, x),
            TupletInteger(_y_co, y),
            TupletInteger(_z_co, z)
        };

        snprintf(buffer, sizeof(buffer), "x: %i y: %i\nz:%i", x,y,z);
        app_sync_set(&sync, values, 3);
        text_layer_set_text(text_layer, buffer);
    }
}

void handle_init(void) {
    accel_data_service_subscribe(20, accel_data_handler);
    accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
}

void handle_deinit(void) {
    accel_data_service_unsubscribe();
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


// Appsync

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

    text_layer_set_text(text_layer, "Updated!");

}


//Appmessage

void out_sent_handler(DictionaryIterator *sent, void *context) {
    text_layer_set_text(text_layer, "Updated!");
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
     text_layer_set_text(text_layer, "Failed :(");
 }


 void in_received_handler(DictionaryIterator *received, void *context) {
   // incoming message received
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

//Loading

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);


    Tuplet initial_values[] = {
        TupletInteger(_x_co      , (uint8_t) 0),
        TupletInteger(_y_co      , (uint8_t) 0),
        TupletInteger(_z_co      , (uint8_t) 0),
        TupletInteger(_robot_move, (bool) false),
        TupletInteger(_mouse     , (bool) false),
        TupletInteger(_keyboard  , (bool) false),
        TupletInteger(_select    , (bool) false),
        TupletInteger(_mod       , (bool) false),
        TupletInteger(_function  , (uint8_t) 0)

    };

    app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
    sync_tuple_changed_callback, sync_error_callback, NULL);

    window_set_background_color(window, GColorWhite);
    actionbarlayer = action_bar_layer_create();
    action_bar_layer_add_to_window(actionbarlayer, window);
    action_bar_layer_set_background_color(actionbarlayer, GColorBlack);

    icon_up= gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP_ARROW);
    icon_down= gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOWN_ARROW);
    icon_square= gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SQUARE);



    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_UP, icon_up);
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_DOWN, icon_down);
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_SELECT, icon_square);

    text_layer = text_layer_create((GRect) { .origin = { 0, bounds.size.h-60 }, .size = { bounds.size.w-ACTION_BAR_WIDTH, 30 } });
    text_layer_updated = text_layer_create((GRect) { .origin = { 0, bounds.size.h-30 }, .size = { bounds.size.w-ACTION_BAR_WIDTH, 30 } });

    text_layer_function = text_layer_create((GRect) { .origin = { bounds.size.w-ACTION_BAR_WIDTH-50, 20 }, .size = { 50, 30 } });
    text_layer_select = text_layer_create((GRect) { .origin = { bounds.size.w-ACTION_BAR_WIDTH-50, 50 }, .size = { 50, 30 } });
    text_layer_modifier = text_layer_create((GRect) { .origin = { bounds.size.w-ACTION_BAR_WIDTH-50, 80 }, .size = { 50, 30 } });


    text_layer_set_text(text_layer, "Move!");
    text_layer_set_text(text_layer_updated, "Not connected...");

    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(text_layer_updated, GTextAlignmentCenter);

    layer_add_child(window_layer, text_layer_get_layer(text_layer));
    layer_add_child(window_layer, text_layer_get_layer(text_layer_updated));
    layer_add_child(window_layer, (Layer *)actionbarlayer);


}

static void window_unload(Window *window) {
    text_layer_destroy(text_layer);
    text_layer_destroy(text_layer_updated);
    action_bar_layer_destroy(actionbarlayer);

    app_sync_deinit(&sync);

}

static void init(void) {
    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const int inbound_size = 64;
    const int outbound_size = 30;
    app_message_open(inbound_size, outbound_size);

    const bool animated = true;
    window_stack_push(window, animated);
    handle_init();

    app_message_open(inbound_size, outbound_size);

}

static void deinit(void) {
    handle_deinit();
    window_destroy(window);
    gbitmap_destroy(icon_up);
    gbitmap_destroy(icon_down);
    gbitmap_destroy(icon_square);

}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}
