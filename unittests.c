#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tests/utest.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "recon.h"
#include "utils.h"

#define INT 0
#define DOUBLE 1

scan * read_bin(char *filename, int dtype);
double avg_error_scan(scan *x, scan *y);
double avg_error_im(image *x, image *y);
image * read_image(char *filename);

    
void print_array(image *a)
{
    for (int i = 0; i < a->ny; ++i)
    {
	for (int j = 0; j < a->nx; ++j)
	{
	    printf("%i ",a->data[i*a->nx + j]);
	}
	printf("\n");
    }

}


UTEST(utils, read_data)
{
    scan *data = read_data("P26113.7");

    ASSERT_EQ(data->ny, 256);
    ASSERT_EQ(data->nx, 256);
    ASSERT_EQ(data->ncoils, 16);
    ASSERT_EQ(data->exam, 2020110305);
    ASSERT_EQ(data->data[0], 82851.);
    ASSERT_EQ(data->data[11], -71551.);

    free(data);
    data = NULL;
}

UTEST(recon, ifft_2D)
{
    int out = system("python3 tests/test_ifft.py");
    (void)out;

    scan *data = read_bin("data.bin", INT);
    scan *im = read_bin("im.bin", DOUBLE);
    
    ifft_2D(data);

    double error = avg_error_scan(data,im);

    ASSERT_LT(error,1e-1);

    free(data);
    free(im);
    data = NULL;
    im = NULL;

    out = system("rm data.bin im.bin");
    (void)out;

}

UTEST(recon, sum_of_squares)
{

    int out = system("python3 tests/test_sos.py");
    (void)out;

    scan *data = read_bin("data.bin", DOUBLE);
    scan *im_temp = read_bin("im.bin", INT);

    image *im = malloc(sizeof(*im) +
		       sizeof(unsigned char)*
		       (size_t)(im_temp->nx*im_temp->ny));

    image *im_sos = sum_of_squares(data);
    
    for (int i = 0; i < im_temp->nx*im_temp->ny; ++i)
    {
	im->data[i] = (unsigned char)im_temp->data[2*i];
    }
    
    im->ny = im_sos->ny;
    im->nx = im_sos->nx;

    double error = avg_error_im(im, im_sos);

    ASSERT_LT(error,1e-3);

    free(data);
    free(im_temp);
    free(im);
    free(im_sos);

    data = NULL;
    im_temp = NULL;
    im = NULL;
    im_sos = NULL;

    out = system("rm data.bin im.bin");
    (void)out;
}


UTEST(recon, ifftshift)
{

    unsigned char A_array[] = {
	1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12,
	13, 14, 15, 16, 17, 18,
	19, 20, 21, 22, 23, 24
    };

    image *A = malloc(sizeof(*A) +
		      sizeof(A_array));
    A->ny = 4;
    A->nx = 6;
    memcpy(A->data, A_array, sizeof(A_array));

    unsigned char B_array[] = {
	16, 17, 18, 13, 14, 15,
	22, 23, 24, 19, 20, 21,
	4, 5, 6, 1, 2, 3,
	10, 11, 12, 7, 8, 9
    };

    image *B = malloc(sizeof(*B) +
		      sizeof(B_array));
    B->ny = 4;
    B->nx = 6;
    memcpy(B->data, B_array, sizeof(B_array));
    
    ifftshift(A);

    for (int i = 0; i < 4*6; ++i)
    {
	ASSERT_EQ(A->data[i],B->data[i]);
    }

    free(A);
    free(B);
    A = NULL;
    B = NULL;
}

UTEST(recon, transpose)
{

    unsigned char A_array[] = {
	1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12,
	13, 14, 15, 16, 17, 18,
	19, 20, 21, 22, 23, 24
    };

    image *A = malloc(sizeof(*A) +
		      sizeof(A_array));
    A->ny = 4;
    A->nx = 6;
    memcpy(A->data, A_array, sizeof(A_array));

    unsigned char B_array[] = {
	1, 7, 13, 19,
	2, 8, 14, 20,
	3, 9, 15, 21,
	4, 10, 16, 22,
	5, 11, 17, 23,
	6, 12, 18, 24	
    };

    image *B = malloc(sizeof(*B) +
		      sizeof(B_array));
    B->ny = 6;
    B->nx = 4;
    memcpy(B->data, B_array, sizeof(B_array));
    
    image *A_T = transpose(A);

    ASSERT_EQ(A_T->ny, B->ny);
    ASSERT_EQ(A_T->nx, B->nx);

    for (int i = 0; i < 4*6; ++i)
    {
	ASSERT_EQ(A_T->data[i],B->data[i]);
    }

    free(A);
    free(B);
    free(A_T);
    A = NULL;
    B = NULL;
    A_T = NULL;
}


UTEST(main, usage)
{
    int out = system("./reconstruction > tmp_1.txt");

    ASSERT_EQ(out, 0);

    FILE *fp = fopen("tmp_2.txt","w");

    fprintf(fp,"Usage:\n    $ ./reconstruction <pfile-in>\n");

    fclose(fp);

    out = system("diff -w tmp_1.txt tmp_2.txt");

    ASSERT_EQ(out, 0);

    int ret = system("rm tmp_1.txt tmp_2.txt");
    (void)ret;
    
}


UTEST(main, output)
{
    
    int out = system("./reconstruction  "
		     "P26113.7 > tmp_1.txt");
    ASSERT_EQ(out, 0);


    FILE *fp = fopen("tmp_2.txt","w");
    fprintf(fp,"exam: 2020110305, ncoils: 16, ny: 256, nx: 256\n"
	    "successfully wrote: image_exam_2020110305.jpg");
    fclose(fp);
    
    out = system("diff -w tmp_1.txt tmp_2.txt");
    
    ASSERT_EQ(out, 0);



    image *im_test = read_image("image_exam_2020110305.jpg");
    image *im_sol = read_image("tests/sol.jpg");

    double error = avg_error_im(im_test, im_sol);
    ASSERT_LT(error, 10e-6);

    free(im_test);
    free(im_sol);
    im_test = NULL;
    im_sol = NULL;

    int ret = system("rm tmp_1.txt tmp_2.txt "
		     "image_exam_2020110305.jpg");
    (void)ret;
    
}



UTEST_MAIN();

scan * read_bin(char *filename, int dtype)
{
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
	printf("ERROR:  file %s is not found\n", filename);
	exit(1);
    }

    int ncoils=0, ny=0, nx=0;

    fseek(fp, 0, SEEK_SET);
    fread(&ncoils, sizeof(int), 1, fp);
    fread(&ny, sizeof(int), 1, fp);
    fread(&nx, sizeof(int), 1, fp);

    int size = ncoils*ny*nx;

    scan *data = malloc(sizeof(*data) +
			sizeof(double)*(size_t)(2*size));

    data->ny = ny;
    data->nx = nx;
    data->ncoils = ncoils;

    if (dtype == INT)
    {
	int *data_raw = (int*)calloc((size_t)(2*size),
				     sizeof(int));
	
	fread(data_raw, sizeof(int), (size_t)(2*size), fp);

	for (int i = 0; i < 2*size; ++i)
	{
	    data->data[i] = (double)data_raw[i];
	}
	
	free(data_raw);
	data_raw = NULL;
    }
    else if (dtype == DOUBLE)
    {

	fread(data->data, sizeof(double), (size_t)(2*size), fp);
	
    }
    else
    {
	printf("ERROR:  type not recognized!");
	exit(1);
    }

    fclose(fp);
    

    
    return data;

}


double avg_error_scan(scan *y, scan *x)
{
    double diff = 0;
    for (int i = 0; i < (y->nx)*(y->ny)*(y->ncoils); i++)
    {
	diff += fabs(x->data[i] - y->data[i]);
    }

    return diff/(y->nx*y->ny*y->ncoils);
}

double avg_error_im(image *y, image *x)
{
    double diff = 0;
    for (int i = 0; i < y->nx*y->ny; i++)
    {
	diff += abs(x->data[i] - y->data[i]);
    }

    return diff/(y->nx*y->ny);
}


image * read_image(char *filename)
{
    int width, height, channels;
    
    unsigned char *im_char = stbi_load(filename, &width,
				       &height, &channels, 0);

    image *im = malloc(sizeof(*im) +
		       sizeof(unsigned char)*(size_t)(width*height));

    im->ny = height;
    im->nx = width;

    memcpy(im->data, im_char,
	   sizeof(unsigned char)*(size_t)(width*height));

    free(im_char);
    im_char = NULL;
    
    return im;
}

