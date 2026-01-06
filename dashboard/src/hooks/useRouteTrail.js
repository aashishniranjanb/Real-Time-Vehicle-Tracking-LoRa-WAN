/**
 * Route Trail Hook
 * Maintains a sliding window of recent GPS positions for trail visualization
 */

import { useEffect, useState, useRef } from 'react';

export function useRouteTrail(lat, lon, maxPoints = 15) {
    const [trail, setTrail] = useState([]);
    const lastPosRef = useRef(null);

    useEffect(() => {
        // Only add point if position actually changed (more than 1 meter)
        const MIN_DISTANCE = 0.00001; // ~1 meter in degrees

        if (lastPosRef.current) {
            const [lastLat, lastLon] = lastPosRef.current;
            const distance = Math.sqrt(
                Math.pow(lat - lastLat, 2) + Math.pow(lon - lastLon, 2)
            );

            if (distance < MIN_DISTANCE) {
                return; // Skip if position hasn't changed significantly
            }
        }

        lastPosRef.current = [lat, lon];

        setTrail(prev => {
            const updated = [...prev, [lat, lon]];
            // Keep only last N points
            return updated.slice(-maxPoints);
        });
    }, [lat, lon, maxPoints]);

    return trail;
}
