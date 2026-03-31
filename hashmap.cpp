#include "hashmap.h"
#include <sstream>

namespace Ip2
{

HashMapException::HashMapException(const std::string &message)
    : std::runtime_error(message)
{
}

struct Node
{
    std::string key;
    std::string value;
    Node *next;

    Node(const std::string &k, const std::string &v)
        : key(k), value(v), next(nullptr)
    {
    }
};

class HashMap::Impl
{
public:
    static const size_t BUCKET_COUNT = 16;
    static size_t objectCount;

    Node *buckets[BUCKET_COUNT];
    size_t count;

    Impl() : count(0)
    {
        for (size_t i = 0; i < BUCKET_COUNT; ++i)
            buckets[i] = nullptr;
        ++objectCount;
    }

    Impl(const Impl &other) : count(other.count)
    {
        for (size_t i = 0; i < BUCKET_COUNT; ++i)
            buckets[i] = nullptr;
        ++objectCount;

        for (size_t i = 0; i < BUCKET_COUNT; ++i)
        {
            for (Node *n = other.buckets[i]; n; n = n->next)
            {
                Node *copy = new Node(n->key, n->value);
                copy->next = buckets[i];
                buckets[i] = copy;
            }
        }
    }

    ~Impl()
    {
        clear();
        --objectCount;
    }

    static size_t hash(const std::string &key)
    {
        size_t h = 5381;
        for (unsigned char c : key)
            h = h * 33 + c;
        return h % BUCKET_COUNT;
    }

    Node *find(const std::string &key) const
    {
        size_t idx = hash(key);
        for (Node *n = buckets[idx]; n; n = n->next)
        {
            if (n->key == key)
                return n;
        }
        return nullptr;
    }

    void insert(const std::string &key, const std::string &value)
    {
        size_t idx = hash(key);
        Node *n = new Node(key, value);
        n->next = buckets[idx];
        buckets[idx] = n;
        ++count;
    }

    void remove(const std::string &key)
    {
        size_t idx = hash(key);
        Node *prev = nullptr;
        Node *curr = buckets[idx];

        while (curr)
        {
            if (curr->key == key)
            {
                if (prev)
                    prev->next = curr->next;
                else
                    buckets[idx] = curr->next;
                delete curr;
                --count;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    void clear()
    {
        for (size_t i = 0; i < BUCKET_COUNT; ++i)
        {
            Node *n = buckets[i];
            while (n)
            {
                Node *next = n->next;
                delete n;
                n = next;
            }
            buckets[i] = nullptr;
        }
        count = 0;
    }

    std::pair<std::string, std::string> *sortedEntries() const
    {
        auto *entries = new std::pair<std::string, std::string>[count];
        size_t idx = 0;

        for (size_t i = 0; i < BUCKET_COUNT; ++i)
        {
            for (Node *n = buckets[i]; n; n = n->next)
                entries[idx++] = {n->key, n->value};
        }

        for (size_t i = 1; i < count; ++i)
        {
            auto tmp = entries[i];
            size_t j = i;
            while (j > 0 && entries[j - 1].first > tmp.first)
            {
                entries[j] = entries[j - 1];
                --j;
            }
            entries[j] = tmp;
        }

        return entries;
    }
};

size_t HashMap::Impl::objectCount = 0;

HashMap::HashMap()
    : pImpl(new Impl())
{
}

HashMap::HashMap(const HashMap &other)
    : pImpl(new Impl(*other.pImpl))
{
}

HashMap &HashMap::operator=(const HashMap &other)
{
    if (this != &other)
    {
        delete pImpl;
        pImpl = new Impl(*other.pImpl);
    }
    return *this;
}

HashMap::~HashMap()
{
    delete pImpl;
}

size_t HashMap::getObjectCount()
{
    return Impl::objectCount;
}

HashMap &HashMap::operator+=(const std::pair<std::string, std::string> &entry)
{
    if (pImpl->find(entry.first))
        throw HashMapException("HashMap::operator+=: duplicate key \"" + entry.first + "\"");
    pImpl->insert(entry.first, entry.second);
    return *this;
}

HashMap &HashMap::operator-=(const std::string &key)
{
    if (!pImpl->find(key))
        throw HashMapException("HashMap::operator-=: key not found \"" + key + "\"");
    pImpl->remove(key);
    return *this;
}

HashMap &HashMap::operator%=(const std::pair<std::string, std::string> &entry)
{
    Node *node = pImpl->find(entry.first);
    if (!node)
        throw HashMapException("HashMap::operator%=: key not found \"" + entry.first + "\"");
    node->value = entry.first;
    return *this;
}

bool HashMap::operator==(const HashMap &other) const
{
    if (pImpl->count != other.pImpl->count)
        return false;

    auto *a = pImpl->sortedEntries();
    auto *b = other.pImpl->sortedEntries();

    bool eq = true;
    for (size_t i = 0; i < pImpl->count; ++i)
    {
        if (a[i] != b[i])
        {
            eq = false;
            break;
        }
    }

    delete[] a;
    delete[] b;
    return eq;
}

bool HashMap::operator!=(const HashMap &other) const
{
    return !(*this == other);
}

bool HashMap::operator<(const HashMap &other) const
{
    size_t na = pImpl->count;
    size_t nb = other.pImpl->count;
    size_t n = na < nb ? na : nb;

    auto *a = pImpl->sortedEntries();
    auto *b = other.pImpl->sortedEntries();

    bool result = false;
    bool decided = false;

    for (size_t i = 0; i < n; ++i)
    {
        if (a[i] < b[i])
        {
            result = true;
            decided = true;
            break;
        }
        if (b[i] < a[i])
        {
            decided = true;
            break;
        }
    }

    if (!decided)
        result = na < nb;

    delete[] a;
    delete[] b;
    return result;
}

bool HashMap::operator<=(const HashMap &other) const
{
    return !(*this > other);
}

bool HashMap::operator>(const HashMap &other) const
{
    return other < *this;
}

bool HashMap::operator>=(const HashMap &other) const
{
    return !(*this < other);
}

HashMap &HashMap::operator!()
{
    pImpl->clear();
    return *this;
}

bool HashMap::operator[](const std::string &key) const
{
    return pImpl->find(key) != nullptr;
}

std::string HashMap::get(const std::string &key) const
{
    Node *node = pImpl->find(key);
    if (!node)
        throw HashMapException("HashMap::get: key not found \"" + key + "\"");
    return node->value;
}

std::string HashMap::toString() const
{
    static const size_t MAX_PREVIEW = 3;

    std::ostringstream oss;
    oss << pImpl->count << "/" << Impl::BUCKET_COUNT << ":";

    if (pImpl->count == 0)
        return oss.str();

    auto *entries = pImpl->sortedEntries();
    size_t limit = pImpl->count < MAX_PREVIEW ? pImpl->count : MAX_PREVIEW;

    for (size_t i = 0; i < limit; ++i)
    {
        if (i > 0)
            oss << ",";
        oss << entries[i].first << "=" << entries[i].second;
    }

    if (pImpl->count > MAX_PREVIEW)
        oss << "...";

    delete[] entries;
    return oss.str();
}

size_t HashMap::size() const
{
    return pImpl->count;
}

bool HashMap::empty() const
{
    return pImpl->count == 0;
}

} // namespace Ip2
