#include <cassert>
#include <iostream>
#include "fifo_evictor.hh"

FifoEvictor e;

void reset() {
  while (e.evict() != "") {}
}

void test_touch_key() {
  reset();

  e.touch_key("hello");
  assert(e.evict() == "hello");

  e.touch_key("world");
  assert(e.evict() == "world");
}

void test_evict() {
  reset();

  assert(e.evict() == "");

  e.touch_key("2");
  e.touch_key("3");
  e.touch_key("1");
  e.touch_key("4");

  assert(e.evict() == "2");
  assert(e.evict() == "3");
  assert(e.evict() == "1");
  assert(e.evict() == "4");
}

int main() {
  reset();
  test_touch_key();
  reset();
  test_evict();
  reset();

  std::cout << "All evictor tests passing \n";
  return 0;
}
