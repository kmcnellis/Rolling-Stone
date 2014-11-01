#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *text_layer_r;

static char buffer[40];
static AppSync sync;
static uint8_t sync_buffer[32];

enum Coordinate {
  x_co = 0x0,         // TUPLE_INT
  y_co = 0x1,       // TUPLE_INT
  z_co = 0x2,       // TUPLE_INT
};

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
            TupletInteger(x_co, x),
            TupletInteger(y_co, y),
            TupletInteger(y_co, z)
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

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

    text_layer_set_text(text_layer, "Updated!");

}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);


    Tuplet initial_values[] = {
        TupletInteger(x_co, (uint8_t) 0),
        TupletInteger(y_co, (uint8_t) 0),
        TupletInteger(y_co, (uint8_t) 0)
    };

    app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
    sync_tuple_changed_callback, sync_error_callback, NULL);


    text_layer = text_layer_create((GRect) { .origin = { 0, 32 }, .size = { bounds.size.w, 40 } });
    text_layer_r = text_layer_create((GRect) { .origin = { 0, 80 }, .size = { bounds.size.w, 40 } });

    text_layer_set_text(text_layer, "Move!");
    text_layer_set_text(text_layer_r, "Not updating...");

    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(text_layer_r, GTextAlignmentCenter);

    layer_add_child(window_layer, text_layer_get_layer(text_layer));
    layer_add_child(window_layer, text_layer_get_layer(text_layer_r));

}

static void window_unload(Window *window) {
    text_layer_destroy(text_layer);
    text_layer_destroy(text_layer_r);

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

}

static void deinit(void) {
    handle_deinit();
    window_destroy(window);

}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}
