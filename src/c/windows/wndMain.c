#include <pebble.h>
#include "wndMain.h"

static Window *wndMain;         // Main window static pointer

static TextLayer *lyrTime;      // time text
static TextLayer *lyrWeather;

#if defined(PBL_COLOR)
static BitmapLayer *lyrBackground;
static GBitmap *bmpBackground;
#endif


/**************************************************************************
 * Main window handler for loading
 **************************************************************************/
static void wndMain_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

#if defined(PBL_COLOR)
  // Create GBitmap
  bmpBackground = gbitmap_create_with_resource(RESOURCE_ID_IMG_GLOBE);
  
  // Create BitmapLayer to display the GBitmap
  lyrBackground = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(lyrBackground, bmpBackground);
  layer_add_child(window_layer, bitmap_layer_get_layer(lyrBackground));
#endif

  // Create the TextLayer with specific bounds
  lyrTime = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(lyrTime, GColorClear);
  text_layer_set_text_color(lyrTime, GColorWhite);
  text_layer_set_text(lyrTime, "00:00");
  text_layer_set_text_alignment(lyrTime, GTextAlignmentCenter);

  // Set text layer font
  text_layer_set_font(lyrTime, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
                      
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(lyrTime));

  // Create temperature Layer
  lyrWeather = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(125, 120), bounds.size.w, 30));

  // Style the text
  text_layer_set_background_color(lyrWeather, GColorClear);
  text_layer_set_text_color(lyrWeather, GColorWhite);
  text_layer_set_text_alignment(lyrWeather, GTextAlignmentCenter);
  text_layer_set_text(lyrWeather, "Loading...");

  text_layer_set_font(lyrWeather, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(lyrWeather));
}


/**************************************************************************
 * Main window handler for unloading
 **************************************************************************/
static void wndMain_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(lyrTime);

  // Destroy weather elements
  text_layer_destroy(lyrWeather);
  
#if defined(PBL_COLOR)
  // Destroy GBitmap
  gbitmap_destroy(bmpBackground);

  // Destroy BitmapLayer
  bitmap_layer_destroy(lyrBackground);
#endif
  
  // Self-destruct! :-)
  window_destroy(wndMain);
}


/**************************************************************************
 * Creates this window and pushes it onto the window stack
 **************************************************************************/
void wndMain_push() {
  // Create main Window element and assign to pointer
  if (!wndMain) {
    wndMain = window_create();

    window_set_background_color(wndMain, GColorBlack);

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(wndMain, (WindowHandlers) {
      .load = wndMain_load,
      .unload = wndMain_unload
    });
  }
  
  // Show the Window on the watch, with animated=true
  window_stack_push(wndMain, true);
}


/**************************************************************************
 * Updates the time in *lyrTime
 **************************************************************************/
void wndMain_updateTime() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char buffer[10];
  strftime(buffer, sizeof(buffer), clock_is_24h_style() ?
      "%H:%M" : "%l:%M %p", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(lyrTime, buffer);
}



/**************************************************************************
 * Updates the weather string in *lyrWeather
 **************************************************************************/
void wndMain_updateWeather(const char* weatherBuffer) {
  text_layer_set_text(lyrWeather, weatherBuffer);
}

