#include <pybind11/pybind11.h>
#include <thread>
#include <memory>

namespace py = pybind11;

// This struct holds the function and the packed arguments
struct Payload {
    py::function func;
    py::args args;
    Payload(py::function f, py::args a) : func(f), args(a) {}
};

class Thread {
public:
    Thread(py::function func, py::args args) {
        // Create the payload on the heap
        auto* payload = new Payload(func, args);

        t = std::thread([payload]() {
            // Acquire GIL for the execution phase
            py::gil_scoped_acquire acquire;
            try {
                // Unpack the args tuple using '*'
                payload->func(*(payload->args));
            } catch (py::error_already_set &e) {
                py::print("C++ Thread Error:", e.what());
            }

            // Cleanup while the GIL is held
            delete payload;
        });
    }

    void join() {
        // Release GIL so the background thread can acquire it to finish
        py::gil_scoped_release release;
        if (t.joinable()) {
            t.join();
        }
    }

    ~Thread() {
        if (t.joinable()) {
            t.detach();
        }
    }

private:
    std::thread t;
};

PYBIND11_MODULE(fth, m) {
    py::class_<Thread>(m, "Thread")
        // Use py::args to capture any number of positional arguments
        .def(py::init<py::function, py::args>())
        .def("join", &Thread::join);
}