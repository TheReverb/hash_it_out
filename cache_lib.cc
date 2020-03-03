#include <iostream>
#include <unordered_map>
#include "fifo_evictor.cc"
#include "cache.hh"

class Cache {
 private:
   // All internal data and functionality is hidden using the Pimpl idiom
   // (see here: https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/)
std::unique_ptr<Impl> pImpl_;

  class Impl {
   private:
    size_type maxmem_;
    float     max_load_factor_ = 0.75;
    Evictor*  evictor_;
    hash_func hasher_;
    std::unordered_map<key_type, val_type, hash_func> data_;
    size_type current_size_; //in uint32_t

    class cache_element {
      public:
        size_type size;
        val_type* val_p;

        cache_element(size_type elem_size, val_type* elem_val) //figure out what elem val reference should be
        : size(elem_size)
        , val_p(elem_val) //might need to dereference
        {}

        ~cache_element(){
          delete val_p;
          delete size;
        }

    };

    size_type size_of_val(val_type v) {
      size_type size = 0;
      while (v[size] != 0) {
        size++;
      }
      return size;
    }

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
    {
      std::unordered_map<key_type, val_type, hash_func>(hash=hasher_) data_;
      data_.max_load_factor(max_load_factor_);
    }

    ~Impl(){ //this will most likely involve calling reset().
      delete data_;
      delete evictor_;
      assert(false);
    }

    void set(key_type key, val_type val, size_type size){
      if (evictor != nullptr) {
        while (current_size_ + size > maxmem_) {
          key_type evictee = evictor_->evict();
          current_size_ -= data_[evictee].size;
          del(evictee);
        }
        if (current_size_ + size < maxmem_) {
            // BUG?? // Ask Eitan: sizeof(size_type)?
            elem = cache_element(size, new val_type(val));
            data.insert_or_assign(key, elem);
            evictor_->store(&key);
            evictor_->touch_key(&key);
            current_size_ += size;
        }
      }
      else if (current_size_ + size < maxmem_) {
            // BUG?? // Ask Eitan: sizeof(size_type)?
            elem = cache_element(size, new val_type(val));
            data.insert_or_assign(key, elem);
            evictor_->store(&key);
            evictor_->touch_key(&key);
            current_size_ += size;
      }
    }

    val_type get(key_type key, size_type& val_size) const{
      elem_iter = data_.find(key);
      if (elem_iter == data_.end()) { return nullptr; }
      else {
        elem = elem_iter->second;
        *val_size = elem.size;
        evictor_->touch_key(key);
        return elem.val_p;
      }
    }

    bool del(key_type key) {
      elem_iter = data_.find(key);
      if (elem_iter == data_.end()) { return false; }
      else {
        elem = elem_iter->second;
        current_size_ -= elem.size;
        // do we need to evict this element? how?
        delete elem;
        return true;
      }
    }

    size_type space_used() const {
      return current_size_; //this one should be simple :)
    }

    void reset() {
      for(auto iter = data_.begin(); iter != data_.end(); iter++){
        delete iter->first;
        // how do we clear the evictor?
      }
    current_size_ = 0;
    // somehow make sure evictor is clean.
    }

  }; //end Impl


 public:
  using byte_type = char;
  using val_type  = const byte_type*;   // Values for K-V pairs
  using size_type = uint32_t;           // Internal indexing to K-V elements

  // A function that takes a key and returns an index to the internal data
  using hash_func = std::function<std::size_t(key_type)>;

  // Create a new cache object with the following parameters:
  // maxmem: The maximum allowance for storage used by values.
  // max_load_factor: Maximum allowed ratio between buckets and table rows.
  // evictor: Eviction policy implementation (if nullptr, no evictions occur
  // and new insertions fail after maxmem has been exceeded).
  // hasher: Hash function to use on the keys. Defaults to C++'s std::hash.
  Cache(size_type maxmem,
        float max_load_factor = 0.75,
        Evictor* evictor = nullptr,
        hash_func hasher = std::hash<key_type>()) {
    std::unique_ptr<Impl> pImpl_ = new Impl(maxmem,
                                            max_load_factor,
                                            evictor,
                                            hasher);
  }

  ~Cache() {
    delete pImpl_;
  }

  // Disallow cache copies, to simplify memory management.
  Cache(const Cache&) = delete;
  Cache& operator=(const Cache&) = delete;

  // Add a <key, value> pair to the cache.
  // If key already exists, it will overwrite the old value.
  // Both the key and the value are to be deep-copied (not just pointer copied).
  // If maxmem capacity is exceeded, enough values will be removed
  // from the cache to accomodate the new value. If unable, the new value
  // isn't inserted to the cache.
  void set(key_type key, val_type val, size_type size) {
    impl_->set(key, val, size);
  }

  // Retrieve a pointer to the value associated with key in the cache,
  // or nullptr if not found.
  // Sets the actual size of the returned value (in bytes) in val_size.
  val_type get(key_type key, size_type& val_size) const;

  // Delete an object from the cache, if it's still there
  bool del(key_type key) {
    return pImpl_->del(key);
  }

  // Compute the total amount of memory used up by all cache values (not keys)
  size_type space_used() const {
    return pImpl_->space_used();
  }

  // Delete all data from the cache
  void reset() {
    return pImpl_->reset();
  }
};
