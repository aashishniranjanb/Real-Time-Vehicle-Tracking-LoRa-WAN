# GPS Lock Failure Under Metal Roof
**Project:** Vehicle Tracking (LoRaWAN)

## Problem
The GPS receiver module failed to acquire a satellite lock when the tracking node entered or parked inside an industrial warehouse building containing metal roofing.

## Root Cause
The metal structure and corrugated iron roof attenuated L1-band GPS signals (1575.42 MHz) below the receiver sensitivity threshold. As a result, cold start times exceeded 5 minutes and time-to-first-fix (TTFF) was never reached.

## What Failed
- The initial design assumption that GPS signals would propagate sufficiently into indoor factory environments.
- The firmware did not provide a fallback or recovery state for signal-denied environments, causing infinite timeout loops.

## What Changed
- Added last-known-position caching using RTC slow memory (persists through deep sleep).
- Implemented geofence-based position estimation (caching the entrance gate location upon signal loss).
- Documented metal roof signal constraints in the project deployment guide, recommending external antennas or hybrid indoor WiFi positioning for indoor tracking.
