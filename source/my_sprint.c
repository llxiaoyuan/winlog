#include "my_sprint.h"
#include <stdarg.h>

static size_t string_length_a(const char* str)
{
	const char* s;
	for (s = str; *s; ++s)
		;
	return (s - str);
}

static size_t string_length_w(const wchar_t* str)
{
	const wchar_t* s;
	for (s = str; *s; ++s)
		;
	return (s - str);
}

#define PRINT_BUFFER_LENGTH (64 + 1)

static long long get_args_value(int z_count, int h_count, int l_count, va_list* args)
{
	long long value;
	if (z_count == 1) {
		value = (long long)((void*)va_arg(*args, void*));
	}
	else if (h_count == 1) {
		value = (short)va_arg(*args, short);
	}
	else if (h_count == 2) {
		value = (char)va_arg(*args, char);
	}
	else if (l_count == 1) {
		value = (long)va_arg(*args, long);
	}
	else if (l_count == 2) {
		value = (long long)va_arg(*args, long long);
	}
	else {
		value = (int)va_arg(*args, int);
	}
	return value;
}


static long long get_integer_part(unsigned char* binary, int size) {
	long long integer = 0;
	long long value = 1;
	for (int i = size - 1; i != 0; i--) {
		if (binary[i] == 1) {
			integer += value;
		}
		value *= 2;
	}
	return integer;
}

static int ipow(int base, int exp)
{
	int result = 1;
	for (;;)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		if (!exp)
			break;
		base *= base;
	}
	return result;
}

static long long get_decimal_part(unsigned char* binary, int size, int precision)
{
	double decimal = 0.0;
	double fraction = 0.5;
	for (int i = 0; i < size; i++) {
		if (binary[i] == 1) {
			decimal += fraction;
		}
		fraction *= 0.5;
	}
	return (long long)(decimal * ipow(10, precision));
}

size_t my_sprint_f(char** out, float f, int precision)
{
	if (precision == 0) {
		precision = 6;
	}

	size_t result = 0;

	unsigned int bits = *(unsigned int*)&f;
	int sign = (bits >> 31) & 1;
	int exponent = ((bits >> 23) & 0xFF);
	unsigned int mantissa = bits & 0x7FFFFF;

	if (sign) {
		result += 1; my_sprint_c(out, '-');
	}

	if (exponent == 0 && mantissa == 0) {
		result += 1; my_sprint_c(out, '0');
		result += 1; my_sprint_c(out, '.');
		result += precision;
		for (int i = 0; i < precision; i++) {
			my_sprint_c(out, '0');
		}
		return result;
	}
	else if (exponent == 0b0111'1111 && mantissa == 0) {
		result += 1; my_sprint_c(out, '1');
		result += 1; my_sprint_c(out, '.');
		result += precision;
		for (int i = 0; i < precision; i++) {
			my_sprint_c(out, '0');
		}
		return result;
	}
	else if (exponent == 0b1111'1111 && mantissa == 0) {
		result += 1; my_sprint_c(out, 'i');
		result += 1; my_sprint_c(out, 'n');
		result += 1; my_sprint_c(out, 'f');
		return result;
	}
	else if (exponent == 0b1111'1111 && mantissa != 0) {
		result += 1; my_sprint_c(out, 'n');
		result += 1; my_sprint_c(out, 'a');
		result += 1; my_sprint_c(out, 'n');
		return result;
	}

	exponent -= 0b0111'1111;
	mantissa |= (1 << 23);

#define BINARY_SIZE 32
	int decimal_part_start = (1 + 8 + exponent);
	if (0 < decimal_part_start && decimal_part_start < BINARY_SIZE){
		unsigned char binary[BINARY_SIZE];
		for (size_t i = 0; i < BINARY_SIZE; i++){
			binary[i] = (mantissa >> ((BINARY_SIZE - 1) - i)) & 1;
		}
		long long integer_part = get_integer_part(binary, decimal_part_start);
		long long decimal_part = get_decimal_part(binary + decimal_part_start, BINARY_SIZE - decimal_part_start, precision);
		result += my_sprint_i(out, integer_part, 10, 0, 0, 0);
		result += 1; my_sprint_c(out, '.');
		result += my_sprint_i(out, decimal_part, 10, 0, 0, 0);
	}
	else {
		result += 1; my_sprint_c(out, 'e');
		result += 1; my_sprint_c(out, 'r');
		result += 1; my_sprint_c(out, 'r');
	}
#undef BINARY_SIZE
	return result;
}

size_t my_sprint_lf(char** out, double lf, int precision)
{
	if (precision == 0) {
		precision = 6;
	}

	size_t result = 0;

	unsigned long long bits = *(unsigned long long*)&lf;
	int sign = (bits >> 63) & 1;
	int exponent = (bits >> 52) & 0x7FF;
	unsigned long long mantissa = bits & 0xF'FFFF'FFFF'FFFF;

	if (sign) {
		result += 1; my_sprint_c(out, '-');
	}

	if (exponent == 0 && mantissa == 0) {
		result += 1; my_sprint_c(out, '0');
		result += 1; my_sprint_c(out, '.');
		result += precision;
		for (int i = 0; i < precision; i++) {
			my_sprint_c(out, '0');
		}
		return result;
	}
	else if (exponent == 0b011'1111'1111 && mantissa == 0) {
		result += 1; my_sprint_c(out, '1');
		result += 1; my_sprint_c(out, '.');
		result += precision;
		for (int i = 0; i < precision; i++) {
			my_sprint_c(out, '0');
		}
		return result;
	}
	else if (exponent == 0b111'1111'1111 && mantissa == 0) {
		result += 1; my_sprint_c(out, 'i');
		result += 1; my_sprint_c(out, 'n');
		result += 1; my_sprint_c(out, 'f');
		return result;
	}
	else if (exponent == 0b111'1111'1111 && mantissa != 0) {
		result += 1; my_sprint_c(out, 'n');
		result += 1; my_sprint_c(out, 'a');
		result += 1; my_sprint_c(out, 'n');
		return result;
	}

	exponent -= 0b011'1111'1111;
	mantissa |= (1ULL << 52);

#define BINARY_SIZE 64
	int decimal_part_start = (1 + 11 + exponent);
	if (0 < decimal_part_start && decimal_part_start < BINARY_SIZE) {
		unsigned char binary[BINARY_SIZE];
		for (size_t i = 0; i < BINARY_SIZE; i++) {
			binary[i] = (mantissa >> ((BINARY_SIZE - 1) - i)) & 1;
		}
		long long integer_part = get_integer_part(binary, decimal_part_start);
		long long decimal_part = get_decimal_part(binary + decimal_part_start, BINARY_SIZE - decimal_part_start, precision);
		result += my_sprint_i(out, integer_part, 10, 0, 0, 0);
		result += 1; my_sprint_c(out, '.');
		result += my_sprint_i(out, decimal_part, 10, 0, 0, 0);
	}
	else {
		result += 1; my_sprint_c(out, 'e');
		result += 1; my_sprint_c(out, 'r');
		result += 1; my_sprint_c(out, 'r');
	}
#undef BINARY_SIZE
	return result;
}

/*static*/ void my_sprint_c(char** out, int c)
{
	if (*out) {
		**out = (char)c;
		++(*out);
	}
	//else {
	//	os_putchar(c);
	//}
}

/*static*/ size_t my_sprint_s_a(char** out, const char* s, size_t width)
{
	size_t length = string_length_a(s);

	if (length < width)
		width -= length;
	else
		width = 0;

	size_t result = 0;

	result += width;
	while (width--)
	{
		my_sprint_c(out, '0');
	}

	result += length;
	while (length--)
	{
		my_sprint_c(out, *(s++));
	}

	return result;
}

/*static*/ size_t my_sprint_s_w(char** out, const wchar_t* s, size_t width)
{
	size_t length = string_length_w(s);

	if (length < width)
		width -= length;
	else
		width = 0;

	size_t result = 0;

	result += width;
	while (width--)
	{
		my_sprint_c(out, '0');
	}

	result += length;
	while (length--)
	{
		my_sprint_c(out, *(s++));
	}

	return result;
}

/*static*/ size_t my_sprint_i(char** out, long long i, int radix, int is_signed, int letter_base, size_t width)
{
	//1111111111111111111111111111111111111111111111111111111111111111
	char print_buffer[PRINT_BUFFER_LENGTH];
	if (i == 0) {
		print_buffer[0] = '0';
		print_buffer[1] = '\0';
		return my_sprint_s_a(out, print_buffer, width);
	}
	unsigned long long u = i;
	int is_negative = 0;
	if (is_signed && radix == 10 && i < 0) {
		is_negative = 1;
		u = -i;
	}

	char* s = print_buffer;
	s += PRINT_BUFFER_LENGTH - 1;
	*s = '\0';

	while (u) {
		int t = u % radix;
		char c;
		if (t >= 10) {
			c = t - 10 + letter_base;
		}
		else {
			c = t + '0';
		}
		*(--s) = c;
		u /= radix;
	}

	if (is_negative) {
		*(--s) = '-';
	}

	return my_sprint_s_a(out, s, width);
}

/*static*/ size_t my_sprint(char* out, const char* format, va_list args)
{
	size_t result = 0;

	int z_count;

	int h_count;

	int l_count;

	int pad_zero;

	size_t width;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			if (*format == '\0') {
				break;
			}
			if (*format == '%') {
				my_sprint_c(&out, *format);
				result++;
				continue;
			}
			if (*format == 'c') {
				my_sprint_c(&out, (char)va_arg(args, int));
				result++;
				continue;
			}

			if (*format == 'p') {
				result += my_sprint_i(&out, (long long)((void*)va_arg(args, void*)), 16, 0, 'a', sizeof(void*) * 2);
				continue;
			}

			if (*format == 'P') {
				result += my_sprint_i(&out, (long long)((void*)va_arg(args, void*)), 16, 0, 'A', sizeof(void*) * 2);
				continue;
			}

			if (*format == 's') {
				result += my_sprint_s_a(&out, (char*)va_arg(args, char*), 0);
				continue;
			}
			if (*format == 'S') {
				result += my_sprint_s_w(&out, (wchar_t*)va_arg(args, wchar_t*), 0);
				continue;
			}

			pad_zero = 0;
			if (*format == '0') {
				++format;
				pad_zero = 1;
			}

			width = 0;
			if (pad_zero) {
				for (; *format >= '0' && *format <= '9'; ++format) {
					width *= 10;
					width += *format - '0';
				}
			}

			z_count = 0;
			if (*format == 'z') {
				++format;
				++z_count;
			}

			h_count = 0;
			if (*format == 'h') {
				++format;
				++h_count;
			}
			if (*format == 'h') {
				++format;
				++h_count;
			}

			l_count = 0;
			if (*format == 'l') {
				++format;
				++l_count;
			}
			if (*format == 'l') {
				++format;
				++l_count;
			}

			int radix = 0;
			int is_signed = 0;
			int letter_base = 0;

			if (*format == 'd') {
				radix = 10;
				is_signed = 1;
			}
			else if (*format == 'u') {
				radix = 10;
			}
			else if (*format == 'x') {
				radix = 16;
				letter_base = 'a';
			}
			else if (*format == 'X') {
				radix = 16;
				letter_base = 'A';
			}
			else if (*format == 'b') {
				radix = 2;
			}

			if (radix != 0) {
				result += my_sprint_i(&out, get_args_value(z_count, h_count, l_count, &args), radix, is_signed, letter_base, width);
				continue;
			}

			if (*format == 'f') {
				if (l_count == 0) {
					result += my_sprint_f(&out, (float)va_arg(args, double), (int)width);
				}
				else {
					result += my_sprint_lf(&out, (double)va_arg(args, double), (int)width);
				}
				continue;
			}
		}
		else {
			my_sprint_c(&out, *format);
			result++;
		}
	}
	return result;
}

//without '\0'
/*static*/ size_t my_sprintf(char* out, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t result = my_sprint(out, format, args);
	va_end(args);
	return result;
}

size_t my_wsprint_f(wchar_t** out, float f, int precision)
{
	if (precision == 0) {
		precision = 6;
	}

	size_t result = 0;
	unsigned int bits = *(unsigned int*)&f;
	int sign = (bits >> 31) & 1;
	int exponent = ((bits >> 23) & 0xFF);
	unsigned int mantissa = bits & 0x7FFFFF;

	if (sign) {
		result += 1; my_wsprint_c(out, '-');
	}

	if (exponent == 0 && mantissa == 0) {
		result += 1; my_wsprint_c(out, '0');
		result += 1; my_wsprint_c(out, '.');
		result += precision;
		for (int i = 0; i < precision; i++) {
			my_wsprint_c(out, '0');
		}
		return result;
	}
	else if (exponent == 0b0111'1111 && mantissa == 0) {
		result += 1; my_wsprint_c(out, '1');
		result += 1; my_wsprint_c(out, '.');
		result += precision;
		for (int i = 0; i < precision; i++) {
			my_wsprint_c(out, '0');
		}
		return result;
	}
	else if (exponent == 0b1111'1111 && mantissa == 0) {
		result += 1; my_wsprint_c(out, 'i');
		result += 1; my_wsprint_c(out, 'n');
		result += 1; my_wsprint_c(out, 'f');
		return result;
	}
	else if (exponent == 0b1111'1111 && mantissa != 0) {
		result += 1; my_wsprint_c(out, 'n');
		result += 1; my_wsprint_c(out, 'a');
		result += 1; my_wsprint_c(out, 'n');
		return result;
	}

	exponent -= 0b0111'1111;
	mantissa |= (1 << 23);

#define BINARY_SIZE 32
	int decimal_part_start = (1 + 8 + exponent);
	if (0 < decimal_part_start && decimal_part_start < BINARY_SIZE) {
		unsigned char binary[BINARY_SIZE];
		for (size_t i = 0; i < BINARY_SIZE; i++)
		{
			binary[i] = (mantissa >> ((BINARY_SIZE - 1) - i)) & 1;
		}

		long long integer_part = get_integer_part(binary, decimal_part_start);
		long long decimal_part = get_decimal_part(binary + decimal_part_start, BINARY_SIZE - decimal_part_start, precision);

		result += my_wsprint_i(out, integer_part, 10, 0, 0, 0);
		result += 1; my_wsprint_c(out, '.');
		result += my_wsprint_i(out, decimal_part, 10, 0, 0, 0);
	}
	else {
		result += 1; my_wsprint_c(out, 'e');
		result += 1; my_wsprint_c(out, 'r');
		result += 1; my_wsprint_c(out, 'r');
	}
#undef BINARY_SIZE
	return result;
}

size_t my_wsprint_lf(wchar_t** out, double lf, int precision)
{
	if (precision == 0) {
		precision = 6;
	}

	size_t result = 0;

	unsigned long long bits = *(unsigned long long*) & lf;
	int sign = (bits >> 63) & 1;
	int exponent = (bits >> 52) & 0x7FF;
	unsigned long long mantissa = bits & 0xF'FFFF'FFFF'FFFF;

	if (sign) {
		result += 1; my_wsprint_c(out, '-');
	}

	if (exponent == 0 && mantissa == 0) {
		result += 1; my_wsprint_c(out, '0');
		result += 1; my_wsprint_c(out, '.');
		result += precision;
		for (int i = 0; i < precision; i++) {
			my_wsprint_c(out, '0');
		}
		return result;
	}
	else if (exponent == 0b011'1111'1111 && mantissa == 0) {
		result += 1; my_wsprint_c(out, '1');
		result += 1; my_wsprint_c(out, '.');
		result += precision;
		for (int i = 0; i < precision; i++) {
			my_wsprint_c(out, '0');
		}
		return result;
	}
	else if (exponent == 0b111'1111'1111 && mantissa == 0) {
		result += 1; my_wsprint_c(out, 'i');
		result += 1; my_wsprint_c(out, 'n');
		result += 1; my_wsprint_c(out, 'f');
		return result;
	}
	else if (exponent == 0b111'1111'1111 && mantissa != 0) {
		result += 1; my_wsprint_c(out, 'n');
		result += 1; my_wsprint_c(out, 'a');
		result += 1; my_wsprint_c(out, 'n');
		return result;
	}

	exponent -= 0b011'1111'1111;
	mantissa |= (1ULL << 52);

#define BINARY_SIZE 64
	int decimal_part_start = (1 + 11 + exponent);
	if (0 < decimal_part_start && decimal_part_start < BINARY_SIZE) {
		unsigned char binary[BINARY_SIZE];
		for (size_t i = 0; i < BINARY_SIZE; i++) {
			binary[i] = (mantissa >> ((BINARY_SIZE - 1) - i)) & 1;
		}
		long long integer_part = get_integer_part(binary, decimal_part_start);
		long long decimal_part = get_decimal_part(binary + decimal_part_start, BINARY_SIZE - decimal_part_start, precision);

		result += my_wsprint_i(out, integer_part, 10, 0, 0, 0);
		result += 1; my_wsprint_c(out, '.');
		result += my_wsprint_i(out, decimal_part, 10, 0, 0, 0);
	}
	else {
		result += 1; my_wsprint_c(out, 'e');
		result += 1; my_wsprint_c(out, 'r');
		result += 1; my_wsprint_c(out, 'r');
	}
#undef BINARY_SIZE
	return result;
}


/*static*/ void my_wsprint_c(wchar_t** out, int c)
{
	if (*out) {
		**out = (wchar_t)c;
		++(*out);
	}
	//else {
	//	os_putchar(c);
	//}
}

/*static*/ size_t my_wsprint_s_a(wchar_t** out, const char* s, size_t width)
{
	size_t length = string_length_a(s);

	if (length < width)
		width -= length;
	else
		width = 0;

	size_t result = 0;

	result += width;
	while (width--)
	{
		my_wsprint_c(out, '0');
	}

	result += length;
	while (length--)
	{
		my_wsprint_c(out, *(s++));
	}

	return result;
}

/*static*/ size_t my_wsprint_s_w(wchar_t** out, const wchar_t* s, size_t width)
{
	size_t length = string_length_w(s);

	if (length < width)
		width -= length;
	else
		width = 0;

	size_t result = 0;

	result += width;
	while (width--)
	{
		my_wsprint_c(out, '0');
	}

	result += length;
	while (length--)
	{
		my_wsprint_c(out, *(s++));
	}

	return result;
}

/*static*/ size_t my_wsprint_i(wchar_t** out, long long i, int radix, int is_signed, int letter_base, size_t width)
{
	//1111111111111111111111111111111111111111111111111111111111111111
	char print_buffer[PRINT_BUFFER_LENGTH];
	if (i == 0) {
		print_buffer[0] = '0';
		print_buffer[1] = '\0';
		return my_wsprint_s_a(out, print_buffer, width);
	}
	unsigned long long u = i;
	int is_negative = 0;
	if (is_signed && radix == 10 && i < 0) {
		is_negative = 1;
		u = -i;
	}

	char* s = print_buffer;
	s += PRINT_BUFFER_LENGTH - 1;
	*s = '\0';

	while (u) {
		int t = u % radix;
		char c;
		if (t >= 10) {
			c = t - 10 + letter_base;
		}
		else {
			c = t + '0';
		}
		*(--s) = c;
		u /= radix;
	}

	if (is_negative) {
		*(--s) = '-';
	}

	return my_wsprint_s_a(out, s, width);
}

/*static*/ size_t my_wsprint(wchar_t* out, const wchar_t* format, va_list args)
{
	size_t result = 0;

	int z_count;

	int h_count;

	int l_count;

	int pad_zero;

	size_t width;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			if (*format == '\0') {
				break;
			}
			if (*format == '%') {
				my_wsprint_c(&out, *format);
				result++;
				continue;
			}
			if (*format == 'c') {
				my_wsprint_c(&out, va_arg(args, int));
				result++;
				continue;
			}

			if (*format == 'p') {
				result += my_wsprint_i(&out, (long long)((void*)va_arg(args, void*)), 16, 0, 'a', sizeof(void*) * 2);
				continue;
			}

			if (*format == 'P') {
				result += my_wsprint_i(&out, (long long)((void*)va_arg(args, void*)), 16, 0, 'A', sizeof(void*) * 2);
				continue;
			}

			if (*format == 's') {
				result += my_wsprint_s_w(&out, (wchar_t*)va_arg(args, wchar_t*), 0);
				continue;
			}
			if (*format == 'S') {
				result += my_wsprint_s_a(&out, (char*)va_arg(args, char*), 0);
				continue;
			}

			pad_zero = 0;
			if (*format == '0') {
				++format;
				pad_zero = 1;
			}

			width = 0;
			if (pad_zero) {
				for (; *format >= '0' && *format <= '9'; ++format) {
					width *= 10;
					width += *format - '0';
				}
			}

			z_count = 0;
			if (*format == 'z') {
				++format;
				++z_count;
			}

			h_count = 0;
			if (*format == 'h') {
				++format;
				++h_count;
			}
			if (*format == 'h') {
				++format;
				++h_count;
			}

			l_count = 0;
			if (*format == 'l') {
				++format;
				++l_count;
			}
			if (*format == 'l') {
				++format;
				++l_count;
			}

			int radix = 0;
			int is_signed = 0;
			int letter_base = 0;

			if (*format == 'd') {
				radix = 10;
				is_signed = 1;
			}
			else if (*format == 'u') {
				radix = 10;
			}
			else if (*format == 'x') {
				radix = 16;
				letter_base = 'a';
			}
			else if (*format == 'X') {
				radix = 16;
				letter_base = 'A';
			}
			else if (*format == 'b') {
				radix = 2;
			}

			if (radix != 0) {
				result += my_wsprint_i(&out, get_args_value(z_count, h_count, l_count, &args), radix, is_signed, letter_base, width);
				continue;
			}

			if (*format == 'f') {
				if (l_count == 0) {
					result += my_wsprint_f(&out, (float)va_arg(args, double), (int)width);
				}
				else {
					result += my_wsprint_lf(&out, (double)va_arg(args, double), (int)width);
				}
				continue;
			}
		}
		else {
			my_wsprint_c(&out, *format);
			result++;
		}
	}
	return result;
}

//without '\0'
/*static*/ size_t my_wsprintf(wchar_t* out, const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t result = my_wsprint(out, format, args);
	va_end(args);
	return result;
}

#undef PRINT_BUFFER_LENGTH