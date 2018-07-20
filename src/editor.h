#pragma once

#include <vector>
#include <string>

#include "tokenizer.h"


struct text_line
{
	std::string raw;
};

struct file_tab
{
	std::vector<text_line> lines;
};

void init_editor();
void close_editor();
void process_key(int key, bool down);
void process_char(char ch);
void update_editor(float delta);

void open_tab(std::string path);
void draw(int xx, int yy, int ww, int hh);

std::string* get_line(int y);
std::string* get_current_line();