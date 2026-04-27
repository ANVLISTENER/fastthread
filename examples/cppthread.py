#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <thread>
#include <vector>
#include <numeric>

namespace py = pybind11;

struct Payload {
    py::function callback;
    long iterations;
    Payload(py::function f, long n) : callback(f), iterations(n) {}
};

class Thread {
public:
    Thread(py::function callback, long iterations) {
        // Create the payload on the heap
        Payload* data = new Payload(callback, iterations);

        t = std::thread([data]() {
            // --- STAGE 1: PURE C++ (No GIL) ---
            // This runs at 100% CPU speed on a separate core.
            std::vector<long> results;
            for (long i = 0; i < data->iterations; ++i) {
                results.push_back(i * i);
            }
            long final_sum = std::accumulate(results.begin(), results.end(), 0L);

            // --- STAGE 2: BACK TO PYTHON (Acquire GIL) ---
            {
                py::gil_scoped_acquire acquire;
                try {
                    // Send the final result back to the Python callback
                    data->callback(final_sum);
                } catch (py::error_already_set &e) {
                    py::print("Python Error:", e.what());
                }
                
                // Clean up Python handles while we have the GIL
                delete data;
            }
        });
    }

    void join() {
        py::gil_scoped_release release;
        if (t.joinable()) t.join();
    }

private:
    std::thread t;
};

PYBIND11_MODULE(fth, m) {
    py::class_<Thread>(m, "Thread")
        .def(py::init<py::function, long>())
        .def("join", &Thread::join);
}