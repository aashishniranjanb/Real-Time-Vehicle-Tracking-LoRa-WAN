/**
 * Zones Hook
 * Provides geofence zone data
 */

import { useState, useEffect } from 'react';

// Mock zone data (matches synthetic data generator)
const MOCK_ZONES = [
    {
        zone_name: "Assembly_Area",
        zone_type: "PRODUCTION",
        color: "#2ECC71",
        polygon: [
            { lat: 12.9718, lon: 80.2201 },
            { lat: 12.9725, lon: 80.2201 },
            { lat: 12.9725, lon: 80.2210 },
            { lat: 12.9718, lon: 80.2210 }
        ]
    },
    {
        zone_name: "Parking_Lot",
        zone_type: "PARKING",
        color: "#3498DB",
        polygon: [
            { lat: 12.9705, lon: 80.2185 },
            { lat: 12.9712, lon: 80.2185 },
            { lat: 12.9712, lon: 80.2193 },
            { lat: 12.9705, lon: 80.2193 }
        ]
    },
    {
        zone_name: "Restricted_Zone",
        zone_type: "RESTRICTED",
        color: "#E74C3C",
        polygon: [
            { lat: 12.9720, lon: 80.2198 },
            { lat: 12.9724, lon: 80.2198 },
            { lat: 12.9724, lon: 80.2203 },
            { lat: 12.9720, lon: 80.2203 }
        ]
    },
    {
        zone_name: "Loading_Bay",
        zone_type: "LOGISTICS",
        color: "#F39C12",
        polygon: [
            { lat: 12.9708, lon: 80.2195 },
            { lat: 12.9714, lon: 80.2195 },
            { lat: 12.9714, lon: 80.2200 },
            { lat: 12.9708, lon: 80.2200 }
        ]
    },
    {
        zone_name: "Testing_Track",
        zone_type: "TESTING",
        color: "#9B59B6",
        polygon: [
            { lat: 12.9726, lon: 80.2205 },
            { lat: 12.9735, lon: 80.2205 },
            { lat: 12.9735, lon: 80.2218 },
            { lat: 12.9726, lon: 80.2218 }
        ]
    }
];

export function useZones() {
    const [zones, setZones] = useState(MOCK_ZONES);

    // For Firebase, use onSnapshot similar to useLiveVehicles

    return zones;
}
