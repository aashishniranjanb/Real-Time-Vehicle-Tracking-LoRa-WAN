/**
 * Smooth Position Hook
 * Interpolates between GPS positions for smooth animation
 */

import { useEffect, useRef, useState } from 'react';

export function useSmoothPosition(targetLat, targetLon, duration = 1000) {
    const [position, setPosition] = useState([targetLat, targetLon]);

    const startRef = useRef([targetLat, targetLon]);
    const startTimeRef = useRef(null);
    const animationRef = useRef(null);

    useEffect(() => {
        // Cancel any existing animation
        if (animationRef.current) {
            cancelAnimationFrame(animationRef.current);
        }

        // Store starting position
        startRef.current = position;
        startTimeRef.current = performance.now();

        function animate(currentTime) {
            const elapsed = currentTime - startTimeRef.current;
            const alpha = Math.min(elapsed / duration, 1);

            // Ease-out interpolation for smoother motion
            const easeAlpha = 1 - Math.pow(1 - alpha, 3);

            const lat = startRef.current[0] + easeAlpha * (targetLat - startRef.current[0]);
            const lon = startRef.current[1] + easeAlpha * (targetLon - startRef.current[1]);

            setPosition([lat, lon]);

            if (alpha < 1) {
                animationRef.current = requestAnimationFrame(animate);
            }
        }

        animationRef.current = requestAnimationFrame(animate);

        return () => {
            if (animationRef.current) {
                cancelAnimationFrame(animationRef.current);
            }
        };
    }, [targetLat, targetLon, duration]);

    return position;
}
