#include<Eigen/Dense>
#include<string>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<cmath>
#include<complex>
#include<omp.h>
#include<gsl/gsl_sf_coupling.h>
#include"general.h"
using namespace std;
using namespace Eigen;

/*
    factorial and double_factorial
*/
double double_factorial(const int& n)
{
    switch (n)
    {
    case 0: return 1.0;
    case 1: return 1.0;
    case 2: return 2.0;
    case 3: return 3.0;
    case 4: return 8.0;
    case 5: return 15.0;
    case 6: return 48.0;
    case 7: return 105.0;
    case 8: return 384.0;
    case 9: return 945.0;
    case 10: return 3840.0;
    case 11: return 10395.0;
    case 12: return 46080.0;
    case 13: return 135135.0;
    case 14: return 645120.0;
    default:
        if(n < 0)
        {
            cout << "ERROR: double_factorial is called for a negative number!" << endl;
            cout << "n is " << n << endl;
            exit(99);
        }
        else return n * double_factorial(n - 2);
    }
}
double factorial(const int& n)
{
    switch (n)
    {
    case 0: return 1.0;
    case 1: return 1.0;
    case 2: return 2.0;
    case 3: return 6.0;
    case 4: return 24.0;
    case 5: return 120.0;
    case 6: return 720.0;
    case 7: return 5040.0;
    case 8: return 40320.0;
    case 9: return 362880.0;
    case 10: return 3628800.0;

    default:
        if(n < 0)
        {
            cout << "ERROR: factorial is called for a negative number!" << endl;
            cout << "n is " << n << endl;
            exit(99);
        }
        return n * factorial(n - 1);
    }
}

/*
    Wigner 3j coefficients with J = l1 + l2 + l3 is even
*/
double wigner_3j(const int& l1, const int& l2, const int& l3, const int& m1, const int& m2, const int& m3)
{
    // return gsl_sf_coupling_3j(2*l1,2*l2,2*l3,2*m1,2*m2,2*m3);

    if(l3 > l1 + l2 || l3 < abs(l1 - l2) || m1 + m2 + m3 != 0 || abs(m1) > abs(l1) || abs(m2) > abs(l2) || abs(m3) > abs(l3))
    {
        return 0.0;
    }
    else if(m1 == 0 && m2 == 0 && m3 == 0)
    {
        return wigner_3j_zeroM(l1,l2,l3);
    }
    else
    {
        Vector3i L(l1,l2,l3), M(m1,m2,m3);
        int tmp, Lmax = L.maxCoeff();
        for(int ii = 0; ii <= 1; ii++)
        {
            if(L(ii) == Lmax)
            {
                tmp = L(ii);
                L(ii) = L(2);
                L(2) = tmp;
                tmp = M(ii);
                M(ii) = M(2);
                M(2) = tmp;
                break;
            }
        }

        if(L(2) == L(0) + L(1))
        {
            return pow(-1, L(0) - L(1) - M(2)) * sqrt(factorial(2*L(0)) * factorial(2*L(1)) / factorial(2*L(2) + 1) * factorial(L(2) - M(2)) * factorial(L(2) + M(2)) / factorial(L(0)+M(0)) / factorial(L(0)-M(0)) / factorial(L(1)+M(1)) / factorial(L(1)-M(1)));
        }
        else
        {
            return gsl_sf_coupling_3j(2*L(0),2*L(1),2*L(2),2*M(0),2*M(1),2*M(2));
        }
        
    }
}
/*
    Wigner 3j coefficients with m1 = m2 = m3 = 0
*/
double wigner_3j_zeroM(const int& l1, const int& l2, const int& l3)
{
    int J = l1+l2+l3, g = J/2;
    if(J%2 || l3 > l1 + l2 || l3 < abs(l1 - l2))
    {
        return 0.0;
    }
    else
    {
        return pow(-1,g) * sqrt(factorial(J - 2*l1) * factorial(J - 2*l2) * factorial(J - 2*l3) / factorial(J + 1)) 
                * factorial(g) / factorial(g-l1) / factorial(g-l2) / factorial(g-l3);
    }
}


double evaluateChange(const MatrixXd& M1, const MatrixXd& M2)
{
    int size = M1.rows();
    double tmp = 0.0;
    for(int ii = 0; ii < size; ii++)
    for(int jj = 0; jj < size; jj++)
        tmp += pow((M1(ii,jj) - M2(ii,jj)),2);

    return sqrt(tmp);
}

MatrixXd matrix_half_inverse(const MatrixXd& inputM)
{
    int size = inputM.rows();
    SelfAdjointEigenSolver<MatrixXd> solver(inputM);
    VectorXd eigenvalues = solver.eigenvalues();
    MatrixXd eigenvectors = solver.eigenvectors();
 
    for(int ii = 0; ii < size; ii++)
    {
        if(eigenvalues(ii) < 0)
        {
            cout << "ERROR: Matrix has negative eigenvalues!" << endl;
            exit(99);
        }
        else
        {
            eigenvalues(ii) = 1.0 / sqrt(eigenvalues(ii));
        }
    }

    MatrixXd tmp(size, size);
    for(int ii = 0; ii < size; ii++)
    for(int jj = 0; jj < size; jj++)
    {
        tmp(ii,jj) = 0.0;
        for(int kk = 0; kk < size; kk++)
            tmp(ii,jj) += eigenvectors(ii,kk) * eigenvalues(kk) * eigenvectors(jj,kk);
    }

    return tmp; 
}

MatrixXd matrix_half(const MatrixXd& inputM)
{
    int size = inputM.rows();
    SelfAdjointEigenSolver<MatrixXd> solver(inputM);
    VectorXd eigenvalues = solver.eigenvalues();
    MatrixXd eigenvectors = solver.eigenvectors();
    
    for(int ii = 0; ii < size; ii++)
    {
        if(eigenvalues(ii) < 0)
        {
            cout << "ERROR: Matrix has negative eigenvalues!" << endl;
            exit(99);
        }
        else
        {
            eigenvalues(ii) = sqrt(eigenvalues(ii));
        }
    }
    MatrixXd tmp(size, size);
    for(int ii = 0; ii < size; ii++)
    for(int jj = 0; jj < size; jj++)
    {
        tmp(ii,jj) = 0.0;
        for(int kk = 0; kk < size; kk++)
            tmp(ii,jj) += eigenvectors(ii,kk) * eigenvalues(kk) * eigenvectors(jj,kk);
    }

    return tmp;
}


void eigensolverG(const MatrixXd& inputM, const MatrixXd& s_h_i, VectorXd& values, MatrixXd& vectors)
{
    MatrixXd tmp = s_h_i * inputM * s_h_i;
    
    SelfAdjointEigenSolver<MatrixXd> solver(tmp);
    values = solver.eigenvalues();
    vectors = s_h_i * solver.eigenvectors();

    return;
}