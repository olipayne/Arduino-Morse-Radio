import fs from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";
import pg from "pg";

const { Client } = pg;

const databaseUrl = process.env.DATABASE_URL;
if (!databaseUrl) {
  throw new Error("DATABASE_URL is required");
}

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const schemaPath = path.resolve(__dirname, "../sql/schema.sql");
const schemaSql = await fs.readFile(schemaPath, "utf8");

const client = new Client({ connectionString: databaseUrl, ssl: { rejectUnauthorized: false } });

await client.connect();
try {
  await client.query(schemaSql);
  console.log("Schema setup complete.");
} finally {
  await client.end();
}
