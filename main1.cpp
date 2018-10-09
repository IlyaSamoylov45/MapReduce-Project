#include "main.h"

//This will hold threaddata

//Information from the mapReduce
//the word is a key and the amount of times they show up is the occurances

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

struct ReduceThreadArgs {
    std::vector<ThreadData> thread_data;
    std::map<std::string, int> *resultMerged;
};

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
  ProcData proc_data[maps];

  std::cout << "\nCurrent arguments are " << argv[1] << " " << app << " "
                                     << argv[3] << " " << impl << " "
                                     << argv[5] << " " << maps << " "
                                     << argv[7] << " " << reduces << " "
                                     << argv[9] << " " << infile << " "
                                     << argv[11] << " " << outfile << "\n" <<std::endl;

  std::vector<std::string> sorted_ary = split_input(infile, maps);
  if(impl.compare("threads") == 0){
    map_threads(sorted_ary, maps, thread_data);
    std::cout << "Finished mapping Threads" << std::endl;
    //std::cout << thread_data[0].line_array << std::endl;
    std::map<std::string, int> result = reduce_threads(reduces, maps, thread_data);
    if(app.compare("wordcount") == 0){
      wordCountPrint(result, outfile);
    } else { // sort
      sortPrint(result, outfile);
    }
  }
  if(impl.compare("procs") == 0){

    
    map_proc(sorted_ary, maps, proc_data);
		//std::string result = proc_data[0].line_array;
		//if(app.compare("wordcount") == 0)
		//{
		//	wordCountPrint(result, outfile);
		//} else {
		//	sortPrint(result, outfile);	
		//}
	  
    std::cout << "Finished mapping Processes" << std::endl;
    //std::cout << proc_data[0].line_array << std::endl;
    //std::map<std::string, int> result = reduce_threads(reduces, maps, thread_data);
    //std::cout << "Process" << std::endl;
    //if(app.compare("wordcount") == 0){
    //  wordCountPrint(result, outfile);
    //} else { // sort
    //  sortPrint(result, outfile);
    //}
	  
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

std::map<std::string, int> reduce_threads(int reduces, int maps, ThreadData thread_data[]){
  if(maps < reduces) reduces = maps;
  std::map<std::string, int> resultMerged;
  // Each reduce thread will get map_thread ids in order to know which
  std::vector<std::vector<int>> mapJobs = assignJobs(reduces, maps);
  pthread_t threads[reduces];

  int iret;
  ReduceThreadArgs reduceThreadArgs[reduces];
  for(int i = 0; i < reduces; i++){

    std::vector<ThreadData> curData;
    for(int j = 0 ; j < mapJobs[i].size(); j++){
	curData.push_back(thread_data[mapJobs[i][j]]);
    }
    reduceThreadArgs[i].thread_data = curData;
    reduceThreadArgs[i].resultMerged = &resultMerged;
    iret = pthread_create(&threads[i], NULL, reduce_function_thread, (void*) &reduceThreadArgs[i]);
    if (iret != 0) {
			std::cout << "Error: Creating thread: " << iret << std::endl;
			exit(EXIT_FAILURE);
    }
  }
    for (int i = 0; i < reduces; i++) {
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
      for(auto it = resultMerged.begin(); it != resultMerged.end(); it++){
		  //std::cout << it->first << " " << it->second << std::endl;
      }
     return resultMerged;

}
std::vector<std::vector<int>> assignJobs(int reduces, int maps){
  std::vector<std::vector<int>> result;
  int amount = maps / reduces;
  int leftOver = maps % reduces;
  int counter = 0;

  for(int i = 0; i < reduces; i++){
    std::vector<int> currentReduce;
    for(int j = 0; j < amount; j++){
      currentReduce.push_back(counter);
      counter++;
    }
    result.push_back(currentReduce);
  }
  for(int i = 0; i < leftOver; i++){
    result[i].push_back(counter);
    counter++;
  }
  return result;
}
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
	  //std::cout << it->first << " " << it->second << std::endl;
	}
}

void *reduce_function_thread(void* thread) {
  struct ReduceThreadArgs *curr_args;
  curr_args = (struct ReduceThreadArgs *) thread;

  std::map<std::string, int> mergeMap;
  std::vector<ThreadData> curData = curr_args->thread_data;

  for(int i = 0; i < curData.size(); i++){
    std::map<std::string, int> curMap = curData[i].counter;

    for(auto it = curMap.begin(); it != curMap.end(); it++){
      if(mergeMap.count(it->first)){
        mergeMap[it->first] = mergeMap[it->first] + it->second;
      } else {
        mergeMap[it->first] = it->second;
      }

    }
  }

  pthread_mutex_lock(&lock1);
  for(auto it = mergeMap.begin(); it != mergeMap.end(); it++){
      if(curr_args->resultMerged->count(it->first)){
        curr_args->resultMerged->operator[](it->first) =  curr_args->resultMerged->operator[](it->first) + it->second;
      } else {
         curr_args->resultMerged->operator[](it->first) = it->second;
      }
  }
  pthread_mutex_unlock(&lock1);

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
    std::replace( word.begin(), word.end(), '-', ' ');
    std::replace( word.begin(), word.end(), ';', ' ');
    std::replace( word.begin(), word.end(), '.', ' ');
    std::replace( word.begin(), word.end(), ':', ' ');
    std::replace( word.begin(), word.end(), '!', ' ');
    std::replace( word.begin(), word.end(), ')', ' ');
    std::replace( word.begin(), word.end(), ',', ' ');
    //std::replace( word.begin(), word.end(), ';', ' ');
    //word.erase(std::remove_if(word.begin(), word.end(), remove_char), word.end());
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    if(word != ""){
      if(count > 0){
        ary[current_map] = ary[current_map] + " " + word;
        //std::cout << ary[current_map] << std::endl;
        count--;
      }
      else{
        //std::cout <<" else" << std::endl;
        //std::cout << ary[current_map] << std::endl;
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
  //std::cout << ary[current_map] << std::endl;
  return ary;
}
//counts the number of words in a file
int count_words(std::string file){
  int count = 0;
  std::string word;
  std::ifstream infile(file.c_str());
  while(infile >> word) {
    std::replace( word.begin(), word.end(), '-', ' ');
    std::replace( word.begin(), word.end(), ';', ' ');
    std::replace( word.begin(), word.end(), '.', ' ');
    std::replace( word.begin(), word.end(), ':', ' ');
    std::replace( word.begin(), word.end(), '!', ' ');
    std::replace( word.begin(), word.end(), ')', ' ');
    std::replace( word.begin(), word.end(), ',', ' ');
    //std::replace( word.begin(), word.end(), ';', ' ');
    //word.erase(std::remove_if(word.begin(), word.end(), remove_char), word.end());
    if(word != ""){
      count++;
    }

    //std::cout << "word is: " << word << " " << count << "\n";
  }
  return count;
}

void map_proc(std::vector<std::string> array, int maps, ProcData proc_data[]){
	std::string sIdentifier;
	pid_t pids[maps];
	pid_t pid;

  //Remove if parent
  shared_memory_object::remove("shared_memory");
	// create a new SHM object and allocate space
	//managed_shared_memory managed_shm(open_or_create, "my_shm", 1024);

  //shared_memory_object shm_obj(create_only, "shared_memory", read_write);
  //shm_obj.truncate(SHAREDMEMSIZE);

  managed_shared_memory segment(create_only,"shared_memory", SHAREDMEMSIZE);
	int status;
  int shm_size = SHAREDMEMSIZE/maps; //size of shared memory each process arguments
  //TODO maybe? handle remainder from division
	for(int i = 0; i < maps; i++){
		if ((pids[i] = fork()) < 0){
			std::cerr << "Error forking" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (pids[i] == 0){
			sIdentifier = "Child Process:";
			
    

      //map part of shared memory for each process
      //mapped_region region(shm_obj, read_write, shm_size/i, shm_size - shm_size/i);
			//mapped_region region(shm_obj, read_write, shm_size*i, shm_size);

			proc_data[i].id = i;
			proc_data[i].line_array = array[i];
			std::map<std::string, int> Map;
			map_function_proc((void*) &proc_data[i]);
			
			exit(0); //exit cleanly
		}
		else if (pids[i] > 0){
			sIdentifier = "Parent Process:";
			pid = wait(&status);
			red_proc();
      shared_memory_object::remove("shared_memory");

			printf("Child process %ld exited with status 0x%x. \n", long(pid), status);
		}
	}


}

void *red_proc(){
    managed_shared_memory segment(open_or_create, "shared_memory", SHAREDMEMSIZE);
    typedef std::string KeyType;
    typedef int MappedType;
    typedef std::pair<const std::string, int> ValueType;
    typedef allocator<ValueType, managed_shared_memory::segment_manager> ShmemAllocator;
    typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MySHMMap;
    ShmemAllocator alloc_inst (segment.get_segment_manager());
    MySHMMap :: iterator iter;
    offset_ptr<MySHMMap> m_pmap = segment.find<MySHMMap>("MySHMMapName").first;
    iter=m_pmap->begin();
    for(; iter!=m_pmap->end();iter++)
    {
	std::cout<< iter->second << " this is a test gap" << iter->first << std::endl;
    }
 
}

void * map_function_proc(void* proc) {
  struct ProcData *curr_proc_data;
	curr_proc_data = (struct ProcData *) proc;
	managed_shared_memory segment(open_or_create, "shared_memory", SHAREDMEMSIZE);
			using namespace boost::interprocess;
			typedef std::string KeyType;
			typedef int MappedType;
			typedef std::pair<const std::string, int> ValueType;
			typedef allocator<ValueType, managed_shared_memory::segment_manager> ShmemAllocator;
			typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MySHMMap;
			ShmemAllocator alloc_inst (segment.get_segment_manager());
			MySHMMap *mymap = segment.construct<MySHMMap>("MySHMMapName") (std::less<std::string>(), alloc_inst);
			//offset_ptr<MySHMMap> m_pmap = segment.construct<MySHMMap>("MySHMMapName")(std::less<std::string>(), alloc_inst);
			
  /**

  /Need semaphore?
	//write proc_data to shared memory so parent process can read it
  std::memset(region, curr_proc_data, size);

  **/
	std::vector<std::string> words = split_string_by_space(curr_proc_data->line_array);
	std::map<std::string, int> counterMap;
  //std::cout <<  curr_proc_data->line_array << std::endl;
	for(int i = 0; i < words.size(); i++){
		if(counterMap.count(words[i])){
			counterMap[words[i]] = counterMap[words[i]] + 1;
		}
    else{
			counterMap[words[i]] = 1;
		}
	}
	curr_proc_data->counter = counterMap;
  std::map<std::string, int> testMap = curr_proc_data->counter;
  
  for(auto it = testMap.begin(); it != testMap.end(); it++){
	  std::cout << it->first << " " << it->second << std::endl;
	  mymap->insert(std::pair<const std::string, int>(it->first, it->second));
		
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


/* TODO
void reduce_proc(int reduces, int maps)
{
	if(maps < reduces) reduces = maps;
	std::map<std::string, int> resultsMerged;
	std::vector<std::vector<int>> mapJobs = assignJobs(reduces, maps);


	//open all of the shared memory with read write priveledge
	shared_memory_object shm_obj (open_only, "shared_memory", read_write);

	//Map all of the shared memory, with read only priv
	mapped_region region (shm_obj, read only);

	//addr is a pointer to the beginning of the shared memory
	void* addr = region.get_address();

	for(int i=0; i<reduces; i++)
	{
		std::vector<ProcData> curData;
		for(int j= 0; j< mapJobs[i].size();j++;)
		{
			curData.push_back([mapJobs[i][j]);
		}
		reduceProcArgs[i].proc_data = curData;
		reduceProcArgs[i].resultsMerged =

	}
}
*/




//pass arg pointer and set all values
void parse_cmdline(int argc, char *arr[]) {
  //check the argument values with -- in front
  check_values(arr[0], "--app");
  if(((std::string) arr[1]).compare("wordcount") != 0 && ((std::string) arr[1]).compare("sort") != 0){
    std::cout << "\n" << "Usage is:--app [wordcount, sort] --impl [procs, threads] --maps num_maps --reduces num_reduces --input infile --output outfile" << std::endl;
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

void wordCountPrint(std::map<std::string, int> result, std::string path) {
  std::ofstream outputFile;
  outputFile.open (path);
  int wordCountFinal = 0;
  //first print numbers
  for(auto it = result.begin(); it != result.end(); it++){
    if(is_number(it->first)){
      outputFile << it->first << " " << it->second << "\n";
      wordCountFinal += it->second;
      result.erase(it->first);
    } 
  }
  // next words
  for(auto it = result.begin(); it != result.end(); it++){
      outputFile << it->first << " " << it->second << "\n";
      wordCountFinal += it->second;
  }
  std::cout << "final word count " << wordCountFinal << std::endl;
  outputFile.close();
}

void sortPrint(std::map<std::string, int> result, std::string path) {
  std::ofstream outputFile;
  outputFile.open (path);
  std::map<int, int> convert; // convert all strings to int for sorting purposes
  for(auto it = result.begin(); it != result.end(); it++){
      int stringToInt = std::stoi(it->first);
      convert[stringToInt] = it->second;
  }

  //numbers are already sorted
  for(auto it = convert.begin(); it != convert.end(); it++){
      //if there are dupes print all the dupes
      for(int i = 0; i < it->second; i++){
	outputFile << it->first << "\n";
      }
  }
  outputFile.close();

}
bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

