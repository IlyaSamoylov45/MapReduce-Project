#include "main.h"

int main(int argc, char* argv[]){

  if(argc != 13){
    cout << "\n" << "Usage is --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << "\n" <<endl;
    exit(EXIT_FAILURE);
  }

  parse_cmdline(argc - 1, argv + 1);
  string app = argv[2];
  string impl = argv[4];
  int maps = stoi(argv[6]);
  int reduces = stoi(argv[8]);
  string infile = argv[10];
  string outfile = argv[12];

  cout << "\nCurrent arguments are " << argv[1] << " " << app << " "
                                      << argv[3] << " " << impl << " "
                                      << argv[5] << " " << maps << " "
                                      << argv[7] << " " << reduces << " "
                                      << argv[9] << " " << infile << " "
                                      << argv[11] << " " << outfile << "\n" <<endl;

}

//checks to see if two values are the same for "--app" type values
void check_values(string value, string correct){
  if(value.compare(correct) != 0){
    cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << endl;
    cout << "Error was in the argument " << correct << endl;
    cout << "You wrote: " << value << "\n" << endl;
    exit(EXIT_FAILURE);
  }
}

//checks to see if char array is a valid number that is not less than zero
void real_int(string num, string correct){
  try {
    int test = stoi(num);
    if(test <= 0){
      cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << endl;
      cout << "Error was in the argument " << correct << endl;
      cout << "Not a nonnegative digit greater than 0! You wrote: " << test << "\n" << endl;
      exit(EXIT_FAILURE);
    }
  }
  catch(exception const & e){
    cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << endl;
    cout << "Error was in the argument " << correct << endl;
    cout << "Error was: " << e.what() << endl;
    cout << "Not a nonnegative digit greater than 0! You wrote: " << num << "\n" << endl;
    exit(EXIT_FAILURE);
  }
}

//pass arg pointer and set all values
void parse_cmdline(int argc, char *arr[]) {
  //check the argument values with -- in front
  check_values(arr[0], "--app");
  if(((string) arr[1]).compare("wordcount") != 0 && ((string) arr[1]).compare("sort") != 0){
    cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << endl;
    cout << "Error was in the argument " << "[wordcount, sort]" << endl;
    cout << "You wrote: " << arr[1] << "\n" << endl;
    exit(EXIT_FAILURE);
  }
  check_values(arr[2], "--impl");

  if(((string)arr[3]).compare("procs") != 0 && ((string)arr[3]).compare("threads") != 0){
    cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << endl;
    cout << "Error was in the argument " << "[procs, threads]" << endl;
    cout << "You wrote: " << arr[3] << "\n" << endl;
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
void check_file(char* filename, string correct){
  struct stat test;
  if (stat(filename, &test) != 0) {
    cout << "\n" << "Usage is: --app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << endl;
    cout << "Error was in the argument " << correct << endl;
    cout << "Error was:" << strerror(errno) << endl;
    cout << "You wrote: " << filename << "\n" << endl;
    exit(EXIT_FAILURE);
  }
}
