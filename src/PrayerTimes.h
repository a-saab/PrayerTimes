/*
 * PrayerTimes Library v2.0
 * 
 * Original Author: Adnan Saab (https://github.com/a-saab)
 * Created: February 2025
 * 
 * A clean, maintainable library for calculating Islamic prayer times
 * with proper separation of astronomical calculations and civil time.
 * 
 * Key improvements in v2.0:
 * - Proper timezone handling (minutes-based offset)
 * - Separated solar calculations from civil time adjustments
 * - Extensible calculation method system
 * - High-latitude handling
 * - Defensive math for embedded systems
 */

#ifndef PRAYERTIMES_H
#define PRAYERTIMES_H

#include <Arduino.h>

// Asr calculation methods
enum AsrMethod {
    SHAFII = 1,  // Standard (shadow length = object height)
    HANAFI = 2   // Hanafi (shadow length = 2x object height)
};

// High-latitude adjustment methods for extreme locations
enum HighLatitudeRule {
    NONE,           // No adjustment (may fail at extreme latitudes)
    MIDDLE_OF_NIGHT, // Fajr/Isha based on middle of night
    ONE_SEVENTH,    // Night divided into sevenths
    ANGLE_BASED     // Use angle-based approximation
};

// Pre-configured calculation method settings
struct CalculationConfig {
    const char* name;
    float fajrAngle;
    float ishaAngle;
    bool ishaIsInterval;  // If true, Isha = Maghrib + ishaMinutes
    int ishaMinutes;      // Used when ishaIsInterval = true
    
    CalculationConfig(const char* n, float fajr, float isha, bool interval = false, int minutes = 0)
        : name(n), fajrAngle(fajr), ishaAngle(isha), ishaIsInterval(interval), ishaMinutes(minutes) {}
};

// Standard calculation methods as constants
namespace CalculationMethods {
    const CalculationConfig MWL("Muslim World League", 18.0, 17.0);
    const CalculationConfig ISNA("ISNA", 15.0, 15.0);
    const CalculationConfig EGYPT("Egyptian", 19.5, 17.5);
    const CalculationConfig MAKKAH("Umm al-Qura", 18.5, 0, true, 90);
    const CalculationConfig KARACHI("Karachi", 18.0, 18.0);
    const CalculationConfig TEHRAN("Tehran", 17.7, 14.0);
    const CalculationConfig JAFARI("Jafari", 16.0, 14.0);
    const CalculationConfig GULF("Gulf", 19.5, 0, true, 90);
    const CalculationConfig KUWAIT("Kuwait", 18.0, 17.5);
    const CalculationConfig QATAR("Qatar", 18.0, 0, true, 90);
    const CalculationConfig SINGAPORE("Singapore", 20.0, 18.0);
    const CalculationConfig FRANCE("France", 12.0, 12.0);
    const CalculationConfig TURKEY("Turkey", 18.0, 17.0);
    const CalculationConfig RUSSIA("Russia", 16.0, 15.0);
    const CalculationConfig DUBAI("Dubai", 18.2, 18.2);
    const CalculationConfig JAKIM("JAKIM", 20.0, 18.0);
    const CalculationConfig TUNISIA("Tunisia", 18.0, 18.0);
    const CalculationConfig ALGERIA("Algeria", 18.0, 17.0);
    const CalculationConfig INDONESIA("Indonesia", 20.0, 18.0);
    const CalculationConfig MOROCCO("Morocco", 19.0, 17.0);
    const CalculationConfig PORTUGAL("Portugal", 18.0, 0, true, 77);
    const CalculationConfig JORDAN("Jordan", 18.0, 18.0);
}

// Structure to hold calculated prayer times (in minutes since midnight)
struct PrayerTimesResult {
    float fajr;
    float sunrise;
    float dhuhr;
    float asr;
    float maghrib;
    float isha;
    bool valid;  // False if calculation failed (e.g., extreme latitude)
    
    PrayerTimesResult() : fajr(0), sunrise(0), dhuhr(0), asr(0), maghrib(0), isha(0), valid(false) {}
};

class PrayerTimes {
public:
    // Constructor: latitude, longitude, timezone offset in MINUTES
    // Examples: 
    //   Montreal (UTC-5): timezoneOffsetMinutes = -300
    //   India (UTC+5:30): timezoneOffsetMinutes = 330
    //   Nepal (UTC+5:45): timezoneOffsetMinutes = 345
    PrayerTimes(float latitude, float longitude, int timezoneOffsetMinutes);

    // Set calculation method using pre-configured settings
    void setCalculationMethod(const CalculationConfig& config);
    
    // Or set custom parameters directly
    void setCustomMethod(float fajrAngle, float ishaAngle, bool ishaIsInterval = false, int ishaMinutes = 0);
    
    // Set Asr calculation method
    void setAsrMethod(AsrMethod method);
    
    // Set high-latitude adjustment rule
    void setHighLatitudeRule(HighLatitudeRule rule);
    
    // Set manual adjustments (in minutes) for fine-tuning
    void setAdjustments(int adjFajr, int adjSunrise, int adjDhuhr, int adjAsr, int adjMaghrib, int adjIsha);
    
    // Calculate prayer times for a given date
    // Returns a structure with all times in minutes since midnight
    PrayerTimesResult calculate(int day, int month, int year);
    
    // Calculate with additional civil time offset (e.g., DST)
    // dstMinutes: typically 0 (no DST) or 60 (DST active)
    PrayerTimesResult calculateWithOffset(int day, int month, int year, int dstMinutes);
    
    // Legacy API compatibility (v1.x interface)
    void calculate(int day, int month, int year,
                   int &fajrHour, int &fajrMinute,
                   int &sunriseHour, int &sunriseMinute,
                   int &dhuhrHour, int &dhuhrMinute,
                   int &asrHour, int &asrMinute,
                   int &maghribHour, int &maghribMinute,
                   int &ishaHour, int &ishaMinute);
    
    // Utility functions for time formatting
    static String formatTime12(int hour, int minute);
    static String formatTime24(int hour, int minute);
    static void minutesToTime(float minutes, int &hour, int &minute);

private:
    // Location and timezone
    float _latitude;
    float _longitude;
    int _timezoneOffsetMinutes;
    
    // Calculation parameters
    float _fajrAngle;
    float _ishaAngle;
    bool _ishaIsInterval;
    int _ishaMinutes;
    AsrMethod _asrMethod;
    HighLatitudeRule _highLatRule;
    
    // Manual adjustments
    int _adjFajr, _adjSunrise, _adjDhuhr, _adjAsr, _adjMaghrib, _adjIsha;
    
    // Core astronomical calculations
    float deg2rad(float degrees);
    float rad2deg(float radians);
    int calculateDayOfYear(int day, int month, int year);
    void calculateSolarParameters(int dayOfYear, float &eqTime, float &solarDec);
    float calculateSolarNoon(float eqTime);
    float calculateTimeForAngle(float angle, float solarNoon, float solarDec, bool isMorning);
    float calculateAsrTime(float solarNoon, float solarDec);
    
    // High-latitude adjustments
    void applyHighLatitudeAdjustments(PrayerTimesResult &times, float solarDec);
    float nightFraction(float angle);
    
    // Utility
    float clamp(float value, float minVal, float maxVal);
    float normalizeTime(float time);
};

#endif