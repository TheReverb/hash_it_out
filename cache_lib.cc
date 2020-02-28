#include <iostream>
#include <unordered_map>

class Cache {
 private:
   // All internal data and functionality is hidden using the Pimpl idiom
   // (see here: https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/)
std::unique_ptr<Impl> pImpl_;

  class Impl {
   private:
    size_type maxmem_;
    float     max_load_factor_;
    Evictor*  evictor_;
    hash_func hasher_;

    size_type current_size_; //in uint32_t

   public:

    Impl(size_type maxmem,
         float     max_load_factor = 0.75,
         Evictor*  evictor         = nullptr,
         hash_func hasher          = std::hash<key_type>())
    : maxmem_(maxmem), max_load_factor_(max_load_factor), evictor_(evictor), hasher_(hasher), current_size_(10){ //eitan-approved method for setting class member varaibles via constructor arguments

    } //for now, our constructor body is empty.


    ~Impl(){  //this will most likely involve calling reset().

    }


    void set(key_type key, val_type val, size_type size){
      //first, check whether adding this element would cause the current load factor to exceed max load factor.
      //if yes: increase maxmem and redistribute key/pointer pairs via the function resize().

      //then:

      // call the hash function using key, store pointer. if the key already points at an existing value, simply overwrite.
      // take the pointer it returns, store **a deep copy** of val at that location.
      // update current_size, current_load_factor,

    }


    val_type get(key_type key, size_type& val_size) const{
      // Retrieve a pointer to the value associated with key in the cache,
      // or nullptr if not found.
      // Sets the actual size of the returned value (in bytes) in val_size.
    }


    bool del(key_type key) {
      //check if object is in cache, potentially by calling get().
      //if it isn't, return false.

      //if it is, free the space in the cache by removing the key/value pair from the hashtable, then return true.
    }


    size_type space_used() const {
      return current_size_; //this one should be simple :)
    }


    void reset() {
      // unsure about the implementation of this one. Potentially calling del() on every key in the hashtable?
      // it might be as simple as removing keys from the hastable, and considering them as good as erased.

    }

  };  //Impl


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
    impl_->set(key_type key, val_type val, size_type size);
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
