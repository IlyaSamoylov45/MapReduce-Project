#include <iostream>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <sys/stat.h>
#include <errno.h>
#include <algorithm>
#include <vector>

void check_values(std::string, std::string);
void parse_cmdline(int, char**);
void real_int(std::string, std::string);
void check_file(char*, std::string);
int count_words(std::string);
void split_input(std::string, int);
bool remove_char(char);
void map_words_to_array(std::string, int, int);
