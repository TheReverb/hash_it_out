#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include <string.h>
#include "fifo_evictor.hh"
#include "cache.hh"

using byte_type = char;
using val_type = const byte_type*;
using size_type = uint32_t;

class Cache::Impl {
 private:
  class CacheElement {
    public:
      size_type size;
      val_type val_p;

      CacheElement(size_type elem_size, val_type elem_val)
      : size(elem_size)
      , val_p(elem_val)
      {
        val_type* val_p;
        memcpy(val_p, elem_val, elem_size); // replace with copy
      }

      CacheElement(const CacheElement& elem) = default;

      ~CacheElement(){
        delete val_p;
      }
  }; // end CacheElement

  size_type maxmem_;
  float     max_load_factor_ = 0.75;
  FifoEvictor*  evictor_;
  hash_func hasher_;
  size_type current_size_; //in uint32_t
  std::unordered_map<key_type, CacheElement, hash_func> data_;

 public:
  Impl(size_type    maxmem,
       float        max_load_factor = 0.75,
       FifoEvictor* evictor         = nullptr,
       hash_func    hasher          = std::hash<key_type>())
  : maxmem_(maxmem)
  , max_load_factor_(max_load_factor)
  , evictor_(evictor)
  , hasher_(hasher)
  , current_size_(0)
  , data_(std::unordered_map<key_type, CacheElement, hash_func>(16, hasher_))
  {
    data_.max_load_factor(max_load_factor_);
  }

  ~Impl(){ //this will most likely involve calling reset().
    delete evictor_;
  }

  void set(key_type key, val_type val, size_type size){
    if (evictor_ != nullptr) {
      while (current_size_ + size > maxmem_) {
        const key_type evictee_key = evictor_->evict();
        const auto elemi = data_.find(evictee_key);
        if (elemi != data_.end()) {
          current_size_ -= elemi->second.size;
          del(evictee_key);
        }
      }

      if (current_size_ + size < maxmem_) {
          const CacheElement new_elem(size, val);
          data_.insert_or_assign(key, new_elem);
          evictor_->touch_key(key);
          current_size_ += size;
      }
    }
    else if (current_size_ + size < maxmem_) {
          const CacheElement new_elem(size, val);
          data_.insert_or_assign(key, new_elem);
          current_size_ += size;
    }
  }

  val_type get(key_type key, size_type& val_size) const{
    const auto elem = data_.at(key);
    // auto elem_iter = data_.find(key);
    // if (elem_iter == data_.end()) { return nullptr; }
    // else {
      // CacheElement elem = elem_iter->second;
    val_size = elem.size;
    evictor_->touch_key(key);
    return elem.val_p;
  }

  bool del(key_type key) {
    const auto elem_iter = data_.find(key);
    if (elem_iter == data_.end()) { return false; }
    else {
      CacheElement elem = elem_iter->second;
      current_size_ -= elem.size;
      // evictor_->remove(key);
          // we don't need to evict the key; if it gets evicted later, we'll just ignore it.
      data_.erase(key);
      return true;
    }
  }

  size_type space_used() const {
    return current_size_;
  }

  void reset() {
    data_.clear();
    while (evictor_->evict() != "") {}
    current_size_ = 0;
  }
}; //end Impl

// Create a new cache object with the following parameters:
// maxmem: The maximum allowance for storage used by values.
// max_load_factor: Maximum allowed ratio between buckets and table rows.
// evictor: Eviction policy implementation (if nullptr, no evictions occur
// and new insertions fail after maxmem has been exceeded).
// hasher: Hash function to use on the keys. Defaults to C++'s std::hash.
Cache::Cache(size_type maxmem,
             float max_load_factor,
             Evictor* evictor,
             hash_func hasher)
{
  std::unique_ptr<Impl> pImpl_(new Impl(maxmem,
                                        max_load_factor,
                                        new FifoEvictor(),
                                        hasher));
}

Cache::~Cache() = default;

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// Both the key and the value are to be deep-copied (not just pointer copied).
// If maxmem capacity is exceeded, enough values will be removed
// from the cache to accomodate the new value. If unable, the new value
// isn't inserted to the cache.
void Cache::set(key_type key, val_type val, size_type size) {
  pImpl_->set(key, val, size);
}

// Retrieve a pointer to the value associated with key in the cache,
// or nullptr if not found.
// Sets the actual size of the returned value (in bytes) in val_size.
val_type Cache::get(key_type key, size_type &val_size) const {
  return pImpl_->get(key, val_size);
}

// Delete an object from the cache, if it's still there
bool Cache::del(key_type key) {
  return pImpl_->del(key);
}

// Compute the total amount of memory used up by all cache values (not keys)
size_type Cache::space_used() const {
  return pImpl_->space_used();
}

// Delete all data from the cache
void Cache::reset() {
  return pImpl_->reset();
}
