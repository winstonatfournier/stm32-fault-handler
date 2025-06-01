# stm32-fault-handler

Firmware fault detection and recovery handlers for STM32 microcontrollers.

These drivers replicate and adapt several modules originally developed for the CubeSat Electrical Power System (EPS) of the University of Toronto Aerospace Team (UTAT). They implement diagnostic logic to detect and handle various failure conditions critical to reliable embedded operation.


## Implemented Fault Cases

- chronic_idle(): Detects persistent idling of the MPPT device, especially in charging conditions where activity is expected.
- source_decay(): Identifies long-term deterioration in solar cell or battery performance. Used to adjust fault tolerance thresholds in other drivers and subsystems.
- pwr_mon_read_error(): Periodically verifies the health of the Power Monitor by checking for consistent readout behavior.

> Development period: May-September 2024