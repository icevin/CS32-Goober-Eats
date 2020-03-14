// ExpandableHashMap.h
#include <iostream>
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
    void print();
  private:
    std::vector<std::list<std::pair<KeyType, ValueType>>> m_buckets;
    double m_maxLoadFactor;
    int m_nAssociations;
    unsigned int getBucketNumber(const KeyType& key) const;
    unsigned int getBucketNumber(const KeyType& key, unsigned int nBuckets) const;
};

template <typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
    : m_buckets(8), m_maxLoadFactor(maximumLoadFactor), m_nAssociations(0) {
}

template <typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap() {
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset() {
    m_buckets.clear();
    m_buckets.reserve(8);
    m_nAssociations = 0;
}

template <typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const {
    return m_nAssociations;
}

// template <typename KeyType, typename ValueType>
// void ExpandableHashMap<KeyType, ValueType>::print() {
//     for(auto a : m_buckets)
//         for(auto b : a)
//             std::cout << b.first.latitudeText << "|" << b.first.longitudeText << std::endl;
// }

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value) {
    double loadFactor = (m_nAssociations + 1.0) / m_buckets.size();
    if (loadFactor > m_maxLoadFactor) {
        std::vector<std::list<std::pair<KeyType, ValueType>>> new_buckets(m_buckets.size() * 2);
        for (auto i = m_buckets.begin(); i != m_buckets.end(); i++)
            for (auto a = i->begin(); a != i->end(); a++)
                new_buckets[getBucketNumber((*a).first, new_buckets.size())].push_back(*a);
        m_buckets.clear();
        m_buckets = new_buckets;
        
    }
    ValueType* valueRef = find(key);
    if (valueRef != nullptr) {
        *valueRef = value;
        return;
    }
    m_buckets[getBucketNumber(key)].push_back(make_pair(key, value));
    m_nAssociations++;
}

template <typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const {
    for (auto a = m_buckets.at(getBucketNumber(key)).begin(); a != m_buckets.at(getBucketNumber(key)).end(); a++)
        if ((*a).first == key)
            return &((*a).second);
    return nullptr;
}

template <typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key) const {
    unsigned int hasher(const KeyType& k);
    return hasher(key) % m_buckets.size();
}

template <typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key, unsigned int nBuckets) const {
    unsigned int hasher(const KeyType& k);
    return hasher(key) % nBuckets;
}