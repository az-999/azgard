#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_timer.h"
#include "esp_zigbee_core.h"
#include "esp_zigbee_attribute.h"
#include "esp_zigbee_cluster.h"
#include "zcl/esp_zigbee_zcl_analog_input.h"

#define GPIO_PULSE_INPUT 2
#define BOOT_BUTTON_GPIO 9
#define LED_GPIO 0
#define ZIGBEE_ENDPOINT 1
#define TAG "WATER_METER"
#define LITERS_PER_PULSE 1
#define NVS_NAMESPACE "storage"
#define NVS_KEY "liters"
#define MULTI_CLICK_INTERVAL_MS 500
#define REQUIRED_CLICKS 3

static volatile uint32_t pulse_count = 0;
static volatile uint8_t click_counter = 0;
static int64_t last_click_time = 0;

void save_pulse_count(uint32_t count) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) == ESP_OK) {
        nvs_set_u32(handle, NVS_KEY, count);
        nvs_commit(handle);
        nvs_close(handle);
        ESP_LOGI(TAG, "💾 Сохранили: %lu литров", count);
    }
}

uint32_t load_pulse_count() {
    nvs_handle_t handle;
    uint32_t count = 0;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) == ESP_OK) {
        nvs_get_u32(handle, NVS_KEY, &count);
        nvs_close(handle);
        ESP_LOGI(TAG, "📥 Загружено из NVS: %lu литров", count);
    }
    return count;
}

static void IRAM_ATTR pulse_isr_handler(void *arg) {
    pulse_count++;
    ESP_EARLY_LOGI(TAG, "💧 Пульс! Счётчик: %lu", pulse_count);
}

static void IRAM_ATTR boot_button_isr_handler(void *arg) {
    int64_t now = esp_timer_get_time() / 1000;
    ESP_EARLY_LOGI(TAG, "🔘 Нажата кнопка BOOT, время: %lld", now);

    if (now - last_click_time > MULTI_CLICK_INTERVAL_MS) {
        click_counter = 0;
    }
    last_click_time = now;
    click_counter++;

    if (click_counter >= REQUIRED_CLICKS) {
        pulse_count = 0;
        save_pulse_count(0);
        ESP_EARLY_LOGI(TAG, "🔄 Тройное нажатие BOOT — счётчик сброшен!");
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(LED_GPIO, 0);
        click_counter = 0;
    }
}

void report_task(void *pvParameters) {
    uint32_t last_reported = -1;
    while (1) {
        uint32_t current_value = pulse_count * LITERS_PER_PULSE;
        if (current_value != last_reported) {
            last_reported = current_value;

            float present_value = (float)current_value;
            esp_zb_zcl_set_attribute_val(
                ZIGBEE_ENDPOINT,
                ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,
                ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
                ESP_ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID,
                &present_value,
                false);

            ESP_LOGI(TAG, "📤 Отправлен Zigbee отчёт: %lu литров", current_value);
            gpio_set_level(LED_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(50));
            gpio_set_level(LED_GPIO, 0);
            save_pulse_count(current_value);
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL << GPIO_PULSE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_PULSE_INPUT, pulse_isr_handler, NULL);

    gpio_config_t btn_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL << BOOT_BUTTON_GPIO,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&btn_conf);
    gpio_isr_handler_add(BOOT_BUTTON_GPIO, boot_button_isr_handler, NULL);

    gpio_config_t led_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LED_GPIO
    };
    gpio_config(&led_conf);

    esp_zb_platform_config_t platform_config = {
        .radio_config = {.radio_mode = ZB_RADIO_MODE_NATIVE},
        .host_config = {.host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE},
    };
    esp_zb_platform_config(&platform_config);

    esp_zb_cfg_t zb_cfg = {
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,
        .install_code_policy = false,
        .nwk_cfg.zed_cfg = {.ed_timeout = 180}
    };
    esp_zb_init(&zb_cfg);

    static char model_id[] = {10, 'W','a','t','e','r','M','e','t','e','r'};
    static char manuf_name[] = {9, 'E','s','p','r','e','s','s','i','f'};

    esp_zb_basic_cluster_cfg_t basic_cfg = {.zcl_version = 3, .power_source = 0x03};
    esp_zb_attribute_list_t *basic_attr_list = esp_zb_basic_cluster_create(&basic_cfg);
    esp_zb_basic_cluster_add_attr(basic_attr_list, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, model_id);
    esp_zb_basic_cluster_add_attr(basic_attr_list, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, manuf_name);

    esp_zb_analog_input_cluster_cfg_t analog_cfg = {
        .out_of_service = false,
        .present_value = 0
    };
    esp_zb_attribute_list_t *analog_attr_list = esp_zb_analog_input_cluster_create(&analog_cfg);

    esp_zb_cluster_list_t *clusters = esp_zb_zcl_cluster_list_create();
    esp_zb_cluster_list_add_basic_cluster(clusters, basic_attr_list, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    esp_zb_cluster_list_add_analog_input_cluster(clusters, analog_attr_list, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    esp_zb_endpoint_config_t ep_config = {
        .endpoint = ZIGBEE_ENDPOINT,
        .app_profile_id = 0x0104,
        .app_device_id = 0x000C, // Analog Input Device
        .app_device_version = 0x1
    };

    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_ep_list_add_ep(ep_list, clusters, ep_config);
    esp_zb_device_register(ep_list);

    pulse_count = load_pulse_count();
    xTaskCreate(report_task, "report_task", 2048, NULL, 5, NULL);

    ESP_ERROR_CHECK(esp_zb_start(true));
    while (1) {
        esp_zb_main_loop_iteration();  // пока оставим
    }
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct) {
    esp_zb_app_signal_type_t sig_type = *(signal_struct->p_app_signal);
    esp_err_t err_status = signal_struct->esp_err_status;

    ESP_LOGI(TAG, "📶 Zigbee сигнал: %d, статус: %s", sig_type, esp_err_to_name(err_status));

    switch (sig_type) {
        case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
            ESP_LOGI(TAG, "✅ Zigbee стек инициализирован");
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
            break;
        case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
            if (err_status == ESP_OK) {
                ESP_LOGI(TAG, "🎉 Устройство присоединилось к сети Zigbee");
            } else {
                ESP_LOGW(TAG, "❌ Ошибка присоединения к сети: %s", esp_err_to_name(err_status));
            }
            break;
        default:
            ESP_LOGW(TAG, "⚠️ Необработанный сигнал: %d", sig_type);
            break;
    }
}
