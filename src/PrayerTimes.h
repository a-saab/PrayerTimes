/*
 * File:     PrayerTimes.cpp
 * Original  Author:  Adnan Saab 
 * Modified by :   Muzammil Patel (zamilpatel329@gmail.com)
 * Created:  2025-08-05
 * Version:  1.0
 * 
 */
 
#ifndef PRAYERTIMES_H
#define PRAYERTIMES_H

#include <Arduino.h>

enum CalculationMethod {
    MWL, ISNA, UmmAlQura, Egyptian, Karachi, Tehran, Jafari, Gulf, Kuwait, Qatar, Singapore, France,
    Turkey, Russia, Dubai, JAKIM, Tunisia, Algeria, Indonesia, Morocco, Portugal, Jordan
};

enum AsrMethod {
    Shafii = 1,
    Hanafi = 2
};

class PrayerTimes {
public:
    PrayerTimes(float latitude, float longitude, float timeZone);

    void setAdjustments(int adjFajr, int adjSunrise, int adjDhuhr, int adjAsr, int adjMaghrib, int adjIsha);
    void setCalculationMethod(CalculationMethod method);
    void setAsrMethod(AsrMethod method);
    void setDST(bool dstOn);

    void calculate(int day, int month, int year,
                   int &fajrHour, int &fajrMinute,
                   int &sunriseHour, int &sunriseMinute,
                   int &dhuhrHour, int &dhuhrMinute,
                   int &asrHour, int &asrMinute,
                   int &maghribHour, int &maghribMinute,
                   int &ishaHour, int &ishaMinute);

    String formatTime(int hour, int minute);
    String formatTime24WithPeriod(int hour, int minute);

private:
    float _latitude, _longitude, _timeZone;
    float _fajrAngle, _ishaAngle;
    float _asrFactor;
    bool _ishaIsInterval, _dst;
    int _ishaInterval;
    int _adjFajr, _adjSunrise, _adjDhuhr, _adjAsr, _adjMaghrib, _adjIsha;

    float deg2rad(float degrees);
    float rad2deg(float radians);
    int calculateDayOfYear(int day, int month, int year);
    void calculateSolarParameters(int dayOfYear, float &eqTime, float &solarDec);
    float calculateDhuhr(float eqTime);
    float calculateTimeForAngle(float angle, float Dhuhr, float solarDec, bool isMorning);
    void convertToHoursMinutes(float time, int &hours, int &minutes);
};

#endif
