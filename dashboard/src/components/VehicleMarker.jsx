/**
 * Vehicle Marker Component
 * Renders individual vehicle on map with smooth animation and route trail
 */

import React from 'react';
import { Marker, Popup, Polyline } from 'react-leaflet';
import L from 'leaflet';
import { useSmoothPosition } from '../hooks/useSmoothPosition';
import { useRouteTrail } from '../hooks/useRouteTrail';

function VehicleMarker({ vehicle }) {
    // Smooth position interpolation
    const [lat, lon] = useSmoothPosition(
        vehicle.latitude,
        vehicle.longitude,
        vehicle.moving ? 1200 : 2000
    );

    // Route trail (last N points)
    const trail = useRouteTrail(vehicle.latitude, vehicle.longitude, 15);

    // Determine color based on status
    const getColor = () => {
        if (vehicle.zone === 'Restricted_Zone') return '#e74c3c'; // Red - Alert
        if (vehicle.moving) return '#2ecc71'; // Green - Moving
        return '#3498db'; // Blue - Idle
    };

    const color = getColor();

    // Create custom icon
    const icon = L.divIcon({
        className: 'vehicle-marker-wrapper',
        html: `
      <div class="vehicle-marker" style="
        background: ${color};
        transform: rotate(${vehicle.heading_deg || 0}deg);
      ">
        🚗
      </div>
    `,
        iconSize: [32, 32],
        iconAnchor: [16, 16]
    });

    // Format last update time
    const formatTime = (isoString) => {
        if (!isoString) return 'Unknown';
        const date = new Date(isoString);
        return date.toLocaleTimeString('en-US', {
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit'
        });
    };

    return (
        <>
            {/* Route Trail */}
            {trail.length > 1 && (
                <Polyline
                    positions={trail}
                    pathOptions={{
                        color: color,
                        weight: 3,
                        opacity: 0.4,
                        dashArray: '5, 10'
                    }}
                />
            )}

            {/* Vehicle Marker */}
            <Marker position={[lat, lon]} icon={icon}>
                <Popup>
                    <div className="popup-title">{vehicle.vehicle_id}</div>

                    <div className="popup-row">
                        <span className="popup-label">Zone</span>
                        <span>{vehicle.zone || 'Outside'}</span>
                    </div>

                    <div className="popup-row">
                        <span className="popup-label">Status</span>
                        <span style={{ color: color }}>
                            {vehicle.zone === 'Restricted_Zone'
                                ? '⚠️ ALERT'
                                : vehicle.moving ? 'Moving' : 'Idle'}
                        </span>
                    </div>

                    <div className="popup-row">
                        <span className="popup-label">Speed</span>
                        <span>{vehicle.speed_kmph?.toFixed(1) || '0'} km/h</span>
                    </div>

                    <div className="popup-row">
                        <span className="popup-label">Last Update</span>
                        <span>{formatTime(vehicle.last_update)}</span>
                    </div>

                    <div className="popup-row">
                        <span className="popup-label">Coordinates</span>
                        <span style={{ fontFamily: 'monospace', fontSize: '10px' }}>
                            {vehicle.latitude?.toFixed(6)}, {vehicle.longitude?.toFixed(6)}
                        </span>
                    </div>
                </Popup>
            </Marker>
        </>
    );
}

export default VehicleMarker;
