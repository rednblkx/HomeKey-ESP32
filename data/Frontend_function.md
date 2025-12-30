# Frontend Module

## Description
The Frontend is a Single Page Application (SPA) built with Svelte and Vite. It provides a modern, responsive user interface for configuring and controlling the HomeKey-ESP32 device.

## Key Responsibilities
- **User Interface**: Displays the current lock state, logs, and configuration forms.
- **API Interaction**: Communicates with the `WebServerManager` via REST APIs to fetch and save settings.
- **Real-time Updates**: Connects via WebSocket to display live logs and instant state changes.
- **HomeKey Management**: Provides a UI to view provisioned HomeKey issuers and manage reader settings.

## Dependencies
- **Svelte**: The UI framework.
- **Vite**: The build tool.
- **Tailwind CSS** (likely): For styling.
- **Backend API**: Depends on the endpoints provided by `WebServerManager`.

## Key Files
- `data/src/` (Source code)
- `data/package.json` (Dependencies)
- `data/vite.config.ts` (Build configuration)
