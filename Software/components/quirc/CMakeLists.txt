idf_component_register(SRCS "decode.c" "identify.c" "quirc.c" "version_db.c"
                    INCLUDE_DIRS "." "../read_nvs"
                    REQUIRES json nvs_flash spiffs )
