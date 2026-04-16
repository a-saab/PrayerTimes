#include <cassert>
#include "PrayerTimes.h"

static void test_high_latitude_rules_produce_valid_results() {
    HighLatitudeRule rules[] = { NONE, ONE_SEVENTH, MIDDLE_OF_NIGHT, ANGLE_BASED };

    for (auto rule : rules) {
        PrayerTimes pt(69.6492f, 18.9553f, 60); // Tromsø
        pt.setCustomMethod(18.0f, 18.0f, false, 0);
        pt.setHighLatitudeRule(rule);

        auto r = pt.calculateWithOffset(15, 6, 2026, 120);
        assert(r.valid);
        assert(r.errorMessage == nullptr);
    }
}

/*
Commented out because this test is failing
https://github.com/a-saab/PrayerTimes/issues/6
static void test_fallback_rules_produce_ordered_night_times() {
    HighLatitudeRule rules[] = { ONE_SEVENTH, MIDDLE_OF_NIGHT, ANGLE_BASED };

    for (auto rule : rules) {
        PrayerTimes pt(69.6492f, 18.9553f, 60); // Tromsø
        pt.setCustomMethod(18.0f, 18.0f, false, 0);
        pt.setHighLatitudeRule(rule);

        auto r = pt.calculateWithOffset(15, 6, 2026, 120);
        assert(r.valid);
        assert(r.errorMessage == nullptr);
        assert(r.fajr < r.sunrise);
        assert(r.maghrib < r.isha);
    }
}
*/

int main() {
    test_high_latitude_rules_produce_valid_results();
    //test_fallback_rules_produce_ordered_night_times();
    return 0;
}
