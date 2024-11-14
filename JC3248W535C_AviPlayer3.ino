#include <Arduino.h>
#include <lvgl.h>
#include "display.h"
#include "esp_bsp.h"
#include "lv_port.h"

#include "JC3248W535.h"

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

size_t output_buf_size;
uint16_t *output_buf;

#include "AviFunc.h"
#include "esp32_audio.h"

#define LVGL_PORT_ROTATION_DEGREE (90)

// Episode data structure
struct Episode {
    uint8_t number;
    const char* title;
    const char* filename;  // Added filename field
};

// Episode database with filenames
const Episode EPISODES[] = {
    {1, "Ship In A Bottle", "BP01.avi"},
    {2, "The Owls of Athens", "BP02.avi"},
    {3, "The Frog Princess", "BP03.avi"},
    {4, "The Ballet Shoe", "BP04.avi"},
    {5, "The Hamish", "BP05.avi"},
    {6, "The Wise Man", "BP06.avi"},
    {7, "The Elephant", "BP07.avi"},
    {8, "The Mouse Mill", "BP08.avi"},
    {9, "The Giant", "BP09.avi"},
    {10, "The Old Man's Beard", "BP10.avi"},
    {11, "The Fiddle", "BP11.avi"},
    {12, "Flying", "BP12.avi"},
    {13, "Uncle Feedle", "BP13.avi"}
};

const int NUM_EPISODES = sizeof(EPISODES) / sizeof(EPISODES[0]);
const char *root = "/root";
const char *avi_folder = "/avi320x480";

// Function to get episode title by number
const char* getEpisodeTitle(uint8_t episodeNumber) {
    if (episodeNumber < 1 || episodeNumber > NUM_EPISODES) {
        return "Invalid Episode";
    }
    return EPISODES[episodeNumber - 1].title;
}

// Function to get episode filename by number
const char* getEpisodeFilename(uint8_t episodeNumber) {
    if (episodeNumber < 1 || episodeNumber > NUM_EPISODES) {
        return nullptr;
    }
    return EPISODES[episodeNumber - 1].filename;
}



void setup() {
    Serial.begin(115200);
    Serial.println("Episode Player Starting");

    // Init SPI if needed
    #ifdef SPI_SCK
        SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    #endif

    #ifdef GFX_EXTRA_PRE_INIT
        GFX_EXTRA_PRE_INIT();
    #endif

    // Initialize display
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = EXAMPLE_LCD_QSPI_H_RES * EXAMPLE_LCD_QSPI_V_RES,
        .rotate = LVGL_PORT_ROTATION_DEGREE == 90 ? LV_DISP_ROT_90 :
                 LVGL_PORT_ROTATION_DEGREE == 270 ? LV_DISP_ROT_270 :
                 LVGL_PORT_ROTATION_DEGREE == 180 ? LV_DISP_ROT_180 : LV_DISP_ROT_NONE,
    };

    bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();

    // Initialize audio
    #ifdef AUDIO_MUTE_PIN
        pinMode(AUDIO_MUTE_PIN, OUTPUT);
        digitalWrite(AUDIO_MUTE_PIN, HIGH);
    #endif
    i2s_init();

    // Initialize filesystem
  #if defined(SD_D1)
    #define FILESYSTEM SD_MMC
      SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */, SD_D1, SD_D2, SD_CS /* D3 */);
      if (!SD_MMC.begin(root, false /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
  #elif defined(SD_SCK)
    #define FILESYSTEM SD_MMC
      pinMode(SD_CS, OUTPUT);
      digitalWrite(SD_CS, HIGH);
      SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */);
      if (!SD_MMC.begin(root, true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
  #elif defined(SD_CS)
    #define FILESYSTEM SD
      if (!SD.begin(SD_CS, SPI, 80000000, "/root"))
  #else
    #define FILESYSTEM FFat
      if (!FFat.begin(false, root))
      // if (!LittleFS.begin(false, root))
      // if (!SPIFFS.begin(false, root))
      #endif
    {
      Serial.println("ERROR: File system mount failed!");
    }
    else
    {
      output_buf_size = gfx->width() * gfx->height() * 2;
      #ifdef RGB_PANEL
          output_buf = gfx->getFramebuffer();
      #else
          output_buf = (uint16_t *)aligned_alloc(16, output_buf_size);
      #endif
      if (!output_buf)
        {
          Serial.println("output_buf aligned_alloc failed!");
        }

      avi_init();

      create_episode_selector();

    }

}


// Function to play a specific episode
void playEpisode(uint8_t episodeNumber) {

  // Clean up LVGL
  lv_obj_clean(lv_scr_act());

  // Create blank black screen
  lv_obj_t* new_screen = lv_obj_create(NULL);
  lv_scr_load(new_screen);
  lv_obj_set_style_bg_color(new_screen, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);

  // Force LVGL to process the screen change
  lv_timer_handler();

  // Small delay to ensure screen is cleared
  delay(50);

  const char* filename = getEpisodeFilename(episodeNumber);
  if (!filename) {
      Serial.println("Invalid episode number");
      return;
  }

  char fullPath[100];
  sprintf(fullPath, "%s/%s", avi_folder, filename);
  
  File aviFile = FILESYSTEM.open(fullPath);
  if (!aviFile) {
    Serial.println("Failed to open file");
    return;
  }

    if (!aviFile.isDirectory())
    {
      std::string s = aviFile.name();
      // if ((!s.starts_with(".")) && (s.ends_with(".avi")))
      if ((s.rfind(".", 0) != 0) && ((int)s.find(".avi", 0) > 0))
      {
        s = root;
        s += aviFile.path();
        if (avi_open((char *)s.c_str()))

        {
          Serial.println("AVI start");
          gfx->fillScreen(BLACK);

          if (avi_aRate > 0)
          {
            i2s_set_sample_rate(avi_aRate);
          }

          avi_feed_audio();

          if (avi_aFormat == PCM_CODEC_CODE)
          {
            Serial.println("Start play PCM audio task");
            BaseType_t ret_val = pcm_player_task_start();
            if (ret_val != pdPASS)
            {
              Serial.printf("pcm_player_task_start failed: %d\n", ret_val);
            }
          }
          else if (avi_aFormat == MP3_CODEC_CODE)
          {
            Serial.println("Start play MP3 audio task");
            BaseType_t ret_val = mp3_player_task_start();
            if (ret_val != pdPASS)
            {
              Serial.printf("mp3_player_task_start failed: %d\n", ret_val);
            }
          }
          else
          {
            Serial.println("No audio task");
          }


          avi_start_ms = millis();

          Serial.println("Start play loop");
          while (avi_curr_frame < avi_total_frames) {
              avi_feed_audio();
              if (avi_decode()) {
                  avi_draw(0, 0);
              }
          }

            avi_close();
            Serial.println("AVI end");
        }
      }
      aviFile.close();
    }
    // Return to LVGL UI after video:
    create_episode_selector(); // Your function to recreate the LVGL UI
}

// Callback for scroll events
static void scroll_event_cb(lv_event_t * e) {
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 5 / 10;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        lv_coord_t x;
        if(diff_y >= r) {
            x = r;
        }
        else {
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);
            x = r - res.i;
        }

        lv_obj_set_style_translate_x(child, x, 0);
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);

        if (diff_y < 10) {
            lv_obj_t* label = lv_obj_get_child(child, 0);
            const char* btn_text = lv_label_get_text(label);
            int episode_num;
            sscanf(btn_text, "Episode %d", &episode_num);
            
            const char* title = getEpisodeTitle(episode_num);
            
            lv_obj_t* play_label = (lv_obj_t*)lv_obj_get_user_data(cont);
            if (play_label) {
                lv_label_set_text_fmt(play_label, "Play - %s", title);
            }
        }
    }
}

// Callback for play button
static void play_btn_event_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * cont = (lv_obj_t*)lv_obj_get_user_data(btn);
    
    // Find the centered episode
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;
    
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);
        
        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        lv_coord_t diff_y = child_y_center - cont_y_center;
        if (LV_ABS(diff_y) < 10) {
            lv_obj_t* label = lv_obj_get_child(child, 0);
            const char* btn_text = lv_label_get_text(label);
            int episode_num;
            sscanf(btn_text, "Episode %d", &episode_num);
            
            // Hide LVGL screen
            lv_obj_add_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN);
            
            // Play the episode
            playEpisode(episode_num);
            
            // Show LVGL screen again
            lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN);
            return;
        }
    }
}

void create_episode_selector(void) {
    // Set background color
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);
    
    // Create and style the play button
    lv_obj_t * play_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(play_btn, 400, 40);
    lv_obj_align(play_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    lv_obj_set_style_bg_color(play_btn, lv_color_hex(0x00aa00), LV_PART_MAIN);
    lv_obj_set_style_bg_color(play_btn, lv_color_hex(0x008800), LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(play_btn, 10, 0);
    lv_obj_set_style_shadow_color(play_btn, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(play_btn, LV_OPA_30, 0);
    
    lv_obj_t * play_label = lv_label_create(play_btn);
    lv_obj_set_style_text_font(play_label, &lv_font_montserrat_24, 0);
    lv_label_set_text(play_label, "Play - Ship In A Bottle");
    lv_obj_center(play_label);

    // Create the container for episodes
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 240, 240);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, -25);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cont, true, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    
    // Store play button label pointer in container's user data
    lv_obj_set_user_data(cont, play_label);
    
    // Store container pointer in play button's user data for callback
    lv_obj_set_user_data(play_btn, cont);
    
    // Add click event to play button
    lv_obj_add_event_cb(play_btn, play_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // Create episode buttons
    for(uint32_t i = 0; i < NUM_EPISODES; i++) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_width(btn, lv_pct(100));
        
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x005577), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
        
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Episode %"LV_PRIu32, i+1);
        lv_obj_center(label);
    }

    lv_event_send(cont, LV_EVENT_SCROLL, NULL);
    lv_obj_scroll_to_view(lv_obj_get_child(cont, 0), LV_ANIM_OFF);
}


void loop() 
{
}

