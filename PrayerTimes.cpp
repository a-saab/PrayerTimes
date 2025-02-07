// Adnan Saab 
// Github: a-saab
// February 07 2025

#include "PrayerTimes.h"
#include <math.h>

// Constructor
PrayerTimes::PrayerTimes(float latitude, float longitude, int timeZone)
    : _latitude(latitude), _longitude(longitude), _timeZone(timeZone),
      _adjFajr(0), _adjSunrise(0), _adjDhuhr(0), _adjAsr(0), _adjMaghrib(0), _adjIsha(0),
      _fajrAngle(18), _ishaAngle(17), _asrFactor(1.0), _ishaIsInterval(false), _ishaInterval(0)
{
    // Default to MWL settings.
}

// Set manual adjustments
void PrayerTimes::setAdjustments(int adjFajr, int adjSunrise, int adjDhuhr, int adjAsr, int adjMaghrib, int adjIsha) {
    _adjFajr    = adjFajr;
    _adjSunrise = adjSunrise;
    _adjDhuhr   = adjDhuhr;
    _adjAsr     = adjAsr;
    _adjMaghrib = adjMaghrib;
    _adjIsha    = adjIsha;
}

// Set the calculation method.
void PrayerTimes::setCalculationMethod(CalculationMethod method) {
    switch (method) {
      case MWL:
        _fajrAngle    = 18.0;
        _ishaAngle    = 17.0;
        _asrFactor    = 1.0;
        _ishaIsInterval = false;
        break;
      case ISNA:
        _fajrAngle    = 15.0;
        _ishaAngle    = 15.0;
        _asrFactor    = 1.0;
        _ishaIsInterval = false;
        break;
      case UmmAlQura:
        _fajrAngle    = 18.5;
        // For Umm al-Qura, Isha is a fixed interval (e.g., 90 minutes after Maghrib)
        _ishaIsInterval = true;
        _ishaInterval = 90;
        _asrFactor    = 1.0;
        break;
      case Egyptian:
        _fajrAngle    = 19.5;
        _ishaAngle    = 17.5;
        _asrFactor    = 1.0;
        _ishaIsInterval = false;
        break;
      case Karachi:
        _fajrAngle    = 18.0;
        _ishaAngle    = 18.0;
        _asrFactor    = 2.0;  // Hanafi method
        _ishaIsInterval = false;
        break;
      case Tehran:
        _fajrAngle    = 17.7;
        _ishaAngle    = 14.0;
        _asrFactor    = 1.0;
        _ishaIsInterval = false;
        break;
      case Jafari:
        _fajrAngle    = 16.0;
        _ishaAngle    = 14.0;
        _asrFactor    = 1.0;
        _ishaIsInterval = false;
        break;
      default:
        // Default to MWL if something goes wrong.
        _fajrAngle    = 18.0;
        _ishaAngle    = 17.0;
        _asrFactor    = 1.0;
        _ishaIsInterval = false;
        break;
    }
}

// Convert degrees to radians.
float PrayerTimes::deg2rad(float degrees) {
    return degrees * (PI / 180);
}

// Convert radians to degrees.
float PrayerTimes::rad2deg(float radians) {
    return radians * (180 / PI);
}

// Format the time into a 12-hour clock string.
String PrayerTimes::formatTime(int hour, int minute) {
    int hour12;
    String period;
    if (hour == 0) {
        hour12 = 12;
        period = "AM";
    } else if (hour < 12) {
        hour12 = hour;
        period = "AM";
    } else if (hour == 12) {
        hour12 = 12;
        period = "PM";
    } else {
        hour12 = hour - 12;
        period = "PM";
    }
    char buffer[10];
    sprintf(buffer, "%d:%02d %s", hour12, minute, period.c_str());
    return String(buffer);
}

// Calculate solar parameters based on the day of the year.
void PrayerTimes::calculateSolarParameters(int dayOfYear, float &eqTime, float &solarDec) {
    float meanAnomaly = 357.5291 + 0.98560028 * dayOfYear;
    float meanLongitude = fmod(280.46646 + 0.9856474 * dayOfYear, 360);
    float eccentricity = 0.016708634 - 0.000042037 * dayOfYear;

    float anomalyRad = deg2rad(meanAnomaly);
    float equationOfCenter = (1.914602 - 0.004817 * dayOfYear) * sin(anomalyRad)
                           + (0.019993 - 0.000101 * dayOfYear) * sin(2 * anomalyRad)
                           + 0.000289 * sin(3 * anomalyRad);

    float trueLongitude = meanLongitude + equationOfCenter;
    solarDec = asin(sin(deg2rad(trueLongitude)) * sin(deg2rad(23.44)));

    float y = tan(deg2rad(23.44) / 2) * tan(deg2rad(23.44) / 2);
    eqTime = 4 * (y * sin(2 * deg2rad(meanLongitude))
                - 2 * eccentricity * sin(anomalyRad)
                + 4 * eccentricity * y * sin(anomalyRad) * cos(2 * deg2rad(meanLongitude))
                - 0.5 * y * y * sin(4 * deg2rad(meanLongitude))
                - 1.25 * eccentricity * eccentricity * sin(2 * anomalyRad));
}

// Calculate solar noon (Dhuhr)
float PrayerTimes::calculateDhuhr(float eqTime) {
    return 720 - 4 * (_longitude - (_timeZone * 15)) - eqTime;
}

// Calculate time for a specific angle (in degrees)
float PrayerTimes::calculateTimeForAngle(float angle, float Dhuhr, float solarDec, bool isMorning) {
    float cosHA = (sin(deg2rad(angle)) - sin(deg2rad(_latitude)) * sin(solarDec)) /
                  (cos(deg2rad(_latitude)) * cos(solarDec));
    // Clamp cosHA to [-1, 1] to avoid errors.
    if (cosHA < -1) cosHA = -1;
    if (cosHA >  1) cosHA =  1;
    float ha = acos(cosHA); // Hour angle in radians
    return isMorning ? Dhuhr - 4 * rad2deg(ha) : Dhuhr + 4 * rad2deg(ha);
}

// Convert minutes (since midnight) to hours and minutes.
void PrayerTimes::convertToHoursMinutes(float time, int &hours, int &minutes) {
    hours = floor(time / 60);
    minutes = round(time - (hours * 60));
}

// Calculate the day of the year.
int PrayerTimes::calculateDayOfYear(int day, int month, int year) {
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    // Handle leap year.
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        daysInMonth[1] = 29;
    int dayOfYear = 0;
    for (int i = 0; i < month - 1; i++) {
        dayOfYear += daysInMonth[i];
    }
    dayOfYear += day;
    return dayOfYear;
}

// Calculate prayer times for a given date.
void PrayerTimes::calculate(int day, int month, int year,
                   int &fajrHour, int &fajrMinute,
                   int &sunriseHour, int &sunriseMinute,
                   int &dhuhrHour, int &dhuhrMinute,
                   int &asrHour, int &asrMinute,
                   int &maghribHour, int &maghribMinute,
                   int &ishaHour, int &ishaMinute) 
{
    int dayOfYear = calculateDayOfYear(day, month, year);
    
    float eqTime, solarDec;
    calculateSolarParameters(dayOfYear, eqTime, solarDec);
    
    // Dhuhr calculation with adjustment.
    float Dhuhr = calculateDhuhr(eqTime);
    Dhuhr += _adjDhuhr;
    convertToHoursMinutes(Dhuhr, dhuhrHour, dhuhrMinute);
    
    // Fajr: calculate using the configured Fajr angle.
    float fajrTime = calculateTimeForAngle(-_fajrAngle, Dhuhr, solarDec, true);
    fajrTime += _adjFajr;
    convertToHoursMinutes(fajrTime, fajrHour, fajrMinute);
    
    // Sunrise: using a standard -0.833°.
    float sunriseTime = calculateTimeForAngle(-0.833, Dhuhr, solarDec, true);
    sunriseTime += _adjSunrise;
    convertToHoursMinutes(sunriseTime, sunriseHour, sunriseMinute);
    
    // Maghrib (sunset): using -0.833°.
    float maghribTime = calculateTimeForAngle(-0.833, Dhuhr, solarDec, false);
    maghribTime += _adjMaghrib;
    convertToHoursMinutes(maghribTime, maghribHour, maghribMinute);
    
    // Isha: either calculated by angle or set as an interval after Maghrib.
    float ishaTime = 0;
    if (_ishaIsInterval) {
      // For methods like Umm al-Qura.
      ishaTime = maghribTime + _ishaInterval;
    } else {
      ishaTime = calculateTimeForAngle(-_ishaAngle, Dhuhr, solarDec, false);
    }
    ishaTime += _adjIsha;
    convertToHoursMinutes(ishaTime, ishaHour, ishaMinute);
    
    // Asr: Using the configured shadow factor.
    float A = atan(1.0 / (_asrFactor + tan(fabs(deg2rad(_latitude) - solarDec))));
    float cosHA_asr = (sin(A) - sin(deg2rad(_latitude)) * sin(solarDec)) /
                      (cos(deg2rad(_latitude)) * cos(solarDec));
    if (cosHA_asr < -1) cosHA_asr = -1;
    if (cosHA_asr >  1) cosHA_asr =  1;
    float ha_asr = acos(cosHA_asr);
    float asrTime = Dhuhr + 4 * rad2deg(ha_asr);
    asrTime += _adjAsr;
    convertToHoursMinutes(asrTime, asrHour, asrMinute);
}