#include "gui.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include <windows.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#include <string>

bool gui::isGuiTopMost = false; // Deklaration der Variable für den TopMost-Zustand

int regionSize = 6; // Variable für die Regionengröße aka FOV

// Definition der statischen Variablen
std::atomic<bool> gui::triggerbot_enabled = false;
std::atomic<bool> gui::bunnyhop_enabled = false;
bool gui::isGuiVisible = true;
bool gui::hold_mode_enabled = false; // Definition der Variable für den Hold-Modus
WNDCLASSEX gui::windowClass; // Definition der Fensterklassenstruktur
HWND gui::window; // Definition des Fenstergriffs
IDirect3D9* gui::d3d = nullptr; // Definition des Direct3D-Objekts
IDirect3DDevice9* gui::device = nullptr; // Definition des Direct3D-Geräts
D3DPRESENT_PARAMETERS gui::presentParameters; // Definition der Präsentationsparameter
POINTS gui::position; // Definition der Klickposition
bool gui::isRunning = true; // Definition der Laufvariable
int gui::color_tolerance = 82; // Deklaration der Variable für die Farbtoleranz

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
        return true;

    switch (message)
    {
    case WM_SIZE: {
        if (gui::device && wideParameter != SIZE_MINIMIZED)
        {
            gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
            gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
            gui::ResetDevice();
        }
    } return 0;

    case WM_SYSCOMMAND: {
        if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
    } break;

    case WM_DESTROY: {
        PostQuitMessage(0);
    } return 0;

    case WM_LBUTTONDOWN: {
        gui::position = MAKEPOINTS(longParameter); // set click points
    } return 0;

    case WM_MOUSEMOVE: {
        if (wideParameter == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(longParameter);
            auto rect = ::RECT{};

            GetWindowRect(gui::window, &rect);

            rect.left += points.x - gui::position.x;
            rect.top += points.y - gui::position.y;

            if (gui::position.x >= 0 &&
                gui::position.x <= gui::WIDTH &&
                gui::position.y >= 0 && gui::position.y <= 19)
                SetWindowPos(
                    gui::window,
                    HWND_TOPMOST,
                    rect.left,
                    rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
        }

    } return 0;

    case WM_KEYDOWN: {
        if (wideParameter == VK_INSERT) {
            const BYTE transparency = gui::isGuiVisible ? 0 : 255;
            SetLayeredWindowAttributes(gui::window, RGB(0, 0, 0), transparency, LWA_ALPHA);
            gui::isGuiVisible = !gui::isGuiVisible;
        }
    } return 0;
    }

    return DefWindowProc(window, message, wideParameter, longParameter);
}

// Callback-Funktion für die Farbtoleranzänderung in der GUI
void gui::ColorToleranceCallback(int* value)
{
    gui::color_tolerance = *value; // Aktualisiere den Wert der Farbtoleranz in der main.cpp
}

// Definieren Sie eine Variable, um den Wert der 40 Millisekunden zu speichern und mehr
int milliseconds = 25;
int color_tolerance = 75;

void gui::RenderTriggerbotTab() noexcept
{
    if (ImGui::BeginTabItem("Triggerbot")) {

        ImGui::Checkbox("TopMost [WORKING]", &isGuiTopMost);

        RenderHoldModeCheckbox();
        RenderTriggerbotCheckbox();
        RenderRegionSizeSlider(); // Render FOV slider

        // Render slider for milliseconds
        ImGui::SliderInt("MS [TESTING]", &milliseconds, 1, 100);

        // Render input field for color tolerance
        ImGui::InputInt("Color [WORKING]", &color_tolerance);
        ColorToleranceCallback(&color_tolerance); // Aktualisiere den Wert der GUI-Farbtoleranz

        ImGui::EndTabItem();
    }
}


// GUI-Funktion zum Rendern eines Schiebereglers für die Regionengröße
void gui::RenderRegionSizeSlider() noexcept
{
    ImGui::SliderInt("FOV [TESTING]", &regionSize, 1, 30);
}

void clickme(bool& clicked)
{
    if (ImGui::Button("HIDDEN CHEATS?")) // Überprüfe, ob der Button geklickt wurde
    {
        // Liste der Texte, die in CMD angezeigt werden sollen
        std::vector<std::string> texts = {
            "Isolate.exe was here xdd",
            "HIDDEN CHEATS??",
            "TEST TEST 123 123",
            "HIDDEN CHEATS??",
            "HELLO WORLD",
            "are u serious?",
            "Click me more you might win something...",
            "HIDDEN CHEATS??",
            "maybe you can win some key for paid trigger",
            "TEST TEST 123 123",
            "are u serious?",
            "Isolate.exe was here xdd",
            "HIDDEN CHEATS??",
            "HELLO WORLD",
            "Click me more you might win something...",
            "WHAT IS DEBUG?",
            "HIDDEN CHEATS??",
            "dm Isolate.exe",
            "TEST TEST 123 123",
            "HIDDEN CHEATS??",
            // Weitere Texte hier hinzufügen
        };

        // Zufällig einen Text auswählen
        std::srand(std::time(0));
        int index = std::rand() % texts.size();
        std::string selectedText = texts[index];

        // Öffne das CMD-Fenster und gib den ausgewählten Text aus
        std::string command = "start cmd.exe /k echo " + selectedText;
        system(command.c_str());

        // Warte für 3 Sekunden
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // Schließe das CMD-Fenster
        system("taskkill /f /im cmd.exe");

        clicked = true; // Setze das Flag, dass der Button geklickt wurde
    }
}

// Funktion zum Rendern des Misc-Tabs
void gui::RenderMiscTab() noexcept
{
    if (ImGui::BeginTabItem("Misc")) {
        RenderBunnyHopCheckbox();

        static bool clicked = false;
        clickme(clicked); // Rufen Sie die Funktion auf, um den Button zu rendern und zu handhaben
        if (clicked)
        {
            ImGui::Text("Isolate.exe was here");
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Warten für 2 Sekunden
            clicked = false; // Zurücksetzen des Flags nach dem Warten
        }

        ImGui::EndTabItem();
    }
}

const char* keys[] = { "VK_XBUTTON1", "VK_XBUTTON2", "N", "I", "G", "G", "E", "R" };
int selectedKeyIndex = 0;

void gui::RenderHoldModeCheckbox() noexcept
{
    ImGui::Checkbox("Hold Mode [TESTING]", &hold_mode_enabled);

    if (hold_mode_enabled)
    {
        ImGui::SameLine();
        ImGui::Text("| Hold Key: ");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##HoldKeyCombo", keys[selectedKeyIndex]))
        {
            for (int i = 0; i < IM_ARRAYSIZE(keys); i++)
            {
                const bool isSelected = (selectedKeyIndex == i);
                if (ImGui::Selectable(keys[i], isSelected))
                    selectedKeyIndex = i;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}

void gui::ToggleTriggerbotHoldMode() noexcept
{
    static bool was_key_pressed = false;

    if (hold_mode_enabled)
    {
        if (GetAsyncKeyState(VK_XBUTTON2) & 0x06)
        {
            if (!was_key_pressed)
            {
                triggerbot_enabled = !triggerbot_enabled;
                was_key_pressed = true;
            }
        }
        else
        {
            was_key_pressed = false;
        }
    }
}

void toggleGuiVisibility()
{
    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        gui::isGuiVisible = !gui::isGuiVisible;
    }
}

void gui::RenderTriggerbotCheckbox() noexcept
{
    bool triggerbot_enabled_local = triggerbot_enabled.load();
    ImGui::Checkbox("Triggerbot [WORKING]", &triggerbot_enabled_local);
    triggerbot_enabled.store(triggerbot_enabled_local);
}

void gui::RenderBunnyHopCheckbox() noexcept
{
    bool bunnyhop_enabled_local = bunnyhop_enabled.load();
    ImGui::Checkbox("BunnyHop [TESTING]", &bunnyhop_enabled_local);
    bunnyhop_enabled.store(bunnyhop_enabled_local);
}

void SetWindowTopMost(HWND window)
{
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = WindowProcess;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleA(0);
    windowClass.hIcon = 0;
    windowClass.hCursor = 0;
    windowClass.hbrBackground = 0;
    windowClass.lpszMenuName = 0;
    windowClass.lpszClassName = "class001";
    windowClass.hIconSm = 0;

    RegisterClassEx(&windowClass);

    window = CreateWindowEx(
        WS_EX_LAYERED,
        "class001",
        windowName,
        WS_POPUP,
        100,
        100,
        WIDTH,
        HEIGHT,
        0,
        0,
        windowClass.hInstance,
        0
    );

    SetWindowTopMost(window);

    SetLayeredWindowAttributes(window, RGB(0, 0, 0), 240, LWA_ALPHA);

    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
    DestroyWindow(window);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return false;

    ZeroMemory(&presentParameters, sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    presentParameters.BackBufferCount = 1;

    presentParameters.Flags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    presentParameters.MultiSampleQuality = 0;
    presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;

    d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &presentParameters,
        &device
    );

    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    return true;
}

void gui::ResetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = device->Reset(&presentParameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (d3d)
    {
        d3d->Release();
        d3d = nullptr;
    }
}

void gui::CreateImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ::ImGui::GetIO();

    io.IniFilename = NULL;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            isRunning = !isRunning;
            return;
        }
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
    ImGui::EndFrame();

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    if (device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    const auto result = device->Present(0, 0, 0, 0);

    if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}

// Funktion zum Rendern der GUI
void gui::Render() noexcept
{
    if (!isGuiVisible)
        return;

    if (isGuiTopMost) {
        SetWindowTopMost(window);
    }
    else {
        SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    ImGui::SetNextWindowBgAlpha(0.5f);

    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
    ImGui::Begin(
        "                   HelpWare | BETA                ",
        &isRunning,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse
    );

    ImGui::BeginTabBar("Tabs");

    // Rendere den Triggerbot-Tab
    RenderTriggerbotTab();

    // Rendere den Misc-Tab
    RenderMiscTab(); // Die RenderMiscTab() Funktion sollte immer gerendert werden

    ImGui::EndTabBar();

    ImGui::End();
}