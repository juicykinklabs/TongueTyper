const output = document.getElementById('output');

document.querySelectorAll('.category-header').forEach(header => {
  header.addEventListener('click', () => {
    header.parentElement.classList.toggle('open');
  });
});

// Generic action buttons (anything in a .buttons row, e.g. GET/POST/DELETE examples)
document.querySelectorAll('.buttons button[data-url]').forEach(btn => {
  btn.addEventListener('click', () => callApi(btn.dataset.method, btn.dataset.url));
});

// --- Sliders: live value display for however many exist on the page ---
document.querySelectorAll('input[type="range"]').forEach(slider => {
  const valueEl = slider.closest('.field')?.querySelector('.field-value');
  if (valueEl) {
    valueEl.textContent = slider.value; // set initial label
    slider.addEventListener('input', () => {
      valueEl.textContent = slider.value;
    });
  }
});

// --- Color pickers: live label while dragging, API call only on release ---
document.querySelectorAll('input[type="color"]').forEach(colorInput => {
  const valueEl = colorInput.closest('.field')?.querySelector('.field-value');
  if (valueEl) valueEl.textContent = colorInput.value;

  colorInput.addEventListener('input', () => {
    if (valueEl) valueEl.textContent = colorInput.value;
  });

  colorInput.addEventListener('change', () => {
    const key = colorInput.dataset.key || colorInput.id || 'color';
    callApi('POST', colorInput.dataset.url, { [key]: colorInput.value });
  });
});

// --- Form submit buttons: gather every text/range/checkbox field in the same
//     section and send them together in one call. Works for any number of
//     submit buttons/sections, each scoped to its own .category-body. ---
document.querySelectorAll('.form-fields button[data-url]').forEach(submitBtn => {
  submitBtn.addEventListener('click', () => {
    const scope = submitBtn.closest('.category-body') || document;
    const fields = scope.querySelectorAll(
      'input[type="text"], input[type="range"], input[type="checkbox"]'
    );

    const payload = {};
    fields.forEach(field => {
      const key = field.dataset.key || field.id;
      if (!key) return; // skip fields with no identifier to key the payload on
      if (field.type === 'checkbox') {
        payload[key] = field.checked;
      } else if (field.type === 'range') {
        payload[key] = Number(field.value);
      } else {
        payload[key] = field.value;
      }
    });

    callApi(submitBtn.dataset.method, submitBtn.dataset.url, payload);
  });
});

async function callApi(method, url, payload) {
  output.textContent = `Calling ${method} ${url} ...`;
  try {
    const options = { method };
    if (method === 'POST' || method === 'PUT') {
      options.headers = { 'Content-Type': 'application/json' };
      options.body = JSON.stringify(payload || { example: 'payload' });
    }

    const res = await fetch(url, options);
    const contentType = res.headers.get('content-type') || '';
    const data = contentType.includes('application/json')
      ? await res.json()
      : await res.text();

    output.textContent =
      `Status: ${res.status} ${res.statusText}\n\n` +
      (typeof data === 'string' ? data : JSON.stringify(data, null, 2));
  } catch (err) {
    output.textContent = `Error: ${err.message}`;
  }
}

// SENSORS

const SENSOR_API_URL = '/api/sensors';
const POLL_INTERVAL_MS = 2000;

// Configure thresholds per sensor key (matched against the keys in the JSON
// the server returns). Any key not listed here just renders as "normal".
// warn/critical are the value at which the color escalates.
const SENSOR_THRESHOLDS = {
  core: { warn: 75, critical: 90, unit: '°C' },
  battery: { warn: 60, critical: 80, unit: '%' },
  pressure: { warn: 1030, critical: 1050, unit: ' hPa' },
};

const sensorPanel = document.getElementById('sensor-panel');
const sensorStatus = document.getElementById('sensor-status');

function classifySensorValue(key, value) {
  // fixed colors:
  if (key == 'x') return 'red';
  if (key == 'y') return 'green';
  if (key == 'z') return 'blue';
  
  // const t = SENSOR_THRESHOLDS[key];
  // if (!t || typeof value !== 'number') return 'normal';
  // if (value >= t.critical) return 'critical';
  // if (value >= t.warn) return 'warning';
  return 'normal';
}

function renderSensors(data) {
  if (!sensorPanel) return;
  sensorPanel.innerHTML = '';

  Object.entries(data).forEach(([key, value]) => {
    const status = classifySensorValue(key, value);
    const unit = SENSOR_THRESHOLDS[key]?.unit || '';

    const row = document.createElement('div');
    row.className = 'sensor-row';

    const label = document.createElement('span');
    label.className = 'sensor-label';
    label.textContent = key;

    const val = document.createElement('span');
    val.className = `sensor-value ${status}`;
    val.textContent = `${value}${unit}`;

    row.appendChild(label);
    row.appendChild(val);
    sensorPanel.appendChild(row);
  });
}

function setSensorStatus(state, message) {
  if (!sensorStatus) return;
  if (state === 'live') {
    sensorStatus.textContent = 'Live';
    sensorStatus.className = 'sensor-status live';
  } else {
    sensorStatus.textContent = `Error: ${message}`;
    sensorStatus.className = 'sensor-status error';
  }
}

var pollingIntervalID

async function fetchSensors() {
  try {
    const res = await fetch(SENSOR_API_URL);
    if (!res.ok) throw new Error(`${res.status} ${res.statusText}`);
    const data = await res.json();
    renderSensors(data);
    setSensorStatus('live');
  } catch (err) {
    setSensorStatus('error', err.message);
    clearInterval(pollingIntervalID); // for debugging, just so we aren't sending network requests with no backend
  }
}

function startSensorPolling() {
  fetchSensors(); // fetch immediately, then keep polling
  pollingIntervalID = setInterval(fetchSensors, POLL_INTERVAL_MS);
}

startSensorPolling();