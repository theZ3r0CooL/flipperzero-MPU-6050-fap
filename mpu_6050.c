#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <furi_hal_i2c.h>
#include <gui/gui.h>
#include <input/input.h>
#include <locale/locale.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define TAG_MPU "mpu_6050_app"

#define MPU6050_ADDR 0x68
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_PWR_MGMT_1 0x6B

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} MpuData;

typedef struct {
    FuriMessageQueue* queue;
    MpuData* data;
    ViewPort* view_port;
} MpuContext;

static void mpu6050_init() {
    uint8_t buffer[2];
    buffer[0] = MPU6050_REG_PWR_MGMT_1;
    buffer[1] = 0x00;
    furi_hal_i2c_write(FuriHalI2cBusI2c1, MPU6050_ADDR, buffer, 2);
}

static void mpu6050_read_data(MpuData* data) {
    uint8_t buffer[14];
    furi_hal_i2c_read_reg(
        FuriHalI2cBusI2c1, MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, buffer, sizeof(buffer));

    data->accel_x = (buffer[0] << 8) | buffer[1];
    data->accel_y = (buffer[2] << 8) | buffer[3];
    data->accel_z = (buffer[4] << 8) | buffer[5];
    data->gyro_x = (buffer[8] << 8) | buffer[9];
    data->gyro_y = (buffer[10] << 8) | buffer[11];
    data->gyro_z = (buffer[12] << 8) | buffer[13];
}

static void render_callback(Canvas* canvas, void* ctx) {
    MpuContext* mpuContext = (MpuContext*)ctx;
    MpuData* mpuData = mpuContext->data;

    mpu6050_read_data(mpuData);

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Accel X: %d", mpuData->accel_x);
    canvas_draw_str_aligned(canvas, 1, 1, AlignLeft, AlignTop, buffer);
    snprintf(buffer, sizeof(buffer), "Accel Y: %d", mpuData->accel_y);
    canvas_draw_str_aligned(canvas, 1, 10, AlignLeft, AlignTop, buffer);
    snprintf(buffer, sizeof(buffer), "Accel Z: %d", mpuData->accel_z);
    canvas_draw_str_aligned(canvas, 1, 20, AlignLeft, AlignTop, buffer);
    snprintf(buffer, sizeof(buffer), "Gyro X: %d", mpuData->gyro_x);
    canvas_draw_str_aligned(canvas, 1, 30, AlignLeft, AlignTop, buffer);
    snprintf(buffer, sizeof(buffer), "Gyro Y: %d", mpuData->gyro_y);
    canvas_draw_str_aligned(canvas, 1, 40, AlignLeft, AlignTop, buffer);
    snprintf(buffer, sizeof(buffer), "Gyro Z: %d", mpuData->gyro_z);
    canvas_draw_str_aligned(canvas, 1, 50, AlignLeft, AlignTop, buffer);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    // Handle input events if necessary
}

int32_t mpu_6050_app(void* p) {
    UNUSED(p);

    MpuContext* mpuContext = malloc(sizeof(MpuContext));
    mpuContext->data = malloc(sizeof(MpuData));
    mpuContext->queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    mpu6050_init();

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, mpuContext);
    view_port_input_callback_set(view_port, input_callback, mpuContext->queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;
    bool processing = true;
    do {
        if(furi_message_queue_get(mpuContext->queue, &event, FuriWaitForever) == FuriStatusOk) {
            // Process input events if necessary
        } else {
            processing = false;
        }
    } while(processing);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(mpuContext->queue);
    free(mpuContext->data);
    free(mpuContext);

    return 0;
}