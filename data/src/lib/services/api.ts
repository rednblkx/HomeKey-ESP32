import type { CertificateUploadResponse, CertificatesStatus, DetailedCertificateStatus, CertificateType, MqttConfig, MiscConfig, ApiResponse } from '../types/api';
import { notifications } from '../stores/notifications.svelte.js';

/**
 * Fetch with retry logic for network errors
 */
async function fetchWithRetry(url: string, options: RequestInit, retries = 2): Promise<Response> {
  for (let i = 0; i <= retries; i++) {
    try {
      const response = await fetch(url, options);
      if (response.status >= 500 || response.status === 429) {
        // Retry on server errors or rate limiting
        if (i < retries) {
          await new Promise(resolve => setTimeout(resolve, 1000 * (i + 1))); // Exponential backoff
          continue;
        }
      }
      return response;
    } catch (error) {
      if (i < retries) {
        await new Promise(resolve => setTimeout(resolve, 1000 * (i + 1)));
        continue;
      }
      throw error;
    }
  }
  throw new Error('Max retries exceeded');
}

/**
 * Get user-friendly error message from response
 */
async function getErrorMessage(response: Response): Promise<string> {
  try {
    const contentType = response.headers.get('content-type');
    if (contentType === 'application/json') {
      const errorData = await response.json();
      return errorData.message || errorData.error || `HTTP ${response.status}`;
    } else if(contentType !== 'text/html') {
      const text = await response.text();
      return text || `HTTP ${response.status}`;
    } else return response.statusText;
  } catch {
    return `HTTP ${response.status}`;
  }
}

export async function saveConfig(type: string, data: MqttConfig | MiscConfig): Promise<ApiResponse<string>> {
  try {
    const response = await fetchWithRetry(`/config/save?type=${type}`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(data),
    });

    if (!response.ok) {
      const errorMessage = await getErrorMessage(response);
      notifications.addError(`Failed to save ${type} config: ${errorMessage}`);
      return { success: false, message: errorMessage, code: response.status };
    }

    const result = await response.text();
    notifications.addSuccess(result);
    return { success: true, data: result };
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to save ${type} config: ${message}`);
    return { success: false, message };
  }
}

// Certificate management endpoints
export async function uploadCertificateBundle(type: string, content: string | ArrayBuffer): Promise<ApiResponse<CertificateUploadResponse>> {
  try {
    const response = await fetchWithRetry(`/certificates/upload?type=${type}`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-pem-file',
      },
      body: content,
    });

    if (!response.ok) {
      const errorMessage = await getErrorMessage(response);
      notifications.addError(`Failed to upload certificate bundle: ${errorMessage}`);
      return { success: false, message: errorMessage, code: response.status };
    }

    const result: CertificateUploadResponse = await response.json();
    if (result.status == "success") {
      notifications.addSuccess(result.message);
    } else {
      notifications.addError(result.message);
    }
    return { success: true, data: result };
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to upload certificate bundle: ${message}`);
    return { success: false, message };
  }
}

export async function getCertificateStatus(): Promise<ApiResponse<CertificatesStatus>> {
  try {
    const response = await fetch(`/certificates/status`);
    console.log(response);
    if (!response.ok) {
      const errorMessage = await getErrorMessage(response);
      notifications.addError(`Failed to get certificate status: ${errorMessage}`);
      return { success: false, message: errorMessage, code: response.status };
    }
    const result: CertificatesStatus = await response.json();
    return { success: true, data: result };
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to get certificate status: ${message}`);
    return { success: false, message };
  }
}

export async function getDetailedCertificateStatus(): Promise<ApiResponse<DetailedCertificateStatus>> {
  try {
    const response = await fetch(`/certificates/status/detailed`);
    if (!response.ok) {
      const errorMessage = await getErrorMessage(response);
      notifications.addError(`Failed to get detailed certificate status: ${errorMessage}`);
      return { success: false, message: errorMessage, code: response.status };
    }
    const result: DetailedCertificateStatus = await response.json();
    return { success: true, data: result };
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to get detailed certificate status: ${message}`);
    return { success: false, message };
  }
}

export async function deleteCertificate(type: CertificateType): Promise<ApiResponse<CertificateUploadResponse>> {
  try {
    const response = await fetchWithRetry(`/certificates/${type}`, {
      method: 'DELETE',
    });

    if (!response.ok) {
      const errorMessage = await getErrorMessage(response);
      notifications.addError(`Failed to delete ${type} certificate: ${errorMessage}`);
      return { success: false, message: errorMessage, code: response.status };
    }

    const result: CertificateUploadResponse = await response.json();
    if (result.status == "success") {
      notifications.addSuccess(`${type} certificate deleted successfully`);
    } else {
      notifications.addError(`Failed to delete ${type} certificate: ${result.message || 'Unknown error'}`);
    }
    return { success: true, data: result };
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to delete ${type} certificate: ${message}`);
    return { success: false, message };
  }
}
