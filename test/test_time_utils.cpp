#include <cassert>
#include "PrayerTimes.h"

static void test_minutes_to_time_rounding() {
    PrayerTimes pt(45.737f, 7.320f, 60);

    int h = 0, m = 0;
    pt.minutesToTime(10.4f, h, m);
    assert(h == 0 && m == 10);

    pt.minutesToTime(10.6f, h, m);
    assert(h == 0 && m == 11);
}

static void test_minutes_to_time_sixty_minute_rollover() {
    PrayerTimes pt(45.737f, 7.320f, 60);

    int h = 0, m = 0;
    pt.minutesToTime(59.6f, h, m);
    assert(h == 1 && m == 0);
}

int main() {
    test_minutes_to_time_rounding();
    test_minutes_to_time_sixty_minute_rollover();
    return 0;
}
