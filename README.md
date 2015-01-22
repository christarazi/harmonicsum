# harmonicsum
Multithreaded C++ application to calculate the partial harmonic sum to N terms.

Has the option to use Kahan sum for greater accuracy.

<b>Compile and run with: </b>

`g++ -std=c++11 -pthread -Ofast harmonic_sum.cpp -o harmonic.out ; ./harmonic.out`
