import type { ApiResponse, ApiSuccess, ApiError, LogLevel, NotificationType, CertificateType } from '$lib/types/api.js';

/**
 * Type guard to check if an API response is successful
 * @param response - The API response to check
 * @returns True if the response is successful
 */
export function isApiSuccess<T>(response: ApiResponse<T>): response is ApiSuccess<T> {
	return response.success === true;
}

/**
 * Type guard to check if an API response is an error
 * @param response - The API response to check
 * @returns True if the response is an error
 */
export function isApiError<T>(response: ApiResponse<T>): response is ApiError {
	return response.success === false;
}

/**
 * Type guard to check if a value is a valid LogLevel
 * @param value - The value to check
 * @returns True if the value is a valid LogLevel
 */
export function isLogLevel(value: any): value is LogLevel {
	return typeof value === 'string' &&
		['ERROR', 'WARN', 'INFO', 'DEBUG', 'VERBOSE', 'NONE'].includes(value);
}

/**
 * Type guard to check if a value is a valid NotificationType
 * @param value - The value to check
 * @returns True if the value is a valid NotificationType
 */
export function isNotificationType(value: any): value is NotificationType {
	return typeof value === 'string' &&
		['success', 'error', 'warning', 'info'].includes(value);
}

/**
 * Type guard to check if a value is a valid CertificateType
 * @param value - The value to check
 * @returns True if the value is a valid CertificateType
 */
export function isCertificateType(value: any): value is CertificateType {
	return typeof value === 'string' &&
		['ca', 'client', 'privateKey'].includes(value);
}