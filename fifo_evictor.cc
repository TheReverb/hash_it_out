#include <string>
#include <list>
#include "fifo_evictor.hh"

FifoEvictor::FifoEvictor() {
  std::list<key_type> q_ = std::list<key_type>();
}

void FifoEvictor::touch_key(const key_type& key) {
  q_.push_back(key);
}

// Request evictor for the next key to evict, and remove it from evictor.
// If evictor doesn't know what to evict, return an empty key ("").
const key_type FifoEvictor::evict() {
  if (q_.empty()) {return "";}
  key_type result = q_.front();
  q_.pop_front();

  return result;
}
