---
title: "WebServerManager"
---

**Description:** Manages the web-based configuration portal. This class initializes and configures the AsyncWebServer, sets up all API and action endpoints, serves static files, and handles requests to view or modify the device's configuration.

### Public Methods:

*   **`WebServerManager(ConfigManager& configManager, ReaderDataManager& readerDataManager)`**
    *   **Description:** Constructs the WebServerManager.
    *   **Parameters:**
        *   `configManager` (`ConfigManager&`): Reference to the configuration manager.
        *   `readerDataManager` (`ReaderDataManager&`): Reference to the HomeKey reader data manager.
*   **`void begin()`**
    *   **Description:** Initializes the web server and attaches all handlers.

