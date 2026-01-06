/**
 * Vehicle Tracking Dashboard
 * Main Dashboard Component
 */

import React from 'react';
import Header from './components/Header';
import VehicleList from './components/VehicleList';
import MapView from './components/MapView';
import EventsPanel from './components/EventsPanel';
import { useLiveVehicles } from './hooks/useLiveVehicles';
import { useZones } from './hooks/useZones';
import { useEvents } from './hooks/useEvents';

function Dashboard() {
    const vehicles = useLiveVehicles();
    const zones = useZones();
    const events = useEvents();

    // Count stats
    const movingCount = vehicles.filter(v => v.moving).length;
    const alertCount = vehicles.filter(v => v.zone === 'Restricted_Zone').length;

    return (
        <div className="dashboard">
            <Header
                totalVehicles={vehicles.length}
                movingCount={movingCount}
                alertCount={alertCount}
            />

            <div className="main">
                <VehicleList vehicles={vehicles} />
                <MapView vehicles={vehicles} zones={zones} />
            </div>

            <EventsPanel events={events} />
        </div>
    );
}

export default Dashboard;
