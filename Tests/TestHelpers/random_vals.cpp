#include "random_vals.h"

template <> bool random_value<bool>() { return (bool)(rand() & 0x1); } 

template <> i8 random_value<i8>() { return u8_rand() ; }
template <> i16 random_value<i16>() { return u16_rand() ; }
template <> i32 random_value<i32>() { return u32_rand() ; }
template <> i64 random_value<i64>() { return u64_rand() ; }

template <> u8 random_value<u8>() { return u8_rand() ; }
template <> u16 random_value<u16>() { return u16_rand() ; }
template <> u32 random_value<u32>() { return u32_rand() ; }
template <> u64 random_value<u64>() { return u64_rand() ; }

template <> float random_value<float>() { return float_rand() ; }
template <> double random_value<double>() { return double_rand() ; }

template <> f32vec2 random_value<f32vec2>() { return f32vec2( float_rand(), float_rand() ) ; }
template <> f64vec2 random_value<f64vec2>() { return f64vec2( double_rand(), double_rand() ) ; }
template <> f32vec3 random_value<f32vec3>() { return f32vec3( float_rand(), float_rand(), float_rand() ) ; }
template <> f64vec3 random_value<f64vec3>() { return f64vec3( double_rand(), double_rand(), double_rand() ) ; }
template <> f32vec4 random_value<f32vec4>() { return f32vec4( float_rand(), float_rand(), float_rand(), float_rand() ) ; }
template <> f64vec4 random_value<f64vec4>() { return f64vec4( double_rand(), double_rand(), double_rand(), double_rand() ) ; }

template <> f32quat random_value<f32quat>() { return f32quat( float_rand(), float_rand(), float_rand(), float_rand() ) ; }
template <> f64quat random_value<f64quat>() { return f64quat( double_rand(), double_rand(), double_rand(), double_rand() ) ; }

template <> i8vec2 random_value<i8vec2>() { return i8vec2( u8_rand(), u8_rand() ) ; }
template <> i16vec2 random_value<i16vec2>() { return i16vec2( u16_rand(), u16_rand() ) ; }
template <> i32vec2 random_value<i32vec2>() { return i32vec2( u32_rand(), u32_rand() ) ; }
template <> i64vec2 random_value<i64vec2>() { return i64vec2( u64_rand(), u64_rand() ) ; }
template <> i8vec3 random_value<i8vec3>() { return i8vec3( u8_rand(), u8_rand(), u8_rand() ) ; }
template <> i16vec3 random_value<i16vec3>() { return i16vec3( u16_rand(), u16_rand(), u16_rand() ) ; }
template <> i32vec3 random_value<i32vec3>() { return i32vec3( u32_rand(), u32_rand(), u32_rand() ) ; }
template <> i64vec3 random_value<i64vec3>() { return i64vec3( u64_rand(), u64_rand(), u64_rand() ) ; }
template <> i8vec4 random_value<i8vec4>() { return i8vec4( u8_rand(), u8_rand(), u8_rand(), u8_rand() ) ; }
template <> i16vec4 random_value<i16vec4>() { return i16vec4( u16_rand(), u16_rand(), u16_rand(), u16_rand() ) ; }
template <> i32vec4 random_value<i32vec4>() { return i32vec4( u32_rand(), u32_rand(), u32_rand(), u32_rand() ) ; }
template <> i64vec4 random_value<i64vec4>() { return i64vec4( u64_rand(), u64_rand(), u64_rand(), u64_rand() ) ; }

template <> u8vec2 random_value<u8vec2>() { return u8vec2( u8_rand(), u8_rand() ) ; }
template <> u16vec2 random_value<u16vec2>() { return u16vec2( u16_rand(), u16_rand() ) ; }
template <> u32vec2 random_value<u32vec2>() { return u32vec2( u32_rand(), u32_rand() ) ; }
template <> u64vec2 random_value<u64vec2>() { return u64vec2( u64_rand(), u64_rand() ) ; }
template <> u8vec3 random_value<u8vec3>() { return u8vec3( u8_rand(), u8_rand(), u8_rand() ) ; }
template <> u16vec3 random_value<u16vec3>() { return u16vec3( u16_rand(), u16_rand(), u16_rand() ) ; }
template <> u32vec3 random_value<u32vec3>() { return u32vec3( u32_rand(), u32_rand(), u32_rand() ) ; }
template <> u64vec3 random_value<u64vec3>() { return u64vec3( u64_rand(), u64_rand(), u64_rand() ) ; }
template <> u8vec4 random_value<u8vec4>() { return u8vec4( u8_rand(), u8_rand(), u8_rand(), u8_rand() ) ; }
template <> u16vec4 random_value<u16vec4>() { return u16vec4( u16_rand(), u16_rand(), u16_rand(), u16_rand() ) ; }
template <> u32vec4 random_value<u32vec4>() { return u32vec4( u32_rand(), u32_rand(), u32_rand(), u32_rand() ) ; }
template <> u64vec4 random_value<u64vec4>() { return u64vec4( u64_rand(), u64_rand(), u64_rand(), u64_rand() ) ; }

template <> f32mat2 random_value<f32mat2>() { return f32mat2(
	f32vec2(float_rand(), float_rand()),
	f32vec2(float_rand(), float_rand())
) ; }
template <> f64mat2 random_value<f64mat2>() { return f64mat2(
	f64vec2(double_rand(), double_rand()),
	f64vec2(double_rand(), double_rand())
) ; }
template <> f32mat3 random_value<f32mat3>() { return f32mat3(
	f32vec3(float_rand(), float_rand(), float_rand()),
	f32vec3(float_rand(), float_rand(), float_rand()),
	f32vec3(float_rand(), float_rand(), float_rand())
) ; }
template <> f64mat3 random_value<f64mat3>() { return f64mat3(
	f64vec3(double_rand(), double_rand(), double_rand()),
	f64vec3(double_rand(), double_rand(), double_rand()),
	f64vec3(double_rand(), double_rand(), double_rand())
) ; }
template <> f32mat4 random_value<f32mat4>() { return f32mat4(
	f32vec4(float_rand(), float_rand(), float_rand(), float_rand()),
	f32vec4(float_rand(), float_rand(), float_rand(), float_rand()),
	f32vec4(float_rand(), float_rand(), float_rand(), float_rand()),
	f32vec4(float_rand(), float_rand(), float_rand(), float_rand())
) ; }
template <> f64mat4 random_value<f64mat4>() { return f64mat4(
	f64vec4(double_rand(), double_rand(), double_rand(), double_rand()),
	f64vec4(double_rand(), double_rand(), double_rand(), double_rand()),
	f64vec4(double_rand(), double_rand(), double_rand(), double_rand()),
	f64vec4(double_rand(), double_rand(), double_rand(), double_rand())
) ; }

template <> uuid random_value<uuid>() { return uuid_rand(); }

template <> hash random_value<hash>() { return hash_rand(); }

template <> item_ref random_value<item_ref>() { return item_ref::make_ref(); } // use the built-in uuid generation, since we can't set a random value

template <> entity_ref random_value<entity_ref>() { return entity_ref(hash_rand()); } 

template <> std::string random_value<std::string>() { return str_rand(); }