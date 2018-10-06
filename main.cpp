#include "main.h"

//This will hold threaddata

//Information from the mapReduce
//the word is a key and the amount of times they show up is the occurances

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
int main(int argc, char* argv[]){

  if(argc != 13){
    std::cout << "\n" << "Usage is --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << "\n" <<std::endl;
    exit(EXIT_FAILURE);
  }
  
  parse_cmdline(argc - 1, argv + 1);
  std::string app = argv[2];
  std::string impl = argv[4];
  int maps = std::stoi(argv[6]);
  int reduces = std::stoi(argv[8]);
  std::string infile = argv[10];
  std::string outfile = argv[12];
  ThreadData thread_data[maps];

  std::cout << "\nCurrent arguments are " << argv[1] << " " << app << " "
                                     << argv[3] << " " << impl << " "
                                     << argv[5] << " " << maps << " "
                                     << argv[7] << " " << reduces << " "
                                     << argv[9] << " " << infile << " "
                                     << argv[11] << " " << outfile << "\n" <<std::endl;

  std::vector<std::string> sorted_ary = split_input(infile, maps);
  if(impl.compare("--threads")){
    map_threads(sorted_ary, maps, thread_data);
    //reduce_threds(reduces, thread_data);
  }
  else{
    //map_proc();
  }
  //print for error checking
  //for (std::vector<std::string>::const_iterator i = sorted_ary.begin(); i != sorted_ary.end(); i++){
  //  std::cout << *i << '\n';
  //}
  //
}


//map using threads
void map_threads(std::vector<std::string> array, int maps, ThreadData thread_data[]){
  pthread_t threads[maps];
  int iret;
  for(int i = 0; i < maps; i++){
    //std::cout << "Creating thread: " << std::endl;
    thread_data[i].id = i;
    thread_data[i].line_array= array[i];
    std::map<std::string, int> tempMap;
    
    iret = pthread_create(&threads[i], NULL, map_function_thread, (void*) &thread_data[i]);
    if (iret != 0) {
			std::cout << "Error: Creating thread: " << iret << std::endl;
			exit(EXIT_FAILURE);
		}
  }
  for (int i = 0; i < maps; i++) {
		iret = pthread_join(threads[i], NULL);
		if (iret) {
			std::cout << "Error: Joining thread: " << iret << std::endl;
			exit(EXIT_FAILURE);
		}
		/*
		std::map<std::string, int> testMap = thread_data[i].counter;
		for(auto it = testMap.begin(); it != testMap.end(); it++){
		  std::cout << it->first << " " << it->second << std::endl;
		} */
	}
	
}

/*std::map<std::string, int> reduce_threds(int reduces, ThreadData thread_data[]){
  std::map<std::string, int> resultMerged;
  pthread_t threads[reduces];
  
} */
/*
 * maps words to frequencies and stores it into thread_data
 */
void *map_function_thread(void* thread) {
	struct ThreadData *curr_thread_data;
	curr_thread_data = (struct ThreadData *) thread;
	
	std::vector<std::string> words = split_string_by_space(curr_thread_data->line_array);

	std::map<std::string, int> counterMap; 

	for(int i = 0; i < words.size(); i++){
	  if(counterMap.count(words[i])){
	    counterMap[words[i]] = counterMap[words[i]] + 1;
	  } else {
	    counterMap[words[i]] = 1;
	  }
	}
	
	//test output
	curr_thread_data->counter = counterMap;
	std::map<std::string, int> testMap = curr_thread_data->counter;
	for(auto it = testMap.begin(); it != testMap.end(); it++){
	  std::cout << it->first << " " << it->second << std::endl;
	}
}

//splits the input before sending to the map function
std::vector<std::string> split_input(std::string file, int maps){
  int word_count = count_words(file);
  //if there are more maps than words return an error
  if(maps > word_count){
    std::cout << "More maps than there are words! You gave " << maps << " maps but there are only " << word_count << " words!" << "\n" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Total words: " << word_count << std::endl;
  std::vector<std::string> ary = map_words_to_array(file, maps, word_count);
  //print for error checking
  //for (std::vector<std::string>::const_iterator i = ary.begin(); i != ary.end(); i++){
  //  std::cout << *i << '\n';
  //}
  //
  return ary;
}

//returns a vector with words organized for each map_num if there are more numbers than can be divided then the modulo will check and set then to different threads.
std::vector<std::string> map_words_to_array(std::string file, int total_maps, int total_words) {
  int words_per_map = total_words/total_maps;
  int remaining_words = total_words%total_maps;


  std::string word;
  std::ifstream infile(file.c_str());

  int count = words_per_map;
  int current_map = 0;
  std::vector<std::string> ary(total_maps);

  if(remaining_words > 0){
    count++;
    remaining_words--;
  }
  while(infile >> word) {
    word.erase(std::remove_if(word.begin(), word.end(), remove_char), word.end());
    if(word != ""){
      if(count > 0){
        ary[current_map] = ary[current_map] + " " + word;
        //std::cout << ary[current_map] << std::endl;
        count--;
      }
      else{
        //std::cout <<" else" << std::endl;
        count = words_per_map;
        if(remaining_words > 0){
          count++;
          remaining_words--;
        }
        current_map++;
        ary[current_map] = ary[current_map] + " " +  word;
        //std::cout << ary[current_map] << std::endl;
        count--;
      }
    }
  }
  return ary;
}
//counts the number of words in a file
int count_words(std::string file){
  int count = 0;
  std::string word;
  std::ifstream infile(file.c_str());
  while(infile >> word) {
    word.erase(std::remove_if(word.begin(), word.end(), remove_char), word.end());
    if(word != ""){
      count++;
    }
    //std::cout << "word is: " << word << " " << count << "\n";
  }
  return count;
}

//removes a character based on delimitors that were given in the announcement. Need to check if negative nums are allowed for int sort!
bool remove_char(char test_character){
  if(test_character == ' ' || test_character == '.' || test_character == ',' || test_character == ';' || test_character == ':'|| test_character == '!' || test_character == '-'){
    return true;
  }
  else{
    return false;
  }
}

//checks to see if two values are the same for "--app" type values
void check_values(std::string value, std::string correct){
  if(value.compare(correct) != 0){
    std::cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << std::endl;
    std::cout << "Error was in the argument " << correct << std::endl;
    std::cout << "You wrote: " << value << "\n" << std::endl;
    exit(EXIT_FAILURE);
  }
}

//checks to see if char array is a valid number that is not less than zero
void real_int(std::string num, std::string correct){
  try {
    int test = std::stoi(num);
    if(test <= 0){
      std::cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << std::endl;
      std::cout << "Error was in the argument " << correct << std::endl;
      std::cout << "Not a nonnegative digit greater than 0! You wrote: " << test << "\n" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  catch(std::exception const & e){
    std::cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << std::endl;
    std::cout << "Error was in the argument " << correct << std::endl;
    std::cout << "Error was: " << e.what() << std::endl;
    std::cout << "Not a nonnegative digit greater than 0! You wrote: " << num << "\n" << std::endl;
    exit(EXIT_FAILURE);
  }
}

//pass arg pointer and set all values
void parse_cmdline(int argc, char *arr[]) {
  //check the argument values with -- in front
  check_values(arr[0], "--app");
  if(((std::string) arr[1]).compare("wordcount") != 0 && ((std::string) arr[1]).compare("sort") != 0){
    std::cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << std::endl;
    std::cout << "Error was in the argument " << "[wordcount, sort]" << std::endl;
    std::cout << "You wrote: " << arr[1] << "\n" << std::endl;
    exit(EXIT_FAILURE);
  }
  check_values(arr[2], "--impl");

  if(((std::string)arr[3]).compare("procs") != 0 && ((std::string)arr[3]).compare("threads") != 0){
    std::cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << std::endl;
    std::cout << "Error was in the argument " << "[procs, threads]" << std::endl;
    std::cout << "You wrote: " << arr[3] << "\n" << std::endl;
    exit(EXIT_FAILURE);
  }

  check_values(arr[4], "--maps");
  real_int(arr[5], "num_maps");
  check_values(arr[6], "--reduces");
  real_int(arr[7], "num_reduces");
  check_values(arr[8], "--input");
  check_file(arr[9], "infile");
  check_values(arr[10], "--output");
}

//Check if a file exists
void check_file(char* filename, std::string correct){
  struct stat test;
  if (stat(filename, &test) != 0) {
    std::cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << std::endl;
    std::cout << "Error was in the argument " << correct << std::endl;
    std::cout << "Error was:" << strerror(errno) << std::endl;
    std::cout << "You wrote: " << filename << "\n" << std::endl;
    exit(EXIT_FAILURE);
  }
}

std::vector<std::string> split_string_by_space(std::string input){
  std::string buf;
  std::stringstream ss(input);
  
  std::vector<std::string> words;
  while(ss >> buf)
    words.push_back(buf);
  
  return words;
}
