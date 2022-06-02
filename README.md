# Treatment for Sudoku OCR
Image Treatment for an OCR type sudoku solver, done as part of a project in S3 at the EPITA.

It is written in C and uses SDL 1.2.

Though the game wasn't originally intended to be standalone, you can try it out.

# Compiling
```bash
make
```

# Usage
```bash
./image <file>
```
Default file is `my_image.jpg`.

Each filter used is applied individually, and you can apply the next one by pressing any key.

At the end of the program, the grid is cut in 81 squares and each square is saved is the current directory in the Bitmap format.

# Filters
The filters applied are, in order :

## Pretreatment
- Grayscale (`grayscale`)
- Contrast (`contrast`)
- Lighting Standardization (`normalize`)
- Median Filter (`median`)
- Gaussian Blur (`blur`)
- Adaptative Thresholding (`adaptative`) - from [Derek Bradley and Gerhard Roth's work](https://people.scs.carleton.ca/~roth/iit-publications-iti/docs/gerh-50002.pdf)
- Dilatation x3 (`dilatation`) (note that my dilatation doesn't fully dilated each pixel, but only to the right ; it was originally a mistake, but as it worked well with the rest of my steps, I kept it as is, but it is easily fixable if needed)
- Sobel Filter (`sobel`)
- Dilatation x3 (`dilatation`)

## Treatment
- Automatic rotation (using Hough Transform)
- Centering on the grid (using Hough Transform)
- Grid detection (using intensity histograms)
- Cutting of the grid

# Hough Transform Thresholding
To find the detection threshold of the lines of the Hough Transform, I set it as the diagonal multiplied by a factor and then count the number of lines above it.
Now, if this count divided by the diagonal is between 0.117 and 0.123, then the threshold if correct. Else, I adjust the factor to get closer to this interval.

Note that this specific interval works with my pretreatment, but probably will not work with a different one : for example, when I only changed my dilatation, the interval wasn't right at all. Try multiple intervals to find the one that fits your pretreatment the most.

# Contact
As this project was made through an entire semester while I was learning C, the code might be a bit messy. If needed, you can contact me at <nolan.djellal@gmail.com> for more information and explanations.
