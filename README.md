## Medication Reminder System

<div align="center">
   <img src="https://media.giphy.com/media/mFDWuDppjQJjite6FS/giphy.gif" width="200"/>
</div>

<div style="text-align: justify">
This project implements a medication reminder system using an Arduino microcontroller. The system integrates various components to manage and remind users of their medication schedules.
</div>

---

### 🛠️ Components

![Arduino Uno](https://img.shields.io/badge/Arduino_Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![RTC DS1302](https://img.shields.io/badge/RTC_DS1302-FF6600?style=for-the-badge&logo=arduino&logoColor=white)
![LCD Display](https://img.shields.io/badge/LCD_Display-000000?style=for-the-badge&logo=arduino&logoColor=white)
![Buzzer](https://img.shields.io/badge/Buzzer-FFD700?style=for-the-badge&logo=arduino&logoColor=black)
![LEDs](https://img.shields.io/badge/LEDs-00FF00?style=for-the-badge&logo=arduino&logoColor=black)
![Bluetooth Module](https://img.shields.io/badge/Bluetooth_Module-007BFF?style=for-the-badge&logo=bluetooth&logoColor=white)
![Button](https://img.shields.io/badge/Button-FFA500?style=for-the-badge&logo=arduino&logoColor=white)

### ⚙️ Features

- **Multi-Treatment Configuration**: Configure up to 5 different medication schedules.
- **Alarm System**: Alerts with a buzzer and LED when it's time for medication.
- **Remote Configuration**: Set up treatment times, frequencies, medication names, and dosages via Bluetooth.
- **Date and Time Handling**: Maintains accurate time and date using the RTC module.

### 💻 How It Works

1. **Initialization**: Configures the RTC module and sets the current time if needed.
2. **Alarm Activation**: When the time matches a scheduled treatment, the buzzer sounds, and the corresponding LED lights up.
3. **Alarm Management**: Use the button to silence alarms and update the next alarm based on the set frequency.
4. **LCD Display**: Shows current date, time, and medication details when an alarm is active.
5. **Bluetooth Configuration**: Allows remote configuration of treatment schedules via Bluetooth.

### ⚙️ Setup

1. **Wiring**: Connect the components to the Arduino as specified in the code.
2. **Upload Code**: Load the Arduino sketch onto your microcontroller using the Arduino IDE.
3. **Bluetooth Setup**: Pair with the Bluetooth module to send configuration data.

### 📜 Code

The code for this project is available in the `main.ino` file. It includes functions for handling the RTC, alarms, LCD, and Bluetooth communication.

### 🤝 Contributing

Feel free to explore, modify, and contribute to the project. Pull requests and issues are welcome!

### 📫 Contact

For questions or collaborations, you can reach me at [juanubaldo2@gmail.com](mailto:juanubaldo2@gmail.com).

### 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

<div align="center">
  <img src="https://media.giphy.com/media/l4FGwHEUCGILz9Kaz/giphy.gif" width="300"/>
</div>
