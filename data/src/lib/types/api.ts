/**
 * HomeKey information structure containing reader and issuer data
 * @type {HKInfo}
 */
export interface HKInfo {
  /** Reader group identifier */
  group_identifier: string;
  /** Reader unique identifier */
  unique_identifier: string;
  /** Array of issuers associated with this reader */
  issuers: Array<{
    /** Unique identifier for the issuer */
    issuerId: string;
    /** Array of endpoints for this issuer */
    endpoints: Array<{
      /** Unique identifier for the endpoint */
      endpointId: string;
    }>;
  }>;
}

/**
 * MQTT configuration structure for broker connection and topic management
 * @type {MqttConfig}
 */
export interface MqttConfig {
  /** MQTT broker hostname or IP address */
  mqttBroker: string;
  /** MQTT broker port number */
  mqttPort: number;
  /** MQTT username for authentication */
  mqttUsername: string;
  /** MQTT password for authentication */
  mqttPassword: string;
  /** Unique MQTT client identifier */
  mqttClientId: string;
  /** Last Will and Testament topic */
  lwtTopic: string;
  /** HomeKey status topic */
  hkTopic: string;
  /** Lock state status topic */
  lockStateTopic: string;
  /** Lock state command topic */
  lockStateCmd: string;
  /** Lock continuous state command topic */
  lockCStateCmd: string;
  /** Lock timed state command topic */
  lockTStateCmd: string;
  /** Battery level command topic */
  btrLvlCmdTopic: string;
  /** HomeKey alternate action topic */
  hkAltActionTopic: string;
  /** Custom lock state topic */
  lockCustomStateTopic: string;
  /** Custom lock state command topic */
  lockCustomStateCmd: string;
  /** Enable custom lock states */
  lockEnableCustomState: boolean;
  /** Enable Home Assistant MQTT discovery */
  hassMqttDiscoveryEnabled: boolean;
  /** Disable NFC tag publishing */
  nfcTagNoPublish: boolean;
  /** Use SSL/TLS for MQTT connection */
  useSSL: boolean;
  /** CA certificate content (required when useSSL is true) */
  caCert?: string;
  /** Client certificate content (optional for client authentication) */
  clientCert?: string;
  /** Client private key content (required when clientCert is provided) */
  clientKey?: string;
  /** Allow insecure SSL connections (not recommended for production) */
  allowInsecure: boolean;
  /** Custom lock state mappings */
  customLockStates: Record<string, number>;
  /** Custom lock action mappings */
  customLockActions: Record<string, number>;
}

/**
 * Miscellaneous configuration structure for device settings and hardware configuration
 * @type {MiscConfig}
 */
export interface MiscConfig {
  /** Device name displayed in HomeKit and web interface */
  deviceName: string;
  /** OTA update password for firmware updates */
  otaPasswd: string;
  /** HomeKey LED color (RGB value) */
  hk_key_color: number;
  /** HomeKit setup code for pairing */
  setupCode: string;
  /** Force lock to always unlock (overrides normal behavior) */
  lockAlwaysUnlock: boolean;
  /** Force lock to always lock (overrides normal behavior) */
  lockAlwaysLock: boolean;
  /** Enable HomeKey auth precompute cache (faster taps, higher CPU/RAM) */
  hkAuthPrecomputeEnabled: boolean;
  /** GPIO pin for lock control */
  controlPin: number;
  /** GPIO pin for HomeSpan status indicator */
  hsStatusPin: number;
  /** Enable web authentication */
  webAuthEnabled: boolean;
  /** Web interface username */
  webUsername: string;
  /** Web interface password */
  webPassword: string;
  /** NFC GPIO pin configuration preset index */
  nfcPinsPreset: number;
  /** NFC GPIO pin configuration [pin1, pin2, pin3, pin4] */
  nfcGpioPins: [number, number, number, number];
  /** Battery low status threshold percentage */
  btrLowStatusThreshold: number;
  /** Enable proximity battery monitoring */
  proxBatEnabled: boolean;
  /** Enable Ethernet connectivity */
  ethernetEnabled: boolean;
  /** Active Ethernet preset index */
  ethActivePreset: number;
  /** Ethernet PHY type */
  ethPhyType: number;
  /** RMII configuration [phy_addr, pin_mcd, pin_mdio, pin_power, pin_rmii_clock] */
  ethRmiiConfig: [number, number, number, number, number];
  /** SPI configuration [freq_mhz, pin_cs, pin_irq, pin_rst, pin_sck, pin_miso, pin_mosi] */
  ethSpiConfig: [number, number, number, number, number, number, number];
}

/**
 * Miscellaneous configuration structure for device settings and hardware configuration
 * @type {ActionsConfig}
 */
export interface ActionsConfig {
  /** GPIO pin for NFC NeoPixel LED */
  nfcNeopixelPin: number;
  /** NeoPixel LED type configuration */
  neoPixelType: number;
  /** RGB color values for successful NFC operations */
  neopixelSuccessColor: [number, number][];
  /** RGB color values for failed NFC operations */
  neopixelFailureColor: [number, number][];
  /** Duration in milliseconds for success LED animation */
  neopixelSuccessTime: number;
  /** Duration in milliseconds for failure LED animation */
  neopixelFailTime: number;
  /** Duration in milliseconds for tag event LED animation */
  neopixelTagEventTime: number;
  /** RGB color values for tag event LED animation */
  neopixelTagEventColor: [number, number][];
  /** GPIO pin for NFC success indicator */
  nfcSuccessPin: number;
  /** Duration in milliseconds for NFC success signal */
  nfcSuccessTime: number;
  /** High/Low state for NFC success signal */
  nfcSuccessHL: boolean;
  /** GPIO pin for NFC failure indicator */
  nfcFailPin: number;
  /** Duration in milliseconds for NFC failure signal */
  nfcFailTime: number;
  /** High/Low state for NFC failure signal */
  nfcFailHL: boolean;
  /** GPIO pin for tag event */
  tagEventPin: number;
  /** Duration in milliseconds for tag event signal */
  tagEventTimeout: number;
  /** High/Low state for tag event signal */
  tagEventHL: boolean;
  /** GPIO pin for action button */
  gpioActionPin: number;
  /** Lock state when GPIO action is triggered */
  gpioActionLockState: boolean;
  /** Unlock state when GPIO action is triggered */
  gpioActionUnlockState: boolean;
  /** Enable momentary GPIO action (timed) */
  gpioActionMomentaryEnabled: number;
  /** HomeKey GPIO controlled state */
  hkGpioControlledState: boolean;
  /** Timeout in milliseconds for momentary GPIO action */
  gpioActionMomentaryTimeout: number;
  /** Enable HomeKey dumb switch mode */
  hkDumbSwitchMode: boolean;
  /** GPIO pin for HomeKey alternate action initialization */
  hkAltActionInitPin: number;
  /** GPIO pin for HomeKey alternate action initialization LED */
  hkAltActionInitLedPin: number;
  /** Timeout in milliseconds for alternate action initialization */
  hkAltActionInitTimeout: number;
  /** GPIO pin for HomeKey alternate action */
  hkAltActionPin: number;
  /** Timeout in milliseconds for alternate action */
  hkAltActionTimeout: number;
  /** GPIO state for HomeKey alternate action */
  hkAltActionGpioState: number;
}

/**
 * Ethernet configuration structure for network connectivity
 * @type {EthConfig}
 */
export interface EthConfig {
  /** Array of supported Ethernet chips */
  supportedChips: Array<{
    /** Chip name */
    name: string;
    /** Whether the chip supports EMAC */
    emac: boolean;
    /** PHY type identifier */
    phy_type: number;
  }>;
  /** Array of board-specific Ethernet presets */
  boardPresets: Array<{
    /** Preset name */
    name: string;
    /** Ethernet chip configuration */
    ethChip: {
      /** Chip name */
      name: string;
      /** Whether the chip supports EMAC */
      emac: boolean;
      /** PHY type identifier */
      phy_type: number;
    };
    /** RMII interface configuration */
    rmii_conf: {
      /** PHY address */
      phy_addr: number;
      /** MDC pin number */
      pin_mcd: number;
      /** MDIO pin number */
      pin_mdio: number;
      /** Power pin number */
      pin_power: number;
      /** RMII clock pin number */
      pin_rmii_clock: number;
    };
    /** SPI interface configuration */
    spi_conf: {
      /** SPI frequency in MHz */
      spi_freq_mhz: number;
      /** Chip select pin number */
      pin_cs: number;
      /** Interrupt pin number */
      pin_irq: number;
      /** Reset pin number */
      pin_rst: number;
      /** SPI clock pin number */
      pin_sck: number;
      /** SPI MISO pin number */
      pin_miso: number;
      /** SPI MOSI pin number */
      pin_mosi: number;
    };
  }>;
  /** Whether Ethernet is enabled */
  ethEnabled: boolean;
}

export interface NfcGpioPinsPreset {
  presets: {
    name: string;
    gpioPins: number[];
  }[];
}

/**
 * OTA (Over-The-Air) update status information
 * @type {OTAStatus}
 */
export interface OTAStatus {
  /** Message type identifier */
  type: "ota_status" | "ota_info";
  /** Whether an OTA update is currently in progress */
  in_progress: boolean;
  /** Number of bytes written so far */
  bytes_written: number;
  /** Type of upload (e.g., 'firmware', 'filesystem') */
  upload_type: string;
  /** Error message if update failed */
  error?: string;
  /** Current firmware version */
  current_version: string;
  /** Currently running partition */
  running_partition: string;
  /** Partition that will be used for the next update */
  next_update_partition: string;
  /** Update progress percentage (0-100) */
  progress_percent?: number;
  /** Total bytes to be written */
  total_bytes?: number;
}

/**
 * Certificate status information for SSL/TLS certificates
 * @type {CertificatesStatus}
 */
export interface CertificatesStatus {
  /** CA certificate status */
  ca?: {
    /** Certificate issuer information */
    issuer: string;
    /** Certificate subject information */
    subject: string;
    /** Certificate expiration date */
    expiration: {
      from: string,
      to: string
    };
  };
  /** Client certificate status */
  client?: {
    /** Certificate issuer information */
    issuer: string;
    /** Certificate subject information */
    subject: string;
    /** Certificate expiration date */
    expiration: {
      from: string,
      to: string
    };
    /** Whether the private key matches this certificate */
    keyMatchesCert?: boolean;
  };
  /** Private key status */
  privateKey?: {
    /** Whether the private key exists */
    exists: boolean;
  };
}

/**
 * Certificate type identifiers for SSL/TLS operations
 * @type {CertificateType}
 */
export type CertificateType = 'ca' | 'client' | 'privateKey';

// Log Viewer Types

/**
 * Log level enumeration matching the backend loggable library.
 */
export type LogLevel = 'ERROR' | 'WARN' | 'INFO' | 'DEBUG' | 'VERBOSE';

/**
 * Log message format received from WebSocket
 * Matches the JSON structure sent by WebSocketLogSinker
 * @type {LogMessage}
 */
export interface LogMessage {
  /** System Clock Timestamp */
  ts: number;
  /** Milliseconds since boot */
  uptime: number;
  /** Message type identifier */
  type: 'log';
  /** Log severity level */
  level: LogLevel;
  /** Log source/component tag */
  tag: string;
  /** Log message content */
  msg: string;
}

/**
 * Extended log entry for component use
 * Includes additional fields added by the LogViewer component
 * @type {LogEntry}
 */
export interface LogEntry extends LogMessage {
  /** Unique identifier for the log entry */
  id: number;
  /** ISO timestamp when received by client */
  localts: string;
  /** UI state for expanded/collapsed view */
  expanded: boolean;
}

// Notification Types
/**
 * Available notification types for user feedback
 * @type {NotificationType}
 */
export type NotificationType = 'success' | 'error' | 'warning' | 'info';

/**
 * Notification structure for user feedback messages
 * @type {Notification}
 */
export interface Notification {
  /** Unique notification identifier */
  id: string;
  /** Type of notification determining styling and icon */
  type: NotificationType;
  /** Notification message content */
  message: string;
  /** Auto-dismiss timeout in milliseconds (optional) */
  timeout?: number;
}

// API Response Types
/**
 * API error response structure
 * @type {ApiError}
 */
export interface ApiError {
  /** Always false for error responses */
  success: false;
  /** Error message describing what went wrong */
  error: string;
}

/**
 * API success response structure
 * @type {ApiSuccess<T>}
 * @template T - The type of data returned in the response
 */
export interface ApiSuccess<T = any> {
  /** Always true for successful responses */
  success: true;
  /** The response data payload */
  data: T;
  /** Message decribing result */
  message: string;
}

/**
 * Union type for API responses (success or error)
 * @type {ApiResponse<T>}
 * @template T - The type of data in successful responses
 */
export type ApiResponse<T = any> = ApiSuccess<T> | ApiError;
export { isApiSuccess, isApiError, isLogLevel, isNotificationType, isCertificateType } from '$lib/utils/index.js';

type Only<T, U> = {
[P in keyof T]: T[P];
} & {
  [P in keyof U]?: never;
};

export type Either<T, U> = Only<T, U> | Only<U, T>;
