# LSB-image-Steganography-Encoding-and-Decoding

 This project implements LSB (Least Significant Bit) based Steganography to securely hide and retrieve confidential data inside a BMP image.

Features:
1. Encodes any text/script file (.txt /.c /.cpp / .py etc..) inside a BMP image.
2. Uses LSB bit-level encoding without affecting the visible image quality.
3. Supports decoding to extract the hidden secret data from the stego image.
4. Ensures data integrity by using a Magic String based validation.
5. Capacity check is performed before encoding to avoid data overflow.
6. Includes informative logs, argument validation and error handling.

Components:
▪ Encoding  — Hides secret data into the image.
▪ Decoding  — Recovers the hidden data back from the image.
▪ Custom CLI interface supporting:
        -e  for encoding operation
        -d  for decoding operation

Output:
Generates a new BMP file (stego image) with encoded data during encoding and restores the original secret file during decoding.
