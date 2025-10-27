import type { CertificatesStatus, CertificateType, MqttConfig, MiscConfig, ApiResponse, ActionsConfig, ApiError, ApiSuccess } from '../types/api';
import { notifications } from '../stores/notifications.svelte.js';

export async function saveConfig<T = MqttConfig | MiscConfig | ActionsConfig>(type: string, data: T): Promise<ApiResponse<T>> {
  try {
    const response = await fetch(`/config/save?type=${type}`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(data),
    });

    if (!response.ok) {
      const errorData : ApiError = await response.json();
      notifications.addError(`Failed to save ${type} config: ${errorData.error}`);
      return errorData;
    }

    const result : ApiSuccess = await response.json();
    notifications.addSuccess(result.message);
    return result;
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to save ${type} config: ${message}`);
    return { success: false, error: message };
  }
}

// Certificate management endpoints
export async function uploadCertificate(type: string, content: string | ArrayBuffer): Promise<ApiResponse<undefined>> {
  try {
    const response = await fetch(`/certificates/upload?type=${type}`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-pem-file',
      },
      body: content,
    });

    if (!response.ok) {
      const errorData : ApiError = await response.json();
      notifications.addError(`Failed to upload certificate bundle: ${errorData.error}`);
      return errorData;
    }

    const result: ApiSuccess = await response.json();
    if (result.success) {
      notifications.addSuccess(result.message);
    } else {
      notifications.addError(result.message);
    }
    return result;
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to upload certificate bundle: ${message}`);
    return { success: false, error: message };
  }
}

export async function getCertificateStatus(): Promise<ApiResponse<CertificatesStatus>> {
  try {
    const response = await fetch(`/certificates/status`);
    console.log(response);
    if (!response.ok) {
      const errorData : ApiError = await response.json();
      notifications.addError(`Failed to get certificate status: ${errorData.error}`);
      return errorData;
    }
    const result: ApiResponse<CertificatesStatus> = await response.json();
    return result;
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to get certificate status: ${message}`);
    return { success: false, error: message };
  }
}

export async function deleteCertificate(type: CertificateType): Promise<ApiResponse<undefined>> {
  try {
    const response = await fetch(`/certificates/${type}`, {
      method: 'DELETE',
    });

    if (!response.ok) {
      const errorData : ApiError = await response.json();
      notifications.addError(`Failed to delete ${type} certificate: ${errorData.error}`);
      return errorData;
    }

    const result: ApiResponse<undefined> = await response.json();
    if (result.success) {
      notifications.addSuccess(`${type} certificate deleted successfully`);
    } else {
      notifications.addError(`Failed to delete ${type} certificate: ${result.error || 'Unknown error'}`);
    }
    return result;
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unknown error occurred';
    notifications.addError(`Failed to delete ${type} certificate: ${message}`);
    return { success: false, error: message };
  }
}
