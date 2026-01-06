/**
 * Events Hook
 * Provides geofence event data
 */

import { useState, useEffect } from 'react';

// Mock event data
const MOCK_EVENTS = [
    {
        vehicle_id: "VH001",
        event_type: "ENTRY",
        zone: "Assembly_Area",
        timestamp: new Date(Date.now() - 120000).toISOString()
    },
    {
        vehicle_id: "VH005",
        event_type: "ENTRY",
        zone: "Restricted_Zone",
        timestamp: new Date(Date.now() - 60000).toISOString()
    },
    {
        vehicle_id: "VH002",
        event_type: "EXIT",
        zone: "Loading_Bay",
        dwell_time_sec: 1800,
        timestamp: new Date(Date.now() - 300000).toISOString()
    },
    {
        vehicle_id: "VH003",
        event_type: "ENTRY",
        zone: "Testing_Track",
        timestamp: new Date(Date.now() - 180000).toISOString()
    },
    {
        vehicle_id: "VH004",
        event_type: "EXIT",
        zone: "Parking_Lot",
        dwell_time_sec: 7200,
        timestamp: new Date(Date.now() - 600000).toISOString()
    }
];

export function useEvents() {
    const [events, setEvents] = useState(MOCK_EVENTS);

    // For Firebase, use onSnapshot similar to useLiveVehicles

    return events;
}
