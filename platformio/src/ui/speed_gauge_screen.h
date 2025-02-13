#pragma once

#include "lvgl.h"
#include "misc/circular_buffer.h"
#include "screen_manager.h"
#include "ui.h"

class SpeedGaugeScreen : public screen_manager::Screen {
 public:
  SpeedGaugeScreen();
  virtual void setup(uint8_t screen_num) override;
  virtual void on_load() override;
  virtual void on_unload() override;
  virtual void loop() override;
  virtual void on_event(ui_events::UiEventId ui_event_id) override;

 private:
  ui::Gauge gauge_;
  ui::Label speed_field_;
  static constexpr int kMaxStepSamples = 10;
  CircularBuffer<int32_t, kMaxStepSamples> recent_full_step_samples_;

  uint8_t label_update_divider_ = 0;
  bool alternative_scale_ = false;
};