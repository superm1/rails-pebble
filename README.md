# Rails Watchface for Pebble Time 2

A feature-rich, configurable digital watchface for the Pebble ecosystem, inspired by the Garmin "Rails" watchface. Designed specifically for the **Pebble Time 2** (Emery)'s large 200×228 color display.

## Features

### ⏰ Time Display
- Large central time with beautiful LECO vector font
- 12-hour or 24-hour format toggle
- Horizontal date display with calendar icon: "Sun [10] May"

### 🎨 2 Color Themes with 6 Accent Colors
**Themes:**
- **Dark** - Black background, white text
- **Light** - White background, black text

**Accent Colors:**
- Blue
- Green
- Red (Crimson)
- Amber
- Purple
- Cyan

Themes are toggled by long-pressing the SELECT button. Accent colors can be cycled through.

### 📊 6 Configurable Data Fields (2 rows × 3 columns)
Each field has an icon and can display:
- **Steps** - Daily step count (with auto-calculated goal from 7-day average)
- **Heart Rate** - Current BPM
- **Calories** - Active calories burned (with flame icon)
- **Distance** - Distance traveled in km/miles (with location pin icon)
- **Battery** - Battery percentage with icon
- **Weather** - Temperature and condition icon

### 📈 2 Rail Gauges
Horizontal progress bars showing daily goal completion:
- Steps (auto-calculated goal or configurable)
- Calories (configurable goal)
- Distance (configurable goal)
- Battery (0-100%)

### 🎯 Smart Icon Design
- All icons use PDC (Pebble Draw Command) vector format for sharp rendering
- Icons dynamically recolor based on theme (light/dark mode support)
- 25×25 pixel field icons, 20×20 pixel bluetooth icon
- Recent icon updates:
  - Bluetooth: Modern filled bluetooth symbol
  - Calories: Clean flame icon
  - Distance: Location pin marker

### ⌨️ On-Watch Controls
- **UP/DOWN buttons**: Cycle through themes/accent colors
- **LONG SELECT**: Toggle theme (Dark ↔ Light)

### 📱 Mobile App Configuration
Settings available via the Pebble mobile app configuration page:
- Theme selection (Dark/Light)
- Accent color selection
- Individual data field selectors (6 slots)
- Gauge type selectors (2 gauges)
- Gauge maximum values (steps, calories, distance)
- Temperature unit (Celsius/Fahrenheit)
- Distance unit (Kilometers/Miles)
- Show phone connection icon toggle

## Layout

```
┌──────────────────────────────────┐
│           [BT]                   │ ← Bluetooth icon (centered)
│                                  │
│  [#]  0  [♥]  0  [📍] 0.0       │ ← Data row 1 (3 fields)
│ ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ │ ← Rail gauge 1
│         11:24                    │ ← Large LECO font time
│ ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ │ ← Rail gauge 2
│  [🌙]  0  [🔋] 80%  [0.0]       │ ← Data row 2 (3 fields)
│                                  │
│       Sun [10] May               │ ← Date with calendar icon
└──────────────────────────────────┘
```

## Building

### Prerequisites

1. Install the Pebble SDK:
   ```bash
   pip3 install pebble-tool
   ```

2. Install the ARM cross-compiler:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install gcc-arm-none-eabi
   
   # macOS
   brew install --cask gcc-arm-embedded
   ```

3. Install Node.js dependencies:
   ```bash
   npm install
   ```

### Build Commands

```bash
# Build the watchface
pebble build

# Install to emulator (Emery/Pebble Time 2)
pebble wipe
pebble install --emulator emery

# Take a screenshot
pebble screenshot
```

### Building Icons

Icons are SVG files converted to PDC format:

```bash
# Convert SVG to PDC (requires pebble SDK tools)
PYTHONPATH=~/.pebble-sdk/SDKs/4.9.169/sdk-core/pebble/common/tools:$PYTHONPATH \
  python3 tools/svg2pdc.py input.svg -o output.pdc
```

SVG files are located in `resources/data/` and converted PDC files go to the same directory.

## Project Structure

```
rails-pebble/
├── package.json              # Project manifest & resource definitions
├── appinfo.json              # Legacy Pebble app metadata
├── README.md                 # This file
├── generate_icons.py         # Python script to generate raster icons
├── src/
│   └── c/
│       ├── rails.c          # Main watchface logic & layout
│       ├── rails.h          # Header with constants, types, & state
│       ├── config.c         # Configuration persistence
│       ├── config.h         # Config header
│       ├── display.c        # Theme & display management
│       ├── fields.c         # Data field rendering & formatting
│       ├── gauges.c         # Rail gauge rendering
│       ├── gauges.h         # Gauge header
│       ├── weather.c        # Weather data integration
│       ├── weather.h        # Weather header
│       ├── icons.c          # Icon drawing (bluetooth, etc.)
│       ├── icons.h          # Icons header
│       ├── util.c           # Utility functions (recoloring)
│       ├── util.h           # Utility header
│       └── storage.c        # Persistent storage
└── resources/
    ├── data/                # PDC vector icons & SVG sources
    │   ├── *.pdc           # PDC vector icons
    │   └── *.svg           # SVG source files
    ├── fonts/              # Vector fonts (from dependencies)
    └── images/             # Raster images (generated)
```

## Dependencies

The project uses npm packages for fonts:
- **pebble-fctx** - FreeType Context rendering library for vector fonts
- **pebble-utf8** - UTF-8 text support

These provide the beautiful LECO and Avenir fonts used in the watchface.

## Technical Details

### Display Specifications
- **Resolution**: 200×228 pixels
- **Platform**: Emery (Pebble Time 2)
- **Color**: 64-color palette

### Field Layout
- **Field slots**: 6 (2 rows × 3 columns)
- **Field text width**: 35 pixels (centered)
- **Icon size**: 25×25 pixels
- **Icon-to-text gap**: 2 pixels
- **Field group spacing**: 4 pixels

### APIs Used

| API | Purpose |
|-----|---------|
| Pebble Health Service | Steps, calories, distance, heart rate |
| Battery Service | Battery percentage & charging status |
| Connection Service | Bluetooth connection status |
| Persistent Storage | Save/load user configuration |
| App Config | Mobile app settings UI |
| GDrawCommand | Vector icon rendering (PDC format) |
| FContext (pebble-fctx) | Vector font rendering |

### Icon Rendering
All icons use `gdraw_command_image_recolor()` to dynamically adapt to the current theme. This ensures:
- Icons appear in the correct color for light/dark themes
- Accent colors are properly applied
- No separate icon sets needed for different themes

## Limitations vs. Garmin Rails

The Pebble Time 2 lacks some sensors present on Garmin watches:

| Feature | Rails (Garmin) | Rails (Pebble) |
|---------|---------------|----------------|
| GPS coordinates | ✅ | ❌ No GPS |
| Barometer | ✅ | ❌ No sensor |
| Altitude | ✅ | ❌ No sensor |
| Floors climbed | ✅ | ❌ No sensor (SDK 4 limitation) |
| Solar intensity | ✅ | ❌ No UV sensor |
| Body Battery | ✅ | ❌ Proprietary |
| Stress score | ✅ | ❌ Proprietary |
| Training effect | ✅ | ❌ Proprietary |

## Known Issues

- Floors climbed field is not available in Pebble SDK 4
- Weather requires phone app integration (not yet implemented)
- Some npm package vulnerabilities (inherited from dependencies)

## License

MIT License - feel free to use, modify, and distribute.

## Credits

- Inspired by [Rails](https://reed.works/vgfsyhcxft-connect-iq-apps/details-rails/) by Reed Works
- Uses fonts and rendering techniques from [TimeStyle](https://github.com/freakified/TimeStylePebble) by freakified
- Icons designed for clarity at small sizes (25×25 pixels)
- Bluetooth icon from [SVG Repo](https://www.svgrepo.com/)
- Calorie flame icon from [SVG Repo](https://www.svgrepo.com/)
