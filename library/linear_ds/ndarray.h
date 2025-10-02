#include <iostream>
#include <vector>
#include <numeric>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <sstream> // For std::stringstream
#include <iomanip> // For std::setw

// Forward declarations
template <typename T>
class NDArray;

template <typename T>
std::ostream &operator<<(std::ostream &os, const NDArray<T> &arr);

/**
 * @brief A multi-dimensional array class like NumPy's ndarray.
 * @tparam T The type of elements to be stored in the array.
 */
template <typename T>
class NDArray
{
private:
    std::vector<size_t> shape_;
    std::vector<size_t> strides_;
    std::vector<T> data_;

    void calculate_strides()
    {
        strides_.resize(shape_.size());
        if (strides_.empty())
            return;

        strides_.back() = 1;
        for (int i = shape_.size() - 2; i >= 0; --i)
        {
            strides_[i] = strides_[i + 1] * shape_[i + 1];
        }
    }

    size_t get_total_size() const
    {
        if (shape_.empty())
            return 0;
        return std::accumulate(shape_.begin(), shape_.end(), 1, std::multiplies<size_t>());
    }

    template <typename T_idx, typename... T_indices>
    size_t get_flat_index_recursive(size_t dim, T_idx idx, T_indices... rest) const
    {
        if (idx >= shape_[dim])
        {
            throw std::out_of_range("Index is out of bounds for dimension " + std::to_string(dim));
        }

        if constexpr (sizeof...(rest) > 0)
        {
            return idx * strides_[dim] + get_flat_index_recursive(dim + 1, rest...);
        }
        else
        {
            return idx * strides_[dim];
        }
    }

    size_t get_max_element_width() const
    {
        if (data_.empty())
        {
            return 0;
        }
        size_t max_width = 0;
        for (const auto &val : data_)
        {
            std::stringstream ss;
            ss << val;
            max_width = std::max(max_width, ss.str().length());
        }
        return max_width;
    }

public:
    // ================== CONSTRUCTORS ==================
    NDArray() = default;

    explicit NDArray(const std::vector<size_t> &shape) : shape_(shape)
    {
        data_.resize(get_total_size());
        calculate_strides();
    }

    NDArray(const std::vector<size_t> &shape, const T &value) : shape_(shape)
    {
        data_.assign(get_total_size(), value);
        calculate_strides();
    }

    // ================== ACCESSORS ==================
    const std::vector<size_t> &shape() const { return shape_; }
    size_t ndim() const { return shape_.size(); }
    size_t size() const { return data_.size(); }

    // ================== ELEMENT ACCESS ==================
    T &operator()(const std::vector<size_t> &indices)
    {
        if (indices.size() != ndim())
        {
            throw std::out_of_range("Incorrect number of indices provided.");
        }
        size_t index = std::inner_product(indices.begin(), indices.end(), strides_.begin(), 0);
        return data_[index];
    }

    const T &operator()(const std::vector<size_t> &indices) const
    {
        if (indices.size() != ndim())
        {
            throw std::out_of_range("Incorrect number of indices provided.");
        }
        size_t index = std::inner_product(indices.begin(), indices.end(), strides_.begin(), 0);
        return data_[index];
    }

    template <typename... Args>
    T &operator()(Args... indices)
    {
        static_assert(std::conjunction_v<std::is_integral<Args>...>, "Indices must be integral types.");
        if (sizeof...(indices) != ndim())
        {
            throw std::out_of_range("Incorrect number of indices provided.");
        }
        return data_[get_flat_index_recursive(0, indices...)];
    }

    template <typename... Args>
    const T &operator()(Args... indices) const
    {
        static_assert(std::conjunction_v<std::is_integral<Args>...>, "Indices must be integral types.");
        if (sizeof...(indices) != ndim())
        {
            throw std::out_of_range("Incorrect number of indices provided.");
        }
        return data_[get_flat_index_recursive(0, indices...)];
    }

    // ================== MANIPULATION ==================
    void reshape(const std::vector<size_t> &new_shape)
    {
        NDArray temp(new_shape);
        if (temp.size() != this->size())
        {
            throw std::runtime_error("Cannot reshape: new shape must have the same total number of elements.");
        }
        shape_ = new_shape;
        calculate_strides();
    }

    void fill(const T &value)
    {
        std::fill(data_.begin(), data_.end(), value);
    }

    friend std::ostream &operator<< <>(std::ostream &os, const NDArray<T> &arr);
};

// ================== PRETTY PRINTING (FINAL VERSION) ==================
namespace detail
{
    template <typename T>
    void print_recursive(std::ostream &os, const NDArray<T> &arr, std::vector<size_t> &indices, size_t dim, size_t element_width)
    {
        if (dim == arr.ndim())
        {
            os << std::setw(element_width) << arr(indices);
            return;
        }

        os << "[";
        for (size_t i = 0; i < arr.shape()[dim]; ++i)
        {
            indices[dim] = i;
            print_recursive(os, arr, indices, dim + 1, element_width);

            if (i < arr.shape()[dim] - 1)
            {
                if (dim == arr.ndim() - 1)
                {
                    os << ", ";
                }
                else
                {
                    os << ",\n"
                       << std::string(dim + 1, ' ');
                }
            }
        }
        os << "]";
    }
} // namespace detail

template <typename T>
std::ostream &operator<<(std::ostream &os, const NDArray<T> &arr)
{
    if (arr.size() == 0)
    {
        os << "[]";
        return os;
    }
    size_t width = arr.get_max_element_width();
    std::vector<size_t> indices(arr.ndim(), 0);
    detail::print_recursive(os, arr, indices, 0, width);
    return os;
}

/*
// ================== MAIN FUNCTION (EXAMPLE USAGE) ==================
int main()
{
    std::cout << "## 2D Array (3x4) with new syntax ##" << std::endl;
    NDArray<int> a({3, 4}, 5);

    a(0, 1) = 100;
    a(2, 3) = 200;

    std::cout << "Modified array a:\n"
              << a << std::endl;
    std::cout << "Value at a(0, 1) is: " << a(0, 1) << std::endl;

    std::cout << "\n## 3D Array (2x3x4) with new syntax ##" << std::endl;
    NDArray<double> b({2, 3, 4});
    double counter = 0.0;
    for (size_t i = 0; i < b.shape()[0]; ++i)
    {
        for (size_t j = 0; j < b.shape()[1]; ++j)
        {
            for (size_t k = 0; k < b.shape()[2]; ++k)
            {
                b(i, j, k) = counter++;
            }
        }
    }
    std::cout << "3D array b:\n"
              << b << std::endl;

    return 0;
}
*/