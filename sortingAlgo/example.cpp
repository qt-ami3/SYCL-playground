#include <sycl/sycl.hpp>
#include <iostream>
#include <random>

using namespace sycl;

int main() {
  queue Q; //  A queue picks a device (defaults to the best available; here the Arc iGPU).

  std::cout << "Running on: "
    << Q.get_device().get_info<info::device::name>() << "\n";

  constexpr int size = 50;

  //  Step 1) Build the unsorted input ON THE HOST.
  //  A sorting algorithm needs data to sort, so we fill a vector with
  //  random integers before we hand it to the device.
  std::vector<int> host(size); //  The host is kind of like the amount of data you choose to work with.

  std::random_device rd;          //  seeds the generator with real entropy
  std::mt19937 gen(rd());         //  Mersenne Twister: fast, good-quality PRNG
  std::uniform_int_distribution<int> dist(0, 999); //  values in [0, 999]

  for (int &x : host)
    x = dist(gen);

  std::cout << "Unsorted: ";
  for (int x : host)
    std::cout << x << " ";
  std::cout << "\n";

  //  Step 2) Wrap the data in a buffer so the device can see it.
  //  Same pattern as before: the buffer owns `host` for its lifetime and
  //  copies results back when it is destroyed at the end of this scope.
  {
    buffer<int> A(host.data(), range<1>(size));

    //********** ODD-EVEN TRANSPOSITION SORT **********//
    //  We run `size` phases. Each phase compares DISJOINT neighbor pairs,
    //  so every compare-and-swap in a phase is independent -> safe in parallel.
    //  Even phases compare pairs starting at 0,2,4...; odd phases at 1,3,5...
    //  Alternating these `size` times is guaranteed to fully sort the array.
    for (int phase = 0; phase < size; phase++) {
      int start = phase % 2;          //  0 = even phase, 1 = odd phase

      Q.submit([&](handler &h) {      //  one kernel launch per phase
        accessor A_acc(A, h);         //  read+write handle to the buffer

        //  Launch one instance per pair. Pairs are (start, start+1),
        //  (start+2, start+3), ... so instance k handles index i = start + 2*k.
        h.parallel_for(range<1>(size / 2), [=](id<1> k) {
          int i = start + 2 * k;      //  left element of this pair

          if (i + 1 < size) {         //  guard the edge
            if (A_acc[i] > A_acc[i + 1]) {
              int tmp      = A_acc[i];
              A_acc[i]     = A_acc[i + 1];
              A_acc[i + 1] = tmp;
            }
          }
        });
      });
    }
  } //  buffer destruction synchronizes and copies results back to `host`.

  std::cout << "Result:   ";
  for (int x : host)
    std::cout << x << " ";
  std::cout << "\n";

  return 0;
}
