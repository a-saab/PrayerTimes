#include <cassert>
#include <cmath>
#include <vector>
#include "PrayerTimes.h"

struct RegressionCase {
    const char* name;
    float lat;
    float lon;
    int tz;
    int day;
    int month;
    int year;
    int dst;
    AsrMethod asrMethod;
    float fajrAngle;
    float ishaAngle;
    bool ishaIsInterval;
    int ishaMinutes;

    float fajr;
    float sunrise;
    float dhuhr;
    float asr;
    float maghrib;
    float isha;

    float tolFajr;
    float tolSunrise;
    float tolDhuhr;
    float tolAsr;
    float tolMaghrib;
    float tolIsha;
};

static bool within(float actual, float expected, float tol) {
    return std::fabs(actual - expected) <= tol;
}

static std::vector<RegressionCase> cases = {
    {
        "Aosta_2026_04_03_Hanafi_18_18",
        45.7f, 7.3f, 60,
        3, 4, 2026, 60,
        HANAFI, 18.0f, 18.0f, false, 0,
        325.0f, 429.0f, 814.0f, 1084.0f, 1200.0f, 1304.0f,
        5.0f, 3.0f, 3.0f, 5.0f, 3.0f, 5.0f
    },
    {
        "London_2026_12_01_Shafii_18_18",
        51.5f, -0.1f, 0,
        1, 12, 2026, 0,
        SHAFII, 18.0f, 18.0f, false, 0,
        342.0f, 463.0f, 710.0f, 818.0f, 956.0f, 1078.0f,
        5.0f, 3.0f, 3.0f, 5.0f, 3.0f, 5.0f
    },
    {
        "Makkah_2026_01_10_Hanafi_18_18",
        21.4f, 39.8f, 180,
        10, 1, 2026, 0,
        HANAFI, 18.0f, 18.0f, false, 0,
        341.0f, 420.0f, 747.0f, 979.0f, 1075.0f, 1153.0f,
        5.0f, 3.0f, 3.0f, 5.0f, 3.0f, 5.0f
    },
    {
        "Karachi_2026_07_15_Hanafi_18_18",
        24.9f, 67.0f, 300,
        15, 7, 2026, 0,
        HANAFI, 18.0f, 18.0f, false, 0,
        264.0f, 351.0f, 758.0f, 1040.0f, 1164.0f, 1251.0f,
        5.0f, 3.0f, 3.0f, 5.0f, 3.0f, 5.0f
    },
};

static void test_regression_cases() {
    for (const auto& c : cases) {
        PrayerTimes pt(c.lat, c.lon, c.tz);
        pt.setAsrMethod(c.asrMethod);
        pt.setCustomMethod(c.fajrAngle, c.ishaAngle, c.ishaIsInterval, c.ishaMinutes);

        auto r = pt.calculateWithOffset(c.day, c.month, c.year, c.dst);
        assert(r.valid);
        assert(r.errorMessage == nullptr);

        assert(within(r.fajr, c.fajr, c.tolFajr));
        assert(within(r.sunrise, c.sunrise, c.tolSunrise));
        assert(within(r.dhuhr, c.dhuhr, c.tolDhuhr));
        assert(within(r.asr, c.asr, c.tolAsr));
        assert(within(r.maghrib, c.maghrib, c.tolMaghrib));
        assert(within(r.isha, c.isha, c.tolIsha));
    }
}

int main() {
    test_regression_cases();
    return 0;
}
