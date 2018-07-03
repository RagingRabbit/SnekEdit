#include "console.h"
#include "editor.h"

int main()
{
	init_scr();
	printf(USE_ALTERNATE_BUFFER CLEAR_SCREEN HIDE_CURSOR);

	run_loop("SnekEdit", init_editor, update_editor, close_editor, process_key);

	_getch();

	printf(USE_MAIN_BUFFER);
	end_scr();
}