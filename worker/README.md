# Arduino Morse Metrics Worker

## 1) Install dependencies

```bash
npm install
```

## 2) Create Hyperdrive binding

```bash
npx wrangler hyperdrive create arduino-morse-metrics-db --connection-string="${DATABASE_URL}"
```

Copy the returned Hyperdrive ID into `wrangler.toml`:

```toml
[[hyperdrive]]
binding = "HYPERDRIVE"
id = "<your-hyperdrive-id>"
```

## 3) Initialize schema

```bash
DATABASE_URL="..." npm run setup-schema
```

## 4) Deploy

```bash
npm run deploy
```

## 5) Test ingest endpoint

```bash
curl -X POST "https://<worker-domain>/ingest" \
  -H "content-type: application/json" \
  -d '{
    "deviceId":"TEST-DEVICE-01",
    "firmwareVersion":"v3.0.2",
    "uptimeSeconds":300,
    "totalOperationSeconds":7200,
    "totalOperationHours":2,
    "batteryVoltage":3.98,
    "batteryPercent":74,
    "usbPowered":true,
    "powerCycleCount":12,
    "sleepCycleCount":22,
    "telemetryPostCount":8,
    "telemetryFailureCount":1,
    "wakeCause":"timer",
    "resetReason":"deepsleep",
    "freeHeapBytes":201000,
    "minFreeHeapBytes":190000
  }'
```

## 6) Verify query shape

```bash
DATABASE_URL="..." npm run query-check
```
