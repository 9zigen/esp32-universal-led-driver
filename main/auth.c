//
// Created by Aleksey Volkov on 25.11.2020.
//
#include <stdio.h>
#include <mbedtls/md.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_random.h"
#include "esp_hmac.h"

#include "auth.h"

static const char *TAG="AUTH";

/***
 * Generate token string.
 * @param token_str
 */
void auth_generate_token(char *token_str)
{
    if (token_str == NULL)
    {
        return;
    }

    char *payload = calloc(32, sizeof(char));
    snprintf(payload, 32, "%lu", esp_random());

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32C6)
    for(int i= 0; i< sizeof(payload); i++){
        printf("%02x", (int)payload[i]);
        sprintf(&token_str[2 * i], "%02x", (int)payload[i]);
    }
    ESP_LOGD(TAG, "%s [%u]",token_str, strlen(token_str));
#else
    char *salt = "Alab Salt";
    uint8_t hmac[32];

    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    const size_t payload_length = strlen(payload);
    const size_t key_length = strlen(salt);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *) salt, key_length);
    mbedtls_md_hmac_update(&ctx, (const unsigned char *) payload, payload_length);
    mbedtls_md_hmac_finish(&ctx, hmac);
    mbedtls_md_free(&ctx);

    ESP_LOGD(TAG, "Hash:");
    for(int i= 0; i< sizeof(hmac); i++){
        printf("%02x", (int)hmac[i]);
        sprintf(&token_str[2 * i], "%02x", (int)hmac[i]);
    }
    ESP_LOGD(TAG, "%s",token_str);
#endif
    free(payload);
}