// Adnan Saab 
// Github: a-saab
// February 07 2025

#include <PrayerTimes.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;
PrayerTimes pt(45.5017, -73.5673, -5);  // Montreal settings

void setup() {
  Serial.begin(9600);
  
  // Set manual adjustment offsets
  pt.setAdjustments(-1, 0, 14, 0, -1, 0);

  // Using ISNA for correct Fajr/Isha angles (15°)
  pt.setCalculationMethod(ISNA);

  if (!rtc.begin()) {
    Serial.println("RTC NOT Found!");
    while (1);
  }
}

void loop() {
  DateTime now = rtc.now();
  int fajrH, fajrM, sunriseH, sunriseM,
      dhuhrH, dhuhrM, asrH, asrM, maghribH, maghribM, ishaH, ishaM;

  pt.calculate(now.day(), now.month(), now.year(),
               fajrH, fajrM,
               sunriseH, sunriseM,
               dhuhrH, dhuhrM,
               asrH, asrM,
               maghribH, maghribM,
               ishaH, ishaM);

  Serial.println("=== Today's Prayer Times ===");
  Serial.print("Fajr: "); Serial.println(PrayerTimes::formatTime(fajrH, fajrM));
  Serial.print("Sunrise: "); Serial.println(PrayerTimes::formatTime(sunriseH, sunriseM));
  Serial.print("Dhuhr: "); Serial.println(PrayerTimes::formatTime(dhuhrH, dhuhrM));
  Serial.print("Asr: "); Serial.println(PrayerTimes::formatTime(asrH, asrM));
  Serial.print("Maghrib: "); Serial.println(PrayerTimes::formatTime(maghribH, maghribM));
  Serial.print("Isha: "); Serial.println(PrayerTimes::formatTime(ishaH, ishaM));

  delay(10000);
}