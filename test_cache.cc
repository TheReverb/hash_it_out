#include "cache.hh"
#include "fifo_evictor.hh"
#include <iostream>


int main() {

  c = Cache(16)

  std::string* first = "first";
  std::string* second = "second";
  std::string* third = "third";
  std::string* fourth = "fourth";


  c.set("1", first, 5);
  assert(c.space_used == 5)

  c.set("2", second, 6);
  assert(c.space_used == 11)

  c.set("3", third, 5);
  assert(c.space_used == 16)

  c.set("4", fourth, 6);
  assert(c.space_used == 11)



return 0;
}
