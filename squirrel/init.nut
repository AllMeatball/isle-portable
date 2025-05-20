const SDL_MESSAGEBOX_ERROR                    = 0x00000010; /**< error dialog */
const SDL_MESSAGEBOX_WARNING                  = 0x00000020; /**< warning dialog */
const SDL_MESSAGEBOX_INFORMATION              = 0x00000040; /**< informational dialog */
const SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT    = 0x00000080; /**< buttons placed left to right */
const SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT    = 0x00000100;

ShowMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game paused = " + LEGO1.IsPaused().tostring());

