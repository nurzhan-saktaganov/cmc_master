#include <algorithm>

using namespace std;

template<typename T>
void decompose(T *a, int *domains, const int lowest_domain, const int k, const int n)
{
    if (k == 1) {
        for (int i = 0; i < n; ++i) {
            domains[i] = lowest_domain;
        }
        return;
    }

    std::sort(T, T + size);

    const int k1 = (k + 1) / 2;
    const int k2 = k - k1;
    const int n1 = 1.0 * n * k1 / k;
    const int n2 = n - n1;

    decompose(a, domains, lowest_domain, k1, n1);
    decompose(a + n1, domains + n1, lowest_domain + k1, k2, n2);
}


int main(int argc, char *argv[])
{

    return 0;
}
