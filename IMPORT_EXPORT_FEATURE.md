# Import/Export Messages Feature

## Overview
Added import and export functionality for radio station messages in the web UI. This allows users to quickly sync messages across multiple devices using a JSON file format.

## Features

### Export
- Click the "📥 Export" button on the Messages page
- Downloads a JSON file with all station messages, settings, and metadata
- File is automatically named with the current date: `radio-messages-YYYY-MM-DD.json`
- Includes:
  - Station index
  - Station name
  - Message content
  - Enabled/disabled state
  - Wave band (LONG_WAVE, MEDIUM_WAVE, SHORT_WAVE)
- **Note:** Frequencies are NOT included, as they may be fine-tuned per device and should not be overridden

### Import
- Click the "📤 Import" button on the Messages page
- Select a previously exported JSON file
- Messages are automatically imported and saved
- Page reloads to show the updated messages
- Success/error notifications displayed via toast messages

## Technical Implementation

### Backend Changes

#### WiFiManager.h
- Added two new handler methods:
  - `handleExportMessages()` - GET /api/messages
  - `handleImportMessages()` - POST /api/messages

#### WiFiManager.cpp

**New API Endpoints:**
1. `GET /api/messages` - Exports all station data as JSON
2. `POST /api/messages` - Imports station data from JSON

**Export Handler:**
- Serializes all stations to JSON format
- Returns structured data including messages, frequencies, and states

**Import Handler:**
- Validates incoming JSON
- Updates station messages and enabled states
- Saves changes to preferences
- Returns success/failure with message count

### Frontend Changes

**JavaScript Functions:**
- `exportMessages()` - Fetches data and triggers file download
- `importMessages()` - Reads uploaded file and posts to server
- `triggerFileInput()` - Opens file picker dialog

**UI Components:**
- Import/Export section added to the Messages page
- Two buttons with clear labels and emoji icons
- Hidden file input for import functionality
- Descriptive text explaining the feature

**CSS Styling:**
- `.import-export-section` - Container styling with border separator
- `.import-export-buttons` - Responsive button layout
- `.btn-export` - Secondary color for export button
- `.btn-import` - Primary color for import button
- Hidden file input element

## Usage

### To Backup Messages:
1. Open the web interface
2. Navigate to "Messages" page
3. Scroll to the bottom
4. Click "📥 Export"
5. Save the downloaded JSON file

### To Sync/Restore Messages:
1. Open the web interface on another device (or same device)
2. Navigate to "Messages" page
3. Scroll to the bottom
4. Click "📤 Import"
5. Select a previously exported JSON file
6. Wait for success message
7. Page will reload with imported messages

## File Format

```json
{
  "stations": [
    {
      "index": 0,
      "name": "Station Name",
      "message": "YOUR MESSAGE HERE",
      "enabled": true,
      "band": "LONG_WAVE"
    }
  ]
}
```

**Note:** Frequencies are intentionally excluded from the export/import process to preserve device-specific tuning calibrations.

## Benefits
- **Easy Backup**: Save your message configurations
- **Quick Sync**: Share messages across multiple devices
- **Version Control**: Keep different message sets for different purposes
- **No Manual Entry**: Avoid typing messages on each device
- **Frontend Only**: All import/export happens in the browser and device

## Compatibility
- Works with all modern browsers
- No external dependencies required
- Compatible with the existing station management system
- Preserves all station metadata during import/export

