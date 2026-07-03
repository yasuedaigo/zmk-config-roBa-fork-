#include <stdint.h>
#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <raw_hid/raw_hid.h>

// report[0] = protocol version
// report[1] = event type (1 = layer changed)
// report[2] = layer number
// report[3] = state (1 = on/enter, 0 = off/leave)
// report[4..31] = reserved

static int layer_hid_listener(const zmk_event_t *eh) {
    const struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    static uint8_t report[32] = {0};
    report[0] = 1;
    report[1] = 1;
    report[2] = (uint8_t)ev->layer;
    report[3] = ev->state ? 1 : 0;

    struct raw_hid_sent_event hid_event = {
        .length = sizeof(report),
        .data = report,
    };

    raise_raw_hid_sent_event(hid_event);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(layer_hid, layer_hid_listener);
ZMK_SUBSCRIPTION(layer_hid, zmk_layer_state_changed);