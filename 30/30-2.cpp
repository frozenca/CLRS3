#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <complex>
#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <numbers>
#include <numeric>
#include <unordered_set>
#include <utility>
#include <random>
#include <ranges>
#include <thread>

namespace crn = std::chrono;
namespace sr = std::ranges;
namespace sn = std::numbers;

template <typename T>
concept Scalar = std::is_arithmetic_v<T> || std::is_same_v<T, std::complex<float>>
                 || std::is_same_v<T, std::complex<double>> || std::is_same_v<T, std::complex<long double>>;

template <Scalar T>
class MatrixView;

template <Scalar T>
class Matrix {
public:
    std::size_t R;
    std::size_t C;

private:
    std::unique_ptr<T[]> data;

public:
    friend class MatrixView<T>;

    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() { return data.get(); }
    [[nodiscard]] const_iterator begin() const { return data.get(); }
    [[nodiscard]] const_iterator cbegin() const { return data.get(); }
    iterator end() { return data.get() + R * C; }
    [[nodiscard]] const_iterator end() const { return data.get() + R * C; }
    [[nodiscard]] const_iterator cend() const { return data.get() + R * C; }

    Matrix(std::size_t R, std::size_t C) : R {R}, C {C}, data(new T[R * C]) {}

    template <Scalar T2>
    Matrix(std::initializer_list<std::initializer_list<T2>> il);

    Matrix(const Matrix& mat);
    Matrix& operator=(const Matrix& mat);

    Matrix(Matrix&& mat) noexcept = default;
    Matrix& operator=(Matrix&& mat) noexcept = default;

    template <Scalar T2>
    Matrix(const Matrix<T2>& mat);

    template <Scalar T2>
    Matrix& operator=(const Matrix<T2>& mat);

    template <Scalar T2>
    Matrix(const MatrixView<T2>& matview);

    template <Scalar T2>
    Matrix& operator=(const MatrixView<T2>& matview);

    T& operator()(std::size_t r, std::size_t c) {
        assert(r < R && c < C);
        return data.get()[r * C + c];
    }

    const T& operator()(std::size_t r, std::size_t c) const {
        assert(r < R && c < C);
        return data.get()[r * C + c];
    }

    MatrixView<T> submatrix(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) {
        assert(r1 <= r2 && c1 <= c2 && r2 < R && c2 < C);
        std::size_t RV = r2 - r1 + 1;
        std::size_t CV = c2 - c1 + 1;
        std::unique_ptr<std::size_t[]> index(new std::size_t[RV * CV]);
        for (std::size_t r = 0; r < RV; r++) {
            for (std::size_t c = 0; c < CV; c++) {
                index.get()[r * CV + c] = (r1 + r) * C + (c1 + c);
            }
        }
        MatrixView<T> sub(RV, CV, &data.get()[0], std::move(index));
        return sub;
    }

    MatrixView<T> submatrix(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const {
        assert(r1 <= r2 && c1 <= c2 && r2 < R && c2 < C);
        std::size_t RV = r2 - r1 + 1;
        std::size_t CV = c2 - c1 + 1;
        std::unique_ptr<std::size_t[]> index(new std::size_t[RV * CV]);
        for (std::size_t r = 0; r < RV; r++) {
            for (std::size_t c = 0; c < CV; c++) {
                index.get()[r * CV + c] = (r1 + r) * C + (c1 + c);
            }
        }
        MatrixView<T> sub(RV, CV, const_cast<T*>(&data.get()[0]), std::move(index));
        return sub;
    }

    Matrix& operator+=(T val);
    Matrix& operator-=(T val);
    Matrix& operator*=(T val);
    Matrix& operator/=(T val);
    template <Scalar T2>
    Matrix& operator+=(const Matrix<T2>& rhs);
    template <Scalar T2>
    Matrix& operator+=(const MatrixView<T2>& rhs);
    template <Scalar T2>
    Matrix& operator-=(const Matrix<T2>& rhs);
    template <Scalar T2>
    Matrix& operator-=(const MatrixView<T2>& rhs);

    friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& mat) {
        os << '{';
        for (std::size_t r = 0; r < mat.R; r++) {
            os << '{';
            for (std::size_t c = 0; c < mat.C; c++) {
                os << mat.data[r * mat.C + c];
                if (c != mat.C - 1) {
                    os << ", ";
                }
            }
            if (r == mat.R - 1) {
                os << '}';
            } else {
                os << "},\n";
            }
        }
        os << "}\n";
        return os;
    }

public:
    void swap(Matrix<T>& other) {
        std::swap(R, other.R);
        std::swap(C, other.C);
        std::swap(data, other.data);
    }
};

namespace std {
    template <Scalar T>
    void swap(Matrix<T>& lhs, Matrix<T>& rhs) {
        lhs.swap(rhs);
    }
}

template <Scalar T>
class MatrixView {
    T* data_view;
    std::unique_ptr<std::size_t[]> index;

    friend class Matrix<T>;
public:
    const std::size_t R;
    const std::size_t C;

    MatrixView(std::size_t R, std::size_t C,
               T* data_view, std::unique_ptr<std::size_t[]> index)
            : data_view {data_view}, index {std::move(index)}, R {R}, C {C} {
    }

    struct MVIterator {
        T* data_view;
        std::size_t* index;

        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;


        MVIterator(T* data_view, std::size_t* index) : data_view {data_view}, index {index} {}

        reference operator*() const {
            return data_view[*index];
        }

        pointer operator->() const {
            return data_view + (*index);
        }

        MVIterator& operator++() {
            index++;
            return *this;
        }

        MVIterator& operator--() {
            index--;
            return *this;
        }

        MVIterator operator++(int) const {
            MVIterator temp = *this;
            temp.index++;
            return temp;
        }

        MVIterator operator--(int) const {
            MVIterator temp = *this;
            temp.index--;
            return temp;
        }

        MVIterator operator+(difference_type n) const {
            MVIterator temp = *this;
            temp.index += n;
            return temp;
        }

        MVIterator& operator+=(difference_type n) {
            index += n;
            return *this;
        }

        MVIterator operator-(difference_type n) const {
            MVIterator temp = *this;
            temp.index -= n;
            return temp;
        }

        MVIterator& operator-=(difference_type n) {
            index -= n;
            return *this;
        }

        reference operator[](difference_type n) const {
            return data_view[index[n]];
        }

        difference_type operator-(const MVIterator& other) const {
            return index - other.index;
        }

        friend auto operator<=>(const MVIterator& it1, const MVIterator& it2) {
            return it1.index <=> it2.index;
        }
    };

    using iterator = MVIterator;

    iterator begin() {
        return iterator(data_view, const_cast<std::size_t*>(index.get()));
    }

    iterator end() {
        return iterator(data_view, const_cast<std::size_t*>(index.get() + R * C));
    }

    template <Scalar T2>
    MatrixView& operator=(const Matrix<T2>& mat);

    template <Scalar T2>
    MatrixView& operator=(const MatrixView<T2>& matview);

    T& operator()(std::size_t r, std::size_t c) {
        assert(r < R && c < C);
        return data_view[index[r * C + c]];
    }

    const T& operator()(std::size_t r, std::size_t c) const {
        assert(r < R && c < C);
        return data_view[index[r * C + c]];
    }

    friend std::ostream& operator<<(std::ostream& os, const MatrixView<T>& matview) {
        os << '{';
        for (std::size_t r = 0; r < matview.R; r++) {
            os << '{';
            for (std::size_t c = 0; c < matview.C; c++) {
                os << matview.data_view[matview.index[r * matview.C + c]];
                if (c != matview.C - 1) {
                    os << ", ";
                }
            }
            if (r == matview.R - 1) {
                os << '}';
            } else {
                os << "},\n";
            }
        }
        os << "}\n";
        return os;
    }

    MatrixView<T> submatrix(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) {
        assert(r1 <= r2 && c1 <= c2 && r2 < R && c2 < C);
        std::size_t RV = r2 - r1 + 1;
        std::size_t CV = c2 - c1 + 1;
        std::unique_ptr<std::size_t[]> index_(new std::size_t[RV * CV]);
        for (std::size_t r = 0; r < RV; r++) {
            for (std::size_t c = 0; c < CV; c++) {
                index_.get()[r * CV + c] = index.get()[(r1 + r) * C + (c1 + c)];
            }
        }
        MatrixView<T> sub(RV, CV, &data_view[0], std::move(index_));
        return sub;
    }

    MatrixView<T> submatrix(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const {
        assert(r1 <= r2 && c1 <= c2 && r2 < R && c2 < C);
        std::size_t RV = r2 - r1 + 1;
        std::size_t CV = c2 - c1 + 1;
        std::unique_ptr<std::size_t[]> index_(new std::size_t[RV * CV]);
        for (std::size_t r = 0; r < RV; r++) {
            for (std::size_t c = 0; c < CV; c++) {
                index_.get()[r * CV + c] = index.get()[(r1 + r) * C + (c1 + c)];
            }
        }
        MatrixView<T> sub(RV, CV, const_cast<T*>(&data_view[0]), std::move(index_));
        return sub;
    }

    MatrixView& operator+=(T val);
    MatrixView& operator-=(T val);
    MatrixView& operator*=(T val);
    MatrixView& operator/=(T val);
    template <Scalar T2>
    MatrixView& operator+=(const Matrix<T2>& rhs);
    template <Scalar T2>
    MatrixView& operator+=(const MatrixView<T2>& rhs);
    template <Scalar T2>
    MatrixView& operator-=(const Matrix<T2>& rhs);
    template <Scalar T2>
    MatrixView& operator-=(const MatrixView<T2>& rhs);
};

template <Scalar T>
Matrix<T>::Matrix(const Matrix<T>& mat) : R {mat.R}, C {mat.C}, data (new T[R * C]) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] = mat.data.get()[i];
    }
}

template <Scalar T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& mat) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] = mat.data.get()[i];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
Matrix<T>::Matrix(const Matrix<T2>& mat) : R {mat.R}, C {mat.C}, data (new T[R * C]) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] = static_cast<T>(mat.data.get()[i]);
    }
}

template <Scalar T>
template <Scalar T2>
Matrix<T>& Matrix<T>::operator=(const Matrix<T2>& mat) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] = static_cast<T>(mat.data.get()[i]);
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T2>> il) : R(il.size()), C(il.begin()->size()), data(new T[R * C]) {
    assert(il.size() == R && sr::all_of(il, [this](const auto& il_) {return il_.size() == C;}));
    std::size_t index = 0;
    for (auto first_il = il.begin(); first_il != il.end(); ++first_il) {
        for (auto first_ptr = first_il->begin(); first_ptr != first_il->end(); ++first_ptr) {
            data.get()[index++] = *first_ptr;
        }
    }
}

template <Scalar T>
template <Scalar T2>
Matrix<T>::Matrix(const MatrixView<T2>& matview) : R {matview.R}, C {matview.C}, data (new T[R * C]) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] = matview.data_view[matview.index.get()[i]];
    }
}

template <Scalar T>
template <Scalar T2>
Matrix<T>& Matrix<T>::operator=(const MatrixView<T2>& matview) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] = matview.data_view[matview.index.get()[i]];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
MatrixView<T>& MatrixView<T>::operator=(const Matrix<T2>& mat) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] = mat.data.get()[i];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
MatrixView<T>& MatrixView<T>::operator=(const MatrixView<T2>& matview) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] = matview.data_view[matview.index.get()[i]];
    }
    return *this;
}

template <Scalar T>
Matrix<T>& Matrix<T>::operator+=(T val) {
    for (auto& n : data.get()) {
        n += val;
    }
    return *this;
}

template <Scalar T>
MatrixView<T>& MatrixView<T>::operator+=(T val) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] += val;
    }
    return *this;
}

template <Scalar T>
Matrix<T> operator+(const Matrix<T>& m, T val) {
    Matrix<T> res = m;
    res += val;
    return res;
}

template <Scalar T>
Matrix<T> operator+(const MatrixView<T>& m, T val) {
    Matrix<T> res = m;
    res += val;
    return res;
}

template <Scalar T>
Matrix<T>& Matrix<T>::operator-=(T val) {
    for (auto& n : data.get()) {
        n += val;
    }
    return *this;
}

template <Scalar T>
MatrixView<T>& MatrixView<T>::operator-=(T val) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] += val;
    }
    return *this;
}

template <Scalar T>
Matrix<T> operator-(const Matrix<T>& m, T val) {
    Matrix<T> res = m;
    res -= val;
    return res;
}

template <Scalar T>
Matrix<T> operator-(const MatrixView<T>& m, T val) {
    Matrix<T> res = m;
    res -= val;
    return res;
}

template <Scalar T>
Matrix<T>& Matrix<T>::operator*=(T val) {
    for (auto& n : data.get()) {
        n *= val;
    }
    return *this;
}

template <Scalar T>
MatrixView<T>& MatrixView<T>::operator*=(T val) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] *= val;
    }
    return *this;
}

template <Scalar T>
Matrix<T> operator*(const Matrix<T>& m, T val) {
    Matrix<T> res = m;
    res *= val;
    return res;
}

template <Scalar T>
Matrix<T> operator*(const MatrixView<T>& m, T val) {
    Matrix<T> res = m;
    res *= val;
    return res;
}

template <Scalar T>
Matrix<T>& Matrix<T>::operator/=(T val) {
    for (auto& n : data.get()) {
        n /= val;
    }
    return *this;
}

template <Scalar T>
MatrixView<T>& MatrixView<T>::operator/=(T val) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] /= val;
    }
    return *this;
}

template <Scalar T>
Matrix<T> operator/(const Matrix<T>& m, T val) {
    Matrix<T> res = m;
    res /= val;
    return res;
}

template <Scalar T>
Matrix<T> operator/(const MatrixView<T>& m, T val) {
    Matrix<T> res = m;
    res /= val;
    return res;
}

template <Scalar T>
template <Scalar T2>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T2>& rhs) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] += rhs.data.get()[i];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
Matrix<T>& Matrix<T>::operator+=(const MatrixView<T2>& rhs) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] += rhs.data_view[rhs.index.get()[i]];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
MatrixView<T>& MatrixView<T>::operator+=(const Matrix<T2>& rhs) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] += rhs.data.get()[i];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
MatrixView<T>& MatrixView<T>::operator+=(const MatrixView<T2>& rhs) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] += rhs.data_view[rhs.index.get()[i]];
    }
    return *this;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator+(const Matrix<T1>& m1, const Matrix<T2>& m2) {
    Matrix<T3> res = m1;
    res += m2;
    return res;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator+(const Matrix<T1>& m1, const MatrixView<T2>& m2) {
    Matrix<T3> res = m1;
    res += m2;
    return res;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator+(const MatrixView<T1>& m1, const Matrix<T2>& m2) {
    Matrix<T3> res = m1;
    res += m2;
    return res;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator+(const MatrixView<T1>& m1, const MatrixView<T2>& m2) {
    Matrix<T3> res = m1;
    res += m2;
    return res;
}

template <Scalar T>
template <Scalar T2>
Matrix<T>& Matrix<T>::operator-=(const Matrix<T2>& rhs) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] -= rhs.data.get()[i];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
Matrix<T>& Matrix<T>::operator-=(const MatrixView<T2>& rhs) {
    for (std::size_t i = 0; i < R * C; i++) {
        data.get()[i] -= rhs.data_view[rhs.index.get()[i]];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
MatrixView<T>& MatrixView<T>::operator-=(const Matrix<T2>& rhs) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] -= rhs.data.get()[i];
    }
    return *this;
}

template <Scalar T>
template <Scalar T2>
MatrixView<T>& MatrixView<T>::operator-=(const MatrixView<T2>& rhs) {
    for (std::size_t i = 0; i < R * C; i++) {
        data_view[index.get()[i]] -= rhs.data_view[rhs.index.get()[i]];
    }
    return *this;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator-(const Matrix<T1>& m1, const Matrix<T2>& m2) {
    Matrix<T3> res = m1;
    res -= m2;
    return res;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator-(const Matrix<T1>& m1, const MatrixView<T2>& m2) {
    Matrix<T3> res = m1;
    res -= m2;
    return res;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator-(const MatrixView<T1>& m1, const Matrix<T2>& m2) {
    Matrix<T3> res = m1;
    res -= m2;
    return res;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator-(const MatrixView<T1>& m1, const MatrixView<T2>& m2) {
    Matrix<T3> res = m1;
    res -= m2;
    return res;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator*(const Matrix<T1>& m1, const Matrix<T2>& m2) {
    std::size_t M = m1.R;
    std::size_t K = m1.C;
    assert(m2.R == K);
    std::size_t N = m2.C;
    Matrix<T3> m3 (M, N);
    for (std::size_t m = 0; m < M; m++) {
        for (std::size_t n = 0; n < N; n++) {
            m3(m, n) = 0;
            for (std::size_t k = 0; k < K; k++) {
                m3(m, n) += m1(m, k) * m2(k, n);
            }
        }
    }
    return m3;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator*(const MatrixView<T1>& m1, const Matrix<T2>& m2) {
    std::size_t M = m1.R;
    std::size_t K = m1.C;
    assert(m2.R == K);
    std::size_t N = m2.C;
    Matrix<T3> m3 (M, N);
    for (std::size_t m = 0; m < M; m++) {
        for (std::size_t n = 0; n < N; n++) {
            m3(m, n) = 0;
            for (std::size_t k = 0; k < K; k++) {
                m3(m, n) += m1(m, k) * m2(k, n);
            }
        }
    }
    return m3;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator*(const MatrixView<T1>& m1, const MatrixView<T2>& m2) {
    std::size_t M = m1.R;
    std::size_t K = m1.C;
    assert(m2.R == K);
    std::size_t N = m2.C;
    Matrix<T3> m3 (M, N);
    for (std::size_t m = 0; m < M; m++) {
        for (std::size_t n = 0; n < N; n++) {
            m3(m, n) = 0;
            for (std::size_t k = 0; k < K; k++) {
                m3(m, n) += m1(m, k) * m2(k, n);
            }
        }
    }
    return m3;
}

template <Scalar T1, Scalar T2, Scalar T3 = std::common_type_t<T1, T2>>
Matrix<T3> operator*(const Matrix<T1>& m1, const MatrixView<T2>& m2) {
    std::size_t M = m1.R;
    std::size_t K = m1.C;
    assert(m2.R == K);
    std::size_t N = m2.C;
    Matrix<T3> m3 (M, N);
    for (std::size_t m = 0; m < M; m++) {
        for (std::size_t n = 0; n < N; n++) {
            m3(m, n) = 0;
            for (std::size_t k = 0; k < K; k++) {
                m3(m, n) += m1(m, k) * m2(k, n);
            }
        }
    }
    return m3;
}

std::vector<std::complex<double>> RecursiveFFT(const std::vector<double>& A) {
    using namespace std::complex_literals;
    std::size_t n = A.size();
    if (n == 1) {
        return std::vector<std::complex<double>> {A[0]};
    }
    auto w_n = std::exp(2.0i * sn::pi / static_cast<double>(n));
    std::complex<double> w = 1;
    std::vector<double> A0 (n / 2);
    std::vector<double> A1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        A0[i / 2] = A[i];
        A1[i / 2] = A[i + 1];
    }
    auto y0 = RecursiveFFT(A0);
    auto y1 = RecursiveFFT(A1);
    std::vector<std::complex<double>> y (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        y[k] = y0[k] + w * y1[k];
        y[k + n / 2] = y0[k] - w * y1[k];
        w *= w_n;
    }
    return y;
}

std::vector<std::complex<double>> InverseDFT(const std::vector<std::complex<double>>& y) {
    using namespace std::complex_literals;
    std::size_t n = y.size();
    if (n == 1) {
        return std::vector<std::complex<double>> {y[0]};
    }
    auto w_n = std::exp(2.0i * sn::pi / (1.0 * n));
    std::complex<double> w = 1.0;
    std::vector<std::complex<double>> y0 (n / 2);
    std::vector<std::complex<double>> y1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        y0[i / 2] = y[i];
        y1[i / 2] = y[i + 1];
    }
    auto a0 = InverseDFT(y0);
    auto a1 = InverseDFT(y1);
    std::vector<std::complex<double>> A (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        A[k] = a0[k] + w * a1[k];
        A[k + n / 2] = a0[k] - w * a1[k];
        w *= (1.0 / w_n);
    }
    for (std::size_t k = 0; k < n; k++) {
        A[k] /= 2.0;
    }
    return A;
}

std::vector<double> PolynomialProduct(const std::vector<double>& A, const std::vector<double>& B) {
    std::size_t big_size = std::max(A.size(), B.size());
    std::size_t poly_size = (1u << (static_cast<std::size_t>(std::ceil(std::log2(big_size))) + 1));
    std::vector<double> A_poly = A;
    std::vector<double> B_poly = B;
    for (std::size_t k = 0; k < poly_size - A.size(); k++) {
        A_poly.push_back(0);
    }
    for (std::size_t k = 0; k < poly_size - B.size(); k++) {
        B_poly.push_back(0);
    }
    auto A_ = RecursiveFFT(A_poly);
    auto B_ = RecursiveFFT(B_poly);
    std::vector<std::complex<double>> AB_ (poly_size);
    sr::transform(A_, B_, AB_.begin(), std::multiplies{});

    auto AB = InverseDFT(AB_);
    std::vector<double> AB_real (AB.size());
    sr::transform(AB, AB_real.begin(), [](auto c) -> double { return std::real(c);});
    while (!AB_real.empty() && std::abs(AB_real.back()) < 1e-8) {
        AB_real.pop_back();
    }
    return AB_real;
}

template <Scalar T>
std::vector<T> VectorizeToeplitz(const Matrix<T>& M) {
    std::size_t n = M.R;
    assert(n == M.C);
    std::vector<T> v;
    for (std::size_t i = n - 1; i < n; i--) {
        v.push_back(M(i, 0));
    }
    for (std::size_t i = 1; i < n; i++) {
        v.push_back(M(0, i));
    }
    return v;
}

template <Scalar T>
std::vector<T> VectorizeColumn(const Matrix<T>& M, std::size_t c) {
    std::size_t n = M.R;
    std::vector<T> v;
    for (std::size_t i = 0; i < n; i++) {
        v.push_back(M(i, c));
    }
    return v;
}

template <Scalar T>
Matrix<T> ToeplitzMultiply(const Matrix<T>& A, const Matrix<T>& B) {
    auto P = VectorizeToeplitz(A);
    std::size_t n = B.C;
    assert(A.R == n && A.C == n && B.R == n);
    Matrix<T> C (n, n);
    for (std::size_t c = 0; c < n; c++) {
        auto Q = VectorizeColumn(B, c);
        sr::reverse(Q);
        auto prod = PolynomialProduct(P, Q);
        for (std::size_t k = 0; k < n; k++) {
            C(k, c) = prod[2 * n - 2 - k];
        }
    }
    return C;
}

int main() {
    Matrix<double> A {{3, 2, 1}, {4, 3, 2}, {5, 4, 3}};
    Matrix<double> B {{3, 2, 1}, {4, 3, 2}, {5, 4, 3}};
    auto C1 = A * B;
    std::cout << C1 << '\n';
    auto C2 = ToeplitzMultiply(A, B);
    std::cout << C2 << '\n';
}