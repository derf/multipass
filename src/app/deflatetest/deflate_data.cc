// CINFO = 7  (window size = 32768)
// FDICT = 0 (no preset dictionary)
// first Huffman block: type=2 is_final=1
// src/app/ledblink/main.cc
unsigned char const inflate_input[] = {120, 156, 109, 144, 65, 79, 195, 48, 12, 133, 207, 203, 175, 48, 229, 210, 77, 148, 150, 29, 217, 78, 172, 32, 38, 129, 132, 232, 36, 184, 85, 93, 227, 118, 22, 94, 82, 37, 233, 36, 132, 248, 239, 164, 101, 208, 30, 122, 74, 108, 127, 121, 239, 57, 241, 66, 192, 2, 54, 186, 249, 52, 84, 31, 28, 44, 147, 101, 2, 105, 161, 8, 25, 30, 12, 161, 69, 246, 64, 199, 100, 47, 233, 123, 244, 68, 37, 42, 139, 209, 86, 162, 114, 84, 17, 154, 91, 184, 203, 210, 104, 25, 109, 184, 104, 45, 122, 48, 22, 151, 164, 74, 110, 37, 66, 80, 152, 242, 112, 125, 8, 70, 29, 105, 232, 132, 38, 174, 27, 210, 147, 3, 235, 164, 110, 221, 228, 168, 109, 28, 29, 177, 27, 137, 147, 38, 9, 172, 117, 19, 118, 183, 185, 248, 18, 179, 94, 145, 81, 230, 78, 215, 53, 99, 120, 51, 95, 121, 137, 74, 98, 5, 187, 237, 243, 253, 107, 158, 137, 217, 135, 151, 134, 245, 26, 36, 150, 221, 113, 22, 172, 209, 229, 54, 156, 119, 29, 84, 146, 253, 51, 100, 191, 201, 63, 29, 236, 17, 27, 216, 123, 183, 96, 204, 40, 73, 149, 248, 22, 130, 148, 131, 99, 65, 106, 136, 210, 111, 109, 209, 181, 77, 232, 67, 252, 38, 27, 202, 78, 118, 40, 71, 193, 181, 10, 147, 63, 160, 247, 125, 68, 102, 125, 5, 111, 218, 176, 188, 24, 121, 15, 196, 14, 173, 155, 0, 206, 17, 72, 50, 230, 253, 47, 245, 70, 198, 123, 26, 5, 201, 202, 167, 254, 1, 87, 177, 154, 147};
unsigned short const inflate_input_size = 288;
unsigned char deflate_output[504];
unsigned short const deflate_output_size = 504;
