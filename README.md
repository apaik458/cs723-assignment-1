# cs723-assignment-1

## Getting Started

Clone the repository into a folder. Open "Nios II Software Build Tools for Eclipse (Quartus Prime 18.1)", and create a workspace at `cs723-assignment-1/workspace`.

You can now import the existing projects `relay` and `relay_bsp` by going to File > Import > General > Existing Projects into Workspace, and selecting `cs723-assignment-1/software` as the root directory.

## Building and Running

To use the blaster, plug a USB cable into the blaster port of the DE2-115 board. Open "Programmer (Quartus Prime 18.1)", click "Hardware Setup", and select "USB-Blaster".

We may now load our `freq_relay_controller.sof` file onto the board with the "Add File..." button. We can now programme the board by clicking "Start".

To load our code onto the board, open Eclipse, right-click the `relay` project, and go to Run As > 3 Nios II Hardware.