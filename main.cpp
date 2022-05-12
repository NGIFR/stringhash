#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <vector>

std::string random_string(std::string::size_type length)
{
    //from https://stackoverflow.com/a/24586587/3972710
    static auto& chrs = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{std::random_device{}()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;

    s.reserve(length);

    while(length--)
        s += chrs[pick(rg)];

    return s;
}
uint32_t GetHashCode( std::string &str)
{
    //https://github.com/dotnet/corefx/blob/a10890f4ffe0fadf090c922578ba0e606ebdd16c/src/Common/src/System/Text/StringOrCharArray.cs
    uint32_t hash1 = (5381 << 16) + 5381;
    uint32_t hash2 = hash1;
    auto count = str.size();
    auto s = str.data();
    for (int i = 0; i < count; ++i)
    {
        int c = *s++;
        hash1 = ((hash1 << 5) + hash1) ^ c;

        if (++i >= count)
            break;

        c = *s++;
        hash2 = ((hash2 << 5) + hash2) ^ c;
    }

    return (hash1 + (hash2 * 1566083941));
}
#define ATTEMPT (uint32_t(-1) - 1u)

int main()
{
    std::string str{"Hello World" };
    std::vector<std::string> vec_str{"rea7YDSN2qcJ7jwWXwQ2" };

    const auto LookUpHash = GetHashCode(str);

    std::cout << str  << ": " << std::hex << "0x" <<  LookUpHash << "\n";
    for (auto & str_item :vec_str)
        std::cout << str_item << ": " << std::hex << "0x" <<  GetHashCode(str_item) << "\n";

    auto start = std::chrono::high_resolution_clock::now();
    size_t attempt = ATTEMPT + 1u;
    while(--attempt) {
        auto str  = random_string(20);
        auto hash = GetHashCode(str);
        if ( LookUpHash == hash ) {
            std::cout << str << "\n";
            std::cout << std::hex << "0x" <<  hash << "\n";
            std::cout << "remaining attempts: " << std::dec << attempt << "\n";
            break;
        }

    }

    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double>  diff = stop - start;

    std::cout << std::dec;
    std::cout << "Duration: " << diff.count() << "\n";
    std::cout << "HPS: " << ATTEMPT/diff.count() << "\n";
    std::cout << std::flush;
    return 0;
}
