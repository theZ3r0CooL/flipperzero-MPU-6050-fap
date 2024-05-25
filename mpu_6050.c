#include <furi.h>
#include <gui/gui.h>
#include <furi_hal_i2c.h>

#define MPU6050_ADDR 0x68

void mpu6050_init() {
    uint8_t wake_up = 0x00;
    furi_hal_i2c_write_mem(
        &furi_hal_i2c_handle_external,
        MPU6050_ADDR,
        0x6B,
        &wake_up,
        1,
        100 // Timeout in milliseconds
    );
}

void mpu6050_read(int16_t* accel_x, int16_t* accel_y, int16_t* accel_z) {
    uint8_t data[6];
    furi_hal_i2c_read_mem(
        &furi_hal_i2c_handle_external,
        MPU6050_ADDR,
        0x3B,
        data,
        6,
        100 // Timeout in milliseconds
    );

    *accel_x = (data[0] << 8) | data[1];
    *accel_y = (data[2] << 8) | data[3];
    *accel_z = (data[4] << 8) | data[5];
}

void mpu6050_display_callback(Canvas* canvas, void* ctx) {
    // Mark the unused parameter to avoid warning
    (void)ctx;

    int16_t accel_x, accel_y, accel_z;
    mpu6050_read(&accel_x, &accel_y, &accel_z);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 10, "MPU-6050 Values:");
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "X: %d", accel_x);
    canvas_draw_str(canvas, 5, 30, buffer);
    snprintf(buffer, sizeof(buffer), "Y: %d", accel_y);
    canvas_draw_str(canvas, 5, 50, buffer);
    snprintf(buffer, sizeof(buffer), "Z: %d", accel_z);
    canvas_draw_str(canvas, 5, 70, buffer);
}

int32_t mpu6050_app(void* p) {
    // Mark the unused parameter to avoid warning
    (void)p;

    mpu6050_init();

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, mpu6050_display_callback, NULL);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while (1) {
        InputEvent event;
        furi_message_queue_get(event_queue, &event, FuriWaitForever);
        if (event.type == InputTypePress && event.key == InputKeyBack) {
            break;
        }
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    return 0;
}

int main() {
    return mpu6050_app(NULL);
}