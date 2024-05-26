// gui.h

#pragma once

#include <atomic>
#include <d3d9.h>
#include <Windows.h>

class gui
{
public:

    static constexpr int WIDTH = 390; // Definition der Fensterbreite
    static constexpr int HEIGHT = 210; // Definition der Fensterh�he


    static bool isGuiTopMost; // Variable f�r den TopMost-Zustand
    static std::atomic<bool> triggerbot_enabled; // Variable f�r den Triggerbot-Status
    static std::atomic<bool> bunnyhop_enabled; // Variable f�r den Bunnyhop-Status
    static bool isGuiVisible; // Variable f�r die Sichtbarkeit der GUI
    static bool hold_mode_enabled; // Variable f�r den Hold-Modus

    static WNDCLASSEX windowClass; // Fensterklassenstruktur
    static HWND window; // Fenstergriff
    static IDirect3D9* d3d; // Direct3D-Objekt
    static IDirect3DDevice9* device; // Direct3D-Ger�t
    static D3DPRESENT_PARAMETERS presentParameters; // Pr�sentationsparameter
    static POINTS position; // Klickposition
    static bool isRunning; // Laufvariable

    static int color_tolerance; // Farbtoleranz

    static void ColorToleranceCallback(int* value); // Callback-Funktion f�r Farbtoleranz�nderung

    static void RenderTriggerbotTab() noexcept;
    static void RenderRegionSizeSlider() noexcept;
    static void RenderMiscTab() noexcept;
    static void RenderHoldModeCheckbox() noexcept;
    static void ToggleTriggerbotHoldMode() noexcept;
    static void RenderTriggerbotCheckbox() noexcept;
    static void RenderBunnyHopCheckbox() noexcept;

    static void CreateHWindow(const char* windowName) noexcept;
    static void DestroyHWindow() noexcept;
    static bool CreateDevice() noexcept;
    static void ResetDevice() noexcept;
    static void DestroyDevice() noexcept;
    static void CreateImGui() noexcept;
    static void DestroyImGui() noexcept;
    static void BeginRender() noexcept;
    static void EndRender() noexcept;
    static void Render() noexcept;
};
