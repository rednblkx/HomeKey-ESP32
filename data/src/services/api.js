const API_BASE_URL = '';

export async function fetchConfig(type) {
  const response = await fetch(`${API_BASE_URL}/config?type=${type}`);
  if (!response.ok) {
    throw new Error(`HTTP error! status: ${response.status}`);
  }
  return response.json();
}

export async function saveConfig(type, data) {
  const response = await fetch(`${API_BASE_URL}/config/save?type=${type}`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(data),
  });
  if (!response.ok) {
    throw new Error(`HTTP error! status: ${response.status}`);
  }
  return response.text();
}

export async function fetchEthConfig() {
  const response = await fetch(`${API_BASE_URL}/eth_get_config`);
  if (!response.ok) {
    throw new Error(`HTTP error! status: ${response.status}`);
  }
  return response.json();
}
