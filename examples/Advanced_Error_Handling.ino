#include <Arduino.h>
#include "PrayerTimes.h"

void setup() {
    Serial.begin(9600);
    delay(2000);

    Serial.println("=== PrayerTimes v2 - Error Handling & High-Latitude Examples ===\n");

    // Example 1: Invalid coordinates (out of range)
    Serial.println("--- Test 1: Invalid Latitude ---");
    PrayerTimes invalidLat(95.0, -73.5673, -300);  // Latitude > 90
    if (!invalidLat.isInitialized()) {
        Serial.println("ERROR: Invalid latitude detected correctly!");
    }

    // Example 2: Valid Montreal coordinates
    Serial.println("\n--- Test 2: Valid Montreal (45.5°N) ---");
    PrayerTimes montreal(45.5017, -73.5673, -300);
    if (montreal.isInitialized()) {
        PrayerTimesResult result = montreal.calculate(22, 12, 2025);
        montreal.setCalculationMethod(CalculationMethods::ISNA);
        
        if (result.valid) {
            Serial.println("Calculation successful");
            int h, m;
            montreal.minutesToTime(result.fajr, h, m);
            Serial.print("Fajr: ");
            Serial.println(montreal.formatTime12(h, m));
        } else {
            Serial.print("ERROR: ");
            Serial.println(result.errorMessage);
        }
    }

    // Example 3: Invalid date
    Serial.println("\n--- Test 3: Invalid Date ---");
    PrayerTimes london(51.5074, -0.1278, 0);
    if (london.isInitialized()) {
        PrayerTimesResult result = london.calculate(32, 13, 2025);  // Invalid month and day
        if (!result.valid) {
            Serial.print("Date validation caught error: ");
            Serial.println(result.errorMessage);
        }
    }

    // Example 4: High latitude (Oslo) - without adjustment
    Serial.println("\n--- Test 4: High Latitude Oslo (59.9°N) - Without adjustment ---");
    PrayerTimes oslo(59.9139, 10.7522, 60);
    oslo.setCalculationMethod(CalculationMethods::ISNA);
    oslo.setHighLatitudeRule(NONE);
    
    PrayerTimesResult osloNoAdj = oslo.calculate(21, 12, 2025);  // Winter solstice
    if (osloNoAdj.valid) {
        Serial.println("Times (within reasonable range, but close to boundary):");
        int h, m;
        oslo.minutesToTime(osloNoAdj.fajr, h, m);
        Serial.print("Fajr: ");
        Serial.println(oslo.formatTime12(h, m));
        oslo.minutesToTime(osloNoAdj.isha, h, m);
        Serial.print("Isha: ");
        Serial.println(oslo.formatTime12(h, m));
    }

    // Example 5: High latitude with adjustment rule
    Serial.println("\n--- Test 5: High Latitude Oslo (59.9°N) - With MIDDLE_OF_NIGHT adjustment ---");
    PrayerTimes osloAdj(59.9139, 10.7522, 60);
    osloAdj.setCalculationMethod(CalculationMethods::ISNA);
    osloAdj.setHighLatitudeRule(MIDDLE_OF_NIGHT);
    
    PrayerTimesResult osloWithAdj = osloAdj.calculate(21, 12, 2025);
    if (osloWithAdj.valid) {
        Serial.println("Times (with MIDDLE_OF_NIGHT adjustment):");
        int h, m;
        osloAdj.minutesToTime(osloWithAdj.fajr, h, m);
        Serial.print("Fajr: ");
        Serial.println(osloAdj.formatTime12(h, m));
        osloAdj.minutesToTime(osloWithAdj.isha, h, m);
        Serial.print("Isha: ");
        Serial.println(osloAdj.formatTime12(h, m));
    }

    // Example 6: POLAR REGION - Tromsø, Norway (69.6°N)
    // This is where high-latitude adjustments are CRITICAL
    Serial.println("\n--- Test 6: POLAR REGION Tromsø (69.6°N) - Winter Solstice ---");
    Serial.println("Note: Tromsø has polar night in winter (sun doesn't rise)");
    
    // Without adjustment: times may be nonsensical
    PrayerTimes tromsoNoAdj(69.6500, 18.9553, 60);
    tromsoNoAdj.setCalculationMethod(CalculationMethods::ISNA);
    tromsoNoAdj.setHighLatitudeRule(NONE);
    
    PrayerTimesResult tromsoNo = tromsoNoAdj.calculate(21, 12, 2025);
    if (tromsoNo.valid) {
        Serial.println("WITHOUT adjustment (unreliable):");
        int h, m;
        tromsoNoAdj.minutesToTime(tromsoNo.sunrise, h, m);
        Serial.print("  Sunrise: ");
        Serial.println(tromsoNoAdj.formatTime12(h, m));
        tromsoNoAdj.minutesToTime(tromsoNo.fajr, h, m);
        Serial.print("  Fajr: ");
        Serial.println(tromsoNoAdj.formatTime12(h, m));
    }
    
    // With adjustment: sensible times based on night middle
    PrayerTimes tromsoAdj(69.6500, 18.9553, 60);
    tromsoAdj.setCalculationMethod(CalculationMethods::ISNA);
    tromsoAdj.setHighLatitudeRule(MIDDLE_OF_NIGHT);
    
    PrayerTimesResult tromsoYes = tromsoAdj.calculate(21, 12, 2025);
    if (tromsoYes.valid) {
        Serial.println("WITH MIDDLE_OF_NIGHT adjustment (recommended for polar regions):");
        int h, m;
        tromsoAdj.minutesToTime(tromsoYes.fajr, h, m);
        Serial.print("  Fajr: ");
        Serial.println(tromsoAdj.formatTime12(h, m));
        tromsoAdj.minutesToTime(tromsoYes.isha, h, m);
        Serial.print("  Isha: ");
        Serial.println(tromsoAdj.formatTime12(h, m));
    }
    
    // Show high-latitude detection
    Serial.print("Is Tromsø high-latitude? ");
    Serial.println(tromsoAdj.isHighLatitude() ? "YES" : "NO");
    Serial.print("Latitude: ");
    Serial.println(tromsoAdj.getLatitude());

    // Example 7: DST offset
    Serial.println("\n--- Test 7: DST Handling (Montreal with DST) ---");
    PrayerTimes montrealDST(45.5017, -73.5673, -300);
    montrealDST.setCalculationMethod(CalculationMethods::ISNA);
    
    PrayerTimesResult noDST = montrealDST.calculate(22, 6, 2025);
    PrayerTimesResult withDST = montrealDST.calculateWithOffset(22, 6, 2025, 60);
    
    if (noDST.valid && withDST.valid) {
        int h1, m1, h2, m2;
        montrealDST.minutesToTime(noDST.fajr, h1, m1);
        montrealDST.minutesToTime(withDST.fajr, h2, m2);
        
        Serial.print("Fajr without DST: ");
        Serial.println(montrealDST.formatTime12(h1, m1));
        Serial.print("Fajr with DST:    ");
        Serial.println(montrealDST.formatTime12(h2, m2));
    }

    Serial.println("\n=== High-Latitude Guidance ===");
    Serial.println("Arctic Circle (66.5°) and beyond require special handling:");
    Serial.println("- NONE: Use only if latitude < 66.5°");
    Serial.println("- MIDDLE_OF_NIGHT: Best for typical locations");
    Serial.println("- ONE_SEVENTH: Divides night into sevenths");
    Serial.println("- ANGLE_BASED: Uses angle approximation");
    Serial.println("Always verify times make sense for the season!");

    Serial.println("\n=== End of Tests ===");
}

void loop() {}
