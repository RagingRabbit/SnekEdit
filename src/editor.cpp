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

struct token_format
{
	int f, b;
	std::string str;
};

static bool changed;

static file_tab *current_tab;
static coord cursor_pos;

Tokenizer tokenizer(false);

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

	tokenizer.addRule(new KeywordRule("\t", "tab", false));

	tokenizer.addRule(new BeginRule("#", "preprocessor", true));
	tokenizer.addRule(new BeginEndRule("\"", "\"", "string-literal"));

	tokenizer.addRule(new BeginRule("//", "single-line-comment", false));

	tokenizer.addRule(new KeywordRule("int", "builtin-type", true));
	tokenizer.addRule(new KeywordRule("void", "builtin-type", true));

	tokenizer.addRule(new KeywordRule("int*", "builtin-type", true));
	tokenizer.addRule(new KeywordRule("void*", "builtin-type", true));

	tokenizer.addRule(new KeywordRule("(", "symbol", false));
	tokenizer.addRule(new KeywordRule(")", "symbol", false));
	tokenizer.addRule(new KeywordRule("[", "symbol", false));
	tokenizer.addRule(new KeywordRule("]", "symbol", false));
	tokenizer.addRule(new KeywordRule("{", "symbol", false));
	tokenizer.addRule(new KeywordRule("}", "symbol", false));

	open_tab("test.txt");
}

void close_editor()
{
	delete current_tab;
}

void move_cursor(int dx, int dy)
{
	if (dy > 0)
	{
		dy = min(dy, (int)current_tab->lines.size() - 1 - cursor_pos.y);
		cursor_pos.y += dy;
	}
	else if (dy < 0)
	{
		dy = max(dy, -cursor_pos.y);
		cursor_pos.y += dy;
	}
	if (dx > 0)
	{
		cursor_pos.x = min((int)current_tab->lines[cursor_pos.y].raw.length(), cursor_pos.x);
		dx = min(dx, (int)current_tab->lines[cursor_pos.y].raw.length() - cursor_pos.x);
		cursor_pos.x += dx;
	}
	else if (dx < 0)
	{
		cursor_pos.x = min((int)current_tab->lines[cursor_pos.y].raw.length(), cursor_pos.x);
		dx = max(dx, -cursor_pos.x);
		cursor_pos.x += dx;
	}
}

void insert_char(int x, int y, char c)
{
	if (x < 0 || y < 0 || y >= current_tab->lines.size() || x > current_tab->lines[y].raw.length())
	{
		return;
	}
	get_line(y)->insert(get_line(y)->begin() + x, c);
}

void delete_char(int x, int y)
{
	if (x < 0 || y < 0 || y >= current_tab->lines.size() || x >= current_tab->lines[y].raw.length())
	{
		return;
	}
	std::string str0 = current_tab->lines[y].raw.substr(0, x);
	std::string str1 = current_tab->lines[y].raw.substr(x + 1);
	current_tab->lines[y].raw = str0 + str1;
}

void insert_line(int y, std::string content)
{
	current_tab->lines.insert(current_tab->lines.begin() + y, { content });
}

void delete_line(int y)
{
	current_tab->lines.erase(current_tab->lines.begin() + y);
}

std::string get_indentation(int y)
{
	for (int i = 0; i <= current_tab->lines[y].raw.length(); i++)
	{
		if (i == current_tab->lines[y].raw.length() || (current_tab->lines[y].raw[i] != ' ' && current_tab->lines[y].raw[i] != '\t'))
		{
			return current_tab->lines[y].raw.substr(0, i);
		}
	}
	return std::string();
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
			move_cursor(1, 0);
		}
		break;
	case VK_LEFT:
		if (kdown)
		{
			move_cursor(-1, 0);
		}
		break;
	case VK_UP:
		if (kdown)
		{
			move_cursor(0, -1);
		}
		break;
	case VK_DOWN:
		if (kdown)
		{
			move_cursor(0, 1);
		}
		break;
	case VK_END:
		if (kdown)
		{
			if (get_key_state(VK_CONTROL))
			{
				move_cursor(0, (int)current_tab->lines.size() - 1 - cursor_pos.y);
			}
			move_cursor((int)get_current_line()->length() - cursor_pos.x, 0);
		}
		break;
	case VK_HOME:
		if (kdown)
		{
			if (get_key_state(VK_CONTROL))
			{
				move_cursor(0, -cursor_pos.y);
			}
			move_cursor(-cursor_pos.x, 0);
		}
		break;
	case VK_BACK:
		if (kdown)
		{
			if (cursor_pos.x > 0)
			{
				move_cursor(-1, 0);
				delete_char(cursor_pos.x, cursor_pos.y);
			}
		}
		break;
	case VK_DELETE:
		if (kdown)
		{
			delete_char(cursor_pos.x, cursor_pos.y);
		}
		break;
	case VK_RETURN:
		if (kdown)
		{
			std::string indentation = get_indentation(cursor_pos.y);
			insert_line(cursor_pos.y + 1, indentation);
			move_cursor(0, 1);
		}
		break;
	case 0x44:
		if (kdown && get_key_state(VK_CONTROL))
		{
			delete_line(cursor_pos.y);
			move_cursor(0, -1);
			move_cursor((int)get_current_line()->length() - cursor_pos.x, 0);
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

void process_char(char ch)
{
	if (ch >= 32 && ch != 127)
	{
		cursor_pos.x = min((int)current_tab->lines[cursor_pos.y].raw.length(), cursor_pos.x);
		insert_char(cursor_pos.x, cursor_pos.y, ch);
		move_cursor(1, 0);
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

token_format get_format_from_token(Token token)
{
	token_format format;
	format.str = token.str;
	format.f = 0xD7D7D7;
	format.b = 0x151515;

	if (token.type == "tab")
	{
		format.f = 0x666666;
		format.b = 0x1C1C1C;
		format.str[0] = (char)175;
	}
	else if (token.type == "preprocessor")
	{
		format.f = 0x8FBFDC;
	}
	else if (token.type == "single-line-comment")
	{
		format.f = 0x808080;
	}
	else if (token.type == "string-literal")
	{
		format.f = 0x87AF5F;
	}
	else if (token.type == "builtin-type")
	{
		format.f = 0x5678A6;
	}
	else if (token.type == "symbol")
	{
		format.f = 0x888888;
	}

	return format;
}

void draw_cursor(int x, int y, int ypos, int ww)
{
	std::string str = get_renderable_str(current_tab->lines[y].raw);

	int capped_x = min((int)get_line(y)->length(), x);

	int xpos = 0;
	for (int i = 0; i < capped_x; i++)
	{
		xpos += current_tab->lines[y].raw[i] == '\t' ? TAB_SIZE : 1;
	}
	char cursor_char = xpos < (int)str.length() ? str[xpos] : ' ';

	put_char(xpos % ww, ypos + xpos / ww, cursor_char, 0, 0xFFFFFF);
}

void draw(int xx, int yy, int ww, int hh)
{
	int y = 0;

	for (int i = 0; y < hh; i++)
	{
		if (i < current_tab->lines.size())
		{
			std::vector<Token> tokens = tokenizer.tokenize(current_tab->lines[i].raw);
			set_cursor_pos(xx + 0, yy + y);

			int numchars = 0;
			for (int j = 0; j < tokens.size(); j++)
			{
				tokens[j].str = get_renderable_str(tokens[j].str);
				token_format format = get_format_from_token(tokens[j]);

				set_color(format.f, format.b);
				print_s(format.str.c_str());

				numchars += format.str.length();
			}

			int numlines = (numchars - 1) / ww + 1;

			set_color(0, 0x151515);
			print_s(std::string(ww - (numchars - numlines * ww + ww), ' ').c_str());

			if (i == cursor_pos.y)
			{
				draw_cursor(cursor_pos.x, cursor_pos.y, y, ww);
			}

			y += numlines;
		}
		else
		{
			set_color(0, 0x151515);
			print_s(std::string(ww, ' ').c_str());
			y++;
		}
	}
}

std::string* get_line(int y)
{
	return &current_tab->lines[y].raw;
}

std::string* get_current_line()
{
	return &current_tab->lines[cursor_pos.y].raw;
}