file(GLOB LIB_SRCS
 "../Lib/Adafruit_GFX_Library/*.cpp"
 "../Lib/Adafruit_GFX_Library/*.c"
 "../Lib/Adafruit_SSD1306/*.cpp"
 "../Lib/Adafruit_SSD1306/*.c"
 "../Lib/Adafruit_BusIO/*.cpp"
  "../Lib/Adafruit_BusIO/*.c"
)

idf_component_register(SRCS "main.cpp" "Screen.cpp" "hal_idf_bluetooth.c" "user_queue.c" "DHT11.cpp" "JSON.cpp"
                    INCLUDE_DIRS "."
                    "../Lib/Adafruit_GFX_Library"
                    "../Lib/Adafruit_SSD1306"
                    "../Lib/Adafruit_BusIO"
                    PRIV_REQUIRES arduino-esp32 bt nvs_flash)

set(CMAKE_CXX_STANDARD 11)
