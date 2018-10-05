#include <iostream>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <sys/stat.h>
#include <errno.h>
#include <algorithm>
#include <vector>
#include <pthread.h>
#include <map>
#include <sstream>



void check_values(std::string, std::string);
void parse_cmdline(int, char**);
void real_int(std::string, std::string);
void check_file(char*, std::string);
int count_words(std::string);
std::vector<std::string> split_input(std::string, int);
bool remove_char(char);
std::vector<std::string> map_words_to_array(std::string, int, int);
void map_threads(std::vector<std::string>, int);
void *map_function_thread(void*);
std::vector<std::string> split_string_by_space(std::string);
