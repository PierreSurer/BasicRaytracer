// This example demonstrates a convenient way to implement
// kernel code with complex number arithmetic using various
// C++ features.

// Define a class Complex, that can perform complex number
// computations with various precision when different
// types for T are used - double, float, half...
template<typename T>
class complex_t {
T m_re; // Real component.
T m_im; // Imaginary component.

public:
complex_t(T re, T im): m_re{re}, m_im{im} {};
complex_t operator*(const complex_t &other) const
{
  return {m_re * other.m_re - m_im * other.m_im,
           m_re * other.m_im + m_im * other.m_re};
}
int get_re() const { return m_re; }
int get_im() const { return m_im; }
};

// A helper function to compute multiplication over
// complex numbers read from the input buffer and
// to store the computed result into the output buffer.
template<typename T>
void compute_helper(global T *in, global T *out) {
  auto idx = get_global_id(0);
  // Every work-item uses 4 consecutive items from the input
  // buffer - two for each complex number.
  auto offset = idx * 4;
  auto num1 = complex_t{in[offset], in[offset + 1]};
  auto num2 = complex_t{in[offset + 2], in[offset + 3]};
  // Perform complex number multiplication.
  auto res = num1 * num2;
  // Every work-item writes 2 consecutive items to the output
  // buffer.
  out[idx * 2] = res.get_re();
  out[idx * 2 + 1] = res.get_im();
}

// This kernel can be used for complex number multiplication
// in single precision.
kernel void compute_sp(global float *in, global float *out) {
  compute_helper(in, out);
}

// This kernel can be used for complex number multiplication
// in half precision.
#pragma OPENCL EXTENSION cl_khr_fp16: enable
kernel void compute_hp(global half *in, global half *out) {
  compute_helper(in, out); 
}