// WARNING! DO NOT EDIT THIS FILE! This file is generated.

// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__ELEMENT_TYPES_INL__
#define __PDS__ELEMENT_TYPES_INL__

// element_types.inl - Implementation of element_types constants in in pds.
// Dont include this file directly, it is included by element_types.h when implementing pds.

namespace pds
{
	const bool element_type_information<bool>::zero = bool(bool_zero); // zero value of bool
	const bool element_type_information<bool>::inf = bool(bool_inf); // limit inferior (minimum bound) of bool
	const bool element_type_information<bool>::sup = bool(bool_sup); // limit superior (maximum bound) of bool

	const i8 element_type_information<i8>::zero = i8(i8_zero); // zero value of i8
	const i8 element_type_information<i8>::inf = i8(i8_inf); // limit inferior (minimum bound) of i8
	const i8 element_type_information<i8>::sup = i8(i8_sup); // limit superior (maximum bound) of i8

	const i16 element_type_information<i16>::zero = i16(i16_zero); // zero value of i16
	const i16 element_type_information<i16>::inf = i16(i16_inf); // limit inferior (minimum bound) of i16
	const i16 element_type_information<i16>::sup = i16(i16_sup); // limit superior (maximum bound) of i16

	const i32 element_type_information<i32>::zero = i32(i32_zero); // zero value of i32
	const i32 element_type_information<i32>::inf = i32(i32_inf); // limit inferior (minimum bound) of i32
	const i32 element_type_information<i32>::sup = i32(i32_sup); // limit superior (maximum bound) of i32

	const i64 element_type_information<i64>::zero = i64(i64_zero); // zero value of i64
	const i64 element_type_information<i64>::inf = i64(i64_inf); // limit inferior (minimum bound) of i64
	const i64 element_type_information<i64>::sup = i64(i64_sup); // limit superior (maximum bound) of i64

	const u8 element_type_information<u8>::zero = u8(u8_zero); // zero value of u8
	const u8 element_type_information<u8>::inf = u8(u8_inf); // limit inferior (minimum bound) of u8
	const u8 element_type_information<u8>::sup = u8(u8_sup); // limit superior (maximum bound) of u8

	const u16 element_type_information<u16>::zero = u16(u16_zero); // zero value of u16
	const u16 element_type_information<u16>::inf = u16(u16_inf); // limit inferior (minimum bound) of u16
	const u16 element_type_information<u16>::sup = u16(u16_sup); // limit superior (maximum bound) of u16

	const u32 element_type_information<u32>::zero = u32(u32_zero); // zero value of u32
	const u32 element_type_information<u32>::inf = u32(u32_inf); // limit inferior (minimum bound) of u32
	const u32 element_type_information<u32>::sup = u32(u32_sup); // limit superior (maximum bound) of u32

	const u64 element_type_information<u64>::zero = u64(u64_zero); // zero value of u64
	const u64 element_type_information<u64>::inf = u64(u64_inf); // limit inferior (minimum bound) of u64
	const u64 element_type_information<u64>::sup = u64(u64_sup); // limit superior (maximum bound) of u64

	const f32 element_type_information<f32>::zero = f32(f32_zero); // zero value of f32
	const f32 element_type_information<f32>::inf = f32(f32_inf); // limit inferior (minimum bound) of f32
	const f32 element_type_information<f32>::sup = f32(f32_sup); // limit superior (maximum bound) of f32

	const f64 element_type_information<f64>::zero = f64(f64_zero); // zero value of f64
	const f64 element_type_information<f64>::inf = f64(f64_inf); // limit inferior (minimum bound) of f64
	const f64 element_type_information<f64>::sup = f64(f64_sup); // limit superior (maximum bound) of f64

	const i8vec2 element_type_information<i8vec2>::zero = i8vec2(i8_zero,i8_zero); // zero value of i8vec2
	const i8vec2 element_type_information<i8vec2>::inf = i8vec2(i8_inf,i8_inf); // limit inferior (minimum bound) of i8vec2
	const i8vec2 element_type_information<i8vec2>::sup = i8vec2(i8_sup,i8_sup); // limit superior (maximum bound) of i8vec2

	const i8vec3 element_type_information<i8vec3>::zero = i8vec3(i8_zero,i8_zero,i8_zero); // zero value of i8vec3
	const i8vec3 element_type_information<i8vec3>::inf = i8vec3(i8_inf,i8_inf,i8_inf); // limit inferior (minimum bound) of i8vec3
	const i8vec3 element_type_information<i8vec3>::sup = i8vec3(i8_sup,i8_sup,i8_sup); // limit superior (maximum bound) of i8vec3

	const i8vec4 element_type_information<i8vec4>::zero = i8vec4(i8_zero,i8_zero,i8_zero,i8_zero); // zero value of i8vec4
	const i8vec4 element_type_information<i8vec4>::inf = i8vec4(i8_inf,i8_inf,i8_inf,i8_inf); // limit inferior (minimum bound) of i8vec4
	const i8vec4 element_type_information<i8vec4>::sup = i8vec4(i8_sup,i8_sup,i8_sup,i8_sup); // limit superior (maximum bound) of i8vec4

	const i16vec2 element_type_information<i16vec2>::zero = i16vec2(i16_zero,i16_zero); // zero value of i16vec2
	const i16vec2 element_type_information<i16vec2>::inf = i16vec2(i16_inf,i16_inf); // limit inferior (minimum bound) of i16vec2
	const i16vec2 element_type_information<i16vec2>::sup = i16vec2(i16_sup,i16_sup); // limit superior (maximum bound) of i16vec2

	const i16vec3 element_type_information<i16vec3>::zero = i16vec3(i16_zero,i16_zero,i16_zero); // zero value of i16vec3
	const i16vec3 element_type_information<i16vec3>::inf = i16vec3(i16_inf,i16_inf,i16_inf); // limit inferior (minimum bound) of i16vec3
	const i16vec3 element_type_information<i16vec3>::sup = i16vec3(i16_sup,i16_sup,i16_sup); // limit superior (maximum bound) of i16vec3

	const i16vec4 element_type_information<i16vec4>::zero = i16vec4(i16_zero,i16_zero,i16_zero,i16_zero); // zero value of i16vec4
	const i16vec4 element_type_information<i16vec4>::inf = i16vec4(i16_inf,i16_inf,i16_inf,i16_inf); // limit inferior (minimum bound) of i16vec4
	const i16vec4 element_type_information<i16vec4>::sup = i16vec4(i16_sup,i16_sup,i16_sup,i16_sup); // limit superior (maximum bound) of i16vec4

	const i32vec2 element_type_information<i32vec2>::zero = i32vec2(i32_zero,i32_zero); // zero value of i32vec2
	const i32vec2 element_type_information<i32vec2>::inf = i32vec2(i32_inf,i32_inf); // limit inferior (minimum bound) of i32vec2
	const i32vec2 element_type_information<i32vec2>::sup = i32vec2(i32_sup,i32_sup); // limit superior (maximum bound) of i32vec2

	const i32vec3 element_type_information<i32vec3>::zero = i32vec3(i32_zero,i32_zero,i32_zero); // zero value of i32vec3
	const i32vec3 element_type_information<i32vec3>::inf = i32vec3(i32_inf,i32_inf,i32_inf); // limit inferior (minimum bound) of i32vec3
	const i32vec3 element_type_information<i32vec3>::sup = i32vec3(i32_sup,i32_sup,i32_sup); // limit superior (maximum bound) of i32vec3

	const i32vec4 element_type_information<i32vec4>::zero = i32vec4(i32_zero,i32_zero,i32_zero,i32_zero); // zero value of i32vec4
	const i32vec4 element_type_information<i32vec4>::inf = i32vec4(i32_inf,i32_inf,i32_inf,i32_inf); // limit inferior (minimum bound) of i32vec4
	const i32vec4 element_type_information<i32vec4>::sup = i32vec4(i32_sup,i32_sup,i32_sup,i32_sup); // limit superior (maximum bound) of i32vec4

	const i64vec2 element_type_information<i64vec2>::zero = i64vec2(i64_zero,i64_zero); // zero value of i64vec2
	const i64vec2 element_type_information<i64vec2>::inf = i64vec2(i64_inf,i64_inf); // limit inferior (minimum bound) of i64vec2
	const i64vec2 element_type_information<i64vec2>::sup = i64vec2(i64_sup,i64_sup); // limit superior (maximum bound) of i64vec2

	const i64vec3 element_type_information<i64vec3>::zero = i64vec3(i64_zero,i64_zero,i64_zero); // zero value of i64vec3
	const i64vec3 element_type_information<i64vec3>::inf = i64vec3(i64_inf,i64_inf,i64_inf); // limit inferior (minimum bound) of i64vec3
	const i64vec3 element_type_information<i64vec3>::sup = i64vec3(i64_sup,i64_sup,i64_sup); // limit superior (maximum bound) of i64vec3

	const i64vec4 element_type_information<i64vec4>::zero = i64vec4(i64_zero,i64_zero,i64_zero,i64_zero); // zero value of i64vec4
	const i64vec4 element_type_information<i64vec4>::inf = i64vec4(i64_inf,i64_inf,i64_inf,i64_inf); // limit inferior (minimum bound) of i64vec4
	const i64vec4 element_type_information<i64vec4>::sup = i64vec4(i64_sup,i64_sup,i64_sup,i64_sup); // limit superior (maximum bound) of i64vec4

	const u8vec2 element_type_information<u8vec2>::zero = u8vec2(u8_zero,u8_zero); // zero value of u8vec2
	const u8vec2 element_type_information<u8vec2>::inf = u8vec2(u8_inf,u8_inf); // limit inferior (minimum bound) of u8vec2
	const u8vec2 element_type_information<u8vec2>::sup = u8vec2(u8_sup,u8_sup); // limit superior (maximum bound) of u8vec2

	const u8vec3 element_type_information<u8vec3>::zero = u8vec3(u8_zero,u8_zero,u8_zero); // zero value of u8vec3
	const u8vec3 element_type_information<u8vec3>::inf = u8vec3(u8_inf,u8_inf,u8_inf); // limit inferior (minimum bound) of u8vec3
	const u8vec3 element_type_information<u8vec3>::sup = u8vec3(u8_sup,u8_sup,u8_sup); // limit superior (maximum bound) of u8vec3

	const u8vec4 element_type_information<u8vec4>::zero = u8vec4(u8_zero,u8_zero,u8_zero,u8_zero); // zero value of u8vec4
	const u8vec4 element_type_information<u8vec4>::inf = u8vec4(u8_inf,u8_inf,u8_inf,u8_inf); // limit inferior (minimum bound) of u8vec4
	const u8vec4 element_type_information<u8vec4>::sup = u8vec4(u8_sup,u8_sup,u8_sup,u8_sup); // limit superior (maximum bound) of u8vec4

	const u16vec2 element_type_information<u16vec2>::zero = u16vec2(u16_zero,u16_zero); // zero value of u16vec2
	const u16vec2 element_type_information<u16vec2>::inf = u16vec2(u16_inf,u16_inf); // limit inferior (minimum bound) of u16vec2
	const u16vec2 element_type_information<u16vec2>::sup = u16vec2(u16_sup,u16_sup); // limit superior (maximum bound) of u16vec2

	const u16vec3 element_type_information<u16vec3>::zero = u16vec3(u16_zero,u16_zero,u16_zero); // zero value of u16vec3
	const u16vec3 element_type_information<u16vec3>::inf = u16vec3(u16_inf,u16_inf,u16_inf); // limit inferior (minimum bound) of u16vec3
	const u16vec3 element_type_information<u16vec3>::sup = u16vec3(u16_sup,u16_sup,u16_sup); // limit superior (maximum bound) of u16vec3

	const u16vec4 element_type_information<u16vec4>::zero = u16vec4(u16_zero,u16_zero,u16_zero,u16_zero); // zero value of u16vec4
	const u16vec4 element_type_information<u16vec4>::inf = u16vec4(u16_inf,u16_inf,u16_inf,u16_inf); // limit inferior (minimum bound) of u16vec4
	const u16vec4 element_type_information<u16vec4>::sup = u16vec4(u16_sup,u16_sup,u16_sup,u16_sup); // limit superior (maximum bound) of u16vec4

	const u32vec2 element_type_information<u32vec2>::zero = u32vec2(u32_zero,u32_zero); // zero value of u32vec2
	const u32vec2 element_type_information<u32vec2>::inf = u32vec2(u32_inf,u32_inf); // limit inferior (minimum bound) of u32vec2
	const u32vec2 element_type_information<u32vec2>::sup = u32vec2(u32_sup,u32_sup); // limit superior (maximum bound) of u32vec2

	const u32vec3 element_type_information<u32vec3>::zero = u32vec3(u32_zero,u32_zero,u32_zero); // zero value of u32vec3
	const u32vec3 element_type_information<u32vec3>::inf = u32vec3(u32_inf,u32_inf,u32_inf); // limit inferior (minimum bound) of u32vec3
	const u32vec3 element_type_information<u32vec3>::sup = u32vec3(u32_sup,u32_sup,u32_sup); // limit superior (maximum bound) of u32vec3

	const u32vec4 element_type_information<u32vec4>::zero = u32vec4(u32_zero,u32_zero,u32_zero,u32_zero); // zero value of u32vec4
	const u32vec4 element_type_information<u32vec4>::inf = u32vec4(u32_inf,u32_inf,u32_inf,u32_inf); // limit inferior (minimum bound) of u32vec4
	const u32vec4 element_type_information<u32vec4>::sup = u32vec4(u32_sup,u32_sup,u32_sup,u32_sup); // limit superior (maximum bound) of u32vec4

	const u64vec2 element_type_information<u64vec2>::zero = u64vec2(u64_zero,u64_zero); // zero value of u64vec2
	const u64vec2 element_type_information<u64vec2>::inf = u64vec2(u64_inf,u64_inf); // limit inferior (minimum bound) of u64vec2
	const u64vec2 element_type_information<u64vec2>::sup = u64vec2(u64_sup,u64_sup); // limit superior (maximum bound) of u64vec2

	const u64vec3 element_type_information<u64vec3>::zero = u64vec3(u64_zero,u64_zero,u64_zero); // zero value of u64vec3
	const u64vec3 element_type_information<u64vec3>::inf = u64vec3(u64_inf,u64_inf,u64_inf); // limit inferior (minimum bound) of u64vec3
	const u64vec3 element_type_information<u64vec3>::sup = u64vec3(u64_sup,u64_sup,u64_sup); // limit superior (maximum bound) of u64vec3

	const u64vec4 element_type_information<u64vec4>::zero = u64vec4(u64_zero,u64_zero,u64_zero,u64_zero); // zero value of u64vec4
	const u64vec4 element_type_information<u64vec4>::inf = u64vec4(u64_inf,u64_inf,u64_inf,u64_inf); // limit inferior (minimum bound) of u64vec4
	const u64vec4 element_type_information<u64vec4>::sup = u64vec4(u64_sup,u64_sup,u64_sup,u64_sup); // limit superior (maximum bound) of u64vec4

	const f32vec2 element_type_information<f32vec2>::zero = f32vec2(f32_zero,f32_zero); // zero value of f32vec2
	const f32vec2 element_type_information<f32vec2>::inf = f32vec2(f32_inf,f32_inf); // limit inferior (minimum bound) of f32vec2
	const f32vec2 element_type_information<f32vec2>::sup = f32vec2(f32_sup,f32_sup); // limit superior (maximum bound) of f32vec2

	const f32vec3 element_type_information<f32vec3>::zero = f32vec3(f32_zero,f32_zero,f32_zero); // zero value of f32vec3
	const f32vec3 element_type_information<f32vec3>::inf = f32vec3(f32_inf,f32_inf,f32_inf); // limit inferior (minimum bound) of f32vec3
	const f32vec3 element_type_information<f32vec3>::sup = f32vec3(f32_sup,f32_sup,f32_sup); // limit superior (maximum bound) of f32vec3

	const f32vec4 element_type_information<f32vec4>::zero = f32vec4(f32_zero,f32_zero,f32_zero,f32_zero); // zero value of f32vec4
	const f32vec4 element_type_information<f32vec4>::inf = f32vec4(f32_inf,f32_inf,f32_inf,f32_inf); // limit inferior (minimum bound) of f32vec4
	const f32vec4 element_type_information<f32vec4>::sup = f32vec4(f32_sup,f32_sup,f32_sup,f32_sup); // limit superior (maximum bound) of f32vec4

	const f64vec2 element_type_information<f64vec2>::zero = f64vec2(f64_zero,f64_zero); // zero value of f64vec2
	const f64vec2 element_type_information<f64vec2>::inf = f64vec2(f64_inf,f64_inf); // limit inferior (minimum bound) of f64vec2
	const f64vec2 element_type_information<f64vec2>::sup = f64vec2(f64_sup,f64_sup); // limit superior (maximum bound) of f64vec2

	const f64vec3 element_type_information<f64vec3>::zero = f64vec3(f64_zero,f64_zero,f64_zero); // zero value of f64vec3
	const f64vec3 element_type_information<f64vec3>::inf = f64vec3(f64_inf,f64_inf,f64_inf); // limit inferior (minimum bound) of f64vec3
	const f64vec3 element_type_information<f64vec3>::sup = f64vec3(f64_sup,f64_sup,f64_sup); // limit superior (maximum bound) of f64vec3

	const f64vec4 element_type_information<f64vec4>::zero = f64vec4(f64_zero,f64_zero,f64_zero,f64_zero); // zero value of f64vec4
	const f64vec4 element_type_information<f64vec4>::inf = f64vec4(f64_inf,f64_inf,f64_inf,f64_inf); // limit inferior (minimum bound) of f64vec4
	const f64vec4 element_type_information<f64vec4>::sup = f64vec4(f64_sup,f64_sup,f64_sup,f64_sup); // limit superior (maximum bound) of f64vec4

	const f32mat2 element_type_information<f32mat2>::zero = f32mat2(f32vec2(f32_zero,f32_zero),f32vec2(f32_zero,f32_zero)); // zero value of f32mat2
	const f32mat2 element_type_information<f32mat2>::inf = f32mat2(f32vec2(f32_inf,f32_inf),f32vec2(f32_inf,f32_inf)); // limit inferior (minimum bound) of f32mat2
	const f32mat2 element_type_information<f32mat2>::sup = f32mat2(f32vec2(f32_sup,f32_sup),f32vec2(f32_sup,f32_sup)); // limit superior (maximum bound) of f32mat2

	const f32mat3 element_type_information<f32mat3>::zero = f32mat3(f32vec3(f32_zero,f32_zero,f32_zero),f32vec3(f32_zero,f32_zero,f32_zero),f32vec3(f32_zero,f32_zero,f32_zero)); // zero value of f32mat3
	const f32mat3 element_type_information<f32mat3>::inf = f32mat3(f32vec3(f32_inf,f32_inf,f32_inf),f32vec3(f32_inf,f32_inf,f32_inf),f32vec3(f32_inf,f32_inf,f32_inf)); // limit inferior (minimum bound) of f32mat3
	const f32mat3 element_type_information<f32mat3>::sup = f32mat3(f32vec3(f32_sup,f32_sup,f32_sup),f32vec3(f32_sup,f32_sup,f32_sup),f32vec3(f32_sup,f32_sup,f32_sup)); // limit superior (maximum bound) of f32mat3

	const f32mat4 element_type_information<f32mat4>::zero = f32mat4(f32vec4(f32_zero,f32_zero,f32_zero,f32_zero),f32vec4(f32_zero,f32_zero,f32_zero,f32_zero),f32vec4(f32_zero,f32_zero,f32_zero,f32_zero),f32vec4(f32_zero,f32_zero,f32_zero,f32_zero)); // zero value of f32mat4
	const f32mat4 element_type_information<f32mat4>::inf = f32mat4(f32vec4(f32_inf,f32_inf,f32_inf,f32_inf),f32vec4(f32_inf,f32_inf,f32_inf,f32_inf),f32vec4(f32_inf,f32_inf,f32_inf,f32_inf),f32vec4(f32_inf,f32_inf,f32_inf,f32_inf)); // limit inferior (minimum bound) of f32mat4
	const f32mat4 element_type_information<f32mat4>::sup = f32mat4(f32vec4(f32_sup,f32_sup,f32_sup,f32_sup),f32vec4(f32_sup,f32_sup,f32_sup,f32_sup),f32vec4(f32_sup,f32_sup,f32_sup,f32_sup),f32vec4(f32_sup,f32_sup,f32_sup,f32_sup)); // limit superior (maximum bound) of f32mat4

	const f64mat2 element_type_information<f64mat2>::zero = f64mat2(f64vec2(f64_zero,f64_zero),f64vec2(f64_zero,f64_zero)); // zero value of f64mat2
	const f64mat2 element_type_information<f64mat2>::inf = f64mat2(f64vec2(f64_inf,f64_inf),f64vec2(f64_inf,f64_inf)); // limit inferior (minimum bound) of f64mat2
	const f64mat2 element_type_information<f64mat2>::sup = f64mat2(f64vec2(f64_sup,f64_sup),f64vec2(f64_sup,f64_sup)); // limit superior (maximum bound) of f64mat2

	const f64mat3 element_type_information<f64mat3>::zero = f64mat3(f64vec3(f64_zero,f64_zero,f64_zero),f64vec3(f64_zero,f64_zero,f64_zero),f64vec3(f64_zero,f64_zero,f64_zero)); // zero value of f64mat3
	const f64mat3 element_type_information<f64mat3>::inf = f64mat3(f64vec3(f64_inf,f64_inf,f64_inf),f64vec3(f64_inf,f64_inf,f64_inf),f64vec3(f64_inf,f64_inf,f64_inf)); // limit inferior (minimum bound) of f64mat3
	const f64mat3 element_type_information<f64mat3>::sup = f64mat3(f64vec3(f64_sup,f64_sup,f64_sup),f64vec3(f64_sup,f64_sup,f64_sup),f64vec3(f64_sup,f64_sup,f64_sup)); // limit superior (maximum bound) of f64mat3

	const f64mat4 element_type_information<f64mat4>::zero = f64mat4(f64vec4(f64_zero,f64_zero,f64_zero,f64_zero),f64vec4(f64_zero,f64_zero,f64_zero,f64_zero),f64vec4(f64_zero,f64_zero,f64_zero,f64_zero),f64vec4(f64_zero,f64_zero,f64_zero,f64_zero)); // zero value of f64mat4
	const f64mat4 element_type_information<f64mat4>::inf = f64mat4(f64vec4(f64_inf,f64_inf,f64_inf,f64_inf),f64vec4(f64_inf,f64_inf,f64_inf,f64_inf),f64vec4(f64_inf,f64_inf,f64_inf,f64_inf),f64vec4(f64_inf,f64_inf,f64_inf,f64_inf)); // limit inferior (minimum bound) of f64mat4
	const f64mat4 element_type_information<f64mat4>::sup = f64mat4(f64vec4(f64_sup,f64_sup,f64_sup,f64_sup),f64vec4(f64_sup,f64_sup,f64_sup,f64_sup),f64vec4(f64_sup,f64_sup,f64_sup,f64_sup),f64vec4(f64_sup,f64_sup,f64_sup,f64_sup)); // limit superior (maximum bound) of f64mat4

	const f32quat element_type_information<f32quat>::zero = f32quat(f32_zero,f32_zero,f32_zero,f32_zero); // zero value of f32quat
	const f32quat element_type_information<f32quat>::inf = f32quat(f32_inf,f32_inf,f32_inf,f32_inf); // limit inferior (minimum bound) of f32quat
	const f32quat element_type_information<f32quat>::sup = f32quat(f32_sup,f32_sup,f32_sup,f32_sup); // limit superior (maximum bound) of f32quat

	const f64quat element_type_information<f64quat>::zero = f64quat(f64_zero,f64_zero,f64_zero,f64_zero); // zero value of f64quat
	const f64quat element_type_information<f64quat>::inf = f64quat(f64_inf,f64_inf,f64_inf,f64_inf); // limit inferior (minimum bound) of f64quat
	const f64quat element_type_information<f64quat>::sup = f64quat(f64_sup,f64_sup,f64_sup,f64_sup); // limit superior (maximum bound) of f64quat

	const uuid element_type_information<uuid>::zero = uuid(uuid_zero); // zero value of uuid
	const uuid element_type_information<uuid>::inf = uuid(uuid_inf); // limit inferior (minimum bound) of uuid
	const uuid element_type_information<uuid>::sup = uuid(uuid_sup); // limit superior (maximum bound) of uuid

	const entity_ref element_type_information<entity_ref>::zero = entity_ref(entity_ref_zero); // zero value of entity_ref
	const entity_ref element_type_information<entity_ref>::inf = entity_ref(entity_ref_inf); // limit inferior (minimum bound) of entity_ref
	const entity_ref element_type_information<entity_ref>::sup = entity_ref(entity_ref_sup); // limit superior (maximum bound) of entity_ref

	const hash element_type_information<hash>::zero = hash(hash_zero); // zero value of hash
	const hash element_type_information<hash>::inf = hash(hash_inf); // limit inferior (minimum bound) of hash
	const hash element_type_information<hash>::sup = hash(hash_sup); // limit superior (maximum bound) of hash

	const item_ref element_type_information<item_ref>::zero = item_ref(item_ref_zero); // zero value of item_ref
	const item_ref element_type_information<item_ref>::inf = item_ref(item_ref_inf); // limit inferior (minimum bound) of item_ref
	const item_ref element_type_information<item_ref>::sup = item_ref(item_ref_sup); // limit superior (maximum bound) of item_ref

	const string element_type_information<string>::zero = string(string_zero); // zero value of string
	const string element_type_information<string>::inf = string(string_inf); // limit inferior (minimum bound) of string

}
// namespace pds
#endif//__PDS__ELEMENT_TYPES_INL__
