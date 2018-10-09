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
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
const int SHAREDMEMSIZE = 10000;
using namespace boost::interprocess;

struct ProcData {
	int id;
	std::string line_array;
	std::map<std::string, int> counter;
};


struct ThreadData {
    int id;
    std::string line_array;
    std::map<std::string, int> counter;
};

void check_values(std::string, std::string);
void parse_cmdline(int, char**);
void real_int(std::string, std::string);
void check_file(char*, std::string);
int count_words(std::string);
std::vector<std::string> split_input(std::string, int);
bool remove_char(char);
std::vector<std::string> map_words_to_array(std::string, int, int);
void map_threads(std::vector<std::string>, int, ThreadData[]);
void *map_function_thread(void*);
std::vector<std::string> split_string_by_space(std::string);
std::map<std::string, int> reduce_threads(int, int, ThreadData[]);
std::vector<std::vector<int>> assignJobs(int, int);
void *reduce_function_thread(void*);
void sortPrint(std::map<std::string, int>, std::string);
void wordCountPrint(std::map<std::string, int>, std::string);
void *map_function_proc(void*);
void map_proc(std::vector<std::string>, int, ProcData[]);

std::map<std::string, int> reduce_proc(int, int, ProcData[]);
void *reduce_function_proc(void*);
bool is_number(const std::string&);
