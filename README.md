## Prerequisetes
Excelfore unibase library must be installed.

POSIX library is basically needed.
To port some program which is built with this 'combase' library into
non-POSIX platform, '*_NON_POSIX_H' part in the header files must support
relplacements of the following macro definitions.

## cb_ethernet
bindings macros to posix socket functions particularly on ethernet functionalities.
One unique feature is supporting of virtual network and ptp devices.

Testing network programs, especially for raw ethernet, is not easy on
a build machine.
Using virtual network and ptp devices, it can be easily done, and unit
tests can include a lot more tests.

For more detail, look at cb_ethernet.h

## cb_inet
Binding macros and utility functions to posix socket functions particularly for network layer functions.

For more detail, look at cb_inet.h

## cb_thread
bindings macros to posix thread,mutex and semaphore functions.

For more detail, look at cb_thread.h

## cb_ipcsock
utility functions to use local socket

For more detail, look at cb_ipcsock.h

## cb_tmevent
utility functions to use timer and event hadling.

For more detail, look at cb_tmevent.h

## cb_ipcshmem
utility functions to use shared memory 

For more detail, look at cb_ipcshmem.h

## cb_xtimer
utility functions to use an independent platform timer. 

For more detail, look at cb_xtimer.h

## cb_crc
utility functions to calculate CRC. 

For more detail, look at cb_crc.h