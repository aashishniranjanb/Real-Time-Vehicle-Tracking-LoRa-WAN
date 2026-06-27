# LoRa Duty Cycle Violation
**Project:** Vehicle Tracking (LoRaWAN)

## Problem
The vehicle tracker device exceeded the regulatory 1% duty cycle limit, causing transmission failures and potential regulatory non-compliance on shared sub-GHz industrial bands.

## Root Cause
The initial firmware implementation was sending telemetry updates on every single GPS fix (1 Hz frequency) while moving, without aggregating data or checking regional airtime limitations. The firmware-side airtime calculation routines were incorrect.

## What Failed
- Did not account for regional LoRaWAN duty cycle regulations (e.g. EU868 / IN865 bands enforce max 1% tx time).
- Transmission rate was too aggressive for low-bandwidth spread spectrum communications.

## What Changed
- Reduced default TX rate to 1 packet per 5 minutes for stationary heartbeats, and 30-60 seconds for adaptive movements.
- Added packet aggregation buffer on the edge to combine multiple coordinates in a single frame.
- Implemented airtime calculator directly in the firmware to block transmission if limits are approached.

## Unresolved Constraints
- Real-time high-resolution tracking (e.g. 1-second interval) is physically not possible within regulatory duty cycle limits on public LoRaWAN frequency spectra.
