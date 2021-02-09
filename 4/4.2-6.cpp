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
#include <numeric>
#include <utility>
#include <random>
#include <ranges>
#include <thread>

namespace crn = std::chrono;
namespace sr = std::ranges;

template <typename T>
concept Scalar = std::is_arithmetic_v<T> || std::is_same_v<T, std::complex<float>>
                 || std::is_same_v<T, std::complex<double>> || std::is_same_v<T, std::complex<long double>>;

template <Scalar T>
class MatrixView;

template <Scalar T>
class Matrix {
public:
    const std::size_t R;
    const std::size_t C;

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
};

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
    for (size_t m = 0; m < M; m++) {
        for (size_t n = 0; n < N; n++) {
            m3(m, n) = 0;
            for (size_t k = 0; k < K; k++) {
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
    for (size_t m = 0; m < M; m++) {
        for (size_t n = 0; n < N; n++) {
            m3(m, n) = 0;
            for (size_t k = 0; k < K; k++) {
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
    for (size_t m = 0; m < M; m++) {
        for (size_t n = 0; n < N; n++) {
            m3(m, n) = 0;
            for (size_t k = 0; k < K; k++) {
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
    for (size_t m = 0; m < M; m++) {
        for (size_t n = 0; n < N; n++) {
            m3(m, n) = 0;
            for (size_t k = 0; k < K; k++) {
                m3(m, n) += m1(m, k) * m2(k, n);
            }
        }
    }
    return m3;
}

template <Scalar T>
Matrix<T> Strassen(const Matrix<T>& A, const Matrix<T>& B) {
    std::size_t N = A.R;
    assert(A.C == N && B.R == N && B.C == N && (N & (N - 1)) == 0);
    if (N == 1) {
        return A * B;
    }
    Matrix<T> C (N, N);
    std::size_t H = N / 2;
    auto A11 = A.submatrix(0, 0, H - 1, H - 1);
    auto A12 = A.submatrix(0, H, H - 1, N - 1);
    auto A21 = A.submatrix(H, 0, N - 1, H - 1);
    auto A22 = A.submatrix(H, H, N - 1, N - 1);
    auto B11 = B.submatrix(0, 0, H - 1, H - 1);
    auto B12 = B.submatrix(0, H, H - 1, N - 1);
    auto B21 = B.submatrix(H, 0, N - 1, H - 1);
    auto B22 = B.submatrix(H, H, N - 1, N - 1);
    auto C11 = C.submatrix(0, 0, H - 1, H - 1);
    auto C12 = C.submatrix(0, H, H - 1, N - 1);
    auto C21 = C.submatrix(H, 0, N - 1, H - 1);
    auto C22 = C.submatrix(H, H, N - 1, N - 1);
    auto S1 = B12 - B22;
    auto S2 = A11 + A12;
    auto S3 = A21 + A22;
    auto S4 = B21 - B11;
    auto S5 = A11 + A22;
    auto S6 = B11 + B22;
    auto S7 = A12 - A22;
    auto S8 = B21 + B22;
    auto S9 = A11 - A21;
    auto S10 = B11 + B12;
    auto P1 = Strassen(A11, S1);
    auto P2 = Strassen(S2, B22);
    auto P3 = Strassen(S3, B11);
    auto P4 = Strassen(A22, S4);
    auto P5 = Strassen(S5, S6);
    auto P6 = Strassen(S7, S8);
    auto P7 = Strassen(S9, S10);
    C11 = P5 + P4 - P2 + P6;
    C12 = P1 + P2;
    C21 = P3 + P4;
    C22 = P5 + P1 - P3 - P7;
    return C;
}

template <Scalar T>
Matrix<T> Strassen(const MatrixView<T>& A, const Matrix<T>& B) {
    std::size_t N = A.R;
    assert(A.C == N && B.R == N && B.C == N && (N & (N - 1)) == 0);
    if (N == 1) {
        return A * B;
    }
    Matrix<T> C (N, N);
    std::size_t H = N / 2;
    auto A11 = A.submatrix(0, 0, H - 1, H - 1);
    auto A12 = A.submatrix(0, H, H - 1, N - 1);
    auto A21 = A.submatrix(H, 0, N - 1, H - 1);
    auto A22 = A.submatrix(H, H, N - 1, N - 1);
    auto B11 = B.submatrix(0, 0, H - 1, H - 1);
    auto B12 = B.submatrix(0, H, H - 1, N - 1);
    auto B21 = B.submatrix(H, 0, N - 1, H - 1);
    auto B22 = B.submatrix(H, H, N - 1, N - 1);
    auto C11 = C.submatrix(0, 0, H - 1, H - 1);
    auto C12 = C.submatrix(0, H, H - 1, N - 1);
    auto C21 = C.submatrix(H, 0, N - 1, H - 1);
    auto C22 = C.submatrix(H, H, N - 1, N - 1);
    auto S1 = B12 - B22;
    auto S2 = A11 + A12;
    auto S3 = A21 + A22;
    auto S4 = B21 - B11;
    auto S5 = A11 + A22;
    auto S6 = B11 + B22;
    auto S7 = A12 - A22;
    auto S8 = B21 + B22;
    auto S9 = A11 - A21;
    auto S10 = B11 + B12;
    auto P1 = Strassen(A11, S1);
    auto P2 = Strassen(S2, B22);
    auto P3 = Strassen(S3, B11);
    auto P4 = Strassen(A22, S4);
    auto P5 = Strassen(S5, S6);
    auto P6 = Strassen(S7, S8);
    auto P7 = Strassen(S9, S10);
    C11 = P5 + P4 - P2 + P6;
    C12 = P1 + P2;
    C21 = P3 + P4;
    C22 = P5 + P1 - P3 - P7;
    return C;
}

template <Scalar T>
Matrix<T> Strassen(const MatrixView<T>& A, const MatrixView<T>& B) {
    std::size_t N = A.R;
    assert(A.C == N && B.R == N && B.C == N && (N & (N - 1)) == 0);
    if (N == 1) {
        return A * B;
    }
    Matrix<T> C (N, N);
    std::size_t H = N / 2;
    auto A11 = A.submatrix(0, 0, H - 1, H - 1);
    auto A12 = A.submatrix(0, H, H - 1, N - 1);
    auto A21 = A.submatrix(H, 0, N - 1, H - 1);
    auto A22 = A.submatrix(H, H, N - 1, N - 1);
    auto B11 = B.submatrix(0, 0, H - 1, H - 1);
    auto B12 = B.submatrix(0, H, H - 1, N - 1);
    auto B21 = B.submatrix(H, 0, N - 1, H - 1);
    auto B22 = B.submatrix(H, H, N - 1, N - 1);
    auto C11 = C.submatrix(0, 0, H - 1, H - 1);
    auto C12 = C.submatrix(0, H, H - 1, N - 1);
    auto C21 = C.submatrix(H, 0, N - 1, H - 1);
    auto C22 = C.submatrix(H, H, N - 1, N - 1);
    auto S1 = B12 - B22;
    auto S2 = A11 + A12;
    auto S3 = A21 + A22;
    auto S4 = B21 - B11;
    auto S5 = A11 + A22;
    auto S6 = B11 + B22;
    auto S7 = A12 - A22;
    auto S8 = B21 + B22;
    auto S9 = A11 - A21;
    auto S10 = B11 + B12;
    auto P1 = Strassen(A11, S1);
    auto P2 = Strassen(S2, B22);
    auto P3 = Strassen(S3, B11);
    auto P4 = Strassen(A22, S4);
    auto P5 = Strassen(S5, S6);
    auto P6 = Strassen(S7, S8);
    auto P7 = Strassen(S9, S10);
    C11 = P5 + P4 - P2 + P6;
    C12 = P1 + P2;
    C21 = P3 + P4;
    C22 = P5 + P1 - P3 - P7;
    return C;
}

template <Scalar T>
Matrix<T> Strassen(const Matrix<T>& A, const MatrixView<T>& B) {
    std::size_t N = A.R;
    assert(A.C == N && B.R == N && B.C == N && (N & (N - 1)) == 0);
    if (N == 1) {
        return A * B;
    }
    Matrix<T> C (N, N);
    std::size_t H = N / 2;
    auto A11 = A.submatrix(0, 0, H - 1, H - 1);
    auto A12 = A.submatrix(0, H, H - 1, N - 1);
    auto A21 = A.submatrix(H, 0, N - 1, H - 1);
    auto A22 = A.submatrix(H, H, N - 1, N - 1);
    auto B11 = B.submatrix(0, 0, H - 1, H - 1);
    auto B12 = B.submatrix(0, H, H - 1, N - 1);
    auto B21 = B.submatrix(H, 0, N - 1, H - 1);
    auto B22 = B.submatrix(H, H, N - 1, N - 1);
    auto C11 = C.submatrix(0, 0, H - 1, H - 1);
    auto C12 = C.submatrix(0, H, H - 1, N - 1);
    auto C21 = C.submatrix(H, 0, N - 1, H - 1);
    auto C22 = C.submatrix(H, H, N - 1, N - 1);
    auto S1 = B12 - B22;
    auto S2 = A11 + A12;
    auto S3 = A21 + A22;
    auto S4 = B21 - B11;
    auto S5 = A11 + A22;
    auto S6 = B11 + B22;
    auto S7 = A12 - A22;
    auto S8 = B21 + B22;
    auto S9 = A11 - A21;
    auto S10 = B11 + B12;
    auto P1 = Strassen(A11, S1);
    auto P2 = Strassen(S2, B22);
    auto P3 = Strassen(S3, B11);
    auto P4 = Strassen(A22, S4);
    auto P5 = Strassen(S5, S6);
    auto P6 = Strassen(S7, S8);
    auto P7 = Strassen(S9, S10);
    C11 = P5 + P4 - P2 + P6;
    C12 = P1 + P2;
    C21 = P3 + P4;
    C22 = P5 + P1 - P3 - P7;
    return C;
}

template <Scalar T>
Matrix<T> FastMultiply1(Matrix<T>& A, Matrix<T>& B) {
    std::size_t KN = A.R;
    std::size_t N = A.C;
    assert(N > 0 && B.R == N && B.C == KN && KN % N == 0);
    std::size_t K = KN / N;
    Matrix<T> C (KN, KN);
    for (size_t r = 0; r < K; r++) {
        for (size_t c = 0; c < K; c++) {
            auto Crc = C.submatrix(r * N, c * N, (r + 1) * N - 1, (c + 1) * N - 1);
            auto Ar = A.submatrix(r * N, 0, (r + 1) * N - 1, N - 1);
            auto Bc = B.submatrix(0, c * N, N - 1, (c + 1) * N - 1);
            Crc = Strassen(Ar, Bc);
        }
    }
    return C;
}

template <Scalar T>
Matrix<T> FastMultiply2(Matrix<T>& A, Matrix<T>& B) {
    std::size_t KN = A.C;
    std::size_t N = A.R;
    assert(N > 0 && B.R == KN && B.C == N && KN % N == 0);
    std::size_t K = KN / N;
    Matrix<T> C (N, N);
    for (size_t k = 0; k < K; k++) {
        auto Ak = A.submatrix(0, k * N, N - 1, (k + 1) * N - 1);
        auto Bk = B.submatrix(k * N, 0, (k + 1) * N - 1, N - 1);
        C += Strassen(Ak, Bk);
    }
    return C;
}

int main() {
    constexpr size_t N = 1u << 3u;
    Matrix<int> m1 (N, N);
    Matrix<int> m2 (N, N);
    std::iota(m1.begin(), m1.end(), 0);
    std::iota(m2.begin(), m2.end(), 0);
    auto m3 = m1 * m2;
    std::cout << m3 << '\n';
    auto m4 = m2 * m1;
    std::cout << m4 << '\n';
    auto m5 = FastMultiply1(m1, m2);
    std::cout << m5 << '\n';
    auto m6 = FastMultiply2(m2, m1);
    std::cout << m6 << '\n';

}