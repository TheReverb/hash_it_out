#include <cassert>
#include <iostream>
#include <cstring>

#include "evictor.hh"
#include "cache.hh"


Cache c(16);

void test_set_and_get(){

  Cache::val_type first = "first";
  Cache::val_type second  = "second";
  Cache::val_type third   = "third";
  Cache::val_type fourth  = "fourth";

  key_type first_key = "1";
  key_type second_key = "2";
  key_type third_key = "3";
  key_type fourth_key = "4";

  Cache::val_type result;


  Cache::size_type val_size = 0;

  assert(c.get(first_key, val_size) == nullptr);

  c.set(first_key, first, 6);

  first = "nothing";
  assert(c.space_used() == 6);

  result = c.get(first_key, val_size);
  assert(result != nullptr);
  assert(std::strcmp(result, "first") == 0);
  assert(val_size == 6);

  c.set(second_key, second, 7);

  second = "nothing";
  assert(c.space_used() == 13);

  result = c.get(second_key, val_size);
  assert(result != nullptr);
  assert(std::strcmp(result, "second") == 0);
  assert(val_size == 7);

  c.set(third_key, third, 6);
  //this should not be inserted.
  result = c.get(third_key, val_size);
  assert(result == nullptr);
  assert(val_size == 7);

  //this should overwrite the value stored at key first_key
  c.set(first_key, fourth, 7);

  assert(c.space_used() == 13);
  result = c.get(first_key, val_size);
  std::cout << result << '\n';
  assert(result != nullptr);
  assert(std::strcmp(result, "fourth") == 0);
  assert(val_size == 7);

}


void test_del(){

  Cache::val_type first = "first";
  Cache::val_type second  = "second";

  key_type first_key = "1";
  key_type second_key = "2";

  Cache::size_type val_size = 0;

  c.set(first_key, first, 6);
  c.set(second_key, second, 7);

  Cache::val_type result = c.get(first_key, val_size);
  assert(result != nullptr);
  assert(std::strcmp(result, "first") == 0);
  assert(val_size == 13);

  assert(c.del(first_key) == true);
  assert(c.space_used() == 7);

  assert(c.get(first_key, val_size) == nullptr);
  assert(c.del(first_key) == false);

  assert(c.space_used() == 7);

}


void test_reset(){

  Cache::val_type first = "first";
  Cache::val_type second  = "second";
  Cache::val_type third   = "th";

  key_type first_key = "1";
  key_type second_key = "2";
  key_type third_key = "3";

  Cache::size_type val_size = 0;

  c.set(first_key, first, 6);
  c.set(second_key, second, 7);
  c.set(third_key, third, 3);

  Cache::val_type result = c.get(first_key, val_size);
  assert(result != nullptr);
  assert(std::strcmp(result, "first") == 0);
  assert(c.space_used() == 16);

  c.reset();

  assert(c.get(first_key, val_size) == nullptr);
  assert(val_size == 0);

  assert(c.get(second_key, val_size) == nullptr);
  assert(val_size == 0);

  assert(c.del(third_key) == false);
  assert(c.space_used() == 0);

}


int main() {

  test_set_and_get();
  c.reset();
  test_del();
  c.reset();
  test_reset();
  c.reset();


  std::cout << "All cache tests passing with no evictor. \n";

  return 0;
}
