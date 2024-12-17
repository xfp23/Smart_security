idf_component_register(SRCS "timer.cpp" "main.cpp" "hal_idf_bluetooth.c" "user_queue.c" "ble.cpp" "DHT11.cpp" "hal_idf.c" "MQ_2.cpp" "Beep.cpp" "System.cpp" "oled.c" "timer.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_timer bt nvs_flash esp_adc) 

set(CMAKE_CXX_STANDARD 11)
