#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Must be power of two!
#define RING_BUFFER_SIZE 16384

#if (RING_BUFFER_SIZE & (RING_BUFFER_SIZE - 1)) != 0
#error "RING_BUFFER_SIZE must be a power of two"
#endif

typedef struct ring_buffer ring_buffer_t;

typedef void (*ring_buffer_callback_t)(ring_buffer_t *rb, void *context);

struct ring_buffer {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;

    // Callback state
    size_t min_notify_threshold;
    size_t high_watermark_threshold;

    ring_buffer_callback_t on_min_bytes;
    void *min_bytes_context;

    ring_buffer_callback_t on_high_watermark;
    void *high_watermark_context;

    // Internal flags
    bool high_watermark_triggered;
};

void ring_buffer_init(ring_buffer_t *rb);

// Status
bool ring_buffer_is_empty(const ring_buffer_t *rb);
bool ring_buffer_is_full(const ring_buffer_t *rb);
size_t ring_buffer_count(const ring_buffer_t *rb);
size_t ring_buffer_space(const ring_buffer_t *rb);

// Task-context put/get (FreeRTOS protected)
size_t ring_buffer_put_task(ring_buffer_t *rb, const uint8_t *data, size_t len);
size_t ring_buffer_get_task(ring_buffer_t *rb, uint8_t *data, size_t len);

// ISR-context put/get
size_t ring_buffer_put_isr(ring_buffer_t *rb, const uint8_t *data, size_t len);
size_t ring_buffer_get_isr(ring_buffer_t *rb, uint8_t *data, size_t len);

// Callback support
void ring_buffer_set_min_bytes_callback(ring_buffer_t *rb, size_t threshold, ring_buffer_callback_t cb, void *context);
void ring_buffer_set_high_watermark_callback(ring_buffer_t *rb, size_t threshold, ring_buffer_callback_t cb, void *context);
void ring_buffer_check_callbacks(ring_buffer_t *rb);

#endif //_RING_BUFFER_H_
