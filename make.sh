g++ -std=c++17 -O3 -Wall -Wextra -pedantic -fPIC -shared cache_lib.cc -o cache_lib.so

g++ -std=c++17 -O3 -Wall -Wextra -pedantic -fPIC -shared fifo_evictor.cc -o fifo_evictor.so

g++ -std=c++17 -O3 -Wall -Wextra -pedantic -fpie test_cache.cc -L. -l:cache_lib.so -o test_cache

g++ -std=c++17 -O3 -Wall -Wextra -pedantic -fpie -L. test_evictor.cc -l:fifo_evictor.so -o test_evictor
