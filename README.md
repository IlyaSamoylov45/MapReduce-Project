# project1
g++ main.cpp -o main -std++11 -lpthread 

# TODO
- wordcount and sort for processes
- make result pdf
- test


# impl
- Threads
  - Each thread is assigned around (amount_words / num_map_threads)  of work
  - Each thread creates its own hashmap where it stores <word, frequency> pairs
  - once all threads are finished, each reducer thread is assigned around (num_map_threads / num_reduce_threads) amount of hashmaps to merge
  - Once each reduce thread finishes merging its given maps, the reduce thread will grab a mutex lock and write to the final merged map safley.
  - once all reduce threads are finished, since map's in C++ are already sorted we can easily print the key value pairs to the output file
  - for the sorted application we use the exact same implementation but when printing, if a frequency for a pair is greater than 1 we just print that number (frequency) amount of times
