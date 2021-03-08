#include <algorithm>
#include <cassert>
#include <complex>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <initializer_list>
#include <list>
#include <memory>
#include <random>
#include <ranges>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace sr = std::ranges;

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

template <Scalar T>
using SlackForm = std::tuple<std::unordered_set<std::size_t>, std::unordered_set<std::size_t>,
        Matrix<T>, Matrix<T>, Matrix<T>, T>;

template <Scalar T>
SlackForm<T> Pivot(const std::unordered_set<std::size_t>& N, const std::unordered_set<std::size_t>& B,
                   const Matrix<T>& A, const Matrix<T>& b, const Matrix<T>& c, T& v,
                   std::size_t l, std::size_t e) {
    std::size_t sz = A.R;
    assert(A.C == sz && l < sz && e < sz);
    Matrix<T> A_hat (sz, sz);
    Matrix<T> b_hat (sz, 1);
    Matrix<T> c_hat (sz, 1);
    sr::fill(A_hat, T{0});
    sr::fill(b_hat, T{0});
    sr::fill(c_hat, T{0});
    b_hat(e, 0) = b(l, 0) / A(l, e);
    for (auto j : N) {
        if (j == e) continue;
        A_hat(e, j) = A(l, j) / A(l, e);
    }
    A_hat(e, l) = T{1} / A(l, e);

    for (auto i : B) {
        if (i == l) continue;
        b_hat(i, 0) = b(i, 0) - A(i, e) * b_hat(e, 0);
        for (auto j : N) {
            if (j == e) continue;
            A_hat(i, j) = A(i, j) - A(i, e) * A_hat(e, j);
        }
        A_hat(i, l) = -A(i, e) * A_hat(e, l);
    }

    T v_hat = v + c(e, 0) * b_hat(e, 0);
    for (auto j : N) {
        if (j == e) continue;
        c_hat(j, 0) = c(j, 0) - c(e, 0) * A_hat(e, j);
    }
    c_hat(l, 0) = -c(e, 0) * A_hat(e, l);

    auto N_hat = N;
    N_hat.erase(e);
    N_hat.insert(l);
    auto B_hat = B;
    B_hat.erase(l);
    B_hat.insert(e);
    return {N_hat, B_hat, std::move(A_hat), std::move(b_hat), std::move(c_hat), v_hat};
}

template <Scalar T>
Matrix<T> SimplexCore(std::unordered_set<std::size_t>& N, std::unordered_set<std::size_t>& B,
                      Matrix<T>& A, Matrix<T>& b, Matrix<T>& c, T& v) {
    std::size_t n = c.R;
    while (sr::any_of(N, [&c](auto j){return c(j, 0) > T{0};})) {
        auto e = *sr::find_if(N, [&c](auto j){return c(j, 0) > T{0};});
        std::size_t l = -1;
        T min_delta = std::numeric_limits<T>::max();
        for (auto i : B) {
            if (A(i, e) > T{0}) {
                if (min_delta > b(i, 0) / A(i, e)) {
                    min_delta = b(i, 0) / A(i, e);
                    l = i;
                }
            }
        }
        if (min_delta == std::numeric_limits<T>::max()) {
            throw std::runtime_error("Unbounded");
        } else {
            std::tie(N, B, A, b, c, v) = Pivot(N, B, A, b, c, v, l, e);
        }
    }
    Matrix<T> x_bar (n, 1);
    for (std::size_t i = 0; i < n; i++) {
        if (B.contains(i)) {
            x_bar(i, 0) = b(i, 0);
        } else {
            x_bar(i, 0) = T{0};
        }
    }
    return x_bar;
}

template <Scalar T>
SlackForm<T> InitializeSimplex(Matrix<T>& A, Matrix<T>& b, Matrix<T>& c) {
    std::size_t m = A.R;
    std::size_t n = A.C;
    assert(m == b.R && n == c.R);
    std::size_t k = std::distance(b.begin(), sr::min_element(b));
    if (b(k, 0) >= T{0}) {
        std::unordered_set<std::size_t> N;
        for (std::size_t i = 0; i < n; i++) {
            N.insert(i);
        }
        std::unordered_set<std::size_t> B;
        for (std::size_t i = n; i < n + m; i++) {
            B.insert(i);
        }
        // standard form to slack form
        Matrix<T> A_ (m + n, m + n);
        for (std::size_t i = n; i < n + m; i++) {
            for (std::size_t j = 0; j < n; j++) {
                A_(i, j) = A(i - n, j);
            }
        }
        std::swap(A, A_);

        Matrix<T> b_ (m + n, 1);
        for (std::size_t i = n; i < n + m; i++) {
            b_(i, 0) = b(i - n, 0);
        }
        std::swap(b, b_);

        Matrix<T> c_ (m + n, 1);
        for (std::size_t i = 0; i < n; i++) {
            c_(i, 0) = c(i, 0);
        }
        std::swap(c, c_);

        return {N, B, std::move(A), std::move(b), std::move(c), 0};
    }
    std::unordered_set<std::size_t> N_prime;
    for (std::size_t i = 0; i <= n; i++) {
        N_prime.insert(i);
    }
    std::unordered_set<std::size_t> B_prime;
    for (std::size_t i = n + 1; i <= n + m; i++) {
        B_prime.insert(i);
    }
    Matrix<T> A_prime (m + n + 1, m + n + 1);
    for (std::size_t i = n + 1; i <= n + m; i++) {
        A_prime(i, 0) = T{-1};
        for (std::size_t j = 0; j < n; j++) {
            A_prime(i, j + 1) = A(i - n - 1, j);
        }
    }
    Matrix<T> b_prime (m + n + 1, 1);
    for (std::size_t i = n + 1; i <= n + m; i++) {
        b_prime(i, 0) = b(i - n - 1, 0);
    }
    Matrix<T> c_prime (m + n + 1, 1);
    c_prime(0, 0) = T{-1};
    for (std::size_t i = 1; i <= m + n; i++) {
        c_prime(i, 0) = T{0};
    }
    T v_prime = T{0};
    std::size_t l_prev = n + 1 + k;
    std::tie(N_prime, B_prime, A_prime, b_prime, c_prime, v_prime)
            = Pivot(N_prime, B_prime, A_prime, b_prime, c_prime, v_prime, l_prev, 0);
    auto x_bar = SimplexCore(N_prime, B_prime, A_prime, b_prime, c_prime, v_prime);
    if (x_bar(0, 0) == T{0}) {
        if (B_prime.contains(0)) {
            std::tie(N_prime, B_prime, A_prime, b_prime, c_prime, v_prime)
                    = Pivot(N_prime, B_prime, A_prime, b_prime, c_prime, v_prime, 0, *N_prime.begin());
        }

        std::unordered_set<std::size_t> N;
        for (auto j : N_prime) {
            if (j) {
                N.insert(j - 1);
            }
        }
        std::unordered_set<std::size_t> B;
        for (auto i : B_prime) {
            B.insert(i - 1);
        }

        // standard form to slack form
        Matrix<T> A_ (m + n, m + n);
        for (std::size_t i = 0; i < m + n; i++) {
            for (std::size_t j = 0; j < m + n; j++) {
                A_(i, j) = A_prime(i + 1, j + 1);
            }
        }
        std::swap(A, A_);

        Matrix<T> b_ (m + n, 1);
        for (std::size_t i = 0; i < n + m; i++) {
            b_(i, 0) = b_prime(i + 1, 0);
        }
        std::swap(b, b_);

        Matrix<T> c_ (m + n, 1);
        for (std::size_t i = 0; i < n; i++) {
            c_(i, 0) = c(i, 0);
        }
        std::swap(c, c_);

        T v = T{0};
        for (auto i : B) {
            for (std::size_t j = 0; j < m + n; j++) {
                c(j, 0) -= c(i, 0) * A(i, j);
            }
            v += c(i, 0) * b(i, 0);

            c(i, 0) = T{0};
        }
        return {N, B, std::move(A), std::move(b), std::move(c), v};
    } else {
        throw std::runtime_error("Infeasible");
    }
}

template <Scalar T>
std::pair<Matrix<T>, T> Simplex(Matrix<T>& A, Matrix<T>& b, Matrix<T>& c) {
    std::size_t n = A.C;
    std::unordered_set<std::size_t> N;
    std::unordered_set<std::size_t> B;
    T v = T{0};
    std::tie(N, B, A, b, c, v) = InitializeSimplex(A, b, c);
    auto x_bar = SimplexCore(N, B, A, b, c, v);
    Matrix<T> x (n, 1);
    for (std::size_t i = 0; i < n; i++) {
        x(i, 0) = x_bar(i, 0);
    }
    return {std::move(x), v};
}

std::mt19937 gen(std::random_device{}());

class BipartiteGraph final {
    const std::size_t n;
    std::vector<std::list<std::size_t>> adj;
    std::unordered_map<std::size_t, std::list<std::size_t>::iterator> edges;
    std::unordered_set<std::size_t> L;
    std::unordered_set<std::size_t> R;

public:
    BipartiteGraph(std::size_t n, std::initializer_list<std::size_t> L_) : n {n}, adj(n), L (L_) {
        assert(sr::all_of(L, [](auto num){return num < n;}) && L.size() < n);
        for (std::size_t u = 0; u < n; u++) {
            if (!L.contains(u)) {
                R.insert(u);
            }
        }
    }

    BipartiteGraph(std::size_t n, const std::vector<std::size_t>& L_) : n {n}, adj(n), L (L_.begin(), L_.end()) {
        assert(sr::all_of(L, [](auto num){return num < n;}) && L.size() < n);
        for (std::size_t u = 0; u < n; u++) {
            if (!L.contains(u)) {
                R.insert(u);
            }
        }
    }

    BipartiteGraph(const BipartiteGraph& g) : n {g.n}, adj(n), L(g.L), R(g.R) {
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : g.adj[u]) {
                addEdge(u, v);
            }
        }
    }

    BipartiteGraph(BipartiteGraph&& g) noexcept = default;

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        assert(L.contains(src) && R.contains(dst));
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst);
            edges[src * n + dst] = adj[src].begin();
        }
    }

    [[nodiscard]] std::vector<std::size_t> getIndependentSet() {
        std::uniform_real_distribution<> dist (0.0, 1.0);

        std::size_t E = 0;
        for (const auto& adj_list : adj) {
            E += adj_list.size();
        }
        Matrix<double> A (E + n, n);
        Matrix<double> b (E + n, 1);
        Matrix<double> c (n, 1);

        // edges
        std::size_t e = 0;
        for (std::size_t l = 0; l < n; l++) {
            for (auto r : adj[l]) {
                A(e, l) = 1;
                A(e, r) = 1;
                e++;
                b(e, 0) = 1;
            }
        }
        assert(e == E);
        // variables
        for (std::size_t i = 0; i < n; i++) {
            A(E + i, i) = 1;
            b(E + i, 0) = 1;
            c(i, 0) = 1;
        }

        auto [x, v] = Simplex(A, b, c);
        std::vector<double> values;
        values.reserve(n);
        for (std::size_t i = 0; i < n; i++) {
            values.push_back(x(i, 0));
        }

        auto t = dist(gen);
        std::vector<std::size_t> ind_set;
        for (auto l : L) {
            if (values[l] >= t) {
                ind_set.push_back(l);
            }
        }
        for (auto r : R) {
            if (values[r] > 1.0 - t) {
                ind_set.push_back(r);
            }
        }
        return ind_set;
    }

};

int main() {
    BipartiteGraph g (8, {0, 1, 2, 3, 4});
    auto id_s = g.getIndependentSet();
    for (auto id : id_s) {
        std::cout << id << ' ';
    }
    std::cout << '\n';
    g.addEdge(0, 5);
    g.addEdge(1, 6);
    g.addEdge(2, 7);
    id_s = g.getIndependentSet();
    for (auto id : id_s) {
        std::cout << id << ' ';
    }
    std::cout << '\n';
    g.addEdge(0, 5);
    g.addEdge(0, 6);
    g.addEdge(0, 7);
    g.addEdge(1, 5);
    g.addEdge(1, 6);
    g.addEdge(1, 7);
    g.addEdge(2, 5);
    g.addEdge(2, 6);
    g.addEdge(2, 7);
    g.addEdge(3, 5);
    g.addEdge(3, 6);
    g.addEdge(3, 7);
    g.addEdge(4, 5);
    g.addEdge(4, 6);
    g.addEdge(4, 7);
    id_s = g.getIndependentSet();
    for (auto id : id_s) {
        std::cout << id << ' ';
    }
    std::cout << '\n';

}
