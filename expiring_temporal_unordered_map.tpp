#ifndef EXPIRING_TEMPORAL_UNORDERED_MAP_TPP
#define EXPIRING_TEMPORAL_UNORDERED_MAP_TPP

#include "expiring_temporal_unordered_map.hpp"
#include <algorithm>

template <typename Key, typename T>
ExpiringTemporalUnorderedMap<Key, T>::ExpiringTemporalUnorderedMap(int time_limit_seconds)
    : time_limit_(std::chrono::seconds(time_limit_seconds)) {}

template <typename Key, typename T>
std::pair<typename std::unordered_map<Key, T>::iterator, bool>
ExpiringTemporalUnorderedMap<Key, T>::insert(const Key& key, const T& value) {
    auto now = Clock::now();
    auto result = elements_.emplace(key, Element(value, now));
    remove_expired_elements();
    return result;
}

template <typename Key, typename T>
template <typename... Args>
std::pair<typename std::unordered_map<Key, T>::iterator, bool>
ExpiringTemporalUnorderedMap<Key, T>::emplace(Args&&... args) {
    auto now = Clock::now();
    auto result = elements_.emplace(std::forward<Args>(args)..., Element(T{}, now));
    remove_expired_elements();
    return result;
}

template <typename Key, typename T>
size_t ExpiringTemporalUnorderedMap<Key, T>::erase(const Key& key) {
    auto erased_count = elements_.erase(key);
    remove_expired_elements();
    return erased_count;
}

template <typename Key, typename T>
void ExpiringTemporalUnorderedMap<Key, T>::clear_expired_elements() {
    remove_expired_elements();
}

template <typename Key, typename T>
void ExpiringTemporalUnorderedMap<Key, T>::remove_expired_elements() {
    auto now = Clock::now();
    for (auto it = elements_.begin(); it != elements_.end();) {
        if ((now - it->second.timestamp) > time_limit_) {
            it = elements_.erase(it); // Erase expired element and move iterator
        } else {
            ++it;
        }
    }
}

#endif // EXPIRING_TEMPORAL_UNORDERED_MAP_TPP
