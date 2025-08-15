function updateDashboard() {
  fetch("/status.json")
    .then(res => res.json())
    .then(data => {
      document.getElementById("pvWatts").textContent = data.pvWatts;
      document.getElementById("homeWatts").textContent = data.homeWatts;
      document.getElementById("evWatts").textContent = data.evWatts;
      document.getElementById("chargeRateWatts").textContent = data.chargeRateWatts;

      document.getElementById("relayState").textContent = data.relayState ? "ON" : "OFF";
      document.getElementById("scheduleMode").textContent = data.scheduleMode;
      document.getElementById("manualOverride").textContent = data.manualOverride ? "Enabled" : "Disabled";

      if (data.chargingTimerActive) {
        document.getElementById("chargingTimer").textContent = `${data.chargingTimeRemaining}s remaining`;
      } else {
        document.getElementById("chargingTimer").textContent = "Inactive";
      }
    })
    .catch(err => console.error("Failed to fetch status:", err));
}

// Poll every 5 seconds
setInterval(updateDashboard, 5000);
updateDashboard();