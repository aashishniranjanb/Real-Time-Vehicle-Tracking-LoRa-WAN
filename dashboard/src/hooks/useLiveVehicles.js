/**
 * Live Vehicles Hook
 * Provides real-time vehicle data from Firebase or mock data
 */

import { useState, useEffect } from 'react';

// Mock data for demo (matches synthetic data generator)
const MOCK_VEHICLES = [
    {
        vehicle_id: "VH001",
        latitude: 12.9719,
        longitude: 80.2204,
        zone: "Assembly_Area",
        moving: true,
        speed_kmph: 14.2,
        heading_deg: 45,
        last_update: new Date().toISOString()
    },
    {
        vehicle_id: "VH002",
        latitude: 12.9709,
        longitude: 80.2190,
        zone: "Parking_Lot",
        moving: false,
        speed_kmph: 0,
        heading_deg: 180,
        last_update: new Date().toISOString()
    },
    {
        vehicle_id: "VH003",
        latitude: 12.9730,
        longitude: 80.2212,
        zone: "Testing_Track",
        moving: true,
        speed_kmph: 28.5,
        heading_deg: 90,
        last_update: new Date().toISOString()
    },
    {
        vehicle_id: "VH004",
        latitude: 12.9711,
        longitude: 80.2197,
        zone: "Loading_Bay",
        moving: false,
        speed_kmph: 0,
        heading_deg: 270,
        last_update: new Date().toISOString()
    },
    {
        vehicle_id: "VH005",
        latitude: 12.9722,
        longitude: 80.2200,
        zone: "Restricted_Zone",
        moving: true,
        speed_kmph: 8.7,
        heading_deg: 135,
        last_update: new Date().toISOString()
    }
];

export function useLiveVehicles() {
    const [vehicles, setVehicles] = useState(MOCK_VEHICLES);

    useEffect(() => {
        // Simulate real-time updates for demo
        const interval = setInterval(() => {
            setVehicles(prev => prev.map(v => {
                if (!v.moving) return v;

                // Small random movement for moving vehicles
                return {
                    ...v,
                    latitude: v.latitude + (Math.random() - 0.5) * 0.0002,
                    longitude: v.longitude + (Math.random() - 0.5) * 0.0002,
                    heading_deg: (v.heading_deg + (Math.random() - 0.5) * 10) % 360,
                    last_update: new Date().toISOString()
                };
            }));
        }, 3000);

        return () => clearInterval(interval);
    }, []);

    return vehicles;
}

// Firebase real-time version (uncomment when Firebase is configured)
/*
import { collection, onSnapshot } from 'firebase/firestore';
import { db } from '../firebase';

export function useLiveVehicles() {
  const [vehicles, setVehicles] = useState([]);

  useEffect(() => {
    const unsubscribe = onSnapshot(
      collection(db, 'live_state'),
      (snapshot) => {
        const data = snapshot.docs.map(doc => doc.data());
        setVehicles(data);
      },
      (error) => {
        console.error('Firebase error:', error);
      }
    );

    return () => unsubscribe();
  }, []);

  return vehicles;
}
*/
