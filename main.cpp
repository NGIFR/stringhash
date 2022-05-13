#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <thread>
#include <future>

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

std::string findSameHash(const uint32_t nb_trial, const uint32_t lookuphash, const uint32_t str_size)
{
    std::string ret{""};

    size_t attempt = (size_t)nb_trial + 1u;
    while(--attempt) {
        auto str  = random_string(str_size);
        auto hash = GetHashCode(str);
        if ( lookuphash == hash ) {
            ret = str;
            break;
        }

    }

    return ret;
}

#define ATTEMPT (1000000000)

int main()
{
    std::string str{"Hello World" };
    std::vector<std::string> vec_str{"rea7YDSN2qcJ7jwWXwQ2","NdHUNr4ctw2LXIHjHw8U","5khkqv7pHsM8aqZOL5Xm" };

    const auto LookUpHash = GetHashCode(str);

    std::cout << str  << ": " << std::hex << "0x" <<  LookUpHash << "\n";
    for (auto & str_item :vec_str)
        std::cout << str_item << ": " << std::hex << "0x" <<  GetHashCode(str_item) << "\n";

    auto processor_count = std::thread::hardware_concurrency();
    if ( !  processor_count ) processor_count = 1;

    auto lmbd = [=]() { return findSameHash(ATTEMPT,LookUpHash,20);};

    std::vector<std::future<std::string>> fut_vec;

    return 0;
}

#if 0
std::cout << str << "\n";
std::cout << std::hex << "0x" <<  hash << "\n";
std::cout << "remaining attempts: " << std::dec << attempt << "\n";

   auto start = std::chrono::high_resolution_clock::now();

   auto stop = std::chrono::high_resolution_clock::now();

   std::chrono::duration<double>  diff = stop - start;




   std::cout << std::dec;
   std::cout << "Duration: " << diff.count() << "\n";
   std::cout << "HPS: " << ATTEMPT/diff.count() << "\n";
   std::cout << std::flush;
#endif
