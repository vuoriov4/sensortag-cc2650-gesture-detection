#define STACKSIZE 1024
#define ACC_SENSOR_DATA_SIZE 48 // abouttia rallaa 3.2 sec
#define ACC_SENSOR_SAMPLING_DELAY 50000 // 0.05 sec
#define GESTURE_THRESHOLD 0.80 // cosine similarity threshold value

enum State { STATE_IDLE, STATE_READING, STATE_RECORDING, STATE_IMAGE, STATE_GESTURE_DETECTED, STATE_MAILBOX };

enum BuzzType { SWEEP_UP, SWEEP_DOWN };

