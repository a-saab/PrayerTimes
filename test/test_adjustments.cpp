#include <cassert>
#include <cmath>
#include "PrayerTimes.h"

static bool approx_eq(float a, float b, float tol = 0.01f) {
    return std::fabs(a - b) <= tol;
}

static void test_manual_adjustments_apply_to_intended_prayers() {
    PrayerTimes base(45.737f, 7.320f, 60);
    base.setCustomMethod(18.0f, 18.0f, false, 0);

    PrayerTimes adj(45.737f, 7.320f, 60);
    adj.setCustomMethod(18.0f, 18.0f, false, 0);
    adj.setAdjustments(2, 3, 4, 5, 6, 7);

    auto a = base.calculateWithOffset(3, 4, 2026, 60);
    auto b = adj.calculateWithOffset(3, 4, 2026, 60);

    assert(a.valid && b.valid);
    assert(a.errorMessage == nullptr);
    assert(b.errorMessage == nullptr);

    assert(approx_eq(b.fajr    - a.fajr,    2.0f));
    assert(approx_eq(b.sunrise - a.sunrise, 3.0f));
    assert(approx_eq(b.dhuhr   - a.dhuhr,   4.0f));
    assert(approx_eq(b.asr     - a.asr,     5.0f));
    assert(approx_eq(b.maghrib - a.maghrib, 6.0f));
    assert(approx_eq(b.isha    - a.isha,    7.0f));
}

static void test_imsak_follows_fajr_adjustment() {
    PrayerTimes base(45.737f, 7.320f, 60);
    base.setCustomMethod(18.0f, 18.0f, false, 0);

    PrayerTimes adj(45.737f, 7.320f, 60);
    adj.setCustomMethod(18.0f, 18.0f, false, 0);
    adj.setAdjustments(4, 0, 0, 0, 0, 0);

    auto a = base.calculateWithOffset(3, 4, 2026, 60);
    auto b = adj.calculateWithOffset(3, 4, 2026, 60);

    assert(a.valid && b.valid);
    assert(a.errorMessage == nullptr);
    assert(b.errorMessage == nullptr);

    assert(approx_eq(b.imsak - a.imsak, 4.0f));
    assert(approx_eq((b.fajr - b.imsak), (a.fajr - a.imsak)));
}

static void test_duha_follows_sunrise_adjustment() {
    PrayerTimes base(45.737f, 7.320f, 60);
    base.setCustomMethod(18.0f, 18.0f, false, 0);

    PrayerTimes adj(45.737f, 7.320f, 60);
    adj.setCustomMethod(18.0f, 18.0f, false, 0);
    adj.setAdjustments(0, 5, 0, 0, 0, 0);

    auto a = base.calculateWithOffset(3, 4, 2026, 60);
    auto b = adj.calculateWithOffset(3, 4, 2026, 60);

    assert(a.valid && b.valid);
    assert(a.errorMessage == nullptr);
    assert(b.errorMessage == nullptr);

    assert(approx_eq(b.duha - a.duha, 5.0f));
}

static void test_dst_shifts_everything_equally() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    pt.setCustomMethod(18.0f, 18.0f, false, 0);

    auto a = pt.calculateWithOffset(3, 4, 2026, 0);
    auto b = pt.calculateWithOffset(3, 4, 2026, 60);

    assert(a.valid && b.valid);
    assert(a.errorMessage == nullptr);
    assert(b.errorMessage == nullptr);

    assert(approx_eq(b.fajr    - a.fajr,    60.0f));
    assert(approx_eq(b.sunrise - a.sunrise, 60.0f));
    assert(approx_eq(b.dhuhr   - a.dhuhr,   60.0f));
    assert(approx_eq(b.asr     - a.asr,     60.0f));
    assert(approx_eq(b.maghrib - a.maghrib, 60.0f));
    assert(approx_eq(b.isha    - a.isha,    60.0f));
}

int main() {
    test_manual_adjustments_apply_to_intended_prayers();
    test_imsak_follows_fajr_adjustment();
    test_duha_follows_sunrise_adjustment();
    test_dst_shifts_everything_equally();
    return 0;
}
