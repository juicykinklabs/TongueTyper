/**
 * clock_diag.h — ESP32-S3 clock register snapshot for Arduino 3.0+ / IDF 5.x
 *
 * Dumps every register relevant to I2S and SPI clocking:
 *   - SYSTEM: SoC clock source, CPU period, APB pre-divider
 *   - I2S0/I2S1: TX/RX clock-source mux, CLKM divider, BCK divider,
 *                fractional (sigma-delta) divider conf
 *   - SPI2/SPI3: CLOCK reg (CLKDIV_PRE, CLKCNT_N, CLK_EQU_SYSCLK)
 *
 * Usage:
 *   #include "clock_diag.h"
 *
 *   // Before SdFat / any operation you want to observe:
 *   ClockSnapshot before = takeClockSnapshot();
 *   printClockSnapshot("before", before);
 *
 *   sdFat.begin(...);          // <-- operation under test
 *
 *   ClockSnapshot after = takeClockSnapshot();
 *   printClockSnapshot("after", after);
 *   diffClockSnapshots(before, after);
 *
 * Headers needed (all present in pioarduino / ESP-IDF 5.x):
 *   soc/soc.h          — REG_I2S_BASE, REG_SPI_BASE, REG_READ
 *   soc/i2s_reg.h      — I2S_TX_CLKM_CONF_REG, etc.
 *   soc/spi_reg.h      — SPI_CLOCK_REG, SPI_CLKDIV_PRE, etc.
 *   soc/system_reg.h   — SYSTEM_SYSCLK_CONF_REG, SYSTEM_CPU_PER_CONF_REG
 */

#pragma once
#include <Arduino.h>
#include "soc/soc.h"
#include "soc/i2s_reg.h"
#include "soc/spi_reg.h"
#include "soc/system_reg.h"

// ---------------------------------------------------------------------------
// Register address helpers
// ESP32-S3 has I2S0 and I2S1 (index 0/1), and GPSPI2 and GPSPI3 (index 2/3).
// REG_I2S_BASE(i) and REG_SPI_BASE(i) are defined in soc/soc.h.
// ---------------------------------------------------------------------------

// I2S clock registers (per-instance, offset from REG_I2S_BASE(n))
//   +0x24  TX_CONF        — TX enable, reset, MSB/LSB, mono/stereo
//   +0x2C  TX_CONF1       — TX_BCK_DIV_NUM [12:7]
//   +0x34  TX_CLKM_CONF   — TX_CLK_SEL [28:27], TX_CLKM_DIV_NUM [7:0], CLK_EN [29]
//   +0x3C  TX_CLKM_DIV_CONF — fractional sigma-delta divider (X, Y, Z, YN1)
//   +0x28  RX_CONF1       — RX_BCK_DIV_NUM [12:7]
//   +0x30  RX_CLKM_CONF   — RX_CLK_SEL [28:27], RX_CLKM_DIV_NUM [7:0], MCLK_SEL [29]
//   +0x38  RX_CLKM_DIV_CONF — fractional divider

// SPI CLOCK register (offset 0x0C from REG_SPI_BASE(n))
//   CLK_EQU_SYSCLK [31]   — 1 = run at APB speed (ignore dividers)
//   CLKDIV_PRE     [21:18] — pre-divider (0 = /1, N = /(N+1))
//   CLKCNT_N       [17:12] — total period count
//   CLKCNT_H       [11:6]  — high half count
//   CLKCNT_L       [5:0]   — low half count

// SYSTEM registers (DR_REG_SYSTEM_BASE)
//   +0x10  CPU_PER_CONF   — CPUPERIOD_SEL [1:0], PLL_FREQ_SEL [2]
//   +0x60  SYSCLK_CONF    — SOC_CLK_SEL [11:10], CLK_XTAL_FREQ [18:12]

// ---------------------------------------------------------------------------
// Snapshot struct
// ---------------------------------------------------------------------------
struct I2SClockRegs {
    uint32_t tx_conf;           // +0x24
    uint32_t tx_conf1;          // +0x2C  (BCK_DIV)
    uint32_t tx_clkm_conf;      // +0x34  (CLK_SEL, CLKM_DIV_NUM, CLK_EN)
    uint32_t tx_clkm_div_conf;  // +0x3C  (fractional)
    uint32_t rx_conf1;          // +0x28  (BCK_DIV)
    uint32_t rx_clkm_conf;      // +0x30  (CLK_SEL, MCLK_SEL, CLKM_DIV_NUM)
    uint32_t rx_clkm_div_conf;  // +0x38  (fractional)
};

struct SPIClockRegs {
    uint32_t clock_reg;  // +0x0C
};

struct SystemClockRegs {
    uint32_t cpu_per_conf;  // CPUPERIOD_SEL, PLL_FREQ_SEL
    uint32_t sysclk_conf;   // SOC_CLK_SEL, CLK_XTAL_FREQ
};

struct ClockSnapshot {
    I2SClockRegs    i2s[2];   // [0] = I2S0, [1] = I2S1
    SPIClockRegs    spi[2];   // [0] = SPI2, [1] = SPI3
    SystemClockRegs sys;
};

// ---------------------------------------------------------------------------
// Read one snapshot
// ---------------------------------------------------------------------------
inline ClockSnapshot takeClockSnapshot() {
    ClockSnapshot s;

    for (int n = 0; n < 2; n++) {
        uint32_t base = REG_I2S_BASE(n);
        s.i2s[n].tx_conf          = REG_READ(base + 0x24);
        s.i2s[n].tx_conf1         = REG_READ(base + 0x2C);
        s.i2s[n].tx_clkm_conf     = REG_READ(base + 0x34);
        s.i2s[n].tx_clkm_div_conf = REG_READ(base + 0x3C);
        s.i2s[n].rx_conf1         = REG_READ(base + 0x28);
        s.i2s[n].rx_clkm_conf     = REG_READ(base + 0x30);
        s.i2s[n].rx_clkm_div_conf = REG_READ(base + 0x38);
    }

    // SPI index 2 and 3 (GPSPI2, GPSPI3) — REG_SPI_BASE() expects the
    // hardware index (2 or 3), which maps to the two GP buses Arduino
    // refers to as SPI and SPI3/HSPI.
    for (int n = 0; n < 2; n++) {
        s.spi[n].clock_reg = REG_READ(REG_SPI_BASE(n + 2) + 0x0C);
    }

    s.sys.cpu_per_conf = REG_READ(SYSTEM_CPU_PER_CONF_REG);
    s.sys.sysclk_conf  = REG_READ(SYSTEM_SYSCLK_CONF_REG);

    return s;
}

// ---------------------------------------------------------------------------
// Decode helpers — print human-readable field values
// ---------------------------------------------------------------------------
static void _printI2SClockRegs(int idx, const I2SClockRegs& r) {
    // TX_CLK_SEL: 0=none, 1=APLL, 2=CLK160(PLL/2), 3=I2S_MCLK_IN
    uint8_t tx_clk_sel   = (r.tx_clkm_conf >> 27) & 0x3;
    uint8_t tx_clkm_div  = (r.tx_clkm_conf >>  0) & 0xFF;
    bool    tx_clk_en    = (r.tx_clkm_conf >> 29) & 0x1;
    uint8_t tx_bck_div   = (r.tx_conf1     >>  7) & 0x3F;

    uint8_t rx_clk_sel   = (r.rx_clkm_conf >> 27) & 0x3;
    uint8_t rx_clkm_div  = (r.rx_clkm_conf >>  0) & 0xFF;
    bool    mclk_sel     = (r.rx_clkm_conf >> 29) & 0x1;  // 0=TX clk, 1=RX clk on MCLK pin
    uint8_t rx_bck_div   = (r.rx_conf1     >>  7) & 0x3F;

    const char* clkSrcName[] = {"NONE", "APLL", "CLK160", "MCLK_IN"};

    Serial.printf("  I2S%d TX: clk_en=%d  src=%s(%d)  clkm_div=%u  bck_div=%u\n",
        idx, tx_clk_en, clkSrcName[tx_clk_sel & 3], tx_clk_sel, tx_clkm_div, tx_bck_div);
    Serial.printf("  I2S%d RX: mclk_sel=%d  src=%s(%d)  clkm_div=%u  bck_div=%u\n",
        idx, mclk_sel, clkSrcName[rx_clk_sel & 3], rx_clk_sel, rx_clkm_div, rx_bck_div);

    // Fractional divider fields (TX): X[26:18], Y[17:9], Z[8:0], YN1[27]
    uint16_t tx_x   = (r.tx_clkm_div_conf >> 18) & 0x1FF;
    uint16_t tx_y   = (r.tx_clkm_div_conf >>  9) & 0x1FF;
    uint16_t tx_z   = (r.tx_clkm_div_conf >>  0) & 0x1FF;
    bool     tx_yn1 = (r.tx_clkm_div_conf >> 27) & 0x1;
    Serial.printf("  I2S%d TX frac: X=%u Y=%u Z=%u YN1=%d  [raw=0x%08x]\n",
        idx, tx_x, tx_y, tx_z, tx_yn1, r.tx_clkm_div_conf);

    Serial.printf("  I2S%d raw: tx_conf=0x%08x tx_conf1=0x%08x tx_clkm=0x%08x\n",
        idx, r.tx_conf, r.tx_conf1, r.tx_clkm_conf);
    Serial.printf("            rx_conf1=0x%08x rx_clkm=0x%08x\n",
        r.rx_conf1, r.rx_clkm_conf);
}

static void _printSPIClockRegs(int hwIdx, const SPIClockRegs& r) {
    // hwIdx is 2 or 3 (the actual hardware SPI index)
    bool    eq_sysclk  = (r.clock_reg >> 31) & 0x1;
    uint8_t pre        = (r.clock_reg >> 18) & 0xF;
    uint8_t clkcnt_n   = (r.clock_reg >> 12) & 0x3F;
    uint8_t clkcnt_h   = (r.clock_reg >>  6) & 0x3F;
    uint8_t clkcnt_l   = (r.clock_reg >>  0) & 0x3F;

    // SPI frequency = APB / (pre+1) / (clkcnt_n+1)  [when eq_sysclk=0]
    // When eq_sysclk=1 the clock runs at full APB speed.
    if (eq_sysclk) {
        Serial.printf("  SPI%d: CLK_EQU_SYSCLK=1  (running at full APB)  [raw=0x%08x]\n",
            hwIdx, r.clock_reg);
    } else {
        Serial.printf("  SPI%d: pre=%u  N=%u  H=%u  L=%u  => div=/%u  [raw=0x%08x]\n",
            hwIdx, pre, clkcnt_n, clkcnt_h, clkcnt_l,
            (pre + 1) * (clkcnt_n + 1), r.clock_reg);
    }
}

static void _printSystemClockRegs(const SystemClockRegs& r) {
    // SOC_CLK_SEL [11:10]: 0=XTAL, 1=PLL, 2=APLL (not used on S3), 3=reserved
    uint8_t soc_clk_sel   = (r.sysclk_conf >> 10) & 0x3;
    uint8_t xtal_freq_mhz = (r.sysclk_conf >> 12) & 0x7F;

    // CPUPERIOD_SEL [1:0]: 0=80MHz, 1=160MHz, 2=240MHz  (when PLL selected)
    // PLL_FREQ_SEL [2]:    0=320MHz PLL,  1=480MHz PLL
    uint8_t cpu_period_sel = (r.cpu_per_conf >>  0) & 0x3;
    bool    pll_480        = (r.cpu_per_conf >>  2) & 0x1;

    const char* sokClkName[] = {"XTAL", "PLL", "APLL", "RSVD"};
    const char* cpuPeriodName[] = {"80MHz", "160MHz", "240MHz", "?"};

    Serial.printf("  SYSTEM: soc_clk_src=%s(%d)  xtal=%uMHz  cpu_period=%s  pll=%s\n",
        sokClkName[soc_clk_sel], soc_clk_sel,
        xtal_freq_mhz,
        cpuPeriodName[cpu_period_sel & 3],
        pll_480 ? "480MHz" : "320MHz");
    Serial.printf("  SYSTEM raw: cpu_per_conf=0x%08x  sysclk_conf=0x%08x\n",
        r.cpu_per_conf, r.sysclk_conf);
}

// ---------------------------------------------------------------------------
// Print a full snapshot
// ---------------------------------------------------------------------------
inline void printClockSnapshot(const char* label, const ClockSnapshot& s) {
    Serial.printf("\n========== Clock Snapshot [%s] ==========\n", label);
    _printSystemClockRegs(s.sys);
    Serial.println("--- I2S ---");
    for (int n = 0; n < 2; n++) _printI2SClockRegs(n, s.i2s[n]);
    Serial.println("--- SPI ---");
    for (int n = 0; n < 2; n++) _printSPIClockRegs(n + 2, s.spi[n]);
    Serial.println("==========================================");
}

// ---------------------------------------------------------------------------
// Diff two snapshots — only prints registers that changed
// ---------------------------------------------------------------------------
static void _diffU32(const char* name, uint32_t a, uint32_t b) {
    if (a != b) {
        Serial.printf("  CHANGED %-40s  0x%08x -> 0x%08x  (delta 0x%08x)\n",
            name, a, b, b ^ a);
    }
}

inline void diffClockSnapshots(const ClockSnapshot& before, const ClockSnapshot& after) {
    Serial.println("\n========== Clock Snapshot DIFF ==========");
    bool anyChange = false;

    char name[64];
    for (int n = 0; n < 2; n++) {
#define DIFF_I2S(field) \
        snprintf(name, sizeof(name), "I2S%d." #field, n); \
        if (before.i2s[n].field != after.i2s[n].field) { \
            anyChange = true; \
            _diffU32(name, before.i2s[n].field, after.i2s[n].field); \
        }
        DIFF_I2S(tx_conf)
        DIFF_I2S(tx_conf1)
        DIFF_I2S(tx_clkm_conf)
        DIFF_I2S(tx_clkm_div_conf)
        DIFF_I2S(rx_conf1)
        DIFF_I2S(rx_clkm_conf)
        DIFF_I2S(rx_clkm_div_conf)
#undef DIFF_I2S
    }

    for (int n = 0; n < 2; n++) {
        snprintf(name, sizeof(name), "SPI%d.clock_reg", n + 2);
        if (before.spi[n].clock_reg != after.spi[n].clock_reg) {
            anyChange = true;
            _diffU32(name, before.spi[n].clock_reg, after.spi[n].clock_reg);
        }
    }

#define DIFF_SYS(field) \
    snprintf(name, sizeof(name), "SYSTEM." #field); \
    if (before.sys.field != after.sys.field) { \
        anyChange = true; \
        _diffU32(name, before.sys.field, after.sys.field); \
    }
    DIFF_SYS(cpu_per_conf)
    DIFF_SYS(sysclk_conf)
#undef DIFF_SYS

    if (!anyChange) {
        Serial.println("  (no changes detected in monitored registers)");
    }
    Serial.println("==========================================\n");
}