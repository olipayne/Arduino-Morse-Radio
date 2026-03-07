import pg from "pg";

const { Client } = pg;

const databaseUrl = process.env.DATABASE_URL;
if (!databaseUrl) {
  throw new Error("DATABASE_URL is required");
}

const client = new Client({ connectionString: databaseUrl, ssl: { rejectUnauthorized: false } });

await client.connect();
try {
  const latest = await client.query(
    `SELECT ts,
            device_id,
            firmware_version,
            battery_percent,
            total_operation_hours,
            power_cycle_count,
            sleep_cycle_count,
            wake_cause
       FROM device_metrics
      ORDER BY ts DESC
      LIMIT 5`,
  );

  const rollup = await client.query(
    `SELECT time_bucket('5 minutes', ts) AS bucket,
            device_id,
            COUNT(*) AS samples,
            AVG(battery_percent) AS avg_battery_percent,
            MAX(total_operation_hours) AS max_operation_hours
       FROM device_metrics
      WHERE ts > NOW() - INTERVAL '24 hours'
      GROUP BY bucket, device_id
      ORDER BY bucket DESC, device_id
      LIMIT 20`,
  );

  console.log("Latest rows:");
  console.table(latest.rows);
  console.log("5-minute rollup (last 24h):");
  console.table(rollup.rows);
} finally {
  await client.end();
}
