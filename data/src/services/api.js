const API_BASE_URL = '';

/**
 * Retrieve configuration data for the specified type from the API.
 * @param {string} type - The configuration type to request.
 * @returns {Promise<object>} The parsed JSON configuration for the requested type.
 * @throws {Error} If the HTTP response status is not OK; message contains the status code.
 */
export async function fetchConfig(type) {
  const response = await fetch(`${API_BASE_URL}/config?type=${type}`);
  if (!response.ok) {
    throw new Error(`HTTP error! status: ${response.status}`);
  }
  return response.json();
}

/**
 * Saves configuration data for the given config type by POSTing JSON to the API.
 * @param {string} type - Identifier of the configuration type to save.
 * @param {any} data - Payload to be saved; will be serialized to JSON.
 * @returns {Promise<string>} The response body as text.
 * @throws {Error} If the HTTP response has a non-OK status; message includes the status code.
 */
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

/**
 * Retrieve Ethernet-related configuration from the API.
 * @returns {Promise<object>} The parsed JSON configuration object.
 * @throws {Error} If the HTTP response has a non-OK status (error message includes the status code).
 */
export async function fetchEthConfig() {
  const response = await fetch(`${API_BASE_URL}/eth_get_config`);
  if (!response.ok) {
    throw new Error(`HTTP error! status: ${response.status}`);
  }
  return response.json();
}
