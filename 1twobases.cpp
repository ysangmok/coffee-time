#include <iostream>
#include <math.h>
using namespace std;
int main(int argc, char *argv[]) {
	int x = 0, y = 0, z = 0;
	x = 5;
	//printf("x = %d, y = %d, z = %d\n", x, y, z);
	int number, number2;
	
	//99x + y - 80z = 0
	for(x = 0; x < 7; x++){
		for(y = 0; y < 9; y++){
			for(z = 1; z < 9; z++){
				
				number = 100 * x + 10 * y + 1 * z;
				number2 = 81*z + 9*y + x;
				
				if(number == number2){
					printf("number = %d\n", number);
					printf("x = %d, y = %d, z = %d", x, y, z);
					return(number);
				}
			}
		}
	}
}