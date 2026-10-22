#!/usr/bin/env bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

echo "=== Building TASK MANAGER for Android ==="
export ANDROID_HOME="/home/codex/Android/Sdk"
export JAVA_HOME="/usr/lib/jvm/java-17-openjdk"

echo "[1/4] Compiling React / TypeScript frontend..."
npm run build

echo "[2/4] Syncing web bundle to Capacitor Android platform..."
sed -i 's/JavaVersion.VERSION_21/JavaVersion.VERSION_17/g' node_modules/@capacitor/local-notifications/android/build.gradle 2>/dev/null || true
sed -i 's/jvmToolchain(21)/jvmToolchain(17)/g' node_modules/@capacitor/local-notifications/android/build.gradle 2>/dev/null || true
npx cap sync android

echo "[3/4] Building Android debug APK with Local Notifications..."
cd android
./gradlew assembleDebug
cd ..

echo "[4/4] Copying final APK..."
cp android/app/build/outputs/apk/debug/app-debug.apk TASK_MANAGER.apk
mkdir -p /home/codex/Desktop /home/codex/Downloads
cp android/app/build/outputs/apk/debug/app-debug.apk /home/codex/Desktop/TASK_MANAGER.apk
cp android/app/build/outputs/apk/debug/app-debug.apk /home/codex/Downloads/TASK_MANAGER.apk

echo "=== SUCCESS! ==="
echo "APK Location: $DIR/TASK_MANAGER.apk"
echo "Desktop: /home/codex/Desktop/TASK_MANAGER.apk"
echo "Downloads: /home/codex/Downloads/TASK_MANAGER.apk"
