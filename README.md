FF
========

FF is a simple linux char device that when read from returns bytes containing only 0xff.

## Compilation
    $ make

## Installation
    $ sudo make load

This will insmod the module and copy a simple udev rule which changes the permissions of /dev/ff to 0666

## Usage
    $ dd if=/dev/ff of=<output-goes-here>

## Deinstallation
    $ sudo make unload

This will rmmod the module and remove the rule from udev
