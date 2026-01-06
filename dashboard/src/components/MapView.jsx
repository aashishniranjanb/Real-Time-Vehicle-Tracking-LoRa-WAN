/**
 * Map View Component
 * Uses Leaflet for map rendering with vehicle markers and zone polygons
 */

import React from 'react';
import { MapContainer, TileLayer } from 'react-leaflet';
import VehicleMarker from './VehicleMarker';
import ZonePolygon from './ZonePolygon';

// Default center: Chennai area (matches synthetic data)
const DEFAULT_CENTER = [12.9719, 80.2204];
const DEFAULT_ZOOM = 17;

function MapView({ vehicles = [], zones = [] }) {
    return (
        <div className="map-container">
            <MapContainer
                center={DEFAULT_CENTER}
                zoom={DEFAULT_ZOOM}
                className="map"
                zoomControl={true}
            >
                {/* Dark tile layer for industrial look */}
                <TileLayer
                    url="https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png"
                    attribution='&copy; OpenStreetMap, &copy; CARTO'
                />

                {/* Render geofence zones */}
                {zones.map(zone => (
                    <ZonePolygon key={zone.zone_name} zone={zone} />
                ))}

                {/* Render vehicle markers with trails */}
                {vehicles.map(vehicle => (
                    <VehicleMarker key={vehicle.vehicle_id} vehicle={vehicle} />
                ))}
            </MapContainer>
        </div>
    );
}

export default MapView;
