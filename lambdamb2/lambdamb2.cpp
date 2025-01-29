#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <pthread.h>
#include <assert.h>
#include <vector>
#include <mutex>
#include <x86intrin.h>
#include <functional>

using u8 = uint8_t; 
using u32 = uint32_t; 
using u64 = uint64_t; 

// -------------------------------------------------------------------------------------
// Template for std::function_ref
template <typename T>
class function_ref;

template <typename Ret, typename... Args>
class function_ref<Ret(Args...)> {
    void* obj = nullptr;
    Ret (*callback)(void*, Args...) = nullptr;

public:
    // Constructor accepting callable objects
    template <typename F>
    function_ref(F&& f) noexcept
        : obj(reinterpret_cast<void*>(std::addressof(f))),
          callback([](void* obj, Args... args) -> Ret {
              return (*reinterpret_cast<F*>(obj))(std::forward<Args>(args)...);
          }) {}

    // Callable operator
    Ret operator()(Args... args) const {
        return callback(obj, std::forward<Args>(args)...);
    }
};

struct TrackingAllocator {
    static inline bool allocation_happened = false;

    static void* operator new(std::size_t size) {
        allocation_happened = true;
        return std::malloc(size);
    }

    static void operator delete(void* ptr) noexcept {
        std::free(ptr);
    }
};

template<uint32_t functor_size>
struct SizedFunctor : public TrackingAllocator {
    char data[functor_size]; 
    int operator()() const {
        return 5;
    }
};


// Example class for CountersClass
struct WorkerCounters {
    std::atomic<u64> counter;
    WorkerCounters() : counter(0) {}
    
    void increment() { counter++; }
};

// Using thread_local storage for each thread's counters
std::mutex mut; 
std::vector<WorkerCounters*> counters; 
thread_local WorkerCounters workerCounters __attribute__ ((tls_model ("local-exec")));

template <class CountersClass, class CounterType, typename T = uint64_t>
T sum(std::vector<CountersClass*>& counters, CounterType CountersClass::*c)
{
   T local_c = 0;
   for (auto& counterInstance : counters) {
      local_c += (counterInstance->*c).exchange(0);  // exchange and reset
   }
   return local_c;
}

// 1. Function to set the CPU affinity of a thread
void set_cpu_affinity(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    
    // Set the CPU affinity for the current thread
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

// 2. Function to read an atomic value every second and reset it
void read_and_reset_atomic_value() {
    set_cpu_affinity(0);  // Reader thread will run on CPU 0

    while (true) {
        std::cout << "Atomic Value: " << sum(counters, &WorkerCounters::counter) / (double) 1e6 << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Wait for 1 second
    }
}

// 3. Function to increment an atomic value in a loop
void increment_atomic_value(int cpuid) {
    set_cpu_affinity(cpuid);  // Reader thread will run on CPU 0
    mut.lock(); 
    counters.push_back(&workerCounters); 
    mut.unlock(); 
    u64 sum = 0; 

    std::function<int()> small_functor = SizedFunctor<32>{};
    
    while (true) {
        auto volatile a = small_functor(); 
        workerCounters.increment();  // Increment the atomic value
    }

    printf("tsc sum: %lu\n", sum);
}


int main() {
    // 4. Create two threads: one for reading and resetting, one for incrementing
    std::thread reader_thread(read_and_reset_atomic_value);

    std::vector<std::thread> incrementer{}; 

    for (uint8_t i = 0; i < 1; i++) {
        incrementer.emplace_back(increment_atomic_value, i + 1); 
    }

    // Set CPU affinity for both threads (for example, assigning them to CPU 0)
    for (auto& t : incrementer) {
        t.detach();; 
    }
    reader_thread.detach();  // Detach reader thread

    // Let the threads run for some time (simulate the microbenchmark)
    std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;
}
