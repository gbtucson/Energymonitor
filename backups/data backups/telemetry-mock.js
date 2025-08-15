// public/js/telemetry-mock.js

export function startTelemetryLoop(config, containerId = 'telemetry-list') {
  const list = document.getElementById(containerId);

  function updateTelemetry() {
    list.innerHTML = '';
    config.ct_clamps.forEach(clamp => {
      const value = (Math.random() * 30).toFixed(2); // Simulated amps
      const color = getRoleColor(clamp.role);

      const item = document.createElement('li');
      item.style.color = color;
      item.textContent = `${clamp.label}: ${value} A`;
      list.appendChild(item);
    });
  }

  updateTelemetry(); // Initial render
  setInterval(updateTelemetry, 2000); // Update every 2 seconds
}

function getRoleColor(role) {
  switch (role) {
    case 'grid': return 'red';
    case 'solar': return 'green';
    case 'ev': return 'blue';
    case 'load': return 'orange';
    default: return 'gray';
  }
}