# CSCI 389 Homework 2
### Spring 2020
### Jonah Kohn and Elijah Wheelock

This project involved the creation of a generic look-aside cache using a set API .hh file. The cache was implemented such that all major functions run in expected constant time, with a few edge cases with worst-case linear time. In order to store key-value pairs in the cache, an std::unordered_map object was used.  Additionally, the size of each cache element is stored by putting the pointer to the data and the size into a CacheElement object, which is then stored in the unordered_map. The unordered_map allows users to set a custom hashing function to handle collision resolution which manages the unordered_map hashtable, although std::hash will be used by default if no arguments are passed. Unordered_map was chosen because it provides a comfortable dynamic resizing strategy, as the unordered map can be dynamically resized using the argument max_load_factor in its construction.

In order to keep the cache running in constant time with the addition of an evictor, our FIFO evictor uses an std::list to keep track of keys which have been touched. Because list::push_back() and list::pop_front() run in constant time, the FIFO evictor is simply an interface for queue which runs in constant time.

We've included some tests designed to confirm the proper behavior of each function in our implementation, including evictor functions. Additionally, a make script is included to ensure the library dependencies are sorted out. 

To build:
```
$ ./make.sh
```

To test: 
```
$ ./run_tests.sh
```
