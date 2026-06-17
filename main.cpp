#include <sycl/sycl.hpp>
#include <iostream>
#include <random>

using namespace sycl;

int main() {
  queue Q; //  queue Q; searches for best hardware to use.

  std::cout << "Running on: "
    << Q.get_device().get_info<info::device::name>() << "\n";

  constexpr int size = 10;
  std::vector<int> host(size, 0); //  Vector with 10 elements of 0
  
  {
    buffer<int> A(host.data(), range<1>(size)); //  The buffer ie. The inside of the two brackets, gains control
                                                //  of th program and its data until destroyed.
    Q.submit([&](handler &h) {
      accessor A_acc(A, h, write_only); //  The accessor, A_acc, is the object that
                                        //  efficiently accesses the buffer elements.
      h.single_task([=]() {
        for (int i = 0; i < 9; i++)
          A_acc[i] = i;
      });
    });
  }

  for (int i = 0; i < size; i++)
    std::cout << host[i] << " ";
  
  std::cout << "\n";

  return 0;
}
