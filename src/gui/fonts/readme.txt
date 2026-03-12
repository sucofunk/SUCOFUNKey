The Original Font is Bai Jamjuree, downloaded here: https://fonts.google.com/specimen/Bai+Jamjuree?query=jamjure#standard-styles
Changed Digits to be monospaced with this tool: https://www.glyphrstudio.com/online/
Converted OTF to TTF File here: https://convertio.co
Converted TTF to gfx here: https://rop.nl/truetype2gfx/

---
64bit icons: https://onlinegdb.com/mYTstKODV

code:

#include <iostream>
#include <stdint.h>

const char *bin = "1111111101111110001111000001100000000000000000001111111100000000";


uint64_t binary_string_to_uint64(const char *binary)
{
	uint64_t result = 0;

	for (int i = 0; i < 64; i++) {
		result <<= 1;

		if (binary[i] == '1') {
			result |= 1ULL;
		}
	}

	return result;
}

int get_bit_at_position(uint64_t icon, uint8_t position)
{
	if (position > 63) {
		return 0; // false
	}

	return (icon >> position) & 1ULL;
}

int main()
{
	uint64_t b = binary_string_to_uint64(bin);
	std::cout<<"Icon as number: ";
	std::cout<<b;
	std::cout<<"\n\n";

	for (int y=7; y>=0; y--) {
		for (int x=7; x>=0; x--) {
			std::cout<<get_bit_at_position(b, y*8+x);
		}
		std::cout<<"\n";
	}


	return 0;
}

---
