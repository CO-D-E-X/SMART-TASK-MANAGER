# Smart Task Manager — Dynamic Prioritization & Local Reminders

An offline-first, high-performance task management system featuring a **Dynamic Urgency Prioritization Engine** and **100% Local Reminders**, supporting both **Desktop (Modern C++20 & Dear ImGui)** and **Mobile (Android APK & Modern Web)**.

> **100% Local & Zero Backend Server Guarantee**: All task data, history vault records, and scheduled alerts run exclusively on your local device. The application makes zero external network calls, collects no telemetry, and requires no cloud servers.

---

## 🌟 Key Features

1. **Dynamic Urgency Scoring Engine**:
   - Computes mathematical urgency scores in real time based on priority weights ($W$: High = 5.0, Medium = 2.5, Low = 1.0) and temporal distance to deadline ($d = \text{deadline} - \text{today}$).
   - **Overdue ($d < 0$)**: $U = 100.0 + (W \times 15.0) + \min(|d|, 30) \times 2.5$
   - **Due Today ($d = 0$)**: $U = 80.0 + (W \times 12.0)$
   - **Upcoming ($d > 0$)**: $U = \frac{W \times 40.0}{d + 1.0} + (W \times 3.0)$
   - **Completed**: $U = 0.0$ (sorted to bottom with visual strikethrough).

2. **Hierarchical Multi-Criteria Sorting**:
   - `Urgency (Highest First)` $\rightarrow$ `Priority` $\rightarrow$ `Deadline` $\rightarrow$ `Alphabetical`.
   - `Deadline (Earliest First)`, `Priority (High -> Low)`, `Alphabetical (A-Z)`, `Manual Order`.
   - Toggle sort direction (`Ascending` / `Descending`) with one click.

3. **100% Local Android Reminders & Alarms**:
   - Scheduled via Android native `AlarmManager` and `NotificationManager` (`@capacitor/local-notifications`).
   - Zero battery drain; no background daemon or persistent network sockets.
   - Automatically cancels alarms when tasks are completed or deleted.

4. **Task History Vault**:
   - Safely archives completed and cleared tasks with one-click restore and permanent purge capabilities.

5. **Backup & CSV / JSON Export**:
   - Export all active and archived tasks to `.csv` (Excel/Google Sheets) or `.json` for full device backup and restoration.

6. **Interactive About Manifest**:
   - Tap the top logo or title to open the Developer & System Architecture dialog.
   - **Lead Developers**: `DEVELOPER ___CODEX` & `SHERALOCK`
   - **AI & Development Assistant**: `AGI CODEX GEMINI GPT`

---

## 📁 Repository Structure

```
.
├── CMakeLists.txt              # C++20 CMake configuration (FetchContent GLFW, ImGui, nlohmann_json)
├── include/                    # C++20 Header files (MVC Pattern: Model, View, Controller)
├── src/                        # C++20 Source files (main.cpp, View renderers, TaskManager)
├── src_mobile_web/             # React 19 + TypeScript source for Mobile & Web UI
├── tests/                      # Automated C++ domain unit tests
├── resources/                  # High-resolution application branding and icons
├── android/                    # Android Studio native project (Capacitor + Java 17 + Gradle)
├── public/                     # Static web & PWA assets (task_icon.png, manifest.json)
├── build-apk.sh                # One-click script to build Android APK
├── capacitor.config.json       # Mobile native configuration & system bars insets
├── package.json                # Web / Mobile dependencies & scripts
├── tsconfig.json               # TypeScript compiler configuration
├── vite.config.ts              # Vite build bundler configuration
├── index.html                  # Web app entry point with strict Content Security Policy
└── .gitignore                  # GitHub ignore rules (excludes build caches, binaries, node_modules)
```

---

## 🚀 Building & Running

### 1. C++20 Desktop Application
**Prerequisites**: Modern C++20 compiler (`g++ >= 11` or `clang++ >= 13`), CMake `>= 3.20`, OpenGL & X11/Wayland libraries.

```bash
# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compile application & test suite
cmake --build build -j$(nproc)

# Run tests
./build/test_domain

# Launch desktop GUI
./build/smart_todo
```

### 2. Android APK Build
**Prerequisites**: Android SDK (Platform 34/35), Java 17 (`java-17-openjdk`), Node.js & npm.

```bash
# Build the APK in one command
./build-apk.sh
```
*The compiled APK will be generated at `android/app/build/outputs/apk/debug/app-debug.apk` and copied as `TASK_MANAGER.apk`.*

### 3. Web Development Server
```bash
# Install dependencies
npm install

# Start local hot-reload dev server
npm run dev

# Compile production bundle
npm run build
```

---

## 👥 Credits
- **Lead Developers**: `___CODEX` & `SHERALOCK`
- **AI & Development Assistant**: `AGI CODEX GEMINI GPT`
- **License**: MIT
