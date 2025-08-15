// public/js/clamp-editor.js

export function renderClampEditor(config, containerId = 'clamp-editor') {
  const container = document.getElementById(containerId);
  container.innerHTML = '';

  config.ct_clamps.forEach(clamp => {
    const div = document.createElement('div');
    div.className = 'clamp-block';
    div.innerHTML = `
      <label><strong>${clamp.label}</strong></label><br>
      Role:
      <select onchange="updateRole('${clamp.id}', this.value)">
        ${['grid', 'solar', 'load', 'ev', 'other'].map(role =>
          `<option value="${role}" ${clamp.role === role ? 'selected' : ''}>${role}</option>`
        ).join('')}
      </select>
      Channel:
      <input type="number" value="${clamp.channel}" onchange="updateChannel('${clamp.id}', this.value)" />
      Calibration:
      <input type="number" step="0.01" value="${clamp.calibration_factor}" onchange="updateCalibration('${clamp.id}', this.value)" />
      <button onclick="deleteClamp('${clamp.id}')">Delete</button>
      <hr>
    `;
    container.appendChild(div);
  });

  const addBtn = document.createElement('button');
  addBtn.textContent = 'Add Clamp';
  addBtn.onclick = () => addClamp(config);
  container.appendChild(addBtn);
}

function updateRole(id, role) {
  const clamp = window.clampConfig.ct_clamps.find(c => c.id === id);
  if (clamp) clamp.role = role;
}

function updateChannel(id, channel) {
  const clamp = window.clampConfig.ct_clamps.find(c => c.id === id);
  if (clamp) clamp.channel = parseInt(channel);
}

function updateCalibration(id, factor) {
  const clamp = window.clampConfig.ct_clamps.find(c => c.id === id);
  if (clamp) clamp.calibration_factor = parseFloat(factor);
}

function deleteClamp(id) {
  window.clampConfig.ct_clamps = window.clampConfig.ct_clamps.filter(c => c.id !== id);
  renderClampEditor(window.clampConfig);
}

function addClamp(config) {
  const newClamp = {
    id: `clamp_${Date.now()}`,
    label: "New Clamp",
    refoss_unit: config.refoss_units[0]?.id || "unit_01",
    channel: 1,
    role: "other",
    enabled: true,
    calibration_factor: 1.0
  };
  config.ct_clamps.push(newClamp);
  renderClampEditor(config);
}