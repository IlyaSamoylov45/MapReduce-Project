#include "main.h"

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

  std::cout << "\nCurrent arguments are " << argv[1] << " " << app << " "
                                     << argv[3] << " " << impl << " "
                                     << argv[5] << " " << maps << " "
                                     << argv[7] << " " << reduces << " "
                                     << argv[9] << " " << infile << " "
                                     << argv[11] << " " << outfile << "\n" <<std::endl;

  split_input(infile, maps);
}

//splits the input before sending to the map function
void split_input(std::string file, int maps){
  int word_count = count_words(file);
  //if there are more maps than words return an error
  if(maps > word_count){
    std::cout << "More maps than there are words! You gave " << maps << " maps but there are only " << word_count << " words!" << "\n" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << word_count << std::endl;
  arymap_words_to_array(file, maps, word_count);
}

void map_words_to_array(std::string file, int total_maps, int total_words) {
  int words_per_map = total_words/total_maps;
  int remaining_words = total_words%total_maps;

  //std::cout << words_per_map << std::endl;
  //std::cout << remaining_words << std::endl;

  std::string word;
  std::ifstream infile(file.c_str());

  std::string *ary[total_maps];
  int count = words_per_map;
  int current_map = 0;

  int total = 0;

  if(remaining_words > 0){
    count++;
    remaining_words--;
  }
  ary[current_map] = new std::string[count];
  while(infile >> word) {
    word.erase(std::remove_if(word.begin(), word.end(), remove_char), word.end());
    if(word != ""){
      //std::cout <<" test" << std::endl;
      if(count > 0){
        //std::cout <<" here" << std::endl;
        ary[current_map][count - 1] = word;
        //std::cout << ary[current_map][count - 1] << " : " << count << std::endl;
        //std::cout << count << std::endl;
        count--;
      }
      else{
        total++;
        //std::cout <<" else" << std::endl;
        count = words_per_map;
        if(remaining_words > 0){
          count++;
          remaining_words--;
        }
        current_map++;
        ary[current_map] = new std::string[count];
        ary[current_map][count - 1] = word;
        //std::cout << ary[current_map][count - 1] << " : " << count << std::endl;
        count--;
      }
    }
  }
  //return ary;
  //std::cout << total << std::endl;
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
