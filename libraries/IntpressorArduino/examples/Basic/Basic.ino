#include <Intpressor.h>

const int NUM_VALUES = 3;  // Adjust the number of values as needed

void setup() {
  Serial.begin(9600);

  int values[NUM_VALUES] = { 10, 5, 3 };
  int sizes[NUM_VALUES] = { 4, 4, 4 };

  char bytes[Intpressor::MAX_VALUES];
  int extractedValues[NUM_VALUES];
  memset(bytes, 0, sizeof(bytes));
  memset(extractedValues, 0, sizeof(extractedValues));

  // Call the compress function
  int numBytes = Intpressor::compress(values, sizes, NUM_VALUES, bytes);

  // Call the extract function
  Intpressor::extract(bytes, sizes, NUM_VALUES, extractedValues);

  // Print the original and extracted values for verification
  Serial.println("Original Values:");
  for (int i = 0; i < NUM_VALUES; i++) {
    Serial.print(values[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println();

  Serial.println("Compressed Values:");
  for (int i = 0; i < numBytes; i++) {
    for (int j = 7; j >= 0; j--) {
      Serial.print((bytes[i] >> j) & 1);
    }
    Serial.print(" ");
  }
  Serial.println();
  Serial.println();

  Serial.println("Extracted Values:");
  for (int i = 0; i < NUM_VALUES; i++) {
    Serial.print(extractedValues[i]);
    Serial.print(" ");
  }
}

void loop() {
  //
}
