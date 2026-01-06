/**
 * Zone Polygon Component
 * Renders geofence zones as colored polygons on the map
 */

import React from 'react';
import { Polygon, Tooltip } from 'react-leaflet';

function ZonePolygon({ zone }) {
    // Convert polygon points to Leaflet format
    const positions = zone.polygon?.map(p => [p.lat, p.lon]) || [];

    if (positions.length < 3) return null;

    // Zone type specific styling
    const getOpacity = () => {
        if (zone.zone_type === 'RESTRICTED') return 0.25;
        return 0.15;
    };

    const getBorderWeight = () => {
        if (zone.zone_type === 'RESTRICTED') return 3;
        return 2;
    };

    return (
        <Polygon
            positions={positions}
            pathOptions={{
                color: zone.color || '#4f7fff',
                fillColor: zone.color || '#4f7fff',
                fillOpacity: getOpacity(),
                weight: getBorderWeight(),
                dashArray: zone.zone_type === 'RESTRICTED' ? '10, 5' : null
            }}
        >
            <Tooltip sticky>
                <div style={{ textAlign: 'center' }}>
                    <strong>{zone.zone_name}</strong>
                    <br />
                    <span style={{
                        fontSize: '10px',
                        color: zone.zone_type === 'RESTRICTED' ? '#e74c3c' : '#8a90a2'
                    }}>
                        {zone.zone_type}
                    </span>
                </div>
            </Tooltip>
        </Polygon>
    );
}

export default ZonePolygon;
