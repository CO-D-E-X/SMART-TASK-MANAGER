package com.codex.taskmanager;

import android.os.Bundle;
import android.view.Window;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import com.getcapacitor.BridgeActivity;

public class MainActivity extends BridgeActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        Window window = getWindow();
        // Prevent webview from drawing under the Android system status bar (time/battery)
        WindowCompat.setDecorFitsSystemWindows(window, true);

        // Ensure white/light status bar icons (clock, battery, Wi-Fi) on dark background
        WindowInsetsControllerCompat insetsController = WindowCompat.getInsetsController(window, window.getDecorView());
        if (insetsController != null) {
            insetsController.setAppearanceLightStatusBars(false);
        }
    }
}
