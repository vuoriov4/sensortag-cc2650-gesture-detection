// maintask.c
int getState(); 
Void setState(int state);
Void startMainTask();
// lcd.c
Void setLcdAccelerationText(float ax, float ay, float az);
Void setLcdGyroText(float gx, float gy, float gz);
Void setLcdStateText(int state);
Void addLcdWirelessMessage(char* msg);
Void closeLcd();
Void startLcdTask();
// buzzer.c
Void buzz(int bType);
Void startBuzzerTask();
// gesture.c
Void recordData(float ax, float ay, float az, float gx, float gy, float gz, float timestamp);
Void recordTrainingData(float ax, float ay, float az, float gx, float gy, float gz, float timestamp);
int calculateGesture();
Void initializeData();
// button.c
Void startButtonListener();
// message.c
Void startMessageTask();
// accsensor.c
Void startAccelerationSensorTask();
