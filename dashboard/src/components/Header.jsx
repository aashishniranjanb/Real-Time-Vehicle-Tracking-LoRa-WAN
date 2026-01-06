/**
 * Header Component
 */

import React from 'react';

function Header({ totalVehicles = 0, movingCount = 0, alertCount = 0 }) {
    return (
        <header className="header">
            <div className="header-left">
                <div className="logo">📡</div>
                <h1>Vehicle Tracking Dashboard</h1>
                <span className="subtitle">LoRaWAN • Real-Time • Geofencing</span>
            </div>

            <div className="header-right">
                <div className="status-indicator">
                    <span className="status-dot"></span>
                    <span>{totalVehicles} Vehicles • {movingCount} Moving</span>
                </div>

                {alertCount > 0 && (
                    <div className="status-indicator" style={{ color: '#e74c3c' }}>
                        <span className="status-dot" style={{ background: '#e74c3c' }}></span>
                        <span>{alertCount} Alert{alertCount > 1 ? 's' : ''}</span>
                    </div>
                )}
            </div>
        </header>
    );
}

export default Header;
