/*!
 *  @file       SwitchMatrix.h
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

#ifndef SwitchMatrix_h
    #define SwitchMatrix_h

#include "Arduino.h"
#if defined(ARDUINO_ARCH_AVR)
  #error "This library is not compatible with boards using AVR architecture."
#endif
#if defined(__AVR__)
  #error "This library is not compatible with boards using AVR architecture."
#endif
#include <vector>
#include <array>

using CallbackFunction = void (*)();
using CoordinatedCallbackFunction = void (*)(uint row, uint col);

class SwitchMatrix
{
    public:
        /**
         * Define a new SwitchMatrix
         * @param rowPins u_int array of pins connected to matrix rows
         * @param colPins u_int array of pins connected to matrix columns
         * @param rows u_int count of total rows
         * @param cols u_int count of total columns
         * @param rowsAreInputs bool boolean of if rows should be treated as input (false -> columns are inputs, rows are outputs)
         * @param inputsNeedPullups bool boolean of if inputs should use built-in pullup resistors (default false)
         */
        SwitchMatrix(const u_int rowPins[], const u_int colPins[], u_int rows, u_int cols, bool rowsAreInputs, bool inputsNeedPullups);
        /**
         * Initialize the matrix with values defined in its' constructor.
         * @note Needs to be called before any other methods.
         * @note poll() will not run unless begin() is called first.
         */
        void begin();
        /**
         * Iterate through all matrix elements and check for presses.
         * @note Skips if millis() is less than defined _minPollDtMs.
         * @note Skips if !_enabled.
         * @note Should be run every loop so debouncing logic works.
         */
        void poll();
        /**
         * Get whether a button is currently pressed
         * @param row uint Corresponding row to check
         * @param col uint Corresponding column to check
         * @returns bool True if the button is pressed
         * @note Returns false if the matrix is disabled
         */
        bool getButtonPressed(uint row, uint col);
        /**
         * Get the debounced table of button values.
         * @returns std::vector<std::vector<bool>> of button values
         * @note Accessible as a double-nested array. eg: var[row][col]
         */
        std::vector<std::vector<bool>> getButtonValues();
        /**
         * Attach a callback function to run when a specific button gets pressed.
         * @param row uint Corresponding row of button
         * @param col uint Corresponding column of button
         * @param func CallbackFunction Function (void) to assign to trigger.
         */
        void attachPressCallbackEvent(uint row, uint col, CallbackFunction func);
        /**
         * Attach a callback function to run continuously while a specific button is held.
         * @param row uint Corresponding row of button
         * @param col uint Corresponding column of button
         * @param func CallbackFunction Function (void) to assign to trigger.
         */
        void attachHeldCallbackEvent(uint row, uint col, CallbackFunction func);
        /**
         * Attach a callback function to run when a specific button is released.
         * @param row uint Corresponding row of button
         * @param col uint Corresponding column of button
         * @param func CallbackFunction Function (void) to assign to trigger.
         */
        void attachReleaseCallbackEvent(uint row, uint col, CallbackFunction func);
        /**
         * Attach a callback function to run when any button is pressed.
         * @param func CallbackFunction Function (void) to assign to trigger.
         */
        void attachGlobalPressCallbackEvent(CoordinatedCallbackFunction func);
        /**
         * Attach a callback function to run continously when any button is held.
         * @param func CallbackFunction Function (void) to assign to trigger.
         */
        void attachGlobalHeldCallbackEvent(CoordinatedCallbackFunction func);
        /**
         * Attach a callback function to run when any button is released.
         * @param func CallbackFunction Function (void) to assign to trigger.
         */
        void attachGlobalReleaseCallbackEvent(CoordinatedCallbackFunction func);
        /**
         * Set whether the matrix should be polling buttons
         * @param t bool Target
         */
        void setEnabled(bool t);
        /**
         * Get whether the matrix should be polling buttons
         * @returns Target
         */
        bool getEnabled();
        /**
         * Set how long the the matrix should delay after driving an output pin HIGH or LOW
         * @param rate uint8_t Delay in microseconds.
         */
        void setPoweredSwitchRateUs(uint8_t rate);
        /**
         * Get how long the the matrix should delay after driving an output pin HIGH or LOW
         * @returns Delay in microseconds.
         */
        uint8_t getPoweredSwitchRateUs();
        /**
         * Set how many samples the matrix should gather to debounce inputs.
         * @note If the button is pressed for more than half of these samples, the button is counted as pressed.
         * @param count short Samplecount.
         */
        void setDebounceSamples(short count);
        /**
         * Get how many samples the matrix should gather to debounce inputs.
         * @note If the button is pressed for more than half of these samples, the button is counted as pressed.
         */
        short getDebounceSamples();
        /**
         * Set the minimum delay between whole-matrix polls.
         * @note The poll is skipped if this time has not elapsed since the last poll.
         * @param ms uint Delay in milliseconds
         */
        void setMinPollDtMs(uint ms);
        /**
         * Get the minimum delay between whole-matrix polls.
         * @returns uint Delay in milliseconds
         */
        uint getMinPollDtMs();
        /**
         * Get the size of the matrix.
         * @returns std::array<uint, 2> Array in the format of {rows, columns}.
         */
        std::array<uint, 2> getSize();

    private:
        uint _rows;
        uint _cols;
        uint _bcount;
        int _pollIterCount = 0;
        bool _rowsAreInputs;
        bool _inputsNeedPullups;
        uint8_t _pollOutputSwitchDelay = 3;
        const u_int* _rowPins;
        const u_int* _colPins;
        std::vector<std::vector<bool>> _pressedCoordinates;
        std::vector<std::vector<u_short>> _debounceCounts;
        short _debounceSamples = 10;
        uint _minPollDtMs = 5;
        unsigned long _lastPoll = 0;
        std::vector<std::vector<CallbackFunction>> _callbackPressFunctions;
        std::vector<std::vector<CallbackFunction>> _callbackReleaseFunctions;
        std::vector<std::vector<CallbackFunction>> _callbackHeldFunctions;
        CoordinatedCallbackFunction _gCallbackPressFunction = nullptr;
        CoordinatedCallbackFunction _gCallbackHeldFunction = nullptr;
        CoordinatedCallbackFunction _gCallbackReleaseFunction = nullptr;
        bool _enabled = true;
        bool _hasInitialised = false;
};

#endif