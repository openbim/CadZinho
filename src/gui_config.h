#ifndef _CZ_GUI_CONFIG_LIB
#define _CZ_GUI_CONFIG_LIB

#include "gui.h"
#include "gui_file.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int gui_load_conf (const char *fname, gui_obj *gui);

int gui_save_init (char *fname, gui_obj *gui);

int gui_load_ini(const char *fname, gui_obj *gui) ;


#endif