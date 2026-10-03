/*!
 *  @file       SwitchMatrix.cpp
 *  Project     SwitchMatrix Library
 *  @brief      Arduino libary for interfacing with button/switch diode matrices.
 *  @author     ve-cto
 *  @date       17/09/2026
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

#include "Arduino.h"
#include "SwitchMatrix.h"
#include <array>
#include <vector>

SwitchMatrix::SwitchMatrix(const u_int rowPins[], const u_int colPins[], u_int rows, u_int cols, bool rowsAreInputs, bool inputsNeedPullups = false) {
    if (rows == 0 || cols == 0) {return;} // TODO: out of range exception
    _rows = rows;
    _cols = cols;
    _bcount = _rows * _cols;
    _rowPins = rowPins;
    _colPins = colPins;
    _rowsAreInputs = rowsAreInputs;
    _pressedCoordinates.resize(_rows, std::vector<bool>(_cols, false)); // Make our coordinate system that tracks whether buttons are triggered the same size as the inputted grid.
    _callbackPressFunctions.resize(_rows, std::vector<CallbackFunction>(_cols, nullptr));
    _callbackReleaseFunctions.resize(_rows, std::vector<CallbackFunction>(_cols, nullptr));
    _callbackHeldFunctions.resize(_rows, std::vector<CallbackFunction>(_cols, nullptr));
    _debounceCounts.resize(_rows, std::vector<u_short>(_cols, 0));

    for (int i = 0; i < rows; i++) {
        if (rowsAreInputs) {if (inputsNeedPullups) {pinMode(rowPins[i], INPUT_PULLUP);} else {pinMode(rowPins[i], INPUT);}} else {pinMode(rowPins[i], OUTPUT); digitalWrite(rowPins[i], HIGH);}
    }
    for (int i = 0; i < cols; i++) {
        if (!rowsAreInputs) {if (inputsNeedPullups) {pinMode(colPins[i], INPUT_PULLUP);} else {pinMode(colPins[i], INPUT);}} else {pinMode(colPins[i], OUTPUT); digitalWrite(colPins[i], HIGH);}
    }
}

void SwitchMatrix::poll() {
    if (!_enabled) {return;}
    const unsigned long now = millis();
    if (now - _lastPoll >= _minPollDtMs) { // Skip
        _lastPoll = now;
    } else {
        return;
    }

    if (_rowsAreInputs) { // If rows are the inputs, we're driving the columns on and off. Else, the rows are being driven and the columns are inputs.
        for (int iter = 0; iter < _cols; iter++) {
            digitalWrite(_colPins[iter], LOW); // Write the column pin low
            delayMicroseconds(_pollOutputSwitchDelay);
            for (int i = 0; i < _rows; i++) { // Read all the row pins one by one
                if (!digitalRead(_rowPins[i])) { // If the pin is LOW, the button is pressed.
                    // _pressedCoordinates[i][iter] = true; // A button is pressed, add it to the list of pressed buttons.

                    if (_debounceCounts[i][iter] < _debounceSamples) {
                        _debounceCounts[i][iter] += u_short(1);
                    }

                    if (_debounceCounts[i][iter] >= _debounceSamples/2) {
                        if (!_pressedCoordinates[i][iter]) { // Runs once when first triggered.
                            if (_gCallbackPressFunction != nullptr) {_gCallbackPressFunction(i, iter);}
                            if (_callbackPressFunctions[i][iter] != nullptr) {_callbackPressFunctions[i][iter]();}    
                        }
                        _pressedCoordinates[i][iter] = true;
                        if (_gCallbackHeldFunction != nullptr) {_gCallbackHeldFunction(i, iter);}
                        if (_callbackHeldFunctions[i][iter] != nullptr) {_callbackHeldFunctions[i][iter]();} // Held functions get run every loop.
                    }
                } else {
                    if (_debounceCounts[i][iter] > 0) {
                        _debounceCounts[i][iter] -= u_short(1);
                    }

                    if (_debounceCounts[i][iter] < _debounceSamples/2) {
                        if (_pressedCoordinates[i][iter]) { // Runs once when first triggered.
                            if (_gCallbackReleaseFunction != nullptr) {_gCallbackReleaseFunction(i, iter);}
                            if (_callbackReleaseFunctions[i][iter] != nullptr) {_callbackReleaseFunctions[i][iter]();}    
                        }
                        _pressedCoordinates[i][iter] = false;
                    }
                }
            }
            digitalWrite(_colPins[iter], HIGH); // Reset the column pin back to high
            delayMicroseconds(_pollOutputSwitchDelay);
        }
    } else { // columns are inputs
        for (int iter = 0; iter < _rows; iter++) {
            digitalWrite(_rowPins[iter], LOW); // Write the row pin low
            delayMicroseconds(_pollOutputSwitchDelay);
            for (int i = 0; i < _cols; i++) { // Read all the row pins one by one
                if (!digitalRead(_colPins[i])) {
                    // _pressedCoordinates[iter][i] = true; // A button is pressed, add it to the list of pressed buttons.

                    if (_debounceCounts[iter][i] < _debounceSamples) {
                        _debounceCounts[iter][i] += u_short(1);
                    }

                    if (_debounceCounts[iter][i] >= _debounceSamples/2) {
                        if (!_pressedCoordinates[iter][i]) { // Runs once when first triggered.
                            if (_gCallbackPressFunction != nullptr) {_gCallbackPressFunction(iter, i);}
                            if (_callbackPressFunctions[iter][i] != nullptr) {_callbackPressFunctions[iter][i]();}    
                        }

                        _pressedCoordinates[iter][i] = true;
                        if (_gCallbackHeldFunction != nullptr) {_gCallbackHeldFunction(iter, i);}
                        if (_callbackHeldFunctions[iter][i] != nullptr) {_callbackHeldFunctions[iter][i]();} // Held functions get run every loop whilst the button is held.
                    }
                } else {
                    if (_debounceCounts[iter][i] > 0) {
                        _debounceCounts[iter][i] -= u_short(1);
                    }

                    if (_debounceCounts[iter][i] < _debounceSamples/2) {
                        if (_pressedCoordinates[iter][i]) { // Runs once when first triggered.
                            if (_gCallbackReleaseFunction != nullptr) {_gCallbackReleaseFunction(iter, i);}
                            if (_callbackReleaseFunctions[iter][i] != nullptr) {_callbackReleaseFunctions[iter][i]();}    
                        }
                        _pressedCoordinates[iter][i] = false;
                    }
                }
            }
            digitalWrite(_rowPins[iter], HIGH); // Reset the row pin back to high
            delayMicroseconds(_pollOutputSwitchDelay);
        }
    }
}

bool SwitchMatrix::getButtonPressed(uint row, uint col) {
    if (!_enabled) {return false;}
    if (row >= _rows || col >= _cols) {return false;}
    return _pressedCoordinates[row][col];
}

std::vector<std::vector<bool>> SwitchMatrix::getButtonValues() {
    if (!_enabled) {return {};}
    return _pressedCoordinates;
}

void SwitchMatrix::attachPressCallbackEvent(uint row, uint col, CallbackFunction func) {
    if (row >= _rows || col >= _cols) {return;}
    _callbackPressFunctions[row][col] = func;
}

void SwitchMatrix::attachHeldCallbackEvent(uint row, uint col, CallbackFunction func = nullptr) {
    if (row >= _rows || col >= _cols) {return;}
    _callbackHeldFunctions[row][col] = func;
}

void SwitchMatrix::attachReleaseCallbackEvent(uint row, uint col, CallbackFunction func = nullptr) {
    if (row >= _rows || col >= _cols) {return;}
    _callbackReleaseFunctions[row][col] = func;
}

void SwitchMatrix::attachGlobalPressCallbackEvent(CoordinatedCallbackFunction func) {
    _gCallbackPressFunction = func;
}

void SwitchMatrix::attachGlobalHeldCallbackEvent(CoordinatedCallbackFunction func) {
    _gCallbackHeldFunction = func;
}

void SwitchMatrix::attachGlobalReleaseCallbackEvent(CoordinatedCallbackFunction func) {
    _gCallbackReleaseFunction = func;
}

void SwitchMatrix::setEnabled(bool t) {
    _enabled = t;
}

bool SwitchMatrix::getEnabled() {
    return _enabled;
}

void SwitchMatrix::setPoweredSwitchRateUs(uint8_t rate) {
    _pollOutputSwitchDelay = rate;
}

uint8_t SwitchMatrix::getPoweredSwitchRateUs() {
    return _pollOutputSwitchDelay;
}

void SwitchMatrix::setDebounceSamples(short count) {
    _debounceSamples = count;
}

short SwitchMatrix::getDebounceSamples() {
    return _debounceSamples;
}

void SwitchMatrix::setMinPollDtMs(uint ms) {
    _minPollDtMs = ms;
}

uint SwitchMatrix::getMinPollDtMs() {
    return _minPollDtMs;
}

std::array<uint, 2> SwitchMatrix::getSize() {
    return { _rows, _cols };
}