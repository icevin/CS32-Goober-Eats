// ExpandableHashMap.h
#include <iterator>
#include <list>
#include <vector>

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template <typename KeyType, typename ValueType>
class ExpandableHashMap {
  public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key) {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

    // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

  private:
    std::vector<std::list<std::pair<KeyType, ValueType>>> m_buckets;
    double m_maxLoadFactor;
    int m_nItems;
    unsigned int getBucketNumber(const KeyType& key) const;
};

template <typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
    : m_buckets(8), m_maxLoadFactor(maximumLoadFactor), m_nItems(0) {
}

template <typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap() {
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset() {
}

template <typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const {
    return m_nItems;
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value) {
}

template <typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const {
    auto bucket = m_buckets.at(getBucketNumber(key));
    for (auto a = bucket.begin(); a != bucket.end(); a++)
        if ((*a).first == key)
            return &((*a).second);
    return nullptr;
}

template <typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key) const {
    unsigned int hasher(const KeyType& k);
    return hasher(key) % m_buckets.size();
}
