#include <sycl/sycl.hpp>
#include <iostream>
#include <random>
#include <vector>

using namespace sycl;

int main() {
  queue Q; //  queue Q; searches for best hardware to use.

  std::cout << "Running on: "
            << Q.get_device().get_info<info::device::name>() << "\n";

  constexpr int size = 10;
  std::vector<int> host(size, 0); 

  // --- Initialize vector with random numbers ---
  std::random_device rd;  // Seed
  std::mt19937 gen(rd()); // Random number generator
  std::uniform_int_distribution<int> distrib(1, 100); // Numbers between 1 and 100

  for (int i = 0; i < size; i++) {
    host[i] = distrib(gen);
  }

  bool unsorted = true;
  while (unsorted) {
    
    unsorted = false;
    {
      buffer<int> A(host.data(), range<1>(size)); //  inside of the two brackets, gains control
                                                  //  of th program and its data until destroyed.
      Q.submit([&](handler &h) {
        accessor A_acc(A, h, write_only); //  The accessor, A_acc, is the object that
                                          //  efficiently accesses the buffer elements.
        h.parallel_for(range<1>(host.size() - 1), [=](id<1> i) { // is the same as main.cpp's for loop.
          
          if (A_acc[i] > A_acc[i + 1]) {
            int bin = A_acc[i];
            A_acc[i] = A_acc[i + 1];
            A_acc[i + 1] = bin;
          }
        });
      });
    }
    
    for (int i = 0; i < (host.size() - 1); i++) {
      if (host[i] > host[i + 1]) {
        unsorted = true;
        break;
      }
    }
  }

  for (int i = 0; i < (host.size() - 1); i++)
    std::cout << host[i] << " ";
  
  std::cout << "\n";

  return 0;
}
