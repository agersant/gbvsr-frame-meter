#pragma once

#define FIELD(OFFSET, TYPE, NAME)                                                \
	void set_##OFFSET(std::add_const_t<std::add_lvalue_reference_t<TYPE>> value) \
	{                                                                            \
		*(std::add_pointer_t<TYPE>)((char *)this + OFFSET) = value;              \
	}                                                                            \
                                                                                 \
	void set_##OFFSET(std::add_rvalue_reference_t<TYPE> value)                   \
	{                                                                            \
		*(std::add_pointer_t<TYPE>)((char *)this + OFFSET) = std::move(value);   \
	}                                                                            \
                                                                                 \
	std::add_lvalue_reference_t<TYPE> get_##OFFSET() const                       \
	{                                                                            \
		return *(std::add_pointer_t<TYPE>)((char *)this + OFFSET);               \
	}                                                                            \
	__declspec(property(get = get_##OFFSET, put = set_##OFFSET)) TYPE NAME

#define BIT_FIELD(OFFSET, MASK, NAME)                         \
	void set_##OFFSET_##MASK(bool value)                      \
	{                                                         \
		if (value)                                            \
			*(int *)((char *)this + OFFSET) |= MASK;          \
		else                                                  \
			*(int *)((char *)this + OFFSET) &= ~MASK;         \
	}                                                         \
                                                              \
	bool get_##OFFSET_##MASK() const                          \
	{                                                         \
		return (*(int *)((char *)this + OFFSET) & MASK) != 0; \
	}                                                         \
	__declspec(property(get = get_##OFFSET_##MASK, put = set_##OFFSET_##MASK)) bool NAME
