#include <Arduino.h>
#include "PrayerTimes.h"

// Fixed test date for all cities
static const int DAY   = 22;
static const int MONTH = 12;
static const int YEAR  = 2025;

// Helper function to get month name
const char* getMonthName(int month) {
  const char* months[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
  };
  if (month >= 1 && month <= 12) {
    return months[month - 1];
  }
  return "Unknown";
}

// Helper function to format date as "December 22, 2025"
String formatDate(int day, int month, int year) {
  char buffer[32];
  sprintf(buffer, "%s %d, %d", getMonthName(month), day, year);
  return String(buffer);
}

// Helper function to format date as "2025-12-22"
String formatDateISO(int day, int month, int year) {
  char buffer[16];
  sprintf(buffer, "%04d-%02d-%02d", year, month, day);
  return String(buffer);
}

void printCity(
  const char* label,
  float lat,
  float lon,
  int tzMinutes
) {
  PrayerTimes pt(lat, lon, tzMinutes);
  
  // For high latitude locations, use adjustment
  if (pt.isHighLatitude()) {
    pt.setHighLatitudeRule(MIDDLE_OF_NIGHT);
  }
  
  pt.setCalculationMethod(CalculationMethods::ISNA);
  pt.setAdjustments(0, 0, 0, 0, 0, 0);

  int fajrH, fajrM, sunriseH, sunriseM;
  int dhuhrH, dhuhrM, asrH, asrM;
  int maghribH, maghribM, ishaH, ishaM;

  pt.calculate(
    DAY, MONTH, YEAR,
    fajrH, fajrM,
    sunriseH, sunriseM,
    dhuhrH, dhuhrM,
    asrH, asrM,
    maghribH, maghribM,
    ishaH, ishaM
  );

  Serial.println();
  Serial.print("=== "); Serial.print(label);
  if (pt.isHighLatitude()) {
    Serial.print(" (High Latitude: ");
    Serial.print(lat, 1);
    Serial.print("°)");
  }
  Serial.println(" ===");
  Serial.print("Fajr:    "); Serial.println(pt.formatTime12(fajrH, fajrM));
  Serial.print("Sunrise: "); Serial.println(pt.formatTime12(sunriseH, sunriseM));
  Serial.print("Dhuhr:   "); Serial.println(pt.formatTime12(dhuhrH, dhuhrM));
  Serial.print("Asr:     "); Serial.println(pt.formatTime12(asrH, asrM));
  Serial.print("Maghrib: "); Serial.println(pt.formatTime12(maghribH, maghribM));
  Serial.print("Isha:    "); Serial.println(pt.formatTime12(ishaH, ishaM));
  
  if (pt.isHighLatitude()) {
    Serial.println("* High latitude adjustments applied");
  }
}

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("=== PrayerTimes v2 Multi-City Test ===");
  
  // Option 1: Friendly format
  Serial.print("Date: ");
  Serial.println(formatDate(DAY, MONTH, YEAR));
  
  // Option 2: ISO format (uncomment if preferred)
  // Serial.print("Date: ");
  // Serial.println(formatDateISO(DAY, MONTH, YEAR));
  
  Serial.println("Method: ISNA");
  Serial.println("----------------------------------");

  // Montreal, Canada (UTC-5)
  printCity(
    "Montreal, Canada",
    45.5017,
    -73.5673,
    -300
  );

  // Mumbai, India (UTC+5:30)
  printCity(
    "Mumbai, India",
    19.0760,
    72.8777,
    330
  );

  // Tokyo, Japan (UTC+9)
  printCity(
    "Tokyo, Japan",
    35.6762,
    139.6503,
    540
  );

  // Oslo, Norway (UTC+1, winter, high latitude)
  printCity(
    "Oslo, Norway",
    59.9139,
    10.7522,
    60
  );

  // Tromsø, Norway (UTC+1, polar region - 69.6°N)
  // This is in the Arctic Circle where special handling is needed
  printCity(
    "Tromsø, Norway (POLAR REGION)",
    69.6500,
    18.9553,
    60
  );

  Serial.println();
  Serial.println("=== End of Test ===");
}

void loop() {}