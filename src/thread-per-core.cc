#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

int main(int argc, const char** argv) {
    unsigned num_cores = std::thread::hardware_concurrency();
    num_cores /= 2;
    num_cores -= 1;
    std::cout << "launching " << num_cores << " worker threads\n";

    std::mutex iomutex;
    std::vector<std::thread> threads(num_cores);
    for (unsigned i = 0; i < num_cores; ++i) {
        threads[i] = std::thread([&iomutex, i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            {
                std::lock_guard<std::mutex> iolock(iomutex);
                std::cout << "Thread #" << i << ": on CPU " << sched_getcpu() << "\n";
            }

            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        });
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);
        int rc = pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpu_set_t), &cpuset);

        sched_param sparams;
        sparams.sched_priority = 10;
        rc = pthread_setschedparam(threads[i].native_handle(), SCHED_FIFO, &sparams);
    }

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
