// Adnan Saab 
// Github: a-saab
// February 07 2025

#ifndef PRAYERTIMES_H
#define PRAYERTIMES_H

#include <Arduino.h>

// Define available calculation methods.
enum CalculationMethod {
  MWL,         // Muslim World League: Fajr 18°, Isha 17°, Asr: 1x
  ISNA,        // Islamic Society of North America: Fajr 15°, Isha 15°, Asr: 1x
  UmmAlQura,   // Umm al-Qura (Makkah): Fajr 18.5°, Isha: 90 min after Maghrib, Asr: 1x
  Egyptian,    // Egyptian General Authority: Fajr 19.5°, Isha 17.5°, Asr: 1x
  Karachi,     // University of Islamic Sciences, Karachi: Fajr 18°, Isha 18°, Asr: 2x (Hanafi)
  Tehran,      // Institute of Geophysics, University of Tehran: Fajr 17.7°, Isha 14°, Asr: 1x
  Jafari       // Jafari (Shia Ithna Ashari): Fajr 16°, Isha 14°, Asr: 1x
};

class PrayerTimes {
public:
    // Constructor: provide location (latitude, longitude) and time zone.
    PrayerTimes(float latitude, float longitude, int timeZone);

    // Set manual adjustment offsets (in minutes) for fine-tuning.
    void setAdjustments(int adjFajr, int adjSunrise, int adjDhuhr, int adjAsr, int adjMaghrib, int adjIsha);

    // Set a preconfigured calculation method.
    void setCalculationMethod(CalculationMethod method);

    // Calculate prayer times for a given date.
    // The times are returned (by reference) in hours and minutes.
    void calculate(int day, int month, int year,
                   int &fajrHour, int &fajrMinute,
                   int &sunriseHour, int &sunriseMinute,
                   int &dhuhrHour, int &dhuhrMinute,
                   int &asrHour, int &asrMinute,
                   int &maghribHour, int &maghribMinute,
                   int &ishaHour, int &ishaMinute);

    // Utility: Format a time (hour, minute) into a 12-hour clock string.
    static String formatTime(int hour, int minute);

private:
    float _latitude;
    float _longitude;
    int   _timeZone;

    // Manual adjustment offsets (in minutes)
    int _adjFajr, _adjSunrise, _adjDhuhr, _adjAsr, _adjMaghrib, _adjIsha;

    // Calculation method parameters.
    float _fajrAngle;    // in degrees (for Fajr calculation)
    float _ishaAngle;    // in degrees (for Isha calculation)
    float _asrFactor;    // shadow length factor for Asr
    bool  _ishaIsInterval; // if true, Isha is computed as Maghrib + interval
    int   _ishaInterval;   // interval (in minutes) after Maghrib for Isha (used if _ishaIsInterval==true)

    // Helper functions for internal calculations.
    float deg2rad(float degrees);
    float rad2deg(float radians);
    void calculateSolarParameters(int dayOfYear, float &eqTime, float &solarDec);
    float calculateDhuhr(float eqTime);
    float calculateTimeForAngle(float angle, float Dhuhr, float solarDec, bool isMorning);
    void convertToHoursMinutes(float time, int &hours, int &minutes);
    int calculateDayOfYear(int day, int month, int year);
};

#endif