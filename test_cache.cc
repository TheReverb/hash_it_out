#include <cassert>
#include <iostream>
#include "evictor.hh"
#include "cache.hh"

int main() {
  // std::cerr << "before Cache\n";
  Cache c(16);
  byte_type* first = "first";
  val_type second  = "second";
  val_type third   = "third";
  val_type fourth  = "fourth";

  size_type val_size = 0;

  // std::cerr << "before 1\n";
  c.set("1", first, 6);
  first = "third";
  assert(c.space_used() == 6);
  assert(c.get("1", val_size) == "first");
  assert(val_size == 6);


  // std::cerr << "after 1\n";
  c.set("2", second, 7);
  assert(c.space_used() == 13);
  assert(c.get("2", val_size) == "second");
  c.set("3", third, 6);
  assert(c.space_used() == 13);
  c.set("4", fourth, 7);
  // std::cout << c.space_used() << '\n';
  assert(c.space_used() == 13);

  std::cout << "all tests passing\n";

  return 0;
}
