#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Dinh nghia Pin LED cho LED ngoai, vi du PA5 */
#define LED_PIN                 GPIO_Pin_5 // Su dung chan PA5
#define LED_PORT                GPIOA      // Port A
#define LED_CLK                 RCC_APB2Periph_GPIOA // Bat clock cho Port A

/* Dinh nghia kieu du lieu de gui qua Queue */
typedef struct {
    uint32_t frequency; // Tan so (Hz)
    uint8_t  dutyCycle; // Do rong xung (0-100%)
} LedControl_t;

/* Khai bao prototype cho cac ham */
void GPIO_Config(void);
void vLedBlinkTask(void *pvParameters);
void vDataGenerateTask(void *pvParameters);

/* Khai bao mot bien handle cho Queue */
QueueHandle_t ledQueueHandle = NULL;

int main(void)
{
    // Cau hinh phan cung
    GPIO_Config();

    // Tao Queue de chua duoc 5 item kieu LedControl_t
    // Kich thuoc moi item la sizeof(LedControl_t)
    ledQueueHandle = xQueueCreate(5, sizeof(LedControl_t));

    // Kiem tra xem Queue da duoc tao thanh cong chua
    if (ledQueueHandle != NULL)
    {
        // Tao 2 tac vu
        xTaskCreate(vLedBlinkTask,"LedBlink",128,NULL,1,NULL);              

        xTaskCreate(vDataGenerateTask,"DataGenerate",128,NULL,2,NULL);

        vTaskStartScheduler();
    }
    
    // Code se khong bao gio chay den day
    while (1);
}

/**
 * @brief Cau hinh chan GPIO cho LED la output
 */
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LED_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    
    // Tat LED ban dau
    GPIO_WriteBit(LED_PORT, LED_PIN, Bit_RESET); // Voi LED ngoai, RESET la TAT
}


/**
 * @brief TASK 1: Nhan du lieu tu Queue va dieu khien LED
 */
void vLedBlinkTask(void *pvParameters)
{
    LedControl_t currentPattern;
    uint32_t on_time_ms = 50, off_time_ms = 50; // Gia tri mac dinh de phong

    // Doi de nhan du lieu lan dau tien, sau do se nhay lien tuc
    if (xQueueReceive(ledQueueHandle, &currentPattern, portMAX_DELAY) == pdPASS)
    {
        // Tinh toan thoi gian on/off cho lan dau
        if (currentPattern.frequency > 0)
        {
            uint32_t period_ms = 1000 / currentPattern.frequency;
            on_time_ms = (period_ms * currentPattern.dutyCycle) / 100;
            off_time_ms = period_ms - on_time_ms;
        }
    }
    
    while (1)
    {
        // Kiem tra xem co mau nhay moi trong Queue khong (khong block)
        if (xQueueReceive(ledQueueHandle, &currentPattern, 0) == pdPASS)
        {
            // Neu co, cap nhat lai thoi gian on/off
            if (currentPattern.frequency > 0)
            {
                uint32_t period_ms = 1000 / currentPattern.frequency;
                on_time_ms = (period_ms * currentPattern.dutyCycle) / 100;
                off_time_ms = period_ms - on_time_ms;
            }
        }

        // Luon luon nhap nhay voi thong so hien tai
        GPIO_WriteBit(LED_PORT, LED_PIN, Bit_SET); // Bat LED
        vTaskDelay(pdMS_TO_TICKS(on_time_ms));
        
        GPIO_WriteBit(LED_PORT, LED_PIN, Bit_RESET); // Tat LED
        vTaskDelay(pdMS_TO_TICKS(off_time_ms));
    }
}

/**
 * @brief TASK 2: Dinh ky tao ra du lieu va gui vao Queue
 */
void vDataGenerateTask(void *pvParameters)
{
    LedControl_t dataToSend;
    uint8_t patternIndex = 0;

    // Tao cac mau nhay CUC NHANH (STROBE EFFECT)
    const LedControl_t patterns[] = {
        {30, 50}, // Nhay rat nhanh, deu dan (30 lan/giay)
        {50, 10}, // Hieu ung Strobe: Chop sang cuc ngan (50 lan/giay)
        {40, 75}, // Nhay rung, gan nhu sang lien tuc
        {25, 25}  // Nhay nhanh, chop ngan
    };
    const uint8_t numPatterns = sizeof(patterns) / sizeof(LedControl_t);

    while (1)
    {
        dataToSend = patterns[patternIndex];

        xQueueSend(ledQueueHandle, &dataToSend, portMAX_DELAY);

        patternIndex = (patternIndex + 1) % numPatterns;

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
