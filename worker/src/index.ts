import { Client } from "pg";

interface Env {
  HYPERDRIVE: Hyperdrive;
}

type JsonObject = Record<string, unknown>;

function toNumber(value: unknown): number | null {
  if (typeof value === "number" && Number.isFinite(value)) {
    return value;
  }
  if (typeof value === "string") {
    const parsed = Number(value);
    if (Number.isFinite(parsed)) {
      return parsed;
    }
  }
  return null;
}

function toInteger(value: unknown): number | null {
  const n = toNumber(value);
  if (n === null) {
    return null;
  }
  return Math.trunc(n);
}

function toBoolean(value: unknown): boolean | null {
  if (typeof value === "boolean") {
    return value;
  }
  if (value === "true") {
    return true;
  }
  if (value === "false") {
    return false;
  }
  return null;
}

function toText(value: unknown): string | null {
  if (typeof value !== "string") {
    return null;
  }
  const trimmed = value.trim();
  return trimmed.length > 0 ? trimmed : null;
}

function jsonResponse(status: number, body: JsonObject): Response {
  return new Response(JSON.stringify(body), {
    status,
    headers: { "content-type": "application/json; charset=utf-8" },
  });
}

async function withClient<T>(env: Env, fn: (client: Client) => Promise<T>): Promise<T> {
  const client = new Client({
    connectionString: env.HYPERDRIVE.connectionString,
  });

  await client.connect();
  try {
    return await fn(client);
  } finally {
    await client.end();
  }
}

export default {
  async fetch(request: Request, env: Env): Promise<Response> {
    const url = new URL(request.url);

    if (!env.HYPERDRIVE?.connectionString) {
      return jsonResponse(500, { error: "HYPERDRIVE binding is not configured" });
    }

    if (request.method === "GET" && url.pathname === "/health") {
      return jsonResponse(200, { ok: true, service: "arduino-morse-metrics" });
    }

    if (request.method !== "POST" || url.pathname !== "/ingest") {
      return jsonResponse(404, { error: "Not found" });
    }

    let rawPayload: JsonObject;
    try {
      const parsed = await request.json();
      if (typeof parsed !== "object" || parsed === null || Array.isArray(parsed)) {
        return jsonResponse(400, { error: "Payload must be a JSON object" });
      }
      rawPayload = parsed as JsonObject;
    } catch {
      return jsonResponse(400, { error: "Invalid JSON" });
    }

    const deviceId = toText(rawPayload.deviceId);
    if (!deviceId) {
      return jsonResponse(400, { error: "deviceId is required" });
    }

    const firmwareVersion = toText(rawPayload.firmwareVersion);
    const wakeCause = toText(rawPayload.wakeCause);
    const resetReason = toText(rawPayload.resetReason);
    const sourceIp = request.headers.get("cf-connecting-ip");
    const userAgent = request.headers.get("user-agent");

    const result = await withClient(env, async (client) => {
      return client.query(
        `INSERT INTO device_metrics (
           device_id,
           firmware_version,
           uptime_seconds,
           total_operation_seconds,
           total_operation_hours,
           battery_voltage,
           battery_percent,
           usb_powered,
           power_cycle_count,
           sleep_cycle_count,
           telemetry_post_count,
           telemetry_failure_count,
           wake_cause,
           reset_reason,
           free_heap_bytes,
           min_free_heap_bytes,
           source_ip,
           user_agent,
           raw_payload
         ) VALUES (
           $1, $2, $3, $4, $5,
           $6, $7, $8, $9, $10,
           $11, $12, $13, $14, $15,
           $16, $17, $18, $19
         )
         RETURNING ts`,
        [
          deviceId,
          firmwareVersion,
          toInteger(rawPayload.uptimeSeconds),
          toInteger(rawPayload.totalOperationSeconds),
          toNumber(rawPayload.totalOperationHours),
          toNumber(rawPayload.batteryVoltage),
          toNumber(rawPayload.batteryPercent),
          toBoolean(rawPayload.usbPowered),
          toInteger(rawPayload.powerCycleCount),
          toInteger(rawPayload.sleepCycleCount),
          toInteger(rawPayload.telemetryPostCount),
          toInteger(rawPayload.telemetryFailureCount),
          wakeCause,
          resetReason,
          toInteger(rawPayload.freeHeapBytes),
          toInteger(rawPayload.minFreeHeapBytes),
          sourceIp,
          userAgent,
          JSON.stringify(rawPayload),
        ],
      );
    });

    const insertedAt = result.rows[0]?.ts ?? null;
    return jsonResponse(202, { ok: true, insertedAt, deviceId });
  },
} satisfies ExportedHandler<Env>;
