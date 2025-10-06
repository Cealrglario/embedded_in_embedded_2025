# Embedded in Embedded Development

EiE is a mentoring program for university engineering students that was started in 2002 at the University of Calgary. The program connects industry engineers with students interested in embedded systems. Together they explore the practical, hands-on technical side of embedded development while building great relationships.
https://embeddedinembedded.com/

[TOC]

## Setup

To install the necessary tools to compile the project see [setup.md](docs/setup.md).
To set up the VSCode development environment see [vscode.md](docs/vscode.md)

## Compiling reference

To compile the project

1. Open a new terminal
2. Run `python waf configure --board=<hardware>` replacing `<hardware>` with either `ASCII` or `DOT_MATRIX` depending on the board you have. This step should only need to be run once if successful.
3. Run `python waf build` to build or `python waf build -F` to build AND flash the device.

## Waf configuration commands
python waf -?
python waf configure --board=ASCII
python waf configure --board=DOT_MATRIX
python waf build
python waf build -F
