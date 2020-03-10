// #include "cache.hh"
// #include "fifo_evictor.hh"
// #include <iostream>
// #include <cassert>
//
// using byte_type = char;
// using val_type = const byte_type*;   // Values for K-V pairs
// using size_type = uint32_t;         // Internal indexing to K-V elements
//
// int main() {
//   std::cerr << "before Cache\n";
//
//   Cache c(16);
//   val_type first  = "first";
//   val_type second = "second";
//   val_type third  = "third";
//   val_type fourth = "fourth";
//
//   std::cerr << "before 1\n";
//   c.set("1", first, 5);
//   assert(c.space_used() == 5);
//   std::cerr << "after 1\n";
//
//   c.set("2", second, 6);
//   assert(c.space_used() == 11);
//
//   c.set("3", third, 5);
//   assert(c.space_used() == 16);
//
//   c.set("4", fourth, 6);
//   assert(c.space_used() == 11);
//
// return 0;
// }
