# Stepping_motor_gui

This project provides a Graphical User Interface (GUI) for controlling a motor. The GUI is written in C using the GTK+3 library.

## Required Packages

To run this program on a Raspberry Pi, you will need the following packages:

1. **gcc**: the GNU Compiler Collection, used to compile C programs.
2. **libgtk-3-dev**: GTK+3 development files, used to compile programs that use GTK+3.
3. **pkg-config**: helper tool used when compiling applications and libraries.

## Installation

To install these packages on a Raspberry Pi running a Debian-based distribution, such as Raspberry Pi OS, you can use the following commands:

```bash
sudo apt update
sudo apt install gcc libgtk-3-dev pkg-config
```

## Compiling the Program
After you have installed the required packages, you can compile the program using gcc:

```bash
gcc `pkg-config --cflags gtk+-3.0` -o steppingMotor steppingMotor.c `pkg-config --libs gtk+-3.0`
```
This command tells gcc to compile the file motor_gui.c and link it with the libraries used by GTK+3, creating an executable file called motor_gui.

## Running the Program
After compiling the program, you can run it with the following command:

```bash
./steppingMotor
```

Instructional for Mac Users
Follow the steps below to run the program on MacOS:

# Step 1: Install Homebrew
Homebrew is a package manager for macOS that simplifies the installation of software. Open your Terminal and install Homebrew by entering:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
## Step 2: Install GTK+ and pkg-config
You will need the GTK+ library to build the GUI and pkg-config to configure compiler and linker flags. Install them using Homebrew by entering:

```bash
brew install gtk+3
brew install pkg-config
```

## Step 3: Install GCC Compiler
Install the GCC compiler using Homebrew by entering:


```bash
brew install gcc
```

## Step 4: Clone the Repository

## Step 5: Compile the Program
Navigate to the repository directory and compile the source code. Replace <source_file> with the name of your C file:

```bash
gcc `pkg-config --cflags gtk+-3.0` -o steppingMotor steppingMotor.c `pkg-config --libs gtk+-3.0`
```

## Step 6: Run the Program

```bash
./steppingMotor
```

The GUI of the program should now appear on your screen.

Note: The steps above assume that you're using a bash or zsh shell. If you're using a different shell, you may need to adjust some of the commands.

Also, please ensure that the USB device path (/dev/tty.usbserial-A601VOHX) is correct for your system. In MacOS, it is common for the path to be something like /dev/tty.usbmodemXXXX. The exact path can be found from the command `ls /dev/`.

You can check this on a raspberry pi by typing `ls /dev/` while the motor is unplugged, then plugging the motor in and typing `ls /dev/` again and looking at the difference.

## Usage
The GUI has several buttons that you can use to control the motor:

Online: Puts the motor online.
Home: Moves the motor to its home position.
Set Position to Zero: Sets the current motor position to zero.
Move Away: Moves the motor a specified distance away.
Move Closer: Moves the motor a specified distance closer.
Get Position: Displays the current position of the motor.
Clear: Clears any pending motor operations.
Kill Op.: Kills the current motor operation.
Status: Displays the current status of the motor.
Offline: Takes the motor offline.
To use the Move Away and Move Closer commands, enter the distance (in mm) that you want the motor to move in the "Enter the Distance (mm):" text box before clicking the button.

Please remember to put the motor online before attempting to control it and to take it offline when you are done using it. ALWAYS use the CLEAR button after every command.


