# stringhash
Some string hashing, we try to find collision of hash between (different) strings. We will use multithreading to maximize the work performed by the processor.

This piece of code is normally devoted to C#, however the chosen language is here C++ and there is no "built-in" (imho, afaik) hashing function for C++ object.
So I looked up for a C# hashing function code on Internet (Link given in the source code)

The base ( reference ) string is "Hello World"

Generation of random string code was found on Stack Overflow ( link given in the source code). 
It is said to be a good random generation with different "seeds" for each thread.

I chose totally be default to generate random strings of size 20 ( maybe I could have go down to 13 or 15 to benefit from SSO ). Dealing with std::string is this case
may also not be optimum and char* could perform better.
Characters are chosen among 2x26+10 = 62, leading to 62^20 = 7E35 or 2^119 possible strings which may be too much.


First mono-thread attempt under Linux (I3-10015G, 2C/4T) gave a little bit mere than 1 Million tests per second (Mtests/s or MT/s).
Firts collision was by chance found after 308 seconds
rea7YDSN2qcJ7jwWXwQ2
and then no more...

Coliru gave 2,2 MT/s but time calculation is  (fortunaltely) limited ...

Mac mini M1 (4 HP,4 HE) averaged 4.6 MTests/s (only 1 process) and 2.9 MT/s for each process (8 processes (100% CPU))

2nd (NdHUNr4ctw2LXIHjHw8U) and 3rd (5khkqv7pHsM8aqZOL5Xm) collisions were found.

Multithreaded version was then elaborated with std::future / std::async. 
We check every 1 second the the launched tasks finished or found a hash, thus there exists a small chance that a thread find a collision right after start.
This would lead to a potential idle core during 1 s.

All stops after 3 found collisions (bare numbers 11, 73 ... are seconds from start when collision was found )

Execution example on Mac mini M1

```
16:05:16: Starting /Users/nils/Code/build-stringhash-Qt_6_2_4_for_macOS-Release/stringhash...
Hello World: 0xc56327ba
rea7YDSN2qcJ7jwWXwQ2: 0xc56327ba
NdHUNr4ctw2LXIHjHw8U: 0xc56327ba
5khkqv7pHsM8aqZOL5Xm: 0xc56327ba
Number of // thread used: 7
11
sTTFaViJKU6oyHIXzG1P
73
nnb1vhOVZeeetAKGssjl
268
HPG92uTEzJF1uC6k1akS
Results:
sTTFaViJKU6oyHIXzG1P
nnb1vhOVZeeetAKGssjl
HPG92uTEzJF1uC6k1akS
16:10:10: /Users/nils/Code/build-stringhash-Qt_6_2_4_for_macOS-Release/stringhash exited with code 0
```
