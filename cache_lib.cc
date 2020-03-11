#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include <string.h>
#include "fifo_evictor.hh"
#include "cache.hh"


class Cache::Impl {
 private:

  class CacheElement {
    public:
      size_type size;
      val_type* val_p;

      CacheElement(size_type elem_size, val_type elem_val) //CacheElement object stores the size and a deep copy of the value
      : size(elem_size)
      , val_p(new val_type(elem_val))
      {
      }

      CacheElement(const CacheElement& elem) = default;
      ~CacheElement() = default;
  }; // end CacheElement

  size_type maxmem_;
  float     max_load_factor_ = 0.75;
  Evictor*  evictor_;
  hash_func hasher_;
  size_type current_size_;
  std::unordered_map<key_type, CacheElement, hash_func> data_; //unordered_map is used as a hashtable to lookup elements via keys in constant time.

 public:
  Impl(size_type maxmem,
       float     max_load_factor = 0.75,
       Evictor*  evictor         = nullptr,
       hash_func hasher          = std::hash<key_type>())
  : maxmem_(maxmem)
  , max_load_factor_(max_load_factor)
  , evictor_(evictor)
  , hasher_(hasher)
  , current_size_(0)
  , data_(std::unordered_map<key_type, CacheElement, hash_func>(16, hasher_))
  {
    data_.max_load_factor(max_load_factor_); //this causes the unordered_map to dynamically resize whenever the max_load_factor is exceeded.
  }

  ~Impl(){
    delete evictor_;
  }

  void set(key_type key, val_type val, size_type size) {
    if (size > maxmem_) {return;}
    const auto old_elemi = data_.find(key);
    if (old_elemi != data_.end()) {
      current_size_ -= old_elemi->second.size;
    }
    if (current_size_ + size > maxmem_) {
      if (evictor_ != nullptr) {
        clear_space(size);
        evictor_->touch_key(key);
        insert_or_assign(key, val, size);
      }
    }
    else {
      insert_or_assign(key, val, size);
    }
  }

  void insert_or_assign(key_type key, val_type val, size_type size) {
    const CacheElement new_elem(size, val);
    data_.insert_or_assign(key, new_elem); //insert or assign will replace the key-value pair
    current_size_ += size;                 //if the key exists, and will add the pair if the key is not in use.
  }

  void clear_space(size_type size) {    //this function will loop until the cache has enough space for the new element, while calling evict.
    while (current_size_ + size > maxmem_) {
      const key_type evictee_key = evictor_->evict();
      const auto elemi = data_.find(evictee_key);
      if (elemi != data_.end()) {
        current_size_ -= elemi->second.size;
        del(evictee_key);
      }
    }
  }

  val_type get(key_type key, size_type& val_size) const{
    const auto elemi = data_.find(key);
    if (elemi != data_.end()) { //if this is false, the element is not in the unordered_map.
      val_type val_p = *elemi->second.val_p;
      val_size = elemi->second.size;
      if (evictor_ != nullptr) {
        evictor_->touch_key(key);
      }
      return val_p;
    }
    else {
      return nullptr;
    }
  }

  bool del(key_type key) {
    const auto elem_iter = data_.find(key);
    if (elem_iter == data_.end()) { return false; }
    else {
      CacheElement elem = elem_iter->second;
      current_size_ -= elem.size;
      data_.erase(key);
      return true;
    }
  }

  size_type space_used() const {
    return current_size_;
  }

  void reset() {
    data_.clear();
    if (evictor_ != nullptr) {
      while (evictor_->evict() != "") {} //if the evictor returns "", it is empty.
    }
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
  pImpl_ = std::make_unique<Impl>(Impl(maxmem, max_load_factor, evictor, hasher));
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
Cache::val_type Cache::get(key_type key, size_type &val_size) const {
  return pImpl_->get(key, val_size);
}

// Delete an object from the cache, if it's still there
bool Cache::del(key_type key) {
  return pImpl_->del(key);
}

// Compute the total amount of memory used up by all cache values (not keys)
Cache::size_type Cache::space_used() const {
  return pImpl_->space_used();
}

// Delete all data from the cache
void Cache::reset() {
  return pImpl_->reset();
}
