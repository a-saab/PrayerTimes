/*
 * File:     PrayerTimes.cpp
 * Original  Author:  Adnan Saab 
 * Modified by :   Muzammil Patel (zamilpatel329@gmail.com)
 * Created:  2025-08-05
 * Version:  1.0
 * 
 */

#include "PrayerTimes.h"
#include <math.h>

PrayerTimes::PrayerTimes(float latitude, float longitude, float timeZone)
    : _latitude(latitude), _longitude(longitude), _timeZone(timeZone),
      _adjFajr(0), _adjSunrise(0), _adjDhuhr(0), _adjAsr(0), _adjMaghrib(0), _adjIsha(0),
      _fajrAngle(18.0), _ishaAngle(17.0), _asrFactor(1.0), _ishaIsInterval(false),
      _ishaInterval(0), _dst(false) {}

void PrayerTimes::setAdjustments(int adjFajr, int adjSunrise, int adjDhuhr, int adjAsr, int adjMaghrib, int adjIsha) {
    _adjFajr    = adjFajr;
    _adjSunrise = adjSunrise;
    _adjDhuhr   = adjDhuhr;
    _adjAsr     = adjAsr;
    _adjMaghrib = adjMaghrib;
    _adjIsha    = adjIsha;
}

void PrayerTimes::setCalculationMethod(CalculationMethod method) {
    switch (method) {
        case MWL: //Muslim World League
            _fajrAngle = 18;
            _ishaAngle = 17;
            _ishaIsInterval = false;
            break;
        case ISNA: //Islamic Society of North America (ISNA)
            _fajrAngle = 15;
            _ishaAngle = 15;
            _ishaIsInterval = false;
            break;
        case UmmAlQura: //Umm Al-Qura University, Makkah
            _fajrAngle = 18.5;
            _ishaInterval = 90;
            _ishaIsInterval = true;
            break;
        case Egyptian: //Egyptian General Authority of Survey
            _fajrAngle = 19.5;
            _ishaAngle = 17.5;
            _ishaIsInterval = false;
            break;
        case Karachi: //University of Islamic Sciences, Karachi 
        _fajrAngle = 18;
        _ishaAngle = 18;
        _ishaIsInterval = false;
        break;
        case Tehran: //Institute of Geophysics, University of Tehran 
        _fajrAngle = 17.7;
        _ishaAngle = 14;
        _ishaIsInterval = false;
        break;
        case Jafari: //Shia Ithna-Ashari, Leva Institute, Qum 
        _fajrAngle = 16;
        _ishaAngle = 14;
        _ishaIsInterval = false;
        break;
        case Gulf: // Gulf Region 
        _fajrAngle = 19.5;
        _ishaInterval = 90;
        _ishaIsInterval = true;
        break;
        case Kuwait: // Kuwait 
        _fajrAngle = 18;
        _ishaAngle = 17.5;
        _ishaIsInterval = false;
        break;
        case Qatar: // Qatar 
        _fajrAngle = 18;
        _ishaInterval = 90;
        _ishaIsInterval = true;
        break;
        case Singapore: // Majlis Ugama Islam Singapura, Singapore 
        _fajrAngle = 20;
        _ishaAngle = 18;
        _ishaIsInterval = false;
        break;
        case France: // Union Organization Islamic de France 
        _fajrAngle = 20;
        _ishaAngle = 18;
        _ishaIsInterval = false;
        break;
        case Turkey: // Diyanet İşleri Başkanlığı, Turkey (experimental) 
        _fajrAngle = 18;
        _ishaAngle = 17;
        _ishaIsInterval = false;
        break;
        case Russia: // Spiritual Administration of Muslims of Russia 
        _fajrAngle = 16;
        _ishaAngle = 15;
        _ishaIsInterval = false;
        break;
        case Dubai: // Dubai (experimental) 
        _fajrAngle = 18.2;
        _ishaAngle = 18.2;
        _ishaIsInterval = false;
        break;
        case JAKIM: // Jabatan Kemajuan Islam Malaysia (JAKIM) 
        _fajrAngle = 20;
        _ishaAngle = 18;
        _ishaIsInterval = false;
        break;
        case Tunisia: // Tunisia
        _fajrAngle = 18;
        _ishaAngle = 18;
        _ishaIsInterval = false;
        break;
        case Algeria: // Algeria
        _fajrAngle = 18;
        _ishaAngle = 17;
        _ishaIsInterval = false;
        break;
        case Indonesia: // Kementerian Agama Republik Indonesia
        _fajrAngle = 20;
        _ishaAngle = 18;
        _ishaIsInterval = false;
        break;
        case Morocco: // Morocco
        _fajrAngle = 19;
        _ishaAngle = 17;
        _ishaIsInterval = false;
        break;
        case Portugal: // Comunidade Islamica de Lisboa
        _fajrAngle = 18;
        _ishaInterval = 77;
        _ishaIsInterval = true;
        break;
        case Jordan: // Ministry of Awqaf, Islamic Affairs and Holy Places, Jordan
        _fajrAngle = 18;
        _ishaAngle = 18;
        _ishaIsInterval = false;
        break;
    }
}

void PrayerTimes::setAsrMethod(AsrMethod method) {
    _asrFactor = (method == Hanafi) ? 2.0 : 1.0;
}

void PrayerTimes::setDST(bool dstOn) {
    _dst = dstOn;
}

float PrayerTimes::deg2rad(float degrees) { 
    return degrees * (PI / 180); 
}

float PrayerTimes::rad2deg(float radians) { 
    return radians * (180 / PI); 
}

int PrayerTimes::calculateDayOfYear(int day, int month, int year) {
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) daysInMonth[1] = 29;
    int doy = day;
    for (int i = 0; i < month - 1; ++i) doy += daysInMonth[i];
    return doy;
}

void PrayerTimes::calculateSolarParameters(int dayOfYear, float &eqTime, float &solarDec) {
    float gamma = 2.0 * PI / 365.0 * (dayOfYear - 1);
    eqTime = 229.18 * (
        0.000075 + 0.001868 * cos(gamma) - 0.032077 * sin(gamma)
        - 0.014615 * cos(2 * gamma) - 0.040849 * sin(2 * gamma));
    solarDec = 0.006918 - 0.399912 * cos(gamma) + 0.070257 * sin(gamma)
             - 0.006758 * cos(2 * gamma) + 0.000907 * sin(2 * gamma)
             - 0.002697 * cos(3 * gamma) + 0.00148 * sin(3 * gamma);
}

float PrayerTimes::calculateDhuhr(float eqTime) {
    return 720 - 4 * (_longitude - (_timeZone * 15)) - eqTime + (_dst ? 60 : 0);
}

float PrayerTimes::calculateTimeForAngle(float angle, float Dhuhr, float solarDec, bool isMorning) {
    float ha = acos((sin(deg2rad(angle)) - sin(deg2rad(_latitude)) * sin(solarDec)) /
                    (cos(deg2rad(_latitude)) * cos(solarDec)));
    float delta = rad2deg(ha) * 4;
    return isMorning ? Dhuhr - delta : Dhuhr + delta;
}

void PrayerTimes::convertToHoursMinutes(float time, int &hours, int &minutes) {
    hours = floor(time / 60);
    minutes = round(time - (hours * 60));
}

String PrayerTimes::formatTime(int hour, int minute) {
    int h = hour % 12;
    if (h == 0) h = 12;
    String period = (hour < 12) ? "AM" : "PM";
    char buf[10];
    sprintf(buf, "%d:%02d %s", h, minute, period.c_str());
    return String(buf);
}

String PrayerTimes::formatTime24WithPeriod(int hour, int minute) {
    String period = (hour < 12) ? "AM" : "PM";

    char buf[10];
    sprintf(buf, "%02d:%02d %s", hour, minute, period.c_str());

    return String(buf);
}

void PrayerTimes::calculate(int day, int month, int year,
    int &fajrHour, int &fajrMinute,
    int &sunriseHour, int &sunriseMinute,
    int &dhuhrHour, int &dhuhrMinute,
    int &asrHour, int &asrMinute,
    int &maghribHour, int &maghribMinute,
    int &ishaHour, int &ishaMinute) {

    int dayOfYear = calculateDayOfYear(day, month, year);
    float eqTime, solarDec;
    calculateSolarParameters(dayOfYear, eqTime, solarDec);

    float Dhuhr = calculateDhuhr(eqTime) + _adjDhuhr;
    convertToHoursMinutes(Dhuhr, dhuhrHour, dhuhrMinute);

    float fajr = calculateTimeForAngle(-_fajrAngle, Dhuhr, solarDec, true) + _adjFajr;
    convertToHoursMinutes(fajr, fajrHour, fajrMinute);

    float sunrise = calculateTimeForAngle(-0.833, Dhuhr, solarDec, true) + _adjSunrise;
    convertToHoursMinutes(sunrise, sunriseHour, sunriseMinute);

    float maghrib = calculateTimeForAngle(-0.833, Dhuhr, solarDec, false) + _adjMaghrib;
    convertToHoursMinutes(maghrib, maghribHour, maghribMinute);

    float isha = _ishaIsInterval ? maghrib + _ishaInterval : calculateTimeForAngle(-_ishaAngle, Dhuhr, solarDec, false);
    isha += _adjIsha;
    convertToHoursMinutes(isha, ishaHour, ishaMinute);

    float angle = rad2deg(atan(1.0 / (_asrFactor + tan(fabs(deg2rad(_latitude) - solarDec)))));
    float haAsr = acos((sin(deg2rad(angle)) - sin(deg2rad(_latitude)) * sin(solarDec)) /
                       (cos(deg2rad(_latitude)) * cos(solarDec)));
    float asr = Dhuhr + rad2deg(haAsr) * 4 + _adjAsr;
    convertToHoursMinutes(asr, asrHour, asrMinute);
}
