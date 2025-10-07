# Ant Module Slave Demoing
This branch is for the testing and learning of the ANT capabilities of the EiE development board. In this case, the EiE development board acts as the Slave, and the connected laptop/computer acts as the Master sending ANT messages to the development board which are then displayed on the board's LCD.

## Setup

To install the necessary tools to compile the project see [setup.md](docs/setup.md)
To set up the VSCode development environment see [vscode.md](docs/vscode.md)

## Compiling reference

To compile the project

1. Open a new terminal
2. Run `./waf configure --board=<hardware>` replacing `<hardware>` with either `ASCII` or `DOT_MATRIX` depending on the board you have. This step should only need to be run once if successful.
3. Run `./waf build` to build or `./waf build -F` to build and flash the device.

python waf -?
python waf configure --board=ASCII
python waf configure --board=DOT_MATRIX
python waf build
python waf build -F
