#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <thread>
#include <future>
#include <condition_variable>
#include <mutex>


//for test only
#include <set>

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

std::string findSameHash(const uint32_t nb_trial, const uint32_t lookuphash, const uint32_t str_size, bool flag_run = true)
{
    std::string ret;

    size_t attempt = (size_t)nb_trial + 1u;
    while((--attempt) && flag_run) {
        auto str  = random_string(str_size);
        auto hash = GetHashCode(str);
        if ( lookuphash == hash ) {
            ret = str;
            break;
        }
    }
    return ret;
}

#define ATTEMPT (100000000)



int main()
{
    const size_t awaited_collision_str_nb = 3;
    std::string str{"Hello World" };
    //previously found collision for "Hello World" string
    std::vector<std::string> vec_str{"rea7YDSN2qcJ7jwWXwQ2","NdHUNr4ctw2LXIHjHw8U","5khkqv7pHsM8aqZOL5Xm" };

    const auto LookUpHash = GetHashCode(str);

    std::cout << str  << ": " << std::hex << "0x" <<  LookUpHash << "\n";
    for (auto & str_item :vec_str)
        std::cout << str_item << ": " << std::hex << "0x" <<  GetHashCode(str_item) << "\n";

    auto processor_count = std::thread::hardware_concurrency();
    if ( !  processor_count ) processor_count = 1u;
    auto nb_task = (processor_count == 1u) ? 1u : processor_count - 1u;

    std::cout << "Number of // thread(s) used: " << std::dec << nb_task << "\n" << std::flush;

    bool flag_run = true;
    auto lmbd = [&]() { return findSameHash(ATTEMPT,LookUpHash,20,flag_run);};

    auto timepoint = std::chrono::high_resolution_clock::now();

    std::vector<std::string> results;
    std::vector<std::future<std::string>> fut_vec(nb_task);
    for ( auto & item : fut_vec)
        item = std::async(std::launch::async,lmbd);

    int i = 0;
    std::condition_variable cv;
    std::mutex cv_m;

    while(awaited_collision_str_nb > results.size()) {

        timepoint += std::chrono::milliseconds(1000u);

        std::unique_lock<std::mutex> lk(cv_m);
        while( std::cv_status::timeout  != cv.wait_until(lk,timepoint )){}

        for ( auto & item : fut_vec) {
            if ( std::future_status::ready == item.wait_for(std::chrono::milliseconds(0))) {
                std::string  str = item.get();
                if (!str.empty()) {
                    std::cout << "\n" << str << "\n" << std::flush;
                    results.push_back(str);
                }

                item = std::async(std::launch::async,lmbd);
            }
        }

        std::cout << ++i << "\r" << std::flush;

    }

    //Print results
    std::cout << "Results: \n";
    for ( const auto & item : results)
        std::cout << item << "\n";

    std::cout << std::flush;

    flag_run = false;

    //We won't wait explicitely for the futures results
    //it will be handled in their xtors

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

   typedef struct {
       std::mutex cv_m;
       std::condition_variable cv;
   }st_context_t;

    st_context_t context { std::mutex(), std::condition_variable()};
#endif
