/*
 * png.c -- png texture loader
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

// neagix: this code was modified for inclusion in rpi2d

#include "pngread.h"

static void GetPNGtextureInfo(int color_type, struct gl_texture_t *texinfo) {
    switch (color_type) {
//        case PNG_COLOR_TYPE_GRAY:
//            texinfo->PixelFormat = GL_LUMINANCE;
//            texinfo->BytesPerPixel = 1;
//            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            texinfo->PixelFormat = GL_LUMINANCE_ALPHA;
            texinfo->BytesPerPixel = 2;
            break;

        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_RGB:
            texinfo->PixelFormat = GL_RGB;
            texinfo->BytesPerPixel = 3;
            break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            texinfo->PixelFormat = GL_RGBA;
            texinfo->BytesPerPixel = 4;
            break;

        default:
            /* Badness */
            break;
    }
}

struct gl_texture_t *loadPNGTexture(const char *filename) {
    struct gl_texture_t texinfo;
    png_byte magic[8];
    png_structp png_ptr;
    png_infop info_ptr;
    int color_type;
    FILE *fp = NULL;
    png_bytep *row_pointers = NULL;
    png_uint_32 w, h;
    int i;

    /* Open image file */
    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "error: couldn't open \"%s\"!\n", filename);
        return NULL;
    }

    /* Read magic number */
    fread(magic, 1, sizeof (magic), fp);

    /* Check for valid magic number */
    if (!png_check_sig(magic, sizeof (magic))) {
        fprintf(stderr, "error: \"%s\" is not a valid PNG image!\n",
                filename);
        fclose(fp);
        return NULL;
    }

    /* Create a png read struct */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return NULL;
    }

    /* Create a png info struct */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }


    /* Initialize the setjmp for returning properly after a libpng
       error occured */
    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        if (row_pointers)
            free(row_pointers);

        return NULL;
    }

    /* Setup libpng for using standard C fread() function
       with our FILE pointer */
    png_init_io(png_ptr, fp);

    /* Tell libpng that we have already read the magic number */
    png_set_sig_bytes(png_ptr, sizeof (magic));

    /* Read png info */
    png_read_info(png_ptr, info_ptr);

    /* Get some usefull information from header */
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);

    /* Convert index color images to RGB images */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    /* Convert 1-2-4 bits grayscale images to RGB */
    if (color_type == PNG_COLOR_TYPE_GRAY) {
        //TODO: check if this is correct
        texinfo.BytesPerPixel = 3;
        png_set_gray_to_rgb(png_ptr);
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    else if (bit_depth < 8)
        png_set_packing(png_ptr);

    /* Update info structure to apply transformations */
    png_read_update_info(png_ptr, info_ptr);

    /* Retrieve updated information */
    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth,
            &color_type, NULL, NULL, NULL);
    texinfo.Width = w;
    texinfo.Height = h;

    /* Get image format and components per pixel */
    GetPNGtextureInfo(color_type, &texinfo);

    /* We can now allocate memory for storing pixel data */
    /* Create our OpenGL texture object */
    struct gl_texture_t *s_texinfo = (struct gl_texture_t *) malloc(TEXTURE_SIZE(texinfo));

    // copy first part of the structure
    memcpy(s_texinfo, &texinfo, sizeof(struct gl_texture_t) - 4);

    /* Setup a pointer array.  Each one points at the beginning of a row. */
    row_pointers = (png_bytep *) malloc(sizeof (png_bytep) * texinfo.Height);

    for (i = 0; i < texinfo.Height; ++i) {
        row_pointers[i] = (png_bytep) (s_texinfo->texels +
                ((texinfo.Height - (i + 1)) * texinfo.Width * texinfo.BytesPerPixel));
    }

    /* Read pixel data using row pointers */
    png_read_image(png_ptr, row_pointers);

    /* Finish decompression and release memory */
    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    /* We don't need row pointers anymore */
    free(row_pointers);

    fclose(fp);

//    if (bit_depth / 8 != texinfo->BytesPerPixel)
//        fprintf(stderr, "pngread: bit depth is %d, but bytes per pixel are %d\n", bit_depth, texinfo->BytesPerPixel);

    return s_texinfo;
}
