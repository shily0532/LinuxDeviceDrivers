# fpga_driver
# Compiling environment

Hardware: AT91SAM9263

Compiler: Cross_compile

Linux core: version Linux 2.6.27

Hardware design:
    Chip Select:    EBI_NCS5
    Data Bus:       EBI_D0 ~ EBI_D7
    Address Bus:    EBI_A2 ~ EBI_A9
    Control Bus:    EBI_nRD / EBI_nWE
    Connect Device: FPGA

Note:
    This demo was tested in our development board. Some changes should be made if the hardware design was changed.
    This driver provides the basic interface for user, read function and write function.


# Running the driver & test app

1.  Cross compile the driver
    $cd /..../FPGA_driver
    $make

2.  $./mkdev.sh
    $cat /proc/devices | grep "port" #check whether the driver is installed
    or: $ls /dev/port -l

# Testing the driver

    $cd ./FPGA_test
    $./testPortIN


