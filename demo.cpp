#include <cassert>
#include <iostream>
#include "hashmap.h"

static void section(const std::string &title)
{
    std::cout << "\n--- " << title << " ---\n";
}

static void show(const std::string &label, const Ip2::HashMap &m)
{
    std::cout << "  " << label << " => " << m.toString() << "\n";
}

int main()
{
    try
    {
        std::cout << "Starting demo...\n";

        section("1. Construction");

        Ip2::HashMap empty;
        show("empty HashMap", empty);
        std::cout << "  size(): " << empty.size() << "\n";
        std::cout << "  empty(): " << (empty.empty() ? "true" : "false") << "\n";


        section("2. operator+= (insert)");

        Ip2::HashMap m;
        m += {"cat", "meow"};
        m += {"dog", "bark"};
        m += {"fox", "ring"};
        show("after 3 inserts", m);


        section("3. operator[] (search) and get()");

        std::cout << "  [\"cat\"] = " << (m["cat"] ? "true" : "false") << "\n";
        std::cout << "  [\"bat\"] = " << (m["bat"] ? "true" : "false") << "\n";
        std::cout << "  get(\"dog\") = " << m.get("dog") << "\n";


        section("4. operator%= (update)");

        m %= {"cat", "PURR"};
        std::cout << "  get(\"cat\") after update = " << m.get("cat") << "\n";
        show("state", m);


        section("5. operator-= (delete)");

        m -= "fox";
        show("after -= \"fox\"", m);
        std::cout << "  [\"fox\"] = " << (m["fox"] ? "true" : "false") << "\n";


        section("6. Copy constructor & copy assignment");

        Ip2::HashMap original;
        original += {"x", "1"};
        original += {"y", "2"};

        Ip2::HashMap copy1(original);
        original += {"z", "3"};
        show("original after += z", original);
        show("copy1 unchanged", copy1);

        Ip2::HashMap copy2;
        copy2 = original;
        original -= "z";
        show("original after -= z", original);
        show("copy2 unchanged", copy2);


        section("7. Comparison operators");

        Ip2::HashMap a, b, c;
        a += {"alpha", "1"};
        a += {"beta", "2"};
        b += {"alpha", "1"};
        b += {"beta", "2"};
        c += {"alpha", "1"};
        c += {"gamma", "3"};

        std::cout << "  a == b : " << (a == b ? "true" : "false") << "\n";
        std::cout << "  a != c : " << (a != c ? "true" : "false") << "\n";
        std::cout << "  a <  c : " << (a < c ? "true" : "false") << "\n";
        std::cout << "  c >  a : " << (c > a ? "true" : "false") << "\n";
        std::cout << "  a <= b : " << (a <= b ? "true" : "false") << "\n";
        std::cout << "  a >= b : " << (a >= b ? "true" : "false") << "\n";


        section("8. operator! (clear)");

        Ip2::HashMap toClear;
        toClear += {"a", "1"};
        toClear += {"b", "2"};
        show("before clear", toClear);

        !toClear;
        show("after !toClear", toClear);
        std::cout << "  empty(): " << (toClear.empty() ? "true" : "false") << "\n";


        section("9. toString()");

        Ip2::HashMap ts;
        ts += {"apple", "fruit"};
        ts += {"banana", "fruit"};
        ts += {"carrot", "veg"};
        ts += {"date", "fruit"};
        std::cout << "  " << ts.toString() << "\n";


        section("10. Exception handling");

        try
        {
            Ip2::HashMap dup;
            dup += {"key", "val1"};
            dup += {"key", "val2"};
        }
        catch (const Ip2::HashMapException &e)
        {
            std::cout << "  Caught HashMapException (duplicate): " << e.what() << "\n";
        }

        try
        {
            Ip2::HashMap missing;
            missing -= "ghost";
        }
        catch (const Ip2::HashMapException &e)
        {
            std::cout << "  Caught HashMapException (delete): " << e.what() << "\n";
        }

        try
        {
            Ip2::HashMap missing;
            missing %= {"ghost", "val"};
        }
        catch (const Ip2::HashMapException &e)
        {
            std::cout << "  Caught HashMapException (update): " << e.what() << "\n";
        }

        try
        {
            Ip2::HashMap missing;
            missing.get("ghost");
        }
        catch (const Ip2::HashMapException &e)
        {
            std::cout << "  Caught HashMapException (get): " << e.what() << "\n";
        }

        std::cout << "\nEnd of Demo.\n";
    }
    catch (...)
    {
        std::cerr << "\n[FATAL] Unexpected exception — demo aborted.\n";
        return 1;
    }

    assert(Ip2::HashMap::getObjectCount() == 0);

    return 0;
}
