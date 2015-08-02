/*
*	###################################
*	###	      bitlib.h		###
*	###     (c) William Thenaers	###
*	###################################
*
*	Contains all possible bit operations.
*/
#ifndef BITLIB_H
#define BITLIB_H

#include <stdio.h>
#include <stdlib.h>

/*
*	Define a new variable with maximum size = 64bits
*	in range: [0x0000000000000000, 0xFFFFFFFFFFFFFFFF]
*		from bit 1 to 64
*/
typedef unsigned long long int bit;

/*
*	The size of <bit>.
*/
const size_t BIT_SIZE = sizeof(bit) * 8;

/*
*	Define a new variable of type enum as Boolean (false = 0, true = 1).
*/
typedef enum { false, true } bool;

/*
*	Shift given bit <n> times to the left.
*/
bit Bit_shiftLeft(bit b, size_t n) {
	return b << n;
}

/*
*	Shift given bit <n> times to the right.
*/
bit Bit_shiftRight(bit b, size_t n) {
	return b >> n;
}

/*	
*	OR 2 given bits.
*
*	b1	|	b2	|	out
*  -------------|---------------|---------------
*	0	|	0	|	0
*	0	|	1	|	1
*	1	|	0	|	1
*	1	|	1	|	1
*/
bit Bit_OR(bit b1, bit b2) {
	return b1 | b2;
}

/*
*	XOR 2 given bits.
*
*	b1	|	b2	|	out
*  -------------|---------------|---------------
*	0	|	0	|	0
*	0	|	1	|	1
*	1	|	0	|	1
*	1	|	1	|	0
*/
bit Bit_XOR(bit b1, bit b2) {
	return b1 ^ b2;
}

/*
*	AND 2 given bits.
*
*	b1	|	b2	|	out
*  -------------|---------------|---------------
*	0	|	0	|	0
*	0	|	1	|	0
*	1	|	0	|	0
*	1	|	1	|	1
*/
bit Bit_AND(bit b1, bit b2) {
	return b1 & b2;
}

/*
*	Return a single-bit-mask for the <n>th bit.
*	Ensure <n> fits inside bit range by clamping between [1, BIT_SIZE]
*/
bit Bit_mask1(size_t n) {
	n = (n < 1 ? 1 : n > BIT_SIZE ? BIT_SIZE : n);
	return Bit_shiftLeft(1, --n);
}

/*
*	Return <b> but with the <n>th bit is on.
*/
bit Bit_turnOn(bit b, size_t n) {
	return Bit_OR(b, Bit_mask1(n));
}

/*
*	Return <b> but with the <n>th bit is off.
*/
bit Bit_turnOff(bit b, size_t n) {
	return Bit_AND(b, ~Bit_mask1(n));
}

/*
*	Return <b> but with the <n>th bit is toggled (negated).
*/
bit Bit_toggle(bit b, size_t n) {
	return Bit_XOR(b, Bit_mask1(n));
}

/*
*	Return the <n>th bit of <b>.
*/
bool Bit_get(bit b, size_t n) {
	return Bit_AND(b, Bit_mask1(n)) != 0;
}

/*
*	Return <b> but with every bit toggled (negated).
*/
bit Bit_toggleAll(bit b) {
	return ~b;
}

/*
*	Return the <n> bits on the left only (rest zeroed).
*/
bit Bit_filterLeft(bit b, size_t n) {
	return Bit_shiftLeft(Bit_shiftRight(b, BIT_SIZE - n), BIT_SIZE - n);
}

/*
*	Return the <n> bits on the right only (rest zeroed).
*/
bit Bit_filterRight(bit b, size_t n) {
	return Bit_shiftRight(Bit_shiftLeft(b, BIT_SIZE - n), BIT_SIZE - n);
}

/*
*	Return the bits between <from> and <to> only (rest zeroed).
*/
bit Bit_filterSectionIncl(bit b, size_t from, size_t to) {
	return Bit_filterLeft(Bit_filterRight(b, to), BIT_SIZE - from);
}

/*
*	Return the bits, except those between <from> and <to> (zeroed).
*/
bit Bit_filterSectionExcl(bit b, size_t from, size_t to) {
	return ~Bit_filterSectionIncl(b, from, to);
}

/*
*	Return the bits between <from> and <to> only (rest zeroed), and shift to LSB.
*/
bit Bit_getSection(bit b, size_t from, size_t to) {
	return Bit_shiftRight(Bit_filterSectionIncl(b, from, to), from);
}

/*
*	Return <b> but with <add> concatted to the left (and shift 1 right).
*/
bit Bit_addLeft(bit b, bool add) {
	return Bit_OR(Bit_shiftRight(b, 1), add ? Bit_mask1(BIT_SIZE) : 0);
}

/*
*	Return <b> but with <left> concatted to the left (and shift <n> right).
*/
bit Bit_addBitsLeft(bit b, bit left, size_t n) {
	return Bit_OR(Bit_shiftRight(b, n), Bit_shiftLeft(Bit_filterRight(left, n), BIT_SIZE - n));
}

/*
*	Return <b> but with <add> concatted to the right (and shift 1 left).
*/
bit Bit_addRight(bit b, bool add) {
	return Bit_OR(Bit_shiftLeft(b, 1), add);
}

/*
*	Return <b> but with <right> concatted to the right (and shift <n> left).
*/
bit Bit_addBitsRight(bit b, bit right, size_t n) {
	return Bit_OR(Bit_shiftLeft(b, n), Bit_filterRight(right, n));
}

/*
*	Return the MSB and remove it by shifting <b> left by 1.
*/
bool Bit_getAndRemoveLeft(bit *b) {
	bool r = Bit_get(*b, BIT_SIZE);
	*b <<= 1;
	return r;
}

/*
*	Return the LSB and remove it by shifting <b> right by 1.
*/
bool Bit_getAndRemoveRight(bit *b) {
	bool r = Bit_get(*b, 1);
	*b >>= 1;
	return r;
}

/*
*	Return <b> in reverse order (MSB becomes LSB and vica versa).
*/
bit Bit_reverse(bit b) {
	size_t i = BIT_SIZE;
	bit rev = 0;
	while (i--) rev = Bit_addRight(rev, Bit_getAndRemoveRight(&b));
	return rev;
}

/*
*	Return <b> but with <n> bits from the left concatted to the right (and shift <n> left).
*/
bit Bit_rotateLeft(bit b, size_t n) {
	return Bit_addBitsRight(b, Bit_getSection(b, BIT_SIZE - n, BIT_SIZE), n);
}

/*
*	Return <b> but with <n> bits from the right concatted to the left (and shift <n> right).
*/
bit Bit_rotateRight(bit b, size_t n) {
	return Bit_addBitsLeft(b, Bit_getSection(b, 0, n), n);
}

/*
*	Return the first occurence of a 1-bit.
*/
size_t Bit_getFirst1(bit b) {
	size_t i = (b > 0); //0
	while (b && !Bit_getAndRemoveRight(&b)) i++;
	return i; //return i + (i > 0 || b != 0);
}

/*
*	Return the amount of 1-bits.
*/
size_t Bit_count1(bit b) {
	size_t i = 0;
	while (b) if (Bit_getAndRemoveRight(&b)) i++;
	return i;
}

/*
*	Return the bit required to make an even parity.
*/
bool Bit_getEvenParityBit(bit b) {
	return Bit_count1(b) % 2;
}

/*
*	Return the size of <b> (== last occurence of 1-bit).
*/
size_t Bit_getSize(bit b) {
	size_t i = (b ? BIT_SIZE : 0);
	while (b && !Bit_getAndRemoveLeft(&b)) i--;
	return i;
}

/*
*	Return new bit parsed from string (only '1' and '0' are considered, rest is skipped),
*	with the first char being the MSB.
*/
bit Bit_fromStringMSB(char* s) {
	bit b = 0;
	size_t i = 0;
	
	while (s[i] != '\0') {
		if (s[i] == '1' || s[i] == '0')	
			b = Bit_addRight(b, s[i] - '0');
		i++;
	}

	return b;
}

/*
*	Return new bit parsed from string (only '1' and '0' are considered, rest is skipped),
*	with the first char being the LSB.
*/
bit Bit_fromStringLSB(char* s) {
	return Bit_reverse(Bit_fromStringMSB(s));
}

/*
*	Print <b> to the console in binary format.
*	inNibbles	Whether to print <b>in nibbles (groups of 4 bits).
*/
void Bit_printBinNibble(bit b, bool inNibbles) {
	size_t i = BIT_SIZE;
	while (i--) {
		printf("%d", Bit_getAndRemoveLeft(&b));
		if (inNibbles && !(i % 4))	putchar(' ');
	}
}

/*
*	Print <b> to the console in binary format.
*/
void Bit_printBin(bit b) {
	Bit_printBinNibble(b, false);
}

/*
*	Return a new string parsed from a given bit <b>,
*	with the MSB bit as first character.
*/
char* Bit_toStringBinMSB(bit b) {
	size_t i = BIT_SIZE;
	char* s = (char*) malloc(i * sizeof(char));
	s[i] = '\0';

	while (i--) s[i] = (Bit_getAndRemoveRight(&b) ? '1' : '0');

	return s;
}

/*
*	Return a new string parsed from a given bit <b>,
*	with the LSB bit as first character.
*/
char* Bit_toStringBinLSB(bit b) {
	return Bit_toStringBinMSB(Bit_reverse(b));
}


/*
*	Print <b> to the console in hexadecimal format.
*		with:	#	print 0x in front
*				0	fill front with zeros
*				18	= 16 hexbits + 2 front chars  to print
*				ll	long long size
*				X	print hex value with capitals
*/
void Bit_printHex(bit b) {
	printf("%#018llX", b);
}

#endif
