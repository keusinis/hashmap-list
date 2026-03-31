/**
 * @brief HashMap ADT
 * @author Rytis Tauras
 *
 * A hash map storing string key-value pairs with separate chaining.
 * Fixed bucket count of 16; collisions resolved via singly-linked lists.
 * @see https://en.wikipedia.org/wiki/Hash_table
 *
 * Implementation developed with assistance from AI tools (Claude Opus 4.6).
 *
 * Namespace: ip2
 */

#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <stdexcept>
#include <utility>

namespace Ip2
{

    class HashMapException : public std::runtime_error
    {
    public:
        explicit HashMapException(const std::string &message);
    };

    class HashMap
    {
    private:
        class Impl;
        Impl *pImpl;

    public:
        HashMap();
        HashMap(const HashMap &other);
        HashMap &operator=(const HashMap &other);
        ~HashMap();

        static size_t getObjectCount();

        HashMap &operator+=(const std::pair<std::string, std::string> &entry);
        HashMap &operator-=(const std::string &key);
        HashMap &operator%=(const std::pair<std::string, std::string> &entry);

        bool operator==(const HashMap &other) const;
        bool operator!=(const HashMap &other) const;
        bool operator<(const HashMap &other) const;
        bool operator<=(const HashMap &other) const;
        bool operator>(const HashMap &other) const;
        bool operator>=(const HashMap &other) const;

        HashMap &operator!();
        bool operator[](const std::string &key) const;

        std::string get(const std::string &key) const;
        std::string toString() const;
        size_t size() const;
        bool empty() const;
    };

} // namespace Ip2

#endif
