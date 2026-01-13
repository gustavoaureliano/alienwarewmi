#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 
	if (gtk_init_check(0, NULL) == FALSE) {
		printf("gtk_init_check is false");
		return -1;
	}
	AppIndicator* indicator = app_indicator_new_with_path("alienware-cli", "icon", APP_INDICATOR_CATEGORY_APPLICATION_STATUS, "/home/gustavo/alienware/alienwarewmi/appindicator");
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
	GtkMenuShell *menu = (GtkMenuShell*)gtk_menu_new();
	app_indicator_set_menu(indicator, GTK_MENU(menu));
	while (TRUE) {
		gtk_main_iteration_do(1);
		printf("iteration\n");
	}
	return 0; 
}
