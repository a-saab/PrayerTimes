#include <cassert>
#include <vector>
#include "PrayerTimes.h"

struct Case {
    float lat;
    float lon;
    int tz;
    int day;
    int month;
    int year;
    int dst;
};

static void assert_strict_order(const PrayerTimesResult& r) {
    assert(r.valid);
    assert(r.imsak < r.fajr);
    assert(r.fajr < r.sunrise);
    assert(r.sunrise < r.dhuhr);
    assert(r.dhuhr < r.asr);
    assert(r.asr < r.maghrib);
    assert(r.maghrib < r.isha);
    assert(r.sunrise < r.duha);
}

static void test_multiple_cities_and_dates() {
    std::vector<Case> cases = {
        {45.737f,   7.320f,   60,  3,  4, 2026, 60}, // Aosta
        {21.4225f, 39.8262f, 180, 10, 1, 2026,  0}, // Makkah
        {24.8607f, 67.0011f, 300, 15, 7, 2026,  0}, // Karachi
        {51.5074f, -0.1278f,   0,  1, 12, 2026,  0}, // London winter
    };

    for (const auto& c : cases) {
        PrayerTimes pt(c.lat, c.lon, c.tz);
        pt.setAsrMethod(HANAFI);
        pt.setCustomMethod(18.0f, 18.0f, false, 0);

        auto r = pt.calculateWithOffset(c.day, c.month, c.year, c.dst);
        assert_strict_order(r);
    }
}

int main() {
    test_multiple_cities_and_dates();
    return 0;
}
