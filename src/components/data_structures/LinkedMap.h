#ifndef DATA_STRUCTURES_LINKED_MAP_HH
#define DATA_STRUCTURES_LINKED_MAP_HH

#include <forward_list>
#include <map>

namespace DataStructures {
    /**
     * A map data structure that also forms a linked list of maps. 
     * Searching for a value from a LinkedMap will traverse the linked list of maps until 
     * it finds a value (assuming a value exists). Values are added by default to the map at the front 
     * of the linked list. By default the LinkedMap is initialized with a single map in the linked list.
     */
    template <class T>
    class LinkedMap {
        public:
            using TMap = std::shared_ptr<std::map<std::string, T>>;
            LinkedMap();
            /**
             * Get the value at the given key.
             */
            T& at(std::string key);
            /**
             * Get the front-most map of values.
             */
            std::map<std::string, T>& front();
            /**
             * Whether a value at the given key exists.
             */
            bool contains(std::string key);
            /**
             * Insert a given value at the given key to the map at the front 
             * of the linked list of maps.
             */
            void insert(std::string key, T value);
            /**
             * Create a new map to the front of the linked list.
             */
            void pushFront();
            /**
             * Remove a map from the front of the linked list. Will not 
             * remove a map if this will result in an empty linked list. LinkedMap 
             * must always maintain at least one map.
             */
            void popFront();
        private:
            std::vector<TMap> _maps;
            int _size = 0;
    };

    template<class T>
    inline LinkedMap<T>::LinkedMap()
    {
        this->pushFront();
    }

    template<class T>
    inline T& LinkedMap<T>::at(std::string key)
    {
        auto mapIt = std::find_if(this->_maps.rbegin(), this->_maps.rend(), [key](LinkedMap::TMap map) {
            return map->contains(key);
        });
        if (mapIt != this->_maps.rend()) {
            return (*mapIt)->at(key);
        } else {
            throw std::runtime_error("No value at key " + key + " in LinkedMap.");
        }
    }

    template<class T>
    inline std::map<std::string, T>& LinkedMap<T>::front()
    {
        return *(this->_maps.back());
    }

    template<class T>
    inline bool LinkedMap<T>::contains(std::string key)
    {
        auto mapIt = std::find_if(this->_maps.rbegin(), this->_maps.rend(), [key](LinkedMap::TMap map) {
            return map->contains(key);
        });
        return (mapIt != this->_maps.rend());
    }

    template<class T>
    inline void LinkedMap<T>::insert(std::string key, T value)
    {
        this->_maps.back()->insert({key, value});
    }

    template<class T>
    inline void LinkedMap<T>::pushFront()
    {
        this->_maps.insert(this->_maps.end(), LinkedMap::TMap{new std::map<std::string, T>});
        this->_size = this->_size + 1;
    }

    template<class T>
    inline void LinkedMap<T>::popFront()
    {
        if (this->_size > 1) {
            this->_maps.pop_back();
            this->_size = this->_size - 1;
        }
    }
};

#endif