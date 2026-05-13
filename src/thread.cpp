#include <pybind11/pybind11.h>
#include <thread>
#include <memory>

namespace py = pybind11;

// We put the Python objects in a separate struct
struct Payload {
    py::function func;
    py::args args;
    Payload(py::function f, py::args a) : func(f), args(a) {}
};

class Thread {
public:
    Thread(py::function func, py::args args) {
        // Move the payload to the heap
        auto payload = std::make_unique<Payload>(func, args);

        // Capture the raw pointer (or a shared_ptr) so we can manually delete it
        Payload* raw_ptr = payload.release();

        t = std::thread([raw_ptr]() {
            // 1. Acquire GIL
            py::gil_scoped_acquire acquire;
            
            try {
                // 2. Run the function
                raw_ptr->func(*(raw_ptr->args));
            } catch (py::error_already_set &e) {
                py::print("Python Error:", e.what());
            }

            // 3. THE FIX: Manually delete the payload while the GIL is held.
            // This destroys 'func' and 'args' safely.
            delete raw_ptr;

            // 4. The GIL is released when 'acquire' goes out of scope.
            // The lambda now has NO captured Python objects left to clean up.
        });
    }

    void join() {
        py::gil_scoped_release release;
        if (t.joinable()) t.join();
    }

    ~Thread() {
        if (t.joinable()) t.detach();
    }

private:
    std::thread t;
};

PYBIND11_MODULE(thread, m) {
    py::class_<Thread>(m, "Thread")
        .def(py::init<py::function, py::args>())
        .def("join", &Thread::join);
}