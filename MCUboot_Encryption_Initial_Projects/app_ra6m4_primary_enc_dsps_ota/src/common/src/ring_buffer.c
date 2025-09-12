#include "ring_buffer.h"
#include "FreeRTOS.h"
#include "task.h"

#define MASK (RING_BUFFER_SIZE - 1)

void ring_buffer_init(ring_buffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->min_notify_threshold = 0;
    rb->high_watermark_threshold = 0;
    rb->on_min_bytes = NULL;
    rb->on_high_watermark = NULL;
    rb->min_bytes_context = NULL;
    rb->high_watermark_context = NULL;
    rb->high_watermark_triggered = false;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb) {
    return rb->head == rb->tail;
}

bool ring_buffer_is_full(const ring_buffer_t *rb) {
    return ((rb->head + 1) & MASK) == rb->tail;
}

size_t ring_buffer_count(const ring_buffer_t *rb) {
    return (rb->head - rb->tail) & MASK;
}

size_t ring_buffer_space(const ring_buffer_t *rb) {
    return (RING_BUFFER_SIZE - 1) - ring_buffer_count(rb);
}

// ===========================
// Task Context Functions
// ===========================

size_t ring_buffer_put_task(ring_buffer_t *rb, const uint8_t *data, size_t len) {
    size_t i = 0;
    taskENTER_CRITICAL();
    while (i < len && !ring_buffer_is_full(rb)) {
        rb->buffer[rb->head] = data[i++];
        rb->head = (rb->head + 1) & MASK;
    }
    taskEXIT_CRITICAL();
    return i;
}

size_t ring_buffer_get_task(ring_buffer_t *rb, uint8_t *data, size_t len) {
    size_t i = 0;
    taskENTER_CRITICAL();
    while (i < len && !ring_buffer_is_empty(rb)) {
        data[i++] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) & MASK;
    }

    // Reset watermark flag if usage drops below
    if (ring_buffer_count(rb) < rb->high_watermark_threshold) {
        rb->high_watermark_triggered = false;
    }
    taskEXIT_CRITICAL();
    return i;
}

// ===========================
// ISR Context Functions
// ===========================

size_t ring_buffer_put_isr(ring_buffer_t *rb, const uint8_t *data, size_t len) {
    size_t i = 0;
    while (i < len && !ring_buffer_is_full(rb)) {
        rb->buffer[rb->head] = data[i++];
        rb->head = (rb->head + 1) & MASK;
    }
    return i;
}

size_t ring_buffer_get_isr(ring_buffer_t *rb, uint8_t *data, size_t len) {
    size_t i = 0;
    while (i < len && !ring_buffer_is_empty(rb)) {
        data[i++] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) & MASK;
    }

    if (ring_buffer_count(rb) < rb->high_watermark_threshold) {
        rb->high_watermark_triggered = false;
    }
    return i;
}

// ===========================
// Callback Support
// ===========================

void ring_buffer_set_min_bytes_callback(ring_buffer_t *rb, size_t threshold, ring_buffer_callback_t cb, void *context) {
    rb->min_notify_threshold = threshold;
    rb->on_min_bytes = cb;
    rb->min_bytes_context = context;
}

void ring_buffer_set_high_watermark_callback(ring_buffer_t *rb, size_t threshold, ring_buffer_callback_t cb, void *context) {
    rb->high_watermark_threshold = threshold;
    rb->on_high_watermark = cb;
    rb->high_watermark_context = context;
    rb->high_watermark_triggered = false;
}

void ring_buffer_check_callbacks(ring_buffer_t *rb) {
    size_t count = ring_buffer_count(rb);

    if (rb->on_min_bytes && count >= rb->min_notify_threshold) {
        rb->on_min_bytes(rb, rb->min_bytes_context);
    }

    if (rb->on_high_watermark && count >= rb->high_watermark_threshold && !rb->high_watermark_triggered) {
        rb->high_watermark_triggered = true;
        rb->on_high_watermark(rb, rb->high_watermark_context);
    }
}
