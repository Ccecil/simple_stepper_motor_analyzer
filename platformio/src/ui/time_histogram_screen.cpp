#include "time_histogram_screen.h"

#include "acquisition/analyzer.h"
#include "ui.h"

static constexpr uint32_t kUpdateIntervalMillis = 200;

static const ui::ChartAxisConfig kXAxisConfig_2500ma{
    .range = {.min = 0, .max = 2000},  // ignored
    .labels = "0\n500\n1000\n1500\n2000",
    .num_ticks = 5,
    .dividers = 3};

static const ui::ChartAxisConfig kAxisYConfig{
    .range = {.min = 0, .max = 100},
    .labels = "100%\n75%\n50%\n25%\n0",
    .num_ticks = 5,
    .dividers = 3};

TimeHistogramScreen::TimeHistogramScreen(){};

void TimeHistogramScreen::setup(uint8_t screen_num) {
  ui::create_screen(&screen_);
  ui::create_page_elements(screen_, "TIME BY STEPS/SEC", screen_num, nullptr);
  ui::create_histogram(screen_, analyzer::kNumHistogramBuckets, kXAxisConfig_2500ma,
                       kAxisYConfig, &histogram_);
};

void TimeHistogramScreen::on_load() {
  // Force display update on first loop.
  display_update_elapsed_.set(kUpdateIntervalMillis + 1);
};

void TimeHistogramScreen::on_unload(){};

void TimeHistogramScreen::on_event(ui_events::UiEventId ui_event_id) {
  switch (ui_event_id) {
    case ui_events::UI_EVENT_RESET:
      analyzer::reset_state();
      break;

    default:
      break;
  }
}

void TimeHistogramScreen::loop() {
  // We update at a fixed rate.
  if (display_update_elapsed_.elapsed_millis() < kUpdateIntervalMillis) {
    return;
  }
  display_update_elapsed_.reset();

  // Sample data and update screen.
  const analyzer::State* state = analyzer::sample_state();

  // Find max ticks in a bucket.
  uint64_t max_ticks = state->buckets[0].total_ticks_in_steps;
  for (int i = 1; i < analyzer::kNumHistogramBuckets; i++) {
    uint64_t ticks = state->buckets[i].total_ticks_in_steps;
    if (ticks > max_ticks) {
      max_ticks = ticks;
    }
  }

  // Update all the histogram points.
  for (int i = 0; i < analyzer::kNumHistogramBuckets; i++) {
    uint64_t ticks = state->buckets[i].total_ticks_in_steps;
    // Scale the value to [0, 100];
    uint16_t val = max_ticks > 0 ? ((ticks * 100) / max_ticks) : 0;

    // Indicate non zero buckets. Even if it's a tiny fraction.
    if (ticks > 0 && val == 0) {
      val = 1;
    }

    // histogram_.lv_series->points[i] = x++ % 100;
    histogram_.lv_series->points[i] = val;
  }

  lv_chart_refresh(histogram_.lv_chart);
}