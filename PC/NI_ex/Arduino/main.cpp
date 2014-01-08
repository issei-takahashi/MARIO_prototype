//---------------------------------------------------------------------------
// Include
//---------------------------------------------------------------------------
#include "winrs.h"
#include <stdio.h>
#include <string.h>
#include <Windows.h>

//---------------------------------------------------------------------------
// Define
//---------------------------------------------------------------------------
#define DIGIT 5


//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
int arduino_main()
{
	WinRS *port = new WinRS(4, 9600, ifLine::cr, "8N1", false);

	if (*port)
	{ 
		printf("COM Port opened!\n");

		int number, binary;
		int i=0;
		int bin[DIGIT] = {0};

		while(1)
		{
			printf("10진수를 입력하고 Enter>");
            scanf("%d", &number);

			if(number<31)
			{
				printf("\n10진수 : %d ", number);

				while(number>0)
				{
					binary=number%2;

					if(binary){
						port->putc1('b');
					}
					else{
						port->putc1('a');
					}

					bin[i]=binary;
					number=number/2;
					i++;
				}

				printf("\n2진수: ");


				for(int j=DIGIT; j>0;j--)
				{
					printf("%d", bin[j-1]);
				}

				printf("\n\n");

				printf("actuator position: %ccm\n", number);
			}
			else{
				printf("0-30범위의 수를 입력하세요\n");
			}

		}

	}else{
		printf("Port open failed.\n");
		return 0;
	}
		
	delete port;
	return 0;

}

