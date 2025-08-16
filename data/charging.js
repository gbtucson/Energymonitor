// charging.js — line 1
// Load live charging status every second
async function loadChargingStatus() {
  const res = await fetch('/charging-status.json');
  const data = await res.json();

  document.getElementById('pvWatts').innerText = data.pvWatts;
  document.getElementById('houseWatts').innerText = data.houseWatts;
  document.getElementById('netSurplus').innerText = data.netSurplus;
  document.getElementById('aggressiveness').innerText = data.aggressiveness;

  // ⚠️ Commented out: this overwrites the saved config value
  // document.getElementById('pvThreshold').value = data.pvThreshold;
}

// ✅ New: Load saved config from charging-settings.json on page load
window.addEventListener("DOMContentLoaded", () => {
  fetch("/charging-settings.json?ts=" + Date.now()) // cache-busting
    .then(res => {
      console.log("Fetch status:", res.status);
      return res.json();
    })
    .then(data => {
      console.log("Fetched config:", data);
      if (data.minimumPVWattsToCharge !== undefined) {
        document.getElementById("pvThreshold").value = data.minimumPVWattsToCharge;
      }
    })
    .catch(err => console.error("Failed to load config:", err));
});

// ✅ Fix: missing closing parenthesis in fetch call
async function saveChargingSettings() {
 const payload = {
  minimumPVWattsToCharge: parseInt(document.getElementById('pvThreshold').value)
};


  await fetch('/charging-settings.json', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(payload)
  }); // ← fixed closing parenthesis
}

// Keep live status updating
setInterval(loadChargingStatus, 1000);
