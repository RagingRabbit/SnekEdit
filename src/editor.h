#pragma once

#include <vector>
#include <string>


struct file_tab
{
	std::vector<std::string> lines;
};

void init_editor();
void close_editor();
void process_key(int key, bool down);
void update_editor(float delta);

void open_tab(std::string path);
void draw(int xx, int yy, int ww, int hh);

std::string* get_current_line();