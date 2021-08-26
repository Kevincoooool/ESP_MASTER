idf_component_register(SRCS "web_server.c"
                    INCLUDE_DIRS "." "../read_nvs"
                    REQUIRES json spiffs esp_https_server nvs_flash 
                    EMBED_FILES "favicon.ico" "upload_script.html")

