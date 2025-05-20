setconsttable(getroottable()) // HACK: this gets constants to map to the root table instead of just poofing after going into runtime
LEGO.IncludeFile("consts/lego")
LEGO.IncludeFile("consts/sdl")

// ShowMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game paused = " + LEGO1.IsPaused().tostring());
// ShowMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game paused = " + LEGO1.GetWorldId("ACT1"));

function CALLBACK_ProcessOneEvent(event) {
    if (LEGO.IsPaused()) {
        return;
    }

    if (event.GetNotification() == NotificationId.c_notificationKeyPress) {
        if (event.GetKey() == SDLK_0) {
            print("0 pressed")
        }
    }
}
