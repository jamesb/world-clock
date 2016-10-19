#include <pebble.h>

#include "windows/wndMain.h"

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1


/**************************************************************************
 * Handles callbacks from the JS component
 **************************************************************************/
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[40];

  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);

  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);

    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    wndMain_updateWeather(weather_layer_buffer);
  }
}


static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


/**************************************************************************
 * Callback for TickTimerService
 **************************************************************************/
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  wndMain_updateTime();

  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}


/**************************************************************************
 * Used for the creation of all Pebble SDK elements.
 **************************************************************************/
static void init() {
  wndMain_push();
  
  // Make sure the time is displayed from the start
  wndMain_updateTime();
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}


/**************************************************************************
 * Used for the destruction of all Pebble SDK elements.
 **************************************************************************/
static void deinit() {
}


/**************************************************************************
 * Standard Pebble main function.
 **************************************************************************/
int main(void) {
  init();

  /* wait for system events until exit*/
  app_event_loop();
  
  deinit();
}
