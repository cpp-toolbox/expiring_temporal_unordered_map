#ifndef EXPIRING_TEMPORAL_UNORDERED_MAP_HPP
#define EXPIRING_TEMPORAL_UNORDERED_MAP_HPP

#include <unordered_map>
#include <chrono>

/**
 * @brief A map-like container where elements expire after a certain time limit and can be retrieved by time.
 * @tparam Key The type of the key.
 * @tparam T The type of the value.
 */
template <typename Key, typename T>
class ExpiringTemporalUnorderedMap {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    /**
     * @brief Constructor to initialize the expiring temporal unordered map with a time limit.
     * @param time_limit The time duration after which elements expire.
     */
    explicit ExpiringTemporalUnorderedMap(int time_limit_seconds); // eventually this should take in a double...


    /**
     * @brief Insert an element into the map with the current timestamp.
     * @param key The key associated with the element.
     * @param value The element to be added.
     * @return A pair consisting of an iterator to the inserted element and a bool indicating if the insertion was successful.
     */
    std::pair<typename std::unordered_map<Key, T>::iterator, bool> insert(const Key& key, const T& value);

    /**
     * @brief Insert an element into the map with the current timestamp using emplace.
     * @param args The arguments to be forwarded to the element constructor.
     * @return A pair consisting of an iterator to the inserted element and a bool indicating if the insertion was successful.
     */
    template <typename... Args>
    std::pair<typename std::unordered_map<Key, T>::iterator, bool> emplace(Args&&... args);

    /**
     * @brief Erase an element by its key.
     * @param key The key of the element to be erased.
     * @return The number of elements erased (1 if the element was found and erased, 0 otherwise).
     */
    size_t erase(const Key& key);

    /**
     * @brief Clear all expired elements from the map.
     */
    void clear_expired_elements();

    T& operator[](const Key& key) {
	remove_expired_elements();  // Remove expired elements before accessing

	// Check if the key exists in the map
	auto it = elements_.find(key);
	if (it != elements_.end()) {
	    return it->second.value;  // Return the existing value
	}

	// If the key doesn't exist, insert a new element with the current timestamp
	auto now = Clock::now();
	return elements_.emplace(key, Element(T{}, now)).first->second.value;
    }


private:
    struct Element {
        T value;
        TimePoint timestamp;

        Element(const T& v, TimePoint t)
            : value(v), timestamp(t) {}
    };

    /**
     * @brief Remove elements older than the specified time limit.
     */
    void remove_expired_elements();

    std::unordered_map<Key, Element> elements_;
    std::chrono::seconds time_limit_;
};

#include "expiring_temporal_unordered_map.tpp"

#endif
