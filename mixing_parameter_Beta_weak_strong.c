/************ Mixing parameter calculation by Doucet et al. *****************/
/* Doucet, J., F. Bertrand, and J. Chaouki. */
/* "A measure of mixing from Lagrangian tracking and its application to granular and fluid flow systems." */
/* Chemical Engineering Research and Design 86.12 (2008): 1313-1321.*/

/**********Developed at Dr. Mahesh V. Panchagnula and Dr. Srikanth Vedantam's group at IIT Madras**************/
/**********Fluid Mechanics, Department of Applied Mechanics, IIT Madras**************/
/**********Final C code developed by Sam Mathew (sam<dot>cfd<dot>iitm<at>gmail<dot>com) **************/

/***********This code is covered under GNU Public licensing*************/

/*------------------------------------------------------------------------------

# License

#
# This is a free code: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This mixing parameter code is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
#
#------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define core 6084
#define total_files 21

void svdlapack(double **a, int row, int col, double **u, double **s, double **v);
void printmatrix(char *var, double **a, int row, int col);
double *array1_create(int len);
double **array2_create(int row, int col);
void array1_free(double *arr);
void array2_free(double **arr);


main()
{
    int row = 2, col = 2, i, j, k, l, m, spatial_dim = 2, size_dim = 1;
    double **a, **b, **u, **s, **v, **M;
    float X[core][6], X0[core][6], energy_file_data[total_files][6], mean[3], std[3], C_ws[2][2], C_ss[2][3], C_ws_t[2][2], C_ss_t[3][2];
    char time_data[50];
    float time, cycle, energy, t_settling = 10.0, duration;
    clock_t t_start = clock();
    FILE *fda, *mx, *ene_FILE;

    ene_FILE = fopen("energy.txt","r");
    if (ene_FILE != NULL)
        for (j = 0; j < total_files; j++)
            for (k=0; k<6;k++)
                fscanf(ene_FILE, "%f", &energy_file_data[j][k]);

    fclose(ene_FILE);

    M = array2_create(row, row);
    u = array2_create(row, row);
    s = array2_create(row, col);
    v = array2_create(col, col);

    mx = fopen("Mixing_ws_ss.txt","w");
    fclose(mx);
    mx = fopen("Mixing_ws_ss.txt","a");
    fprintf(mx, "#Time\tCycle\tEnergy\tBeta_ws\tAlphaX_ws\tAlphaY_ws\tBeta_ss\tAlphaX_ss\tAlphay_ss\n");

    for (i=0; i<total_files; i++)
        {
        for (k=0; k < spatial_dim + size_dim; k++)
            {
            mean[k] = 0.0;
            std[k] = 0.0;
            }

        time = (float)i*0.05;
        if (time < t_settling)
            cycle = 0.0;
        else
            cycle = (time - t_settling)*0.5/0.4;

        energy = energy_file_data[i][2];
        snprintf(time_data,sizeof time_data, "./particle_data/interior_%.2f.txt",time);
        fda = fopen(time_data,"r");
        for (j=0; j<core;j++)
            for (k=0; k<6;k++)
                fscanf(fda, "%f", &X[j][k]);
        fclose(fda);

        for (j=0; j<core;j++)
            for (k = 0; k < spatial_dim + size_dim; k++)
                mean[k] += X[j][k];

        for (k = 0; k < spatial_dim + size_dim; k++)
            mean[k] /= core;

        for (j=0; j<core;j++)
            for (k = 0; k < spatial_dim + size_dim; k++)
                std[k] += (X[j][k] - mean[k])*(X[j][k] - mean[k]);

        for (k = 0; k < spatial_dim + size_dim; k++)
            {
            std[k] /= (core -1);
            std[k] = sqrt(std[k]);
            }


        for (j=0; j<core;j++)
            for (k = 0; k < spatial_dim + size_dim; k++)
                X[j][k] = (X[j][k] - mean[k])/std[k];   // Mean normalization and scaling

        if (i == 0 || i == 1)                                         // Storing X0 for comparison with future spatial positions
            {
            for (j=0; j<core;j++)
                for (k = 0; k < spatial_dim + size_dim; k++)
                    X0[j][k] = X[j][k];
            }

        // Initializing the Correlation matrix
        for (l=0; l<spatial_dim;l++)
            for (m=0; m<spatial_dim + size_dim;m++)
                {
                C_ss[l][m] = 0.0;
                if (m<2) C_ws[l][m] = 0.0;
                }
        // Calculating the Correlation matrix
        for (l=0; l<spatial_dim;l++)
            for (m=0; m<spatial_dim + size_dim;m++)
                for (j=0; j<core;j++)
                    {
                    C_ss[l][m] += X[j][l]*X0[j][m];
                    }
        // Calculating the mean
        for (l=0; l<spatial_dim;l++)
            for (m=0; m<spatial_dim + size_dim;m++)
                {
                C_ss[l][m] /= core;
                if (m<2) C_ws[l][m] = C_ss[l][m]; // Since C_ws has the same elements as C_ss, except for the last column
                }
        // Calculating the transpose
        for (l=0; l<spatial_dim;l++)
            for (m=0; m<spatial_dim + size_dim;m++)
                {
                C_ss_t[m][l] = C_ss[l][m];
                if (m<2) C_ws_t[m][l] = C_ws[l][m];
                }

        // Calculating M for beta_ws
            // Initializing M
        M = array2_create(row, row);


        for (j=0; j<spatial_dim;j++)
            for (k=0; k<spatial_dim;k++)
                M[j][k] = 0.0;

            // Updating M through matrix multiplication of C and C_transpose
        for (j=0; j<spatial_dim;j++)
            for (k=0; k<spatial_dim;k++)
                for (l=0;l<spatial_dim;l++) // l is the dimensions of correlation matrix. For beta_ws, only spatial (2) dimensions
                    M[j][k] += C_ws[j][l]*C_ws_t[l][k];

        u = array2_create(row, row);
        s = array2_create(row, col);
        v = array2_create(col, col);

        svdlapack(M, row, col, u, s, v);
        fprintf(mx, "%f\t%f\t%f\t%f\t", time, cycle, energy, sqrt(s[0][0]/spatial_dim));
        for (j=0;j<spatial_dim;j++)
            fprintf(mx, "%f\t", u[j][0]);

        array2_free(M);
        array2_free(u);
        array2_free(v);
        array2_free(s);
        
        // Calculating M for beta_ss
            // Initializing M
        M = array2_create(row, row);

        for (j=0; j<spatial_dim;j++)
            for (k=0; k<spatial_dim;k++)
                M[j][k] = 0.0;

            // Updating M through matrix multiplication of C and C_transpose
        for (j=0; j<spatial_dim;j++)
            for (k=0; k<spatial_dim;k++)
                for (l=0;l<spatial_dim + size_dim;l++) // l is the dimensions of correlation matrix. For beta_ss, spatial (2) + size (1) dimensions
                    M[j][k] += C_ss[j][l]*C_ss_t[l][k];   
        u = array2_create(row, row);
        s = array2_create(row, col);
        v = array2_create(col, col);
        svdlapack(M, row, col, u, s, v);


        fprintf(mx, "%f", sqrt(s[0][0]/(spatial_dim + size_dim)));
        for (j=0;j<spatial_dim;j++)
            fprintf(mx, "\t%f", u[j][0]);
        fprintf(mx, "\n");


        array2_free(M);
        array2_free(u);
        array2_free(v);
        array2_free(s);


        }
    fclose(mx);

    duration = (double)(clock() - t_start) / CLOCKS_PER_SEC;
    printf("\nCalculation time = %.2lf\n",duration);

}


void svdlapack(double **a, int row, int col, double **u, double **s, double **v)
{
    /* Locals  */
    int info, lwork;
    double *tmpa, *tmpu, *tmpv, *tmps;
    double wkopt;
    double* work;
    int i, j, ioff, iss;
 
    tmpa = array1_create(row*col);
    tmpu = array1_create(row*row);
    tmpv = array1_create(col*col);
    tmps = array1_create(col);
 
    /* convert input to matrix 1D */
    ioff = 0;
    for(i=0; i<col; i++)
    {
        for(j=0; j<row; j++)
        {
            tmpa[ioff] = a[j][i];
            ioff = ioff+1;
        }
    }
 
    /* Query and allocate the optimal workspace */
    lwork = -1;
    dgesvd_( "All", "All", &row, &col, tmpa, &row, tmps, tmpu, &row, tmpv, &col, &wkopt,
         &lwork, &info );
 
    lwork = (int)wkopt;
    work = (double*)malloc( lwork*sizeof(double) );
 
    /* Compute SVD */
    dgesvd_( "All", "All", &row, &col, tmpa, &row, tmps, tmpu, &row, tmpv, &col, work,
         &lwork, &info );
 
    /* Check for convergence */
    if( info > 0 ) {
        printf( "The algorithm computing SVD failed to converge.\n" );
        exit( 1 );
    }
 
    /* Convert from tmpu (matrix 1D) to u (matrix 2D) */
    ioff = 0;
    for(i=0; i<row; i++)
    {
        for(j=0; j<row; j++)
        {
            u[j][i] = tmpu[ioff];
            ioff = ioff+1;
        }
    }
 
    /* Convert from tmpv (matrix 1D) to v (matrix 2D) */
    ioff = 0;
    for(i=0; i<col; i++)
    {
        for(j=0; j<col; j++)
        {
            v[j][i] = tmpv[ioff];
            ioff = ioff+1;
        }
    }
 
    /* get minimum size from row and columns */
    if(row<col) iss = row;
    else iss=col;
 
    /* Convert from tmps (matrix 1D) to s (matrix 2D) */
    for(i=0; i<iss; i++)
      s[i][i] = tmps[i];
 
    /* free allocated memory */
    free( (void*)work );
    array1_free(tmpa);
    array1_free(tmpu);
    array1_free(tmpv);
    array1_free(tmps);
}

double *array1_create(int len)
{
    double *arr=NULL;
 
    /* allocate pointers to rows */
    arr = (double *) malloc((len*sizeof(double)));
    if (!arr) {
        fprintf(stderr, "array1_create : error allocation array");
        exit(0);
    }
    return(arr);
}
 
double **array2_create(int row, int col)
{
    int i;
    double **arr=NULL;
 
    /* allocate pointers to rows */
    arr = (double **) malloc((row*sizeof(double*)));
    if (!arr) {
        fprintf(stderr, "array2_create : error allocation row");
        exit(0);
    }
 
    /* allocate rows and set pointers to them */
    arr[0]=(double*) malloc((row*col)*sizeof(double));
    if (!arr[0]) {
        fprintf(stderr, "array2_create : error allocation column");
        exit(0);
    }
 
    for(i=1; i<row; i++)
        arr[i]=arr[i-1] + col;
 
    /* return pointer to array of pointers to rows */
    return arr;
}
 
void array1_free(double *arr)
{
    free(arr);
}
 
void array2_free(double **arr)
{
    free (arr[0]);
    free (arr);
}




void printmatrix(char *var, double **a, int row, int col)
{
    int i, j;
    fprintf(stderr, "\nMatrix %s, row=%i col=%i \n", var, row, col);
    for(i=0; i<row; i++)
    {
        for(j=0; j<col; j++)
        {
            fprintf(stderr, "%f   ", a[i][j]);
        }
        fprintf(stderr, "\n");
    }
}
