/**
 * Vehicle List Panel Component
 */

import React, { useState } from 'react';

function VehicleList({ vehicles = [] }) {
    const [selectedId, setSelectedId] = useState(null);

    const getStatusClass = (vehicle) => {
        if (vehicle.zone === 'Restricted_Zone') return 'alert';
        return vehicle.moving ? 'moving' : 'idle';
    };

    const getStatusText = (vehicle) => {
        if (vehicle.zone === 'Restricted_Zone') return 'ALERT';
        return vehicle.moving ? 'Moving' : 'Idle';
    };

    const getIcon = (vehicle) => {
        if (vehicle.zone === 'Restricted_Zone') return '🚨';
        return vehicle.moving ? '🚗' : '🅿️';
    };

    return (
        <div className="vehicle-list">
            <h3>Fleet Status</h3>

            {vehicles.length === 0 ? (
                <p style={{ color: 'var(--text-secondary)', fontSize: '12px' }}>
                    No vehicles connected
                </p>
            ) : (
                vehicles.map(vehicle => (
                    <div
                        key={vehicle.vehicle_id}
                        className={`vehicle-item ${selectedId === vehicle.vehicle_id ? 'active' : ''}`}
                        onClick={() => setSelectedId(vehicle.vehicle_id)}
                    >
                        <div className="vehicle-info">
                            <span className="vehicle-icon">{getIcon(vehicle)}</span>
                            <div className="vehicle-details">
                                <h4>{vehicle.vehicle_id}</h4>
                                <span>{vehicle.zone || 'Unknown'}</span>
                            </div>
                        </div>

                        <span className={`vehicle-status ${getStatusClass(vehicle)}`}>
                            {getStatusText(vehicle)}
                        </span>
                    </div>
                ))
            )}
        </div>
    );
}

export default VehicleList;
