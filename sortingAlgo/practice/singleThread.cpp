#include <sycl/sycl.hpp>
#include <iostream>
#include <vector>
#include <random>

using namespace sycl;

int main() {
  queue Q; 

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
  
  {
    // We change the accessor permissions to read_write (or read_only) 
    // so the buffer actually tracks the random numbers we just generated on the host.
    buffer<int> A(host.data(), range<1>(size)); 
                                                
    Q.submit([&](handler &h) {
      accessor A_acc(A, h, read_write); 
      
      h.single_task([=]() {
        int bin;

        while (true) {
        bool sorted = true;
          for (int i = 0; i < (A_acc.size() - 1); i++) {
            if (A_acc[i] > A_acc[i + 1]) {
              int bin = A_acc[i];
              A_acc[i] = A_acc[i + 1];
              A_acc[i + 1] = bin;
              sorted = false;
            }
          }

          if (sorted) {break;}
        }
      });
    });
  } // Buffer scope ends here, synchronization happens, and data copies back to 'host'

  std::cout << "Unsorted vector: ";
  for (int i = 0; i < size; i++)
    std::cout << host[i] << " ";
  
  std::cout << std::endl << "size of vector: " << host.size() << std::endl;
  std::cout << "\n";

  return 0;
}
