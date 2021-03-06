#include <pebble.h>
#define error_log true
#define unit_one false
#define margin 40

#define ACCEL_RATIO 10
#define ACCEL_STEP_MS 50
#define MESS_STEP_MS 750

static Window *window;
static GFont s_res_gothic_14;
static GFont s_res_gothic_18_bold;

static ActionBarLayer *actionbarlayer;

static TextLayer *text_layer_modifier;
static TextLayer *text_layer_select;
static TextLayer *text_layer_action;
static TextLayer *text_layer;
static TextLayer *text_layer_updated;
static TextLayer *text_layer_info;

static GBitmap* icon_up;
static GBitmap* icon_square;
static GBitmap* icon_down;
static GBitmap* icon_music;
static GBitmap* icon_m_right;
static GBitmap* icon_m_left;
static GBitmap* icon_keyboard;
static GBitmap* icon_power;
static GBitmap* icon_next;


static AppTimer *mess_timer;
static AppTimer *accel_timer;
static AppTimer *go_timer;

static char buffer[40];

static bool local_robot_move = true;
static bool local_mouse = true;
static bool local_keyboard = false;
static int  local_action =0;
static bool local_enable =false;

static int  local_mod =0;
static int  local_sel =0;

static int count=0;


int x_co = 0, y_co=0, z_co=0;

#define X_KEY 0
#define Y_KEY 1
#define Z_KEY 2
#define SELECT_KEY 3
#define MOD_KEY 4
#define ACTION_KEY 5
#define EXTRA_KEY 6

enum Sync_Data {
  x = 0x0,         // TUPLE_INT
  y = 0x1,       // TUPLE_INT
  z = 0x2,       // TUPLE_INT
  robot_move = 0x4, // TUPLE_DOOL
  mouse = 0x5, // TUPLE_BOOL
  keyboard = 0x6, // TUPLE_BOOL
  select = 0x7, // TUPLE_BOOL
  mod = 0x7, // TUPLE_BOOL
  action = 0x8 // TUPLE_INT
};

//Accelerometer
void send_message();
static void set_ui(void);
void accel_data_handler(AccelData *data, uint32_t num_samples) {
    uint32_t a=0;
    int16_t num= 0;

    for (a =0; a<num_samples; a++){
        if (data[a].did_vibrate==false){
            num++;
            x_co+=(data[a].x);
            x_co=x_co/2;
            y_co+=(data[a].y);
            y_co=x_co/2;
            z_co+=(data[a].z);
            z_co=z_co/2;
            // x_co=(data[a].x);
            // y_co=(data[a].y);
            // z_co=(data[a].z);
            int x_=data[a].x, y_=data[a].y, z_=data[a].z;
             if (error_log){ APP_LOG(APP_LOG_LEVEL_DEBUG, "RR X: %d Y: %d Z: %d",x_co,x_co,x_co);}

        }
    }
}
static void accel_timer_callback(void *arg) {
    AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };

    accel_service_peek(&accel);

    uint32_t a=0;
    int16_t num= 0;


    if (accel.did_vibrate==false){
        num++;
        // x_co+=(accel.x);
        // x_co=x_co/2;
        // y_co+=(accel.y);
        // y_co=x_co/2;
        // z_co+=(accel.z);
        // z_co=z_co/2;
        x_co=(accel.x);
        y_co=(accel.y);
        z_co=(accel.z);
        int x_=accel.x, y_=accel.y, z_=accel.z;
        //  if (error_log){ APP_LOG(APP_LOG_LEVEL_DEBUG, "RT X: %d Y: %d Z: %d",x_,y_,z_);}

    }

  accel_timer = app_timer_register(ACCEL_STEP_MS, accel_timer_callback, NULL);
}

//Clicks


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    // DictionaryIterator *iter;
    // app_message_outbox_begin(&iter);
    // int val = 1;
    switch (local_action){
        case 0: //mouse
        case 1: //car
            if (local_enable){
                local_enable=false;
                text_layer_set_text(text_layer_info, "Disabled");
                text_layer_set_text(text_layer_select, "On");
            }
            else{
                local_enable=true;

                text_layer_set_text(text_layer_info, "Enabled");
                text_layer_set_text(text_layer_select, "Off");
            }
            break;
        case 2: //keyboard
            text_layer_set_text(text_layer_info, "Select");
            local_sel = 1;

            break;
    }
    // dict_write_int(iter, MOD_KEY, &val, sizeof(int), true);
    // dict_write_int(iter, ACTION_KEY, &local_action, sizeof(int), true);
    //
    // app_message_outbox_send();


}
void double_click_handler(ClickRecognizerRef recognizer, void *context){
    switch (local_action){

    case 0: //mouse
    case 1: //car
        local_sel = 1;
        text_layer_set_text(text_layer_info, "Double Click");

        break;
    case 2: //keyboard


        break;
}
}

static void mod_click_handler(ClickRecognizerRef recognizer, void *context) {
    // DictionaryIterator *iter;
    // app_message_outbox_begin(&iter);

    switch (local_action){
        case 0: //mouse
            text_layer_set_text(text_layer_info, "Click");
            break;
        case 1: //car
            text_layer_set_text(text_layer_info, "Play Music");
            break;
        case 2: //keyboard
            text_layer_set_text(text_layer_info, "Next");
            break;
    }
    // dict_write_int(iter, MOD_KEY, &val, sizeof(int), true);
    // dict_write_int(iter, ACTION_KEY, &local_action, sizeof(int), true);
    //
    // app_message_outbox_send();
    local_mod = 1;

}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    local_enable=false;
    local_action++;
    local_action=local_action%3;

    set_ui();
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, mod_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
    window_multi_click_subscribe(BUTTON_ID_UP, 2,0,0,true,double_click_handler);
}


//Appmessage

void send_message(){

    int x_=0, y_=0, z_=0;
    if(local_enable){

        // snprintf(buffer, sizeof(buffer), "x: %i\ny: %i\nz: %i", x_co,y_co,z_co);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        if (!unit_one){
             x_=(x_co/ACCEL_RATIO);
             y_=(y_co/ACCEL_RATIO);
             z_=(z_co/ACCEL_RATIO);
        }
        else{
            if(x_co > margin) x_=1;
            else if(x_co < -1*margin) x_=-1;
            else x_=0;

            if(y_co > margin) y_=1;
            else if(y_co < -1*margin) y_=-1;
            else y_=0;

            if(z_co > margin) z_=1;
            else if(z_co < -1*margin) z_=-1;
            else z_=0;
        }

        snprintf(buffer, sizeof(buffer), "x: %i\ny: %i\nz: %i", x_,y_,z_);
if(error_log){APP_LOG(APP_LOG_LEVEL_DEBUG, "OT X: %d Y: %d Z: %d mod: %d sel: %d", x_, y_, z_, local_mod, local_sel);}

        dict_write_int(iter, X_KEY, &x_, sizeof(int), true);
        dict_write_int(iter, Y_KEY, &y_, sizeof(int), true);
        dict_write_int(iter, Z_KEY, &z_, sizeof(int), true);
        dict_write_int(iter, ACTION_KEY, &local_action, sizeof(int), true);
        dict_write_int(iter, MOD_KEY, &local_mod, sizeof(int), true);
        dict_write_int(iter, SELECT_KEY, &local_sel, sizeof(int), true);
        local_mod=0;
        local_sel=0;
        app_message_outbox_send();
        text_layer_set_text(text_layer, buffer);
    }
    else{
        // snprintf(buffer, sizeof(buffer), "x: %i\ny: %i\nz: %i", x_co,y_co,z_co);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        x_=0;
        y_=0;
        z_=0;
        int val = 0;
        snprintf(buffer, sizeof(buffer), "x: 0\ny: 0\nz:0");

        if(error_log){APP_LOG(APP_LOG_LEVEL_DEBUG, "OT X: %d Y: %d Z: %d mod: %d sel: %d", x_, y_, z_, local_mod, local_sel);}

        dict_write_int(iter, X_KEY, &x_, sizeof(int), true);
        dict_write_int(iter, Y_KEY, &y_, sizeof(int), true);
        dict_write_int(iter, Z_KEY, &z_, sizeof(int), true);
        dict_write_int(iter, ACTION_KEY, &local_action, sizeof(int), true);
        dict_write_int(iter, MOD_KEY, &local_mod, sizeof(int), true);
        dict_write_int(iter, SELECT_KEY, &local_sel, sizeof(int), true);
        local_mod=0;
        local_sel=0;
        app_message_outbox_send();

        text_layer_set_text(text_layer, buffer);
    }
//    text_layer_set_text(text_layer_info, "");


}

static void message_timer_callback(void *arg) {
    text_layer_set_text(text_layer_updated, "Waiting...");
    if (error_log){ APP_LOG(APP_LOG_LEVEL_DEBUG, "message_timer_callback");}

    mess_timer=NULL;
    go_timer = app_timer_register(250, send_message, NULL);

    //send_message();
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
    if(mess_timer!=NULL){
        app_timer_cancel(mess_timer);
    }
    mess_timer = app_timer_register(MESS_STEP_MS, message_timer_callback, NULL);
    text_layer_set_text(text_layer_updated, "Updated!"); if (error_log){APP_LOG(APP_LOG_LEVEL_DEBUG, "out_sent_handler");}
    //psleep	(250);
    go_timer = app_timer_register(250, send_message, NULL);

    //send_message();
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
    if(mess_timer!=NULL){
        app_timer_cancel(mess_timer);
    }
    text_layer_set_text(text_layer_updated, "Failed :(");
    if (error_log){ APP_LOG(APP_LOG_LEVEL_DEBUG, "out_failed_handler");}
        go_timer = app_timer_register(250, send_message, NULL);

//    send_message();

 }



 void in_received_handler(DictionaryIterator *received, void *context) {
   // incoming message received
   if (error_log){APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler");}

 }



 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
   if (error_log){APP_LOG(APP_LOG_LEVEL_DEBUG, "in_dropped_handlers");}

 }

//UI
static void initialise_ui(void) {
  window_set_fullscreen(window, false);

  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  icon_up = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ARROW_UP);
  icon_square = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SQUARE);
  icon_down = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ARROW_DOWN);
  icon_music = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MUSIC);
  icon_m_right = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOUSE_RIGHT);
  icon_m_left = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOUSE_LEFT);
  icon_keyboard = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KEYBOARD);
  icon_power = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_POWER);
  icon_next = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEXT);


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
  action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_UP, icon_square);
  action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_SELECT, icon_square);
  action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_DOWN, icon_square);
  layer_add_child(window_get_root_layer(window), (Layer *)actionbarlayer);

  // text_layer_select
  text_layer_select = text_layer_create(GRect(57, 68, 64, 15));
  text_layer_set_background_color(text_layer_select, GColorClear);
  text_layer_set_text(text_layer_select, "Text layer");
  text_layer_set_text_alignment(text_layer_select, GTextAlignmentRight);
  text_layer_set_font(text_layer_select, s_res_gothic_14);
  layer_add_child(window_get_root_layer(window), (Layer *)text_layer_select);

  // text_layer_action
  text_layer_action = text_layer_create(GRect(5, 3, 79, 20));
  text_layer_set_text(text_layer_action, "Text layer");
  text_layer_set_font(text_layer_action, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(window), (Layer *)text_layer_action);

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


static void car_ui(void) {
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_UP, icon_music);
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_SELECT, icon_power);
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_DOWN, icon_square);
    text_layer_set_text(text_layer_modifier, "Music");
    text_layer_set_text(text_layer_select, "On");
    text_layer_set_text(text_layer_info, "Stopped");
    text_layer_set_text(text_layer_action, "Control Car");


}
static void keyboard_ui(void) {
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_UP, icon_next);
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_SELECT, icon_keyboard);
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_DOWN, icon_square);
    text_layer_set_text(text_layer_modifier, "Keys");
    text_layer_set_text(text_layer_select, "Select");
    text_layer_set_text(text_layer_info, "Key Shortcuts");
    text_layer_set_text(text_layer_action, "Keyboard");

}
static void mouse_ui(void) {
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_UP, icon_m_left);
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_SELECT, icon_power);
    action_bar_layer_set_icon(actionbarlayer, BUTTON_ID_DOWN, icon_square);
    text_layer_set_text(text_layer_modifier, "Click");
    text_layer_set_text(text_layer_select, "On");
    text_layer_set_text(text_layer_info, "Stopped");
    text_layer_set_text(text_layer_action, "Mouse");

}

static void set_ui(void){
    switch (local_action){
        case 0:
            mouse_ui();
            break;
        case 1:
            car_ui();
            break;
        case 2:
            keyboard_ui();
            break;
    }
}
static void destroy_ui(void) {
  text_layer_destroy(text_layer_modifier);
  action_bar_layer_destroy(actionbarlayer);
  text_layer_destroy(text_layer_select);
  text_layer_destroy(text_layer_action);
  text_layer_destroy(text_layer);
  text_layer_destroy(text_layer_updated);
  text_layer_destroy(text_layer_info);

  gbitmap_destroy(icon_up);
  gbitmap_destroy(icon_square);
  gbitmap_destroy(icon_down);
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
        TupletInteger(action  , (uint8_t) 0)

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
    const int outbound_size = 160;
    app_message_open(inbound_size, outbound_size);

    //Accelerometer
    //accel_data_service_subscribe(1, accel_data_handler);
    accel_data_service_subscribe(0, NULL);
    accel_timer = app_timer_register(ACCEL_STEP_MS, accel_timer_callback, NULL);
    go_timer = app_timer_register(250, send_message, NULL);

    accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);


}

static void deinit(void) {
    //UI
    window_destroy(window);
    //Accelerometer
    accel_data_service_unsubscribe();


}

int main(void) {
    init();
    //send_message();
     if (error_log){APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);}
    set_ui();
    app_event_loop();
    deinit();
}
