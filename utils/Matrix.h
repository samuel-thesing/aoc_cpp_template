#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <type_traits>
#include <iostream>
#include <limits>
#include <cassert>

#define MAT_TEMPLATE template<typename U> requires supports_arithmetics<U, T> && std::is_convertible_v<U, T>

template<typename T>
concept is_quad = requires (int M, int N) {
	M = N;
};
template<typename T, typename U>
concept supports_arithmetics = requires(T t, U u) {
	{ t + u } -> std::convertible_to<T>;
	{ t - u } -> std::convertible_to<T>;
	{ t * u } -> std::convertible_to<T>;
	{ t / u } -> std::convertible_to<T>;
};

template<int M, int N, typename T>
class Matrix {
public:
	typedef Matrix<M, 1, T> ColType;
	typedef Matrix<1, N, T> RowType;
	typedef Matrix<N, M, T> TransposeType;
	typedef Matrix<M, N, T> Type;

private:
	T data[M * N];

public:
	Matrix() : data{static_cast<T>(0)} {}

	template<typename... Ts> requires ((std::is_convertible_v<Ts, T> && ...) && (sizeof...(Ts) == M * N))
	explicit Matrix(Ts... elements)
		: data{(T)elements...} {}

	explicit Matrix(Type const& m) : data{} {
		std::memcpy(data, m.data, sizeof(T) * M * N);
	}

	template<typename U> requires std::is_convertible_v<U, T>
	Matrix(const Matrix<M, N, U>& m) : data{} {
		for (int i = 0; i < M * N; ++i) {
			data[i] = static_cast<T>(m.get(i));
		}
	}

public:
	template<typename U> requires std::is_convertible_v<U, T>
	inline void set(int i, U value) {
		data[i] = static_cast<T>(value);
	}

	template<typename U> requires std::is_convertible_v<U, T>
	inline void set(int i, int j, U value) {
		data[i * N + j] = static_cast<T>(value);
	}

	inline T get(int i) const {
		return data[i];
	}

	inline T get(int i, int j) const {
		return data[i * N + j];
	}

	T x() const { return data[0]; }
	T y() const { return data[1]; }
	T z() const { return data[2]; }
	T w() const { return data[3]; }

	T& x() { return data[0]; }
	T& y() { return data[1]; }
	T& z() { return data[2]; }
	T& w() { return data[3]; }

	const T* getValuePtr() const { return &data[0]; }

	bool operator==(const Type& m) const {
		for (int i = 0; i < M * N; ++i) {
			if (get(i) != m.get(i)) {
				return false;
			}
		}
		return true;
	}

	Type& operator=(Type const& m) {
		std::memcpy(this->data, m.data, M*N*sizeof(T));
		return *this;
	}

	MAT_TEMPLATE
	Type& operator+=(U const& s) {
		for (int i = 0; i < M * N; ++i) {
			data[i] += s;
		}
		return *this;
	}

	MAT_TEMPLATE
	Type& operator+=(Matrix<M, N, U> const& m) {
		for (int i = 0; i < M * N; ++i) {
			data[i] += m.get(i);
		}
		return *this;
	}

	MAT_TEMPLATE
	Type& operator-=(U const& s) {
		for (int i = 0; i < M * N; ++i) {
			data[i] -= s;
		}
		return *this;
	}

	MAT_TEMPLATE
	Type& operator-=(Matrix<M, N, U> const& m) {
		for (int i = 0; i < M * N; ++i) {
			data[i] -= m.get(i);
		}
		return *this;
	}

	MAT_TEMPLATE
	Type& operator*=(U const& s) {
		for (int i = 0; i < M * N; ++i) {
			data[i] *= s;
		}
		return *this;
	}

	MAT_TEMPLATE
	Type& operator*=(Matrix<M, N, U> const& m) {
		for (int i = 0; i < M * N; ++i) {
			data[i] *= m.get(i);
		}
		return *this;
	}

	MAT_TEMPLATE
	Type& operator/=(U const& s) {
		for (int i = 0; i < M * N; ++i) {
			data[i] /= s;
		}
		return *this;
	}

	MAT_TEMPLATE
	Type& operator/=(Matrix<M, N, U> const& m) {
		for (int i = 0; i < M * N; ++i) {
			data[i] /= m.get(i);
		}
		return *this;
	}

	Type operator-() const {
		auto result = Type();
		for (int i = 0; i < M * N; ++i) {
			result.set(i, -get(i));
		}
		return result;
	}

	MAT_TEMPLATE
	Type operator +(Matrix<M, N, U> const& m) const {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, get(i) + m.get(i));
		}
		return result;
	}

	MAT_TEMPLATE
	friend Type operator +(Type const& m, U const& s) {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, m.get(i) + s);
		}
		return result;
	}

	MAT_TEMPLATE
	friend Type operator +(U const& s, Type const& m) {
		return m + s;
	}

	MAT_TEMPLATE
	Type operator -(Matrix<M, N, U> const& m) const {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, get(i) - m.get(i));
		}
		return result;
	}

	MAT_TEMPLATE
	friend Type operator -(Type const& m, U const& s) {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, m.get(i) - s);
		}
		return result;
	}

	MAT_TEMPLATE
	friend Type operator -(U const& s, Type const& m) {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, s - m.get(i));
		}
		return result;
	}

	MAT_TEMPLATE
	friend Type operator *(Type const& m, U const& s) {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, m.get(i) * s);
		}
		return result;
	}

	MAT_TEMPLATE
	friend Type operator *(U const& s, Type const& m) {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, s * m.get(i));
		}
		return result;
	}

	template<int L, typename U> requires (std::is_convertible_v<U, T>)
	Matrix<M, L, T> operator *(Matrix<N, L, U> const& m) const {
		auto result = Matrix<M, L, T>();
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < L; j++) {
				T sum = (T)0;
				for (int k = 0; k < N; k++) {
					sum += (T)(this->get(i, k) * m.get(k, j));
				}
				result.set(i, j, sum);
			}
		}
		return result;
	}

	MAT_TEMPLATE
	friend Type operator /(Type const& m, U const& s) {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, m.get(i) / s);
		}
		return result;
	}

	MAT_TEMPLATE
	friend Type operator /(U const& s, Type const& m) {
		auto result = Type();
		for (int i = 0; i < M*N; i++) {
			result.set(i, s / m.get(i));
		}
		return result;
	}

	Type transposed() const {
		auto result = Type();
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				result.set(i, j, get(j, i));
			}
		}
	}

	T norm() const requires (N == 1) {
		float sum = 0.f;
		for (int i = 0; i < M; i++) {
			sum += get(i) * get(i);
		}
		return std::sqrt(sum);
	}

	float dot(const Matrix<M, 1, T>& m) const requires (N == 1) {
		float sum = 0.f;
		for (int i = 0; i < M; i++) {
			sum += get(i) * m.get(i);
		}
		return sum;
	}

	template<typename U> requires (std::is_convertible_v<U, T> && M == 3 && N == 1)
	Type cross(Matrix<3, 1, U> const& m) const {
		return Matrix<3, 1, T>(
			get(1) * m.get(2) - get(2) * m.get(1),
			get(2) * m.get(0) - get(0) * m.get(2),
			get(0) * m.get(1) - get(1) * m.get(0)
		);
	}

	T det() const requires (M == 1 && N == 1) {
		return get(0);
	}

	T det() const requires (M == 2 && N == 2) {
		return get(0) * get(3) - get(1) * get(2);
	}

	T det() const requires (M == 3 && N == 3) {
		return get(0) * get(4) * get(8) + get(1) * get(5) * get(6) + get(2) * get(3) * get(7)
			- (get(2) * get(4) * get(6) + get(1) * get(3) * get(8) + get(0) * get(5) * get(7));
	}

	T det() const requires (M == N && M > 3) {
		auto cut = cutRow(0);
		bool negative = false;
		T sum = (T)0;
		for (int i = 0; i < N; i++) {
			sum += (1 - 2 * negative) * cut.cutCol(i).det() * get(i);
			negative = !negative;
		}
		return sum;
	}

	Type normalized() const requires (N == 1) {
		return *this / norm();
	}

	/**
	 * Counter clockwise normal for 2d
	 */
	Type normal2d() const requires (M == 2 && N == 1) {
		return Type(-get(1), get(0));
	}

	static Type identity() requires (M == N) {
		auto result = Type();
		for (int i = 0; i < M; i++) {
			result.set(i, i, static_cast<T>(1));
		}
		return result;
	}

	static Matrix<4, 4, T> lookAt(Matrix<3, 1, T> const& camera_pos, Matrix<3, 1, T> const& camera_dir, Matrix<3, 1, T> const& up) {
		Matrix<3, 1, T> const f((camera_dir - camera_pos).normalized());
		Matrix<3, 1, T> const s(f.cross(up).normalized());
		Matrix<3, 1, T> const u(s.cross(f));

		auto result = Matrix<4, 4, T>::identity();
		result.set(0, 0, s.get(0));
		result.set(1, 0, s.get(1));
		result.set(2, 0, s.get(2));
		result.set(0, 1, u.get(0));
		result.set(1, 1, u.get(1));
		result.set(2, 1, u.get(2));
		result.set(0, 2, -f.get(0));
		result.set(1, 2, -f.get(1));
		result.set(2, 2, -f.get(2));
		result.set(3, 0, -s.dot(camera_pos));
		result.set(3, 1, -u.dot(camera_pos));
		result.set(3, 2, f.dot(camera_pos));

		return result;
	}

	static Matrix<4, 4, T> ortho(T left, T right, T bottom, T top) {
		Matrix<4, 4, T> result = Matrix<4, 4, T>::identity();
		result.set(0, 0, static_cast<T>(2) / (right - left));
		result.set(1, 1, static_cast<T>(2) / (top - bottom));
		result.set(2, 2, -static_cast<T>(1));
		result.set(0, 3, -(right + left) / (right - left));
		result.set(1, 3, -(top + bottom) / (top - bottom));
		return result;
	}

	static Matrix<4, 4, T> ortho(T left, T right, T bottom, T top, T zNear, T zFar) {
		Matrix<4, 4, T> result = Matrix<4, 4, T>::identity();
		result.set(0, 0, (static_cast<T>(2) * zNear) / (right - left));
		result.set(1, 1, (static_cast<T>(2) * zNear) / (top - bottom));
		result.set(2, 0, (right + left) / (right - left));
		result.set(2, 1, (top + bottom) / (top - bottom));
		result.set(2, 2, -(zFar + zNear) / (zFar - zNear));
		result.set(2, 3, static_cast<T>(-1));
		result.set(3, 2, -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear));
		return result;
	}

	static Matrix<4, 4, T> perspective(T fovy, T aspect, T zNear, T zFar) requires (M == 4 && N == 4) {
		assert(std::abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

		T const tanHalfFovy = tan(fovy / static_cast<T>(2));

		Matrix<4, 4, T> result = Matrix<4, 4, T>();
		result.set(0, 0, static_cast<T>(1) / (aspect * tanHalfFovy));
		result.set(1, 1, static_cast<T>(1) / tanHalfFovy);
		result.set(2, 2, -(zFar + zNear) / (zFar - zNear));
		result.set(2, 3, -static_cast<T>(1));
		result.set(3, 2, -(static_cast<T>(2) * zNear * zFar) / (zFar - zNear));

		return result;
	}

	static Matrix<4, 4, T> perspectiveFov(T fov, T width, T height, T zNear, T zFar) {
		assert(width > static_cast<T>(0));
		assert(height > static_cast<T>(0));
		assert(fov > static_cast<T>(0));

		T const rad = fov;
		T const h = cos(static_cast<T>(0.5) * rad) / sin(static_cast<T>(0.5) * rad);
		T const w = h * height / width;

		auto result = Matrix<4, 4, T>();
		result.set(0, 0, w);
		result.set(1, 1, h);
		result.set(2, 2, -(zFar + zNear) / (zFar - zNear));
		result.set(2, 3, -static_cast<T>(1));
		result.set(3, 2, -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear));

		return result;
	}

	static Type translate(Matrix<M-1, 1, T> const& vec) {
		auto result = Type::identity();
		for (int i = 0; i < M; i++) {
			result.set(N-1, i, vec.get(i));
		}
		return result;
	}

	Matrix<M-1, N, T> cutRow(int i) const requires (M > 1) {
		auto m = Matrix<M-1, N, T>();
		for (int j = 0; j < N * i; j++) {
			m.set(j, get(j));
		}
		for (int j = N*i; j < (M-1) * N; j++) {
			m.set(j, get(j+N));
		}
		return m;
	}

	Matrix<M, N-1, T> cutCol(int i) const requires (N > 1) {
		auto m = Matrix<M, N-1, T>();
		for (int k = 0; k < M; k++) {
			for (int j = 0; j < i; j++) {
				m.set(k, j, get(k, j));
			}
			for (int j = i; j < N-1; j++) {
				m.set(k, j, get(k, j + 1));
			}
		}
		return m;
	}

	friend std::ostream& operator <<(std::ostream& os, Type const& m) {
		os << '{';
		for (int i = 0; i < M; i++) {
			if (i != 0) os << ' ';
			os << '{';
			for (int j = 0; j < N; j++) {
				os << m.get(i, j);
				if (j != N-1) os << ", ";
			}
			if (i != M-1) os << "},\n";
		}
		os << "}}";
		return os;
	}

};

typedef Matrix<2, 1, float> Vec2;
typedef Matrix<3, 1, float> Vec3;
typedef Matrix<4, 1, float> Vec4;

typedef Matrix<2, 1, int> Vec2i;
typedef Matrix<3, 1, int> Vec3i;
typedef Matrix<4, 1, int> Vec4i;

typedef Matrix<2, 2, float> Mat2;
typedef Matrix<3, 3, float> Mat3;
typedef Matrix<4, 4, float> Mat4;

template <int M, int N>
struct std::hash<Matrix<M, N, int>> {
	size_t operator()(Matrix<M, N, int> const& m) const {
		size_t hash = 0;
		for (int i = 0; i < M * N; ++i) {
			unsigned part_hash = static_cast<size_t>(m.get(i)) << (8 * (i % 4));
			if (i % 8 < 4)
				hash ^= part_hash;
			else
				hash ^= std::byteswap(hash);
		}
		return hash;
	}
};

#undef MAT_TEMPLATE

#endif //MATRIX_H