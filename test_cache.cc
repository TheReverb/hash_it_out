#include "cache.hh"
#include "fifo_evictor.hh"
#include <iostream>


int main() {

  c = Cache(16)

  val_type first = "first";
  val_type second = "second";
  val_type third = "third";
  val_type fourth = "fourth";


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
