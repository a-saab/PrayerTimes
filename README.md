# PrayerTimes Library

A lightweight Arduino library for calculating Islamic prayer times based on multiple calculation methods. Supports various regions and methods such as MWL, ISNA, Umm al-Qura, Egyptian, Karachi, Tehran, and Jafari.

**v2.0 Highlights:** Input validation, error handling, high-latitude polar region support, and automatic smart defaults.

## Quick Start

```cpp
#include "PrayerTimes.h"

// Create with Montreal coordinates and timezone (UTC-5 = -300 minutes)
PrayerTimes pt(45.5017, -73.5673, -300);

// Check if coordinates are valid
if (!pt.isInitialized()) {
    Serial.println("Invalid coordinates!");
    return;
}

// Set calculation method
pt.setCalculationMethod(CalculationMethods::ISNA);

// Auto-detect and handle high-latitude locations
if (pt.isHighLatitude()) {
    pt.setHighLatitudeRule(MIDDLE_OF_NIGHT);
}

// Calculate prayer times
PrayerTimesResult result = pt.calculate(22, 12, 2025);

if (result.valid) {
    int fajrH, fajrM;
    pt.minutesToTime(result.fajr, fajrH, fajrM);
    Serial.println(pt.formatTime12(fajrH, fajrM));  // 6:01 AM
} else {
    Serial.println(result.errorMessage);  // Check for errors
}
```

## Features
- Supports multiple **calculation methods**:
  - **Muslim World League (MWL)**
  - **Islamic Society of North America (ISNA)**
  - **Umm al-Qura (Makkah)**
  - **Egyptian General Authority of Survey**
  - **University of Islamic Sciences, Karachi** (Hanafi Asr)
  - **Institute of Geophysics, University of Tehran**
  - **Jafari (Shia Ithna Ashari)**
  - Plus 15+ additional methods for worldwide coverage
- Computes all **five daily prayer times** + **sunrise**:
  - **Fajr**, **Sunrise**, **Dhuhr**, **Asr**, **Maghrib**, **Isha**
- **Input validation** for coordinates (±90° latitude, ±180° longitude) and dates
- **Error handling** with diagnostic messages for invalid inputs
- **High-latitude support** (Arctic/Antarctic regions >66.5°) with four adjustment methods:
  - NONE, MIDDLE_OF_NIGHT, ONE_SEVENTH, ANGLE_BASED
- **Automatic high-latitude detection** and smart adjustment defaults
- Allows **manual adjustments** to fine-tune prayer times
- Supports **DST offsets** for daylight saving time
- Provides **formatted output** in 12-hour AM/PM or 24-hour format
- Optimized for **Arduino and ESP-based microcontrollers**
- Simple and intuitive **API** with full backward compatibility

## Installation

### Manual Installation
1. Download the latest release from [GitHub](https://github.com/a-saab/PrayerTimes)
2. Extract the folder and place it in your Arduino libraries directory:
   - Windows: `Documents\Arduino\libraries`
   - macOS: `~/Documents/Arduino/libraries`
   - Linux: `~/Arduino/libraries`
3. Restart the Arduino IDE.

## v2.0 Features & Improvements

### Input Validation
All inputs are now validated to prevent silent failures:

```cpp
PrayerTimes pt(95.0, -73.5673, -300);  // Invalid latitude (>90°)
if (!pt.isInitialized()) {
    // Caught! Coordinates are invalid
}

PrayerTimesResult result = pt.calculate(32, 13, 2025);  // Invalid date
if (!result.valid) {
    Serial.println(result.errorMessage);  // "Invalid date"
}
```

### High-Latitude Detection & Adjustment
Automatic support for polar regions with smart defaults:

```cpp
PrayerTimes tromso(69.6500, 18.9553, 60);  // Tromsø, Norway

// Auto-detect and apply appropriate adjustment
if (tromso.isHighLatitude()) {
    tromso.setHighLatitudeRule(MIDDLE_OF_NIGHT);  // Smart default
}

float latitude = tromso.getLatitude();  // Get location latitude
```

### Error Handling
All calculation results include diagnostic information:

```cpp
PrayerTimesResult result = pt.calculateWithOffset(22, 12, 2025, 60);  // DST offset

if (!result.valid) {
    // Check error message
    if (result.errorMessage != nullptr) {
        Serial.println(result.errorMessage);  // "Invalid coordinates" or "Invalid date"
    }
}
```

### DST Support
Easily handle daylight saving time:

```cpp
// Standard time
PrayerTimesResult winter = pt.calculate(22, 12, 2025);

// With DST (+60 minutes)
PrayerTimesResult summer = pt.calculateWithOffset(22, 6, 2025, 60);
```

## Available Calculation Methods

| Method                                    | Fajr Angle | Isha Angle / Interval | Asr Calculation    | Used In |
|------------------------------------------|-----------|---------------------|-------------------|---------|
| Muslim World League (MWL)                 | 18°       | 17°                 | Shadow = 1x       | Europe, Middle East, North America |
| Islamic Society of North America (ISNA)   | 15°       | 15°                 | Shadow = 1x       | North America |
| Umm al-Qura (Makkah)                      | 18.5°     | 90 min after Maghrib | Shadow = 1x       | Saudi Arabia, Gulf countries |
| Egyptian General Authority of Survey      | 19.5°     | 17.5°               | Shadow = 1x       | Egypt, Africa |
| University of Islamic Sciences, Karachi   | 18°       | 18°                 | Shadow = 2x (Hanafi) | Pakistan, India, Bangladesh |
| Institute of Geophysics, University of Tehran | 17.7°  | 14°                 | Shadow = 1x       | Iran |
| Jafari (Shia Ithna Ashari)                | 16°       | 14°                 | Shadow = 1x       | Shia communities worldwide |

## Examples

The library includes comprehensive examples:

- **Multi-City_Test.ino** - Calculate prayer times for 5 cities worldwide (Montreal, Mumbai, Tokyo, Oslo, Tromsø)
- **Advanced_Error_Handling.ino** - Demonstrates error handling, validation, and high-latitude adjustments with 7 test cases

Run these examples to see the library in action and learn best practices.

## High-Latitude Adjustment Methods

For locations beyond 66.5° latitude (Arctic/Antarctic circles), choose an adjustment method:

| Method | Use Case |
|--------|----------|
| **NONE** | Use only if latitude < 66.5° |
| **MIDDLE_OF_NIGHT** | Recommended - uses astronomical midnight as reference |
| **ONE_SEVENTH** | Divides night into sevenths |
| **ANGLE_BASED** | Uses angle-based approximation |

Example:
```cpp
// For Tromsø (69.6°N) during winter solstice
PrayerTimes tromso(69.6500, 18.9553, 60);
tromso.setHighLatitudeRule(MIDDLE_OF_NIGHT);  // Sensible times for polar night
```

## License
This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

## Contributing
Contributions are welcome! Feel free to submit **issues**, **feature requests**, and **pull requests**.

## Credits
Developed by **Adnan Saab**.

---
📌 **GitHub Repository:** [https://github.com/a-saab/PrayerTimes](https://github.com/a-saab/PrayerTimes)

