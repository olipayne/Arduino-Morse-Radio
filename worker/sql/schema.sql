CREATE EXTENSION IF NOT EXISTS timescaledb;

CREATE TABLE IF NOT EXISTS device_metrics (
  ts TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  device_id TEXT NOT NULL,
  firmware_version TEXT,
  uptime_seconds INTEGER,
  total_operation_seconds INTEGER,
  total_operation_hours DOUBLE PRECISION,
  battery_voltage DOUBLE PRECISION,
  battery_percent DOUBLE PRECISION,
  usb_powered BOOLEAN,
  power_cycle_count INTEGER,
  sleep_cycle_count INTEGER,
  telemetry_post_count INTEGER,
  telemetry_failure_count INTEGER,
  wake_cause TEXT,
  reset_reason TEXT,
  free_heap_bytes INTEGER,
  min_free_heap_bytes INTEGER,
  source_ip TEXT,
  user_agent TEXT,
  raw_payload JSONB NOT NULL
);

SELECT create_hypertable('device_metrics', 'ts', if_not_exists => TRUE);

CREATE INDEX IF NOT EXISTS idx_device_metrics_ts_desc
  ON device_metrics (ts DESC);

CREATE INDEX IF NOT EXISTS idx_device_metrics_device_ts_desc
  ON device_metrics (device_id, ts DESC);

CREATE INDEX IF NOT EXISTS idx_device_metrics_wake_cause_ts_desc
  ON device_metrics (wake_cause, ts DESC);
