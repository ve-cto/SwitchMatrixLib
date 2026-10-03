# SwitchMatrixLib
Library for interfacing with button/switch diode matrices.
> This documentation is incomplete, and this library is still in early development.

Made for matrices defined in this (or similar) format.
![](https://ve-cto.github.io/portfolio/diodematrix1.png "")

## Basic Implementation
Matrices are created by defining pins for rows and columns, setting the matrix size, stating whether the rows are inputs, and whether the inputs need internal pullup resistors. The library configures the pins for you.
```
const u_int rowPins[] = {0, 1, 2, 3};
const u_int colPins[] = {4, 5, 6, 7};
const boolean isRowsInputs = false;
const boolean isUsingInternalPullups = false;

SwitchMatrix matrix(rowPins, colPins, 4, 4, isRowsInputs, isUsingInternalPullups); // Create a 4*4 matrix where the columns are inputs, and using internal pullup resistors.
```
Matrices then need to be polled every loop (or however often you want it to update). Note that the library contains debouncing logic, and samples only accrue when the matrix is polled.
The matrix can then be queried for whether a coordinates' button is pressed.
```
void loop() {
    matrix.poll();
    if (matrix.getButtonPressed(2,3)) {
      // Button with coordinates (2,3) is pressed, do something!
    }
}
```
## Using Callbacks
Callback functions can be assigned to press, release, and held events. Held events rerun every poll() iteration whilst the button is pressed.
A callback function is either set as Global (IE, all buttons on the matrix can trigger it), or Local (where only one button can trigger it).

> Global callbacks must have two parameters which indicate the coordinate of the pressed button. The library calls the method with the following syntax: `globalPressCallback(uint row, uint column);` This is not needed on local callbacks.
```
void pressCallback() {
  Serial.println("Pressed!");
}

void heldCallback() {
  Serial.println("Holding....");
}

void releaseCallback() {
  Serial.println("Released!");
}

void globalPressCallback(uint row, uint column) {
  String msg = "Button (" + String(row) + ", " + String(column) + ") got pressed!";
  Serial.println(msg);
}

void setup() {
  Serial.begin(9600);
  matrix.attachPressCallbackEvent(1, 1, pressCallback); // Assign callbacks to button with coordinates (1, 1)
  matrix.attachHeldCallbackEvent(1, 1, heldCallback);
  matrix.attachReleaseCallbackEvent(1, 1, releaseCallback);
  matrix.attachGlobalPressCallbackEvent(globalPressCallback); // Assign global callback to all buttons
}

void loop() {
  matrix.poll();
}
```

## Configuring
In addition to callbacks, the library allows you to modify its' internal poll timings and other options. Getters are available for all of these methods to retrieve their values.
```
matrix.setPoweredSwitchRateUs(5); // Set how long the the matrix should delay after driving an output pin HIGH or LOW

matrix.setDebounceSamples(5); // Set how many samples the matrix should gather to debounce inputs.

matrix.setMinPollDtMs(40); // Set the minimum delay between whole-matrix polls.

matrix.setEnabled(true); // Set whether the calls to poll() are accepted.
```

Information about the matrix is also exposed.
```
std::array<uint, 2> size = matrix.getSize(); // Coordinate size of the matrix as (row, col)

std::vector<std::vector<bool>> debounced = matrix.getButtonValues(); The debounced table of button values as (row, col)
``` 