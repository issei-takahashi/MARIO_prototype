#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "includes.hpp"
#include "Wave.hpp"

class FT
{
public:
	static void FFT( const Wave& inWave, Wave& outWave, int frame );
	static void DFT( const Wave& inWave, Wave& outWave, int frame );
	static void IFFT( const Wave& inWave, Wave& outWave, int frame );
	static void IDFT( const Wave& inWave, Wave& outWave, int frame );

private:


};