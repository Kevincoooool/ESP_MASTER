if(ESP_PLATFORM)
set(SOURCES "disp_driver.c")
# Include only the source file of the selected
# display controller.
if(CONFIG_LV_TFT_DISPLAY_CONTROLLER_ST7789)
    list(APPEND SOURCES "st7789.c")

elseif(CONFIG_LV_TFT_DISPLAY_CONTROLLER_FT81X)
    list(APPEND SOURCES "EVE_commands.c")
    list(APPEND SOURCES "FT81x.c")

else()
    message("DISPLAY CONTROLLER NOT DEFINED")
endif()

if(CONFIG_LV_TFT_DISPLAY_PROTOCOL_SPI)
    list(APPEND SOURCES "disp_spi.c")
endif()

# Print the included source files
message("SOURCES contents: " "${SOURCES}")

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "." "../../lvgl/lvgl"
                       REQUIRES lvgl)

endif()
