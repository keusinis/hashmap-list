#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include "hashmap.h"

class Logger
{
public:
    explicit Logger(const std::string &filename)
        : file(filename)
    {
    }

    template <typename T>
    Logger &operator<<(const T &value)
    {
        std::cout << value;
        file << value;
        return *this;
    }

    Logger &operator<<(std::ostream &(*manip)(std::ostream &))
    {
        manip(std::cout);
        manip(file);
        return *this;
    }

private:
    std::ofstream file;
};

static int passed = 0;
static int failed = 0;

static void report(Logger &log, const std::string &name, bool condition, const std::string &detail)
{
    std::string verdict = condition ? "PASS" : "FAIL";
    log << "  [" << verdict << "] " << name << "\n";
    log << "         " << detail << "\n\n";
    if (condition)
        ++passed;
    else
        ++failed;
}

static void runTests(Logger &log)
{
    log << "Starting tests...\n\n";

    {
        Ip2::HashMap m;
        report(log, "T01  Default construction",
               m.empty() && m.size() == 0,
               "empty()=true, size()=0");
    }

    {
        Ip2::HashMap m;
        m += {"cat", "meow"};
        report(log, "T02  operator+= single entry",
               m.size() == 1 && m.get("cat") == "meow",
               "size()=1, get(\"cat\")=\"meow\"");
    }

    {
        Ip2::HashMap m;
        m += {"a", "1"};
        m += {"b", "2"};
        m += {"c", "3"};
        report(log, "T03  operator+= multiple entries",
               m.size() == 3 && m.get("b") == "2",
               "size()=3, get(\"b\")=\"2\"");
    }

    {
        Ip2::HashMap m;
        m += {"cat", "meow"};
        report(log, "T04  operator[] key found",
               m["cat"] == true,
               "[\"cat\"]=true");
    }

    {
        Ip2::HashMap m;
        m += {"cat", "meow"};
        report(log, "T05  operator[] key not found",
               m["dog"] == false,
               "[\"dog\"]=false");
    }

    {
        Ip2::HashMap m;
        m += {"cat", "meow"};
        m %= {"cat", "PURR"};
        report(log, "T06  operator%= update value",
               m.get("cat") == "PURR",
               "get(\"cat\")=\"PURR\"");
    }

    {
        Ip2::HashMap m;
        m += {"cat", "meow"};
        m += {"dog", "bark"};
        m -= "cat";
        report(log, "T07  operator-= delete entry",
               m.size() == 1 && !m["cat"] && m["dog"],
               "size()=1, cat gone, dog remains");
    }

    {
        Ip2::HashMap m;
        m += {"a", "1"};
        m += {"b", "2"};
        !m;
        report(log, "T08  operator! clear",
               m.empty() && m.size() == 0,
               "empty()=true, size()=0");
    }

    {
        Ip2::HashMap original;
        original += {"x", "1"};
        original += {"y", "2"};
        Ip2::HashMap copy(original);
        original += {"z", "3"};
        report(log, "T09  Copy constructor deep copy",
               copy.size() == 2 && !copy["z"],
               "copy.size()=2, copy[\"z\"]=false after original modified");
    }

    {
        Ip2::HashMap src;
        src += {"a", "1"};
        Ip2::HashMap dst;
        dst = src;
        src += {"b", "2"};
        report(log, "T10  Copy assignment deep copy",
               dst.size() == 1 && !dst["b"],
               "dst.size()=1, dst[\"b\"]=false after src modified");
    }

    {
        Ip2::HashMap a, b;
        a += {"x", "1"};
        a += {"y", "2"};
        b += {"x", "1"};
        b += {"y", "2"};
        report(log, "T11  operator== equal maps",
               a == b,
               "identical content => true");
    }

    {
        Ip2::HashMap a, b;
        a += {"x", "1"};
        b += {"x", "99"};
        report(log, "T12  operator== different values",
               !(a == b),
               "same key, different value => false");
    }

    {
        Ip2::HashMap a, b;
        a += {"x", "1"};
        b += {"y", "1"};
        report(log, "T13  operator!=",
               a != b,
               "different keys => true");
    }

    {
        Ip2::HashMap a, b;
        a += {"apple", "1"};
        b += {"banana", "1"};
        report(log, "T14  operator< lexicographic",
               a < b && !(b < a),
               "apple < banana");
    }

    {
        Ip2::HashMap a, b;
        a += {"banana", "1"};
        b += {"apple", "1"};
        report(log, "T15  operator>",
               a > b,
               "banana > apple");
    }

    {
        Ip2::HashMap a, b;
        a += {"x", "1"};
        b += {"x", "1"};
        report(log, "T16  operator<= equal case",
               a <= b,
               "equal maps => true");
    }

    {
        Ip2::HashMap a, b;
        a += {"a", "1"};
        b += {"b", "1"};
        report(log, "T17  operator<= less case",
               a <= b,
               "a <= b => true");
    }

    {
        Ip2::HashMap p, q;
        p += {"cat", "1"};
        q += {"dog", "1"};
        bool neqOk = (p != q) == !(p == q);
        bool leqOk = (p <= q) == !(p > q);
        bool geqOk = (q >= p) == !(q < p);
        report(log, "T18  Comparison alignment",
               neqOk && leqOk && geqOk,
               "!= == !(==), <= == !(>), >= == !(<)");
    }

    {
        Ip2::HashMap a, b;
        a += {"x", "1"};
        report(log, "T19  operator< different sizes",
               Ip2::HashMap() < a,
               "empty < non-empty => true");
    }

    {
        bool caught = false;
        try
        {
            Ip2::HashMap m;
            m += {"k", "v"};
            m += {"k", "v2"};
        }
        catch (const Ip2::HashMapException &)
        {
            caught = true;
        }
        report(log, "T20  Exception: duplicate key on +=",
               caught, "HashMapException thrown for duplicate key");
    }

    {
        bool caught = false;
        try
        {
            Ip2::HashMap m;
            m -= "ghost";
        }
        catch (const Ip2::HashMapException &)
        {
            caught = true;
        }
        report(log, "T21  Exception: delete missing key",
               caught, "HashMapException thrown for missing key on -=");
    }

    {
        bool caught = false;
        try
        {
            Ip2::HashMap m;
            m %= {"ghost", "val"};
        }
        catch (const Ip2::HashMapException &)
        {
            caught = true;
        }
        report(log, "T22  Exception: update missing key",
               caught, "HashMapException thrown for missing key on %=");
    }

    {
        bool caught = false;
        try
        {
            Ip2::HashMap m;
            m.get("ghost");
        }
        catch (const Ip2::HashMapException &)
        {
            caught = true;
        }
        report(log, "T23  Exception: get missing key",
               caught, "HashMapException thrown for missing key on get()");
    }

    {
        Ip2::HashMap m;
        m += {"hello", "world"};
        std::string s = m.toString();
        bool hasCount = s.find("1/") != std::string::npos;
        bool hasBuckets = s.find("/16:") != std::string::npos;
        bool hasEntry = s.find("hello=world") != std::string::npos;
        report(log, "T24  toString() format",
               hasCount && hasBuckets && hasEntry,
               "toString()=" + s);
    }

    log << "Results: " << passed << " passed, " << failed << " failed\n";
}

int main()
{
    try
    {
        Logger log("log.txt");
        runTests(log);
    }
    catch (...)
    {
        std::cerr << "[FATAL] Unexpected exception in test runner.\n";
        return 1;
    }

    assert(Ip2::HashMap::getObjectCount() == 0);

    return (failed == 0) ? 0 : 1;
}
