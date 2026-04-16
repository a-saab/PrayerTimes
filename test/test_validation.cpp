#include <cassert>
#include <string>
#include "PrayerTimes.h"

static void assert_invalid_with_message(const PrayerTimesResult& r, const char* expectedMessage) {
    assert(!r.valid);
    assert(r.errorMessage != nullptr);
    assert(std::string(r.errorMessage) == expectedMessage);
}

static void test_valid_inputs() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    auto r = pt.calculate(3, 4, 2026);

    assert(r.valid);
    assert(r.errorMessage == nullptr);
}

static void test_invalid_latitude() {
    PrayerTimes pt(95.0f, 7.320f, 60);
    auto r = pt.calculate(3, 4, 2026);

    assert_invalid_with_message(r, "Invalid coordinates");
}

static void test_invalid_longitude() {
    PrayerTimes pt(45.737f, 181.0f, 60);
    auto r = pt.calculate(3, 4, 2026);

    assert_invalid_with_message(r, "Invalid coordinates");
}

static void test_invalid_month() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    auto r = pt.calculate(3, 13, 2026);

    assert_invalid_with_message(r, "Invalid date");
}

static void test_invalid_day() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    auto r = pt.calculate(32, 1, 2026);

    assert_invalid_with_message(r, "Invalid date");
}

static void test_non_leap_feb_29_invalid() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    auto r = pt.calculate(29, 2, 2025);

    assert_invalid_with_message(r, "Invalid date");
}

static void test_leap_feb_29_valid() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    auto r = pt.calculate(29, 2, 2024);

    assert(r.valid);
    assert(r.errorMessage == nullptr);
}

static void test_year_too_low_invalid() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    auto r = pt.calculate(1, 1, 1899);

    assert_invalid_with_message(r, "Invalid date");
}

static void test_year_too_high_invalid() {
    PrayerTimes pt(45.737f, 7.320f, 60);
    auto r = pt.calculate(1, 1, 2101);

    assert_invalid_with_message(r, "Invalid date");
}

int main() {
    test_valid_inputs();
    test_invalid_latitude();
    test_invalid_longitude();
    test_invalid_month();
    test_invalid_day();
    test_non_leap_feb_29_invalid();
    test_leap_feb_29_valid();
    test_year_too_low_invalid();
    test_year_too_high_invalid();
    return 0;
}
