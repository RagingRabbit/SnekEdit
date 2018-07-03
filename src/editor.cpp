#include "editor.h"
#include "utils.h"
#include "console.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#define TAB_SIZE 4

struct coord
{
	int x, y;
};

static bool changed;

static file_tab *current_tab;
static coord cursor_pos;

std::string load_file(std::string path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

void init_editor()
{
	set_title("SnekEdit");
	cursor_pos = { 0, 0 };
	running = true;
	changed = true;

	open_tab("src/main.cpp");
}

void close_editor()
{
	delete current_tab;
}

void process_key(int key, bool kdown)
{
	bool keyfound = true;

	switch (key)
	{
	case VK_ESCAPE:
		running = false;
		break;
	case VK_RIGHT:
		if (kdown)
		{
			cursor_pos.x = min_int(current_tab->lines[cursor_pos.y].length(), cursor_pos.x);
			if (cursor_pos.x < (int)current_tab->lines[cursor_pos.y].length())
			{
				cursor_pos.x++;
			}
		}
		break;
	case VK_LEFT:
		if (kdown)
		{
			cursor_pos.x = min_int(current_tab->lines[cursor_pos.y].length(), cursor_pos.x);
			if (cursor_pos.x > 0)
			{
				cursor_pos.x--;
			}
		}
		break;
	case VK_UP:
		if (kdown)
		{
			if (cursor_pos.y > 0)
			{
				cursor_pos.y--;
			}
		}
		break;
	case VK_DOWN:
		if (kdown)
		{
			if (cursor_pos.y < (int)current_tab->lines.size() - 1)
			{
				cursor_pos.y++;
			}
		}
		break;
	case VK_END:
		if (kdown)
		{
			cursor_pos.x = get_current_line()->length();
		}
		break;
	default:
		keyfound = false;
		break;
	}

	if (keyfound)
	{
		changed = true;
	}
}

void update_editor(float delta)
{
	if (changed)
	{
		draw(0, 0, get_width(), get_height());
		changed = false;
	}
}

void open_tab(std::string path)
{
	std::string content = load_file(path);
	current_tab = new file_tab();
	current_tab->lines = split_string(content, "\n");

	changed = true;
}

std::string get_renderable_str(std::string str)
{
	replace_all(str, std::string("\t"), std::string(TAB_SIZE, ' '));

	return str;
}

void draw(int xx, int yy, int ww, int hh)
{
	for (size_t y = 0; y < current_tab->lines.size(); y++)
	{
		if (y >= 0 && (int)y < hh)
		{
			std::string str = get_renderable_str(current_tab->lines[y].substr(0, ww));
			print_s(0, y, str.c_str(), 0xCCCCCC, 0x3420F2);
			put_char(' ');

			if (y == cursor_pos.y)
			{
				int capped_x = min_int(str.length(), cursor_pos.x);

				int x = 0;
				for (; x < capped_x;)
				{
					x += current_tab->lines[y][x] == '\t' ? TAB_SIZE : 1;
				}
				char cursor_char = x < (int)str.length() ? str[x] : ' ';

				put_char(x, y, cursor_char, 0, 0xFFFFFF);
			}
		}
	}
}

std::string* get_current_line()
{
	return &current_tab->lines[cursor_pos.y];
}