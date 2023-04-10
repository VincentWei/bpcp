#include <cstddef>
#include <vector>
#include <iostream>

using namespace std;

size_t EratosthenesSieve(vector<unsigned> &primes, unsigned maxn)
{
    vector<unsigned> num;

    for (unsigned i = 0; i <= maxn; i++)
        num.push_back(i);
    num[1] = 0;

    for (unsigned i = 2; i <= maxn; i++) {
        if (!num[i])
            continue;

        primes.push_back(i);

        for (unsigned j = i; j < maxn && i * j <= maxn; j++)
            num[i*j] = 0;
    }

    return primes.size();
}

int main()
{
    vector<unsigned> primes;

    EratosthenesSieve(primes, 10000);

    for (unsigned j = 0; j < primes.size(); j++)
        cout << primes[j] << " ";

    cout << endl;

    return 0;
}
