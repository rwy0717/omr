#if !defined(OMR_SIZEP2_HPP_)
#define OMR_SIZEP2_HPP_

#include <omrcfg.h>

#include <cstdint>
#include <cstddef>

namespace OMR {

/// Helper for working with a power-of-two std::size_t.
/// Cannot represent zero, which is not a power-of-two.
class SizeP2 {
public:
	explicit constexpr SizeP2(std::uint8_t exponent = 0)
		: modmask_(modmask(exponent)), exponent_(exponent) {}

	constexpr SizeP2(const SizeP2&) = default;

	/// mod x by value.
	constexpr std::size_t mod(std::size_t x) const noexcept {
		return x & modmask_;
	}

	/// get log2(value)
	constexpr std::size_t log2() const noexcept {
		return exponent_;
	}

	/// double the current value, N times.
	constexpr SizeP2 x2(std::size_t n = 1) const noexcept {
		return SizeP2(exponent_ + n);
	}

	/// double the current value.
	constexpr SizeP2 dbl() const noexcept {
		return x2();
	}

	/// get the current value
	constexpr std::size_t get() const noexcept {
		return 1 << exponent_;
	}

	explicit operator std::size_t() const noexcept { return get(); }

private:
	/// Compute the modmask for a given exponent.
	/// The modmask can be used to mask the high bits of an integer, 
	static constexpr std::size_t modmask(std::uint8_t exponent) noexcept {
		return (std::size_t(1) << exponent) - 1;
	}

	std::size_t modmask_; // memoized mask to compute `x % get()`
	std::uint8_t exponent_;
};

constexpr std::size_t operator%(std::size_t a, const SizeP2& n) {
	return n.mod(a);
}

constexpr SizeP2 SIZEP2_1(std::uint8_t(0));
constexpr SizeP2 SIZEP2_2(std::uint8_t(1));
constexpr SizeP2 SIZEP2_4(std::uint8_t(2));
constexpr SizeP2 SIZEP2_8(std::uint8_t(3));
constexpr SizeP2 SIZEP2_16(std::uint8_t(4));
constexpr SizeP2 SIZEP2_32(std::uint8_t(5));
constexpr SizeP2 SIZEP2_64(std::uint8_t(6));
constexpr SizeP2 SIZEP2_128(std::uint8_t(7));
constexpr SizeP2 SIZEP2_256(std::uint8_t(8));
constexpr SizeP2 SIZEP2_MAX(std::uint8_t(63));

} // namespace OMR

#endif // OMR_SIZEP2_HPP_
