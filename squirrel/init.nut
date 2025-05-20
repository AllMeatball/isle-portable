setconsttable(getroottable()) // HACK: this gets constants to map to the root table instead of just poofing after going into runtime
LEGO.IncludeFile("consts/lego")
LEGO.IncludeFile("consts/sdl")

// ShowMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game paused = " + LEGO1.IsPaused().tostring());
// ShowMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game paused = " + LEGO1.GetWorldId("ACT1"));

GLOBALS <- {
    mouse_x = 0,
    mouse_y = 0,
};

function CALLBACK_ProcessOneEvent(event) {
    if (event.GetNotification() == NotificationId.c_notificationKeyPress) {
        if (event.GetKey() == SDLK_M) {
            // ShowMessageBox(
            //     SDL_MESSAGEBOX_INFORMATION,
            //     "Mouse Position X=" + GLOBALS.mouse_x + ", Y=" + GLOBALS.mouse_y
            // )
            // LEGO.GetGameState().SwitchArea(Area.e_police)
            LEGO.InvokeAction(ActionType.e_openram, g_policeScript, 0);
        }
    }

    if (event.GetNotification() == NotificationId.c_notificationMouseMove) {
        GLOBALS.mouse_x = event.GetX()
        GLOBALS.mouse_y = event.GetY()
    }

    if (event.GetNotification() == NotificationId.c_notificationTransitioned) {
        ShowMessageBox(
            SDL_MESSAGEBOX_INFORMATION,
            "Scene Change"
        )
    }
}
