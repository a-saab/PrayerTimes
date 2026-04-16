#include <cassert>
#include <cmath>
#include "PrayerTimes.h"

static bool approx_eq(float a, float b, float tol = 0.1f) {
    return std::fabs(a - b) <= tol;
}

static void test_hanafi_asr_is_later_than_shafii() {
    PrayerTimes shafii(45.737f, 7.320f, 60);
    shafii.setAsrMethod(SHAFII);
    shafii.setCustomMethod(18.0f, 18.0f, false, 0);

    PrayerTimes hanafi(45.737f, 7.320f, 60);
    hanafi.setAsrMethod(HANAFI);
    hanafi.setCustomMethod(18.0f, 18.0f, false, 0);

    auto s = shafii.calculateWithOffset(3, 4, 2026, 60);
    auto h = hanafi.calculateWithOffset(3, 4, 2026, 60);

    assert(s.valid && h.valid);
    assert(s.errorMessage == nullptr);
    assert(h.errorMessage == nullptr);
    assert(h.asr > s.asr);
}

static void test_larger_fajr_angle_makes_fajr_earlier() {
    PrayerTimes p1(45.737f, 7.320f, 60);
    p1.setCustomMethod(15.0f, 18.0f, false, 0);

    PrayerTimes p2(45.737f, 7.320f, 60);
    p2.setCustomMethod(18.0f, 18.0f, false, 0);

    auto r1 = p1.calculateWithOffset(3, 4, 2026, 60);
    auto r2 = p2.calculateWithOffset(3, 4, 2026, 60);

    assert(r1.valid && r2.valid);
    assert(r1.errorMessage == nullptr);
    assert(r2.errorMessage == nullptr);
    assert(r2.fajr < r1.fajr);
}

static void test_larger_isha_angle_makes_isha_later() {
    PrayerTimes p1(45.737f, 7.320f, 60);
    p1.setCustomMethod(18.0f, 15.0f, false, 0);

    PrayerTimes p2(45.737f, 7.320f, 60);
    p2.setCustomMethod(18.0f, 18.0f, false, 0);

    auto r1 = p1.calculateWithOffset(3, 4, 2026, 60);
    auto r2 = p2.calculateWithOffset(3, 4, 2026, 60);

    assert(r1.valid && r2.valid);
    assert(r1.errorMessage == nullptr);
    assert(r2.errorMessage == nullptr);
    assert(r2.isha > r1.isha);
}

static void test_interval_isha_is_fixed_minutes_after_maghrib() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    pt.setCustomMethod(18.0f, 18.0f, true, 90);

    auto r = pt.calculateWithOffset(3, 4, 2026, 60);

    assert(r.valid);
    assert(r.errorMessage == nullptr);
    assert(r.isha > r.maghrib);
    assert(approx_eq(r.isha - r.maghrib, 90.0f));
}

int main() {
    test_hanafi_asr_is_later_than_shafii();
    test_larger_fajr_angle_makes_fajr_earlier();
    test_larger_isha_angle_makes_isha_later();
    test_interval_isha_is_fixed_minutes_after_maghrib();
    return 0;
}
