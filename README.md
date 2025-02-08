# PrayerTimes Library

A lightweight Arduino library for calculating Islamic prayer times based on multiple calculation methods. Supports various regions and methods such as MWL, ISNA, Umm al-Qura, Egyptian, Karachi, Tehran, and Jafari.

## Features
- Supports multiple **calculation methods**:
  - **Muslim World League (MWL)**
  - **Islamic Society of North America (ISNA)**
  - **Umm al-Qura (Makkah)**
  - **Egyptian General Authority of Survey**
  - **University of Islamic Sciences, Karachi** (Hanafi Asr)
  - **Institute of Geophysics, University of Tehran**
  - **Jafari (Shia Ithna Ashari)**
- Computes all **five daily prayer times** + **sunrise**:
  - **Fajr**, **Sunrise**, **Dhuhr**, **Asr**, **Maghrib**, **Isha**
- Allows **manual adjustments** to fine-tune prayer times
- Provides **formatted output** in a **12-hour AM/PM format**
- Optimized for **Arduino and ESP-based microcontrollers**
- Simple and intuitive **API** for developers

## Installation

### Manual Installation
1. Download the latest release from [GitHub](https://github.com/a-saab/PrayerTimes)
2. Extract the folder and place it in your Arduino libraries directory:
   - Windows: `Documents\Arduino\libraries`
   - macOS: `~/Documents/Arduino/libraries`
   - Linux: `~/Arduino/libraries`
3. Restart the Arduino IDE.

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

## License
This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

## Contributing
Contributions are welcome! Feel free to submit **issues**, **feature requests**, and **pull requests**.

## Credits
Developed by **Adnan Saab**.

---
📌 **GitHub Repository:** [https://github.com/a-saab/PrayerTimes](https://github.com/a-saab/PrayerTimes)

