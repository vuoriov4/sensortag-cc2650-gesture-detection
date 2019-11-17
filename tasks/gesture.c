#include <stdio.h>
#include <math.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include "Board.h"
#include "Prototypes.h"
#include "Constants.h"

int debugPrintFlag = 0;
float timestampData[ACC_SENSOR_DATA_SIZE];
float accDataX[ACC_SENSOR_DATA_SIZE];
float accDataY[ACC_SENSOR_DATA_SIZE];
float accDataZ[ACC_SENSOR_DATA_SIZE];
float gyroDataX[ACC_SENSOR_DATA_SIZE];
float gyroDataY[ACC_SENSOR_DATA_SIZE];
float gyroDataZ[ACC_SENSOR_DATA_SIZE];
float accTrnDataX[ACC_SENSOR_DATA_SIZE];
float accTrnDataY[ACC_SENSOR_DATA_SIZE];
float accTrnDataZ[ACC_SENSOR_DATA_SIZE];
float gyroTrnDataX[ACC_SENSOR_DATA_SIZE];
float gyroTrnDataY[ACC_SENSOR_DATA_SIZE];
float gyroTrnDataZ[ACC_SENSOR_DATA_SIZE];

Void resetData() {
    int i;
    for (i = 0; i < ACC_SENSOR_DATA_SIZE; i++) {
        accDataX[i] = 0.0f;
        accDataY[i] = 0.0f;
        accDataZ[i] = 0.0f;
        gyroDataX[i] = 0.0f;
        gyroDataY[i] = 0.0f;
        gyroDataZ[i] = 0.0f;
    }
}

Void initializeData() {
  int i;
  for (i = 0; i < ACC_SENSOR_DATA_SIZE; i++) {
      accDataX[i] = 0.0f;
      accDataY[i] = 0.0f;
      accDataZ[i] = 0.0f;
      gyroDataX[i] = 0.0f;
      gyroDataY[i] = 0.0f;
      gyroDataZ[i] = 0.0f;
      accTrnDataX[i] = 0.0f;
      accTrnDataY[i] = 0.0f;
      accTrnDataZ[i] = 0.0f;
      gyroTrnDataX[i] = 0.0f;
      gyroTrnDataY[i] = 0.0f;
      gyroTrnDataZ[i] = 0.0f;
  }
}

int calculateGesture() {
    float accSimilarity = 0.0f;
    float accDataMagnitude = 0.0f;
    float accTrnDataMagnitude = 0.0;
    float gyroSimilarity = 0.0f;
    float gyroDataMagnitude = 0.0f;
    float gyroTrnDataMagnitude = 0.0f;
    float amx = 0.0;  float amy = 0.0;  float amz = 0.0;
    float atmx = 0.0; float atmy = 0.0; float atmz = 0.0;
    float gmx = 0.0;  float gmy = 0.0;  float gmz = 0.0;
    float gtmx = 0.0; float gtmy = 0.0; float gtmz = 0.0;
    // mean
    int i; for (i = 0; i < ACC_SENSOR_DATA_SIZE; i++) {
      amx += accDataX[i]; amy += accDataY[i]; amz += accDataZ[i];
      atmx += accTrnDataX[i]; atmy += accTrnDataY[i]; atmz += accTrnDataZ[i];
      gmx += gyroDataX[i]; gmy += gyroDataY[i]; gmz += gyroDataZ[i];
      gtmx += gyroTrnDataX[i]; gtmy += gyroTrnDataY[i]; gtmz += gyroTrnDataZ[i];
    }
    amx /= ACC_SENSOR_DATA_SIZE; amy /= ACC_SENSOR_DATA_SIZE; amz /= ACC_SENSOR_DATA_SIZE;
    atmx /= ACC_SENSOR_DATA_SIZE; atmy /= ACC_SENSOR_DATA_SIZE; atmz /= ACC_SENSOR_DATA_SIZE;
    gmx /= ACC_SENSOR_DATA_SIZE; gmy /= ACC_SENSOR_DATA_SIZE; gmz /= ACC_SENSOR_DATA_SIZE;
    gtmx /= ACC_SENSOR_DATA_SIZE; gtmy /= ACC_SENSOR_DATA_SIZE; gtmz /= ACC_SENSOR_DATA_SIZE;
    // corr
    for (i = 0; i < ACC_SENSOR_DATA_SIZE; i++) {
      accSimilarity += (accDataX[i] - amx) * (accTrnDataX[i] - atmx) + (accDataY[i] - amy) * (accTrnDataY[i] - atmy)  + (accDataZ[i] - amz) * (accTrnDataZ[i] - atmz);
      accDataMagnitude += (accDataX[i] - amx) * (accDataX[i] - amx) + (accDataY[i] - amy) * (accDataY[i] - amy) + (accDataZ[i] - amz) * (accDataZ[i] - amz);
      accTrnDataMagnitude += (accTrnDataX[i] - atmx) * (accTrnDataX[i] - atmx) + (accTrnDataY[i] - atmy) * (accTrnDataY[i] - atmy) + (accTrnDataZ[i] - atmz) * (accTrnDataZ[i] - atmz);
      gyroSimilarity += (gyroDataX[i] - gmx) * (gyroTrnDataX[i] - gtmx) + (gyroDataY[i] - gmy) * (gyroTrnDataY[i] - gtmy)  + (gyroDataZ[i] - gmz) * (gyroTrnDataZ[i] - gtmz);
      gyroDataMagnitude += (gyroDataX[i] - gmx) * (gyroDataX[i] - gmx) + (gyroDataY[i] - gmy) * (gyroDataY[i] - gmy) + (gyroDataZ[i] - gmz) * (gyroDataZ[i] - gmz);
      gyroTrnDataMagnitude += (gyroTrnDataX[i] - gtmx) * (gyroTrnDataX[i] - gtmx) + (gyroTrnDataY[i] - gtmy) * (gyroTrnDataY[i] - gtmy) + (gyroTrnDataZ[i] - gtmz) * (gyroTrnDataZ[i] - gtmz);
    }
    accSimilarity /= sqrt(accDataMagnitude * accTrnDataMagnitude);
    gyroSimilarity /= sqrt(gyroDataMagnitude * gyroTrnDataMagnitude);
    //char sax[16]; char say[16];            
    //snprintf (sax, sizeof(sax), "%f", accSimilarity);
    //snprintf (say, sizeof(say), "%f", gyroSimilarity);
    //printf("%s, %s\n", sax, say);
    if (sqrt(accSimilarity*accSimilarity + gyroSimilarity * gyroSimilarity) > GESTURE_THRESHOLD) return true;
    return false;
}

Void recordData(float ax, float ay, float az, float gx, float gy, float gz, float timestamp) {
    int i; for (i = ACC_SENSOR_DATA_SIZE - 1; i > 0; i--) {        
        // shift
        accDataX[i] = accDataX[i-1];
        accDataY[i] = accDataY[i-1];
        accDataZ[i] = accDataZ[i-1];
        gyroDataX[i] = gyroDataX[i-1];
        gyroDataY[i] = gyroDataY[i-1];
        gyroDataZ[i] = gyroDataZ[i-1];
        timestampData[i] = timestampData[i-1];
    }
    // push 
    int smooth = 4;
    accDataX[0] = ax / smooth;
    accDataY[0] = ay / smooth;
    accDataZ[0] = az / smooth;
    gyroDataX[0] = gx / smooth;
    gyroDataY[0] = gy / smooth;
    gyroDataZ[0] = gz / smooth;
    for (i = 1; i < smooth; i++) {
        accDataX[0] += accDataX[i] / smooth;
        accDataY[0] += accDataY[i] / smooth;
        accDataZ[0] += accDataZ[i] / smooth;
        gyroDataX[0] += gyroDataX[i] / smooth;
        gyroDataY[0] += gyroDataY[i] / smooth;
        gyroDataZ[0] += gyroDataZ[i] / smooth;
    }
    timestampData[0] = timestamp;
}

Void recordTrainingData(float ax, float ay, float az, float gx, float gy, float gz, float timestamp) {
    int i; for (i = ACC_SENSOR_DATA_SIZE - 1; i > 0; i--) {        
        // shift
        accTrnDataX[i] = accTrnDataX[i-1];
        accTrnDataY[i] = accTrnDataY[i-1];
        accTrnDataZ[i] = accTrnDataZ[i-1];
        gyroTrnDataX[i] = gyroTrnDataX[i-1];
        gyroTrnDataY[i] = gyroTrnDataY[i-1];
        gyroTrnDataZ[i] = gyroTrnDataZ[i-1];
        timestampData[i] = timestampData[i-1];
    }
    // push 
    int smooth = 4;
    accTrnDataX[0] = ax / smooth;
    accTrnDataY[0] = ay / smooth;
    accTrnDataZ[0] = az / smooth;
    gyroTrnDataX[0] = gx / smooth;
    gyroTrnDataY[0] = gy / smooth;
    gyroTrnDataZ[0] = gz / smooth;
    for (i = 1; i < smooth; i++) {
    	accTrnDataX[0] += accTrnDataX[i] / smooth;
    	accTrnDataY[0] += accTrnDataY[i] / smooth;
    	accTrnDataZ[0] += accTrnDataZ[i] / smooth;
    	gyroTrnDataX[0] += gyroTrnDataX[i] / smooth;
    	gyroTrnDataY[0] += gyroTrnDataY[i] / smooth;
    	gyroTrnDataZ[0] += gyroTrnDataZ[i] / smooth;
    }
    timestampData[0] = timestamp;
}
