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
  //std::cout << word_count;
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
    std::cout << "word is: " << word << " " << count << "\n";
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
