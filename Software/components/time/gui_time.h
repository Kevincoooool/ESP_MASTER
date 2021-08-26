#ifndef _GUITIME_
#define  _GUITIME_

/*******sntp********/
#include <time.h>
#include <sys/time.h>
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"


void initialize_sntp(void);
void time_init();
void get_time(struct tm *timeinfo);

#endif