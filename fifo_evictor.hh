#pragma   once
#include <string>
#include <list>
#include "evictor.hh"

// Data type to use as keys for Cache and Evictors:
using key_type = std::string;

// Abstract base class to define evictions policies.
// It allows touching a key (on a set or get event), and request for
// eviction, which also deletes a key. There is no explicit deletion
// mechanism other than eviction.
class FifoEvictor : public Evictor {
 private:
    std::list<key_type> q_;

 public:
  FifoEvictor();

  ~FifoEvictor() = default;

  void touch_key(const key_type& key_p);

  // Request evictor for the next key to evict, and remove it from evictor.
  // If evictor doesn't know what to evict, return an empty key ("").
  const key_type evict();

  void store(key_type key);

  void remove(key_type key);

  void reset();
};
