/**
 * Events Panel Component
 * Shows timeline of geofence events
 */

import React from 'react';

function EventsPanel({ events = [] }) {
    const formatTime = (isoString) => {
        if (!isoString) return '--:--:--';
        const date = new Date(isoString);
        return date.toLocaleTimeString('en-US', {
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit',
            hour12: false
        });
    };

    const getEventClass = (eventType) => {
        switch (eventType?.toUpperCase()) {
            case 'ENTRY': return 'entry';
            case 'EXIT': return 'exit';
            case 'DWELL_ALERT':
            case 'RESTRICTED_ZONE_ENTRY': return 'alert';
            default: return '';
        }
    };

    const formatDwellTime = (seconds) => {
        if (!seconds) return null;
        const minutes = Math.floor(seconds / 60);
        if (minutes < 60) return `${minutes}m`;
        const hours = Math.floor(minutes / 60);
        const remainingMins = minutes % 60;
        return `${hours}h ${remainingMins}m`;
    };

    // Sort events by timestamp (newest first)
    const sortedEvents = [...events].sort((a, b) =>
        new Date(b.timestamp) - new Date(a.timestamp)
    ).slice(0, 10);

    return (
        <div className="events-panel">
            <h3>Recent Events</h3>

            {sortedEvents.length === 0 ? (
                <p style={{ color: 'var(--text-secondary)', fontSize: '12px' }}>
                    No events recorded
                </p>
            ) : (
                <div className="events-list">
                    {sortedEvents.map((event, index) => (
                        <div key={index} className="event-item">
                            <span className="event-time">{formatTime(event.timestamp)}</span>
                            <span className="event-vehicle">{event.vehicle_id}</span>
                            <span className={`event-type ${getEventClass(event.event_type)}`}>
                                {event.event_type}
                                {event.dwell_time_sec && (
                                    <span style={{ marginLeft: '4px', opacity: 0.7 }}>
                                        ({formatDwellTime(event.dwell_time_sec)})
                                    </span>
                                )}
                            </span>
                            <span className="event-zone">{event.zone}</span>
                        </div>
                    ))}
                </div>
            )}
        </div>
    );
}

export default EventsPanel;
