/*
 * PrayerTimes Library v2.0 - Implementation
 * 
 * Original Author: Adnan Saab (https://github.com/a-saab)
 * Created: February 2025
 */

#include "PrayerTimes.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

// Constructor
PrayerTimes::PrayerTimes(float latitude, float longitude, int timezoneOffsetMinutes)
    : _latitude(latitude), 
      _longitude(longitude), 
      _timezoneOffsetMinutes(timezoneOffsetMinutes),
      _adjFajr(0), _adjSunrise(0), _adjDhuhr(0), 
      _adjAsr(0), _adjMaghrib(0), _adjIsha(0),
      _fajrAngle(18.0), _ishaAngle(17.0), 
      _asrMethod(SHAFII), _highLatRule(NONE),
      _ishaIsInterval(false), _ishaMinutes(0)
{
    // Default to Muslim World League
}

void PrayerTimes::setCalculationMethod(const CalculationConfig& config) {
    _fajrAngle = config.fajrAngle;
    _ishaAngle = config.ishaAngle;
    _ishaIsInterval = config.ishaIsInterval;
    _ishaMinutes = config.ishaMinutes;
}

void PrayerTimes::setCustomMethod(float fajrAngle, float ishaAngle, bool ishaIsInterval, int ishaMinutes) {
    _fajrAngle = fajrAngle;
    _ishaAngle = ishaAngle;
    _ishaIsInterval = ishaIsInterval;
    _ishaMinutes = ishaMinutes;
}

void PrayerTimes::setAsrMethod(AsrMethod method) {
    _asrMethod = method;
}

void PrayerTimes::setHighLatitudeRule(HighLatitudeRule rule) {
    _highLatRule = rule;
}

void PrayerTimes::setAdjustments(int adjFajr, int adjSunrise, int adjDhuhr, 
                                  int adjAsr, int adjMaghrib, int adjIsha) {
    _adjFajr = adjFajr;
    _adjSunrise = adjSunrise;
    _adjDhuhr = adjDhuhr;
    _adjAsr = adjAsr;
    _adjMaghrib = adjMaghrib;
    _adjIsha = adjIsha;
}

float PrayerTimes::deg2rad(float degrees) {
    return degrees * (PI / 180.0);
}

float PrayerTimes::rad2deg(float radians) {
    return radians * (180.0 / PI);
}

float PrayerTimes::clamp(float value, float minVal, float maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

float PrayerTimes::normalizeTime(float time) {
    while (time < 0) time += 1440;  // 1440 minutes in a day
    while (time >= 1440) time -= 1440;
    return time;
}

int PrayerTimes::calculateDayOfYear(int day, int month, int year) {
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Handle leap years
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[1] = 29;
    }
    
    int dayOfYear = day;
    for (int i = 0; i < month - 1; i++) {
        dayOfYear += daysInMonth[i];
    }
    
    return dayOfYear;
}

void PrayerTimes::calculateSolarParameters(int dayOfYear, float &eqTime, float &solarDec) {
    // Using NOAA solar calculations (more accurate)
    float gamma = 2.0 * PI / 365.0 * (dayOfYear - 1);
    
    // Equation of time in minutes
    eqTime = 229.18 * (
        0.000075 + 
        0.001868 * cos(gamma) - 0.032077 * sin(gamma) -
        0.014615 * cos(2 * gamma) - 0.040849 * sin(2 * gamma)
    );
    
    // Solar declination in radians
    solarDec = 
        0.006918 - 0.399912 * cos(gamma) + 0.070257 * sin(gamma) -
        0.006758 * cos(2 * gamma) + 0.000907 * sin(2 * gamma) -
        0.002697 * cos(3 * gamma) + 0.00148 * sin(3 * gamma);
}

float PrayerTimes::calculateSolarNoon(float eqTime) {
    // Solar noon in minutes since midnight
    // Longitude: negative for west, positive for east
    return 720.0 - 4.0 * _longitude - eqTime + _timezoneOffsetMinutes;
}

float PrayerTimes::calculateTimeForAngle(float angle, float solarNoon, float solarDec, bool isMorning) {
    float latRad = deg2rad(_latitude);
    
    // Hour angle calculation with defensive clamping
    float cosH = (sin(deg2rad(angle)) - sin(latRad) * sin(solarDec)) / 
                 (cos(latRad) * cos(solarDec));
    
    // Clamp to prevent NaN from acos (critical for embedded systems)
    cosH = clamp(cosH, -1.0, 1.0);
    
    float hourAngle = acos(cosH);
    float delta = rad2deg(hourAngle) * 4.0;  // Convert to minutes
    
    return isMorning ? solarNoon - delta : solarNoon + delta;
}

float PrayerTimes::calculateAsrTime(float solarNoon, float solarDec) {
    float latRad = deg2rad(_latitude);
    float shadowFactor = (_asrMethod == HANAFI) ? 2.0 : 1.0;
    
    // Asr angle calculation
    float angle = atan(1.0 / (shadowFactor + tan(fabs(latRad - solarDec))));
    
    float cosH = (sin(angle) - sin(latRad) * sin(solarDec)) / 
                 (cos(latRad) * cos(solarDec));
    
    cosH = clamp(cosH, -1.0, 1.0);
    
    float hourAngle = acos(cosH);
    return solarNoon + rad2deg(hourAngle) * 4.0;
}

float PrayerTimes::nightFraction(float angle) {
    // For high-latitude adjustments
    switch (_highLatRule) {
        case ONE_SEVENTH:
            return 1.0 / 7.0;
        case MIDDLE_OF_NIGHT:
            return 0.5;
        case ANGLE_BASED:
            return angle / 60.0;
        default:
            return 0;
    }
}

void PrayerTimes::applyHighLatitudeAdjustments(PrayerTimesResult &times, float solarDec) {
    if (_highLatRule == NONE) return;
    
    float nightLength = times.sunrise - times.maghrib;
    if (nightLength < 0) nightLength += 1440;
    
    // Check if Fajr/Isha calculations are problematic
    float fajrDiff = times.sunrise - times.fajr;
    float ishaDiff = times.isha - times.maghrib;
    
    // Apply adjustment if times are unreasonable
    if (fajrDiff < 0 || fajrDiff > nightLength * 0.5) {
        float portion = nightFraction(_fajrAngle);
        times.fajr = times.sunrise - nightLength * portion;
    }
    
    if (ishaDiff < 0 || ishaDiff > nightLength * 0.5) {
        float portion = nightFraction(_ishaAngle);
        times.isha = times.maghrib + nightLength * portion;
    }
}

PrayerTimesResult PrayerTimes::calculate(int day, int month, int year) {
    return calculateWithOffset(day, month, year, 0);
}

PrayerTimesResult PrayerTimes::calculateWithOffset(int day, int month, int year, int dstMinutes) {
    PrayerTimesResult result;
    
    int dayOfYear = calculateDayOfYear(day, month, year);
    
    float eqTime, solarDec;
    calculateSolarParameters(dayOfYear, eqTime, solarDec);
    
    // Calculate solar noon (pure astronomical time)
    float solarNoon = calculateSolarNoon(eqTime);
    
    // Calculate all prayer times (still in astronomical time)
    result.fajr = calculateTimeForAngle(-_fajrAngle, solarNoon, solarDec, true);
    result.sunrise = calculateTimeForAngle(-0.833, solarNoon, solarDec, true);
    result.dhuhr = solarNoon;
    result.asr = calculateAsrTime(solarNoon, solarDec);
    result.maghrib = calculateTimeForAngle(-0.833, solarNoon, solarDec, false);
    
    // Isha: either interval-based or angle-based
    if (_ishaIsInterval) {
        result.isha = result.maghrib + _ishaMinutes;
    } else {
        result.isha = calculateTimeForAngle(-_ishaAngle, solarNoon, solarDec, false);
    }
    
    // Apply high-latitude adjustments if needed
    applyHighLatitudeAdjustments(result, solarDec);
    
    // Now apply ALL time offsets at once (manual adjustments + DST)
    result.fajr = normalizeTime(result.fajr + _adjFajr + dstMinutes);
    result.sunrise = normalizeTime(result.sunrise + _adjSunrise + dstMinutes);
    result.dhuhr = normalizeTime(result.dhuhr + _adjDhuhr + dstMinutes);
    result.asr = normalizeTime(result.asr + _adjAsr + dstMinutes);
    result.maghrib = normalizeTime(result.maghrib + _adjMaghrib + dstMinutes);
    result.isha = normalizeTime(result.isha + _adjIsha + dstMinutes);
    
    result.valid = true;
    return result;
}

// Legacy API for backward compatibility
void PrayerTimes::calculate(int day, int month, int year,
                             int &fajrHour, int &fajrMinute,
                             int &sunriseHour, int &sunriseMinute,
                             int &dhuhrHour, int &dhuhrMinute,
                             int &asrHour, int &asrMinute,
                             int &maghribHour, int &maghribMinute,
                             int &ishaHour, int &ishaMinute) {
    PrayerTimesResult result = calculate(day, month, year);
    
    minutesToTime(result.fajr, fajrHour, fajrMinute);
    minutesToTime(result.sunrise, sunriseHour, sunriseMinute);
    minutesToTime(result.dhuhr, dhuhrHour, dhuhrMinute);
    minutesToTime(result.asr, asrHour, asrMinute);
    minutesToTime(result.maghrib, maghribHour, maghribMinute);
    minutesToTime(result.isha, ishaHour, ishaMinute);
}

void PrayerTimes::minutesToTime(float minutes, int &hour, int &minute) {
    hour = (int)(minutes / 60.0);
    minute = (int)round(minutes - (hour * 60.0));
    
    // Handle edge case where rounding causes 60 minutes
    if (minute >= 60) {
        minute = 0;
        hour++;
    }
    
    // Ensure hour is within 0-23
    hour = hour % 24;
}

String PrayerTimes::formatTime12(int hour, int minute) {
    int h = hour % 12;
    if (h == 0) h = 12;
    String period = (hour < 12) ? "AM" : "PM";
    
    char buffer[12];
    sprintf(buffer, "%d:%02d %s", h, minute, period.c_str());
    return String(buffer);
}

String PrayerTimes::formatTime24(int hour, int minute) {
    char buffer[8];
    sprintf(buffer, "%02d:%02d", hour, minute);
    return String(buffer);
}