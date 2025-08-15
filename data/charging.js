async function loadChargingStatus() {
  const res = await fetch('/charging-status.json');
  const data = await res.json();

  document.getElementById('pvWatts').innerText = data.pvWatts;
  document.getElementById('houseWatts').innerText = data.houseWatts;
  document.getElementById('netSurplus').innerText = data.netSurplus;
  document.getElementById('aggressiveness').innerText = data.aggressiveness;
  document.getElementById('pvThreshold').value = data.pvThreshold;
}

async function saveChargingSettings() {
  const payload = {
    pvThreshold: parseInt(document.getElementById('pvThreshold').value)
  };

  await fetch('/charging-settings.json', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(payload)
  };
}

setInterval(loadChargingStatus, 1000);
