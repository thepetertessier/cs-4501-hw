#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <bit>

#define DEBUG false

void debug(const std::string& msg) {
    if (DEBUG) {
        std::cout << msg << std::endl;
    }
}

class OneIndexedVector {
private:
    std::vector<size_t> data;
    
public:
    // Constructors
    OneIndexedVector(size_t size) : data(size + 1, 0) {}  // +1 for 1-based indexing
    OneIndexedVector(const std::vector<size_t>& vec) : data(vec.size() + 1, 0) {
        for (size_t i = 0; i < vec.size(); ++i) {
            data[i + 1] = vec[i];  // Fill data starting from index 1
        }
    }

    // Access operator with 1-based indexing
    size_t& operator[](size_t index) {
        if (index < 1 || index >= data.size()) {
            throw std::out_of_range("Index out of range (must be 1 to size)");
        }
        return data[index];
    }

    // Const overload for read-only access
    const size_t& operator[](size_t index) const {
        if (index < 1 || index >= data.size()) {
            throw std::out_of_range("Index out of range (must be 1 to size)");
        }
        return data[index];
    }

    // Get the size of the array (excluding the extra 0th element)
    size_t size() const {
        return data.size() - 1;
    }
};

    
class Fenwick {
private:
    size_t capacity;
    OneIndexedVector t_vector;
    OneIndexedVector a_vector;

    size_t reverse(size_t index) {
        return index + (index & -index);
    }

    void update(size_t index, int delta, bool first = true) {
        debug("Now updating index " + std::to_string(index) + " with delta " + std::to_string(delta));
        if (index < 1 || index > t_vector.size()) return;
        if (first) a_vector[index] += delta;
        t_vector[index] += delta;
        update(reverse(index), delta, false);
    }

    size_t _find_deletion_index(size_t threshold) {
        size_t sum_before_idx = 0;
        size_t idx = 1;
        for (size_t mask = 1UL << (std::bit_width(t_vector.size()) - 1); mask > 0; mask >>= 1) {
            size_t i = mask + idx - 1;
            size_t sum_at_i = (i <= t_vector.size()) ? t_vector[i] + sum_before_idx : std::numeric_limits<size_t>::max();
            if (sum_at_i < threshold) {
                idx = i + 1;
                sum_before_idx = sum_at_i;
            }
        }
        return idx;
    }

public:
    // Constructor
    Fenwick(const std::vector<size_t>& servers, size_t cap)
        : capacity(cap), 
          t_vector(servers.size()),
          a_vector(servers) {
        for (size_t i = 1; i <= servers.size(); ++i) {
            t_vector[i] += servers[i - 1];
            debug("t[" + std::to_string(i) + "] = " + std::to_string(t_vector[i] - servers[i - 1]) + " + " + std::to_string(servers[i - 1]) + " = " + std::to_string(t_vector[i]));
            size_t j = reverse(i);
            if (j <= t_vector.size()) {
                t_vector[j] += t_vector[i];
                debug("t[" + std::to_string(j) + "] = " + std::to_string(t_vector[j] - t_vector[i]) + " + " + std::to_string(t_vector[i]) + " = " + std::to_string(t_vector[j]));
            }
        }
    }

    void store_data(size_t size, size_t index) {
        size_t to_store = size;
        for (size_t i = index; i < std::min(index + 5, t_vector.size() + 1); ++i) {
            if (to_store <= 0) return;
            size_t stored_at_i = a_vector[i];
            if (to_store + stored_at_i > capacity) {
                size_t to_store_here = capacity - stored_at_i;
                update(i, to_store_here);
                to_store -= to_store_here;
            } else {
                update(i, to_store);
                return;
            }
        }
    }

    void delete_data(size_t size, size_t threshold) {
        size_t index = _find_deletion_index(threshold);
        size_t stored_at_index = a_vector[index];
        size_t to_delete = std::min(stored_at_index, size);
        update(index, -to_delete);
    }

    size_t sum(size_t index) {
        if (index < 1) return 0;
        if (index > t_vector.size()) {
            throw std::out_of_range("Tried to get sum from 1 to " + std::to_string(index) + ", which is higher than the max (" + std::to_string(t_vector.size()) + ")");
        }
        return t_vector[index] + sum(index & (index - 1));
    }

    size_t query_data(int left, int right) {
        if (right - left + 1 < __builtin_popcount(right) + __builtin_popcount(left - 1)) {
            size_t result = 0;
            for (int i = left; i <= right; ++i) {
                result += a_vector[i];
            }
            return result;
        } else {
            return sum(right) - sum(left - 1);
        }
    }
};

void main_logic() {
    size_t server_count, capacity, command_count;
    std::cin >> server_count >> capacity >> command_count;
    std::vector<size_t> servers(server_count);
    for (size_t& s : servers) std::cin >> s;
    Fenwick fenwick(servers, capacity);
    for (size_t i = 0; i < command_count; ++i) {
        char command;
        size_t x, y;
        std::cin >> command >> x >> y;
        switch (command) {
        case 'F':
            fenwick.store_data(x, y + 1);
            break;
        case 'D':
            fenwick.delete_data(x, y);
            break;
        case 'Q':
            std::cout << "(" << x << "," << y << "): " << fenwick.query_data(x+1, y+1) << std::endl;
            break;
        default:
            std::cerr << "Command not recognized: " << command << std::endl;
        }
    }
}

int main() {
    main_logic();
    return 0;
}
