#include <zephyr/kernel.h>
#include <lvgl.h>

#include <zmk/display.h>
#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zmk/hid_indicators_types.h>

#define NUM_LOCK_MASK BIT(0)

static lv_obj_t *num_lock_label;

struct num_lock_state {
    bool enabled;
};

static struct num_lock_state num_lock_get_state(const zmk_event_t *event) {
    ARG_UNUSED(event);

    return (struct num_lock_state){
        .enabled =
            (zmk_hid_indicators_get_current_profile() &
             NUM_LOCK_MASK) != 0,
    };
}

static void num_lock_update(struct num_lock_state state) {
    if (num_lock_label != NULL) {
        lv_label_set_text(
            num_lock_label,
            state.enabled ? "N:ON" : "N:OFF"
        );
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(
    keyball61_num_lock,
    struct num_lock_state,
    num_lock_update,
    num_lock_get_state
)

ZMK_SUBSCRIPTION(
    keyball61_num_lock,
    zmk_hid_indicators_changed
);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(
        &battery_status_widget,
        screen
    );

    lv_obj_align(
        zmk_widget_battery_status_obj(
            &battery_status_widget
        ),
        LV_ALIGN_TOP_RIGHT,
        0,
        0
    );
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(
        &output_status_widget,
        screen
    );

    lv_obj_align(
        zmk_widget_output_status_obj(
            &output_status_widget
        ),
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(
        &layer_status_widget,
        screen
    );

    lv_obj_align(
        zmk_widget_layer_status_obj(
            &layer_status_widget
        ),
        LV_ALIGN_BOTTOM_LEFT,
        0,
        0
    );
#endif

    num_lock_label = lv_label_create(screen);
    lv_label_set_text(num_lock_label, "N:OFF");

    lv_obj_align(
        num_lock_label,
        LV_ALIGN_TOP_MID,
        0,
        0
    );

    keyball61_num_lock_init();

    return screen;
}