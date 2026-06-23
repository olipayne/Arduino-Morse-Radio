# Microcontroller Recommendation — Morse Radio

_Research date: 2026-06-23. Goal: the best ESP (or similar) for this project —
**affordable**, **always in stock**, and able to do everything this project
does in the **most power‑efficient** way._

## TL;DR

| Priority | Pick | Why |
|---|---|---|
| **Best all‑around (recommended)** | **ESP32‑C6** — board: **Adafruit ESP32‑C6 Feather** | Newest low‑power RISC‑V part with a dedicated low‑power core, ~7 µA deep sleep, WiFi (no BLE needed here), cheaper silicon than the S3, ubiquitous stock. The Feather board is a near drop‑in for the current FeatherS3 form factor (LiPo charge + fuel gauge + NeoPixel). |
| **Cheapest / maximum availability** | **ESP32‑C3** — bare module `ESP32‑C3‑MINI‑1` (~$1.15) | The most mature, most‑stocked, lowest‑cost WiFi RISC‑V part. Great if you spin your own PCB and don't need a low‑power coprocessor. Slightly fewer usable GPIO. |
| **Zero code change** | Stay on **ESP32‑S3** (current FeatherS3) | Works today, but it is the **most expensive and least power‑efficient** option for what this project actually uses (dual core + PSRAM are both wasted here). |

**Bottom line:** the project is over‑specced on the ESP32‑S3. It uses **WiFi only
(no Bluetooth), no PSRAM, and never uses the second CPU core.** A single‑core
RISC‑V part (C3 or C6) does everything this firmware needs, costs less, and draws
less. Of those, the **ESP32‑C6** is the best long‑term pick for "most power
efficient"; the **ESP32‑C3** is the best pick for "cheapest + always in stock."

---

## What the project actually requires (from the code)

Audited from `src/` — these are the real hardware needs, not the headline specs:

| Capability | Used? | Notes |
|---|---|---|
| **WiFi** | ✅ Required | OTA updates (`ElegantOTA`) + metrics POST. `WiFiManager.cpp` is the largest file. |
| **Bluetooth / BLE** | ❌ Not used | `PowerManager::begin()` calls `btStop()` — radio is explicitly disabled. |
| **PSRAM** | ❌ Not used | `-DBOARD_HAS_PSRAM` is set in `platformio.ini`, but there are **no** `ps_malloc`/`heap_caps`/PSRAM allocations anywhere in `src/`. Wasted on this firmware. |
| **2nd CPU core** | ❌ Not used | One FreeRTOS LED task + main loop; no core‑pinning, no parallel compute. Single core is plenty. |
| **GPIO** | 18 signals | See pin list below — this is the **main sizing constraint**. Rules out 11‑pin boards (XIAO / QT Py). |
| **ADC** | 2 channels | Tuning pot + volume pot, 12‑bit, 11 dB atten. |
| **LEDC PWM** | **4 channels actually attached** | Despite 8 channel *numbers* defined in `Config.h`, only **speaker, decode, carrier, power‑LED** are ever `ledcSetup`/`ledcAttachPin`'d. The LW/MW/SW LEDs use plain `digitalWrite`. Fits the 6‑channel LEDC on C3/C6. |
| **Deep sleep + wake on button** | ✅ Required | `esp_sleep_enable_ext0_wakeup()` on the power switch GPIO; RTC‑GPIO hold. ⚠️ See migration note — `ext0` is Xtensa‑only. |
| **LiPo battery mgmt** | ✅ Required | Battery voltage, VBUS detect, 2nd LDO control, NeoPixel — currently via the FeatherS3‑specific `UMS3` library. |

**18 GPIO signals in use** (`src/Config.h`): power switch, LW/MW band switches,
slow/med decode switches, WiFi button, 2× pots (ADC), backlight, power LED,
LW/MW/SW LEDs, lock LED, carrier PWM, decode PWM, morse LEDs, speaker — plus the
onboard NeoPixel and battery sense.

---

## Power efficiency — the honest picture

There are two very different power states here, and they matter differently:

1. **In use (most of the time): device is awake**, generating audio via PWM,
   reading pots, driving LEDs, WiFi off. Active current is dominated by the CPU
   plus the analog/LED load. All three candidates sit around **~20–24 mA** at
   3.3 V with WiFi off. The lever that actually moves this number is **clock
   speed**, not chip family: the radio workload is trivial, so a single‑core
   C3/C6 can run at 80 MHz and draw less than a dual‑core S3 at 240 MHz.
   → **C3/C6 win modestly here; the analog front‑end and speaker are the real
   battery drain, so don't expect a dramatic in‑use change from the chip alone.**

2. **Off / idle (deep sleep):** this is where chip choice is clear‑cut.
   - ESP32‑C3: ~5 µA (chip), board‑dependent
   - ESP32‑C6: **~7 µA** with low‑power memory retained
   - ESP32‑S3: ~8 µA (module)

   All three are excellent. The **board's regulators and fuel gauge** usually
   dominate real standby current more than the SoC — which is why the *board*
   choice (below) matters as much as the *chip*.

**Conclusion:** for "most power efficient," prefer a **single‑core RISC‑V part
(C6 or C3) you can downclock**, on a **board with low‑quiescent regulators**. The
ESP32‑C6 additionally has a separate **low‑power (LP) core @ ~20 MHz** that could
later offload the always‑on input/LED polling for further savings.

---

## Cost & availability (June 2026, LCSC)

| Part | ~Unit price | Stock |
|---|---|---|
| ESP32‑C3 (QFN) | **$1.15** | Deep, always available |
| ESP32‑C3‑MINI‑1 module | ~$1.6–2.0 | Deep |
| ESP32‑C6 (QFN) | **$2.03** | Deep |
| ESP32‑C6‑MINI‑1‑N4 module | ~$2.8 | Deep |
| ESP32‑S3 (current) | higher | Good |

All are first‑party Espressif parts with long lifecycles — none are at
availability risk. C3 is the volume leader and the safest "always in stock" bet;
C6 is newer but already stocked everywhere.

---

## Recommended boards (keep the existing Feather form factor)

The current code leans on a Feather‑class board with onboard **LiPo charging,
battery monitoring, a 2nd switchable LDO, and a NeoPixel**. To keep migration
small, pick a board that provides the same:

- **Adafruit ESP32‑C6 Feather (PID 5933)** — *recommended.* Same Feather
  footprint, LiPo charge + **MAX17048 fuel gauge** (more accurate than the
  FeatherS3's voltage divider), NeoPixel, and a **second low‑quiescent LDO** for
  ultra‑low‑power — i.e. it replaces everything `UMS3` gives you today. Lots of
  broken‑out GPIO.
- **ePulse Feather C6** — a Feather‑form C6 board explicitly optimized for very
  low deep‑sleep current, if battery standby is the top priority.
- **Adafruit QT Py ESP32‑C3 / Seeed XIAO C3/C6** — **not suitable as‑is**: only
  ~11 GPIO broken out, fewer than the 18 this firmware drives. Would require
  cutting features or adding a GPIO expander.

---

## Migration effort (C6/C3) — what changes in firmware

Contained, not trivial. Budget roughly a day:

1. **Battery/power abstraction:** replace the FeatherS3‑specific `UMS3` library
   (`src/PowerManager.cpp`) with the target board's equivalents — e.g. Adafruit
   `MAX17048` library for battery %, a GPIO read for VBUS, `digitalWrite` for the
   2nd‑LDO enable, and Adafruit NeoPixel. This is the bulk of the work.
2. **Deep‑sleep wake:** `esp_sleep_enable_ext0_wakeup()` is **Xtensa‑only
   (ESP32/S2/S3)**. On C3/C6 switch to `esp_deep_sleep_enable_gpio_wakeup()`
   (C6 LP‑GPIO0–7) — pick the power‑switch pin from a wake‑capable GPIO.
3. **Pin remap:** update `namespace Pins` in `src/Config.h` to the new board's
   GPIO numbers (C6 ADC is limited to GPIO0–6 — assign the two pots there).
4. **`platformio.ini`:** change `board` (e.g. `adafruit_feather_esp32c6`), drop
   the unused `-DBOARD_HAS_PSRAM` / `-mfix-esp32-psram-cache-issue` flags, and
   drop the `UMS3` dependency.
5. **PWM:** no change needed — only 4 LEDC channels are used (≤ 6 available).

If you'd rather not touch any code right now, the **FeatherS3 (S3)** keeps
working — just know you're paying for a dual core + PSRAM the firmware never uses.

---

## Sources

- [Seeed: XIAO ESP32 S3/C3/C6 comparison](https://www.seeedstudio.com/blog/2026/01/14/xiao-esp32-s3-c3-c6-comparison/)
- [Seeed forum: sleep current C6/S3/C3](https://forum.seeedstudio.com/t/comparison-of-sleep-currents-for-xiao-esp32c6-s3-and-c3/276444)
- [ESP32-C6 datasheet (Espressif)](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf)
- [ESP-IDF: ESP32-C6 module current measurement](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-guides/current-consumption-measurement-modules.html)
- [ESP-IDF: ESP32-C3 module current measurement](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-guides/current-consumption-measurement-modules.html)
- [ESP32-C6 GPIO/ADC/LEDC reference](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/peripherals/gpio.html)
- [Adafruit ESP32-C6 Feather (product 5933)](https://www.adafruit.com/product/5933)
- [Adafruit ESP32-C6 Feather — MAX17048 battery monitor](https://learn.adafruit.com/adafruit-esp32-c6-feather/battery-monitor-max17048)
- [LCSC: ESP32-C3 price/stock](https://lcsc.com/product-detail/Microcontroller-Units-MCUs-MPUs-SOCs_Espressif-Systems-ESP32-C3_C2838500.html)
- [LCSC: ESP32-C6 price/stock](https://www.lcsc.com/product-detail/Microcontrollers-MCU-MPU-SOC_Espressif-Systems-ESP32-C6_C5364646.html)
- [ePulse Feather C6 (low-power Feather)](https://www.cnx-software.com/2024/03/06/epulse-feather-c6-esp32-c6-development-board-adafruit-feather-lipo-battery/)
