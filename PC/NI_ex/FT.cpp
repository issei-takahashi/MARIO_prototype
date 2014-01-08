#include "FT.hpp"
#include "Wave.hpp"

#define PI 3.141592

void FT::FFT( const Wave& inWave, Wave& outWave, int N )
{
	int i, j, k, n, n2;
	double theta, wr, wi;

	static double *rbuf, *ibuf;
	static int    bufsize = 0;

	/* memory allocation for buffers */
	if( bufsize != N ) {
		bufsize = N;
		rbuf = (double*)calloc( sizeof(double), bufsize );
		ibuf = (double*)calloc( sizeof(double), bufsize );
	}

	/* bit reverse of inWave.m_r[] & inWave.m_i[] --> store to rbuf[] and ibuf[] */
	i = j = 0 ;
	rbuf[j] = inWave.getRe(j,true);
	ibuf[j] = inWave.getIm(j,true);
	for( j = 1 ; j < N-1 ; j++ ) {
		for( k = N/2 ; k <= i ; k /= 2 )  i -= k;
		i += k;
		rbuf[j] = inWave.getRe(i,true);  
		ibuf[j] = inWave.getIm(i,true);
	}
	rbuf[j] = inWave.getRe(j,true);  
	ibuf[j] = inWave.getIm(j,true);

	/* butterfly calculation */
	theta = -2.0*PI;
	for( n = 1 ; ( n2 = n*2 ) <= N ; n = n2 ) {
		theta *= 0.5;
		for ( i = 0 ; i < n ; i++ ) {
			wr = cos(theta*i);
			wi = sin(theta*i);
			for ( j = i ; j < N ; j += n2 ) {
				k = j + n;
				outWave.m_r[j] = rbuf[j] + ( wr * rbuf[k] - wi * ibuf[k] );  /* ??????????, using wr, wi, rbuf, and ibuf */
				outWave.m_i[j] = ibuf[j] + ( wi * rbuf[k] + wr * ibuf[k] );  /* ??????????, using wr, wi, rbuf, and ibuf */
				outWave.m_r[k] = rbuf[j] - ( wr * rbuf[k] - wi * ibuf[k] );  /* ??????????, using wr, wi, rbuf, and ibuf */
				outWave.m_i[k] = ibuf[j] - ( wi * rbuf[k] + wr * ibuf[k] );  /* ??????????, using wr, wi, rbuf, and ibuf */
			}
		}

		for( i = 0 ; i < N ; i++ ) {
			rbuf[i] = outWave.getRe(i,true);
			ibuf[i] = outWave.getIm(i,true);
		}
	}
	return;
}

void FT::IFFT( const Wave& inWave, Wave& outWave, int N )
{
	int i, j, k, n, n2;
	double theta, wr, wi;

	static double *rbuf, *ibuf;
	static int    bufsize = 0;

	/* memory allocation for buffers */
	if( bufsize != N ) {
		bufsize = N;
		rbuf = (double*)calloc( sizeof(double), bufsize );
		ibuf = (double*)calloc( sizeof(double), bufsize );
	}

	/* bit reverse of inWave.m_r[] & inWave.m_i[] --> store to rbuf[] and ibuf[] */
  i = j = 0 ;
  rbuf[j] = inWave.getRe(j,true)/N;
  ibuf[j] = inWave.getIm(j,true)/N;
  for( j = 1 ; j < N-1 ; j++ ) {
	  for( k = N/2 ; k <= i ; k /= 2 )  i -= k;
	  i += k;
	  rbuf[j] = inWave.getRe(i,true)/N;
	  ibuf[j] = inWave.getIm(i,true)/N;
  }
  rbuf[j] = inWave.getRe(j,true)/N;
  ibuf[j] = inWave.getIm(j,true)/N;

  /* butterfly calculation */
  theta = 2.0*PI;  /* not -2.0*PI !!! */
  for( n = 1 ; ( n2 = n*2 ) <= N ; n = n2 ) {
	  theta *= 0.5;
	  for ( i = 0 ; i < n ; i++ ) {
		  wr = cos(theta*i);  wi = sin(theta*i);
		  for ( j = i ; j < N ; j += n2 ) {
			  k = j + n;
			  outWave.m_r[j] = rbuf[j] + ( wr * rbuf[k] - wi * ibuf[k] );  /* ??????????, using wr, wi, rbuf, and ibuf */
			  outWave.m_i[j] = ibuf[j] + ( wi * rbuf[k] + wr * ibuf[k] );  /* ??????????, using wr, wi, rbuf, and ibuf */
			  outWave.m_r[k] = rbuf[j] - ( wr * rbuf[k] - wi * ibuf[k] );  /* ??????????, using wr, wi, rbuf, and ibuf */
			  outWave.m_i[k] = ibuf[j] - ( wi * rbuf[k] + wr * ibuf[k] );  /* ??????????, using wr, wi, rbuf, and ibuf */
		  }
	  }

	  for( i = 0 ; i < N ; i++ ) {
		  rbuf[i] = outWave.getRe(i,true);
		  ibuf[i] = outWave.getIm(i,true);
	  }
  }
  return;
}

