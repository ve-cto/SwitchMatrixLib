/*!
 *  @file       generic_example.cpp
 *  Project     SwitchMatrix Library
 *  @brief      Arduino libary for interfacing with button/switch diode matrices.
 *  @author     ve-cto
 *  @date       17/09/2026main
 *  @license    MIT - Copyright (c) 2026 ve-cto
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <SwitchMatrix.h>
#include <vector>

const u_int rowPins[] = {0, 1, 2, 3, 4, 5, 6};
const u_int colPins[] = {7, 8, 9, 10};

SwitchMatrix matrix(rowPins, colPins, 7, 4, false, false);

void pressCallback() {
  Serial.println("Pressed!");
}

void heldCallback() {
  Serial.println("Holding....");
}

void releaseCallback() {
  Serial.println("Released!");
}

void globalPressCallback(uint row, uint col) {
  String msg = "Button (" + String(row) + ", " + String(col) + ") got pressed!";
  Serial.println(msg);
}

void setup() {
  Serial.begin(115200);

  matrix.setPoweredSwitchRateUs(3);                             // 3 microsecond break between recording samples & turning an output pin on/off
  matrix.setDebounceSamples(10);                                // gather 10 samples for debouncing
  matrix.setMinPollDtMs(5);                                     // 5 millisecond break between each round of sampling 
  matrix.attachPressCallbackEvent(1, 1, pressCallback);         // when button (1, 1) is pressed
  matrix.attachHeldCallbackEvent(1, 1, heldCallback);           // while button (1, 1) is held, run repeatedly
  matrix.attachReleaseCallbackEvent(1, 1, releaseCallback);     // when button (1, 1) is released
  matrix.attachGlobalPressCallbackEvent(globalPressCallback);   // when any button is pressed
}

void loop() {
  matrix.poll(); // update the matrix
}