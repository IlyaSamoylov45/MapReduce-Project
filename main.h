#include <iostream>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <sys/stat.h>
#include <errno.h>

using namespace std;

void check_values(string, string);
void parse_cmdline(int, char**);
void real_int(string, string);
void check_file(char*, string);
