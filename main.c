#include <reg51.h>
// RSA Functions
unsigned char primality_test(unsigned int prime_candidate);
unsigned long square_mul(unsigned long number, unsigned long mod, unsigned long power);
unsigned long Extended_Eculdian_Algorthim(unsigned long phi, unsigned long e);
unsigned long gcd(unsigned long n1, unsigned long n2);
unsigned long Encryption_Decryption(unsigned long public_private_key, unsigned long n, unsigned int plain_cipher_text);
//==================================================================================================================
// UART Functions
void serial_init();
void send_data_serial(char data_t);
char recive_data_serial(void);
unsigned long UART_ReceiveNumber(void);
void UART_SendString(char *str);
void intToASCII(unsigned long number, char *asciiString);
//==================================================================================================================
// Variables
unsigned int p, q; //p and q is prime numbers
unsigned char co_prime_check; // variable used to store return value of primality test
unsigned char public_key_check; // variable used to store return value of gcd check for public key with phi
unsigned long phi, n, public_key, private_key; //RSA main variables 
unsigned int cipher_plain_text; // Cipher text variable
 // plain text variable
unsigned char temp[6]; //variable used to show plain text and cipher text or any other value in Uart Terminal as ascii value
//==================================================================================================================
int main() {
    serial_init(); //initialization of serial
    while (1) {
				 //primality test to check if p and q is prime numbers
				do{
					UART_SendString("Enter p (prime): ");
					p = UART_ReceiveNumber(); // Enter P prime number
					
					UART_SendString("Enter q (prime): ");
					q = UART_ReceiveNumber(); // Enter q prime number
					co_prime_check = (primality_test(p) & primality_test(q));
				}while(co_prime_check != 1);
				co_prime_check = 0; //so code can work again and we can enter next p and q
				
				phi = (p - 1) *(q - 1); // phi calculation
				n = p * q; // n calculation
				
				 //gcd test to check if public key is correct or not
				do{
					UART_SendString("Enter public key (e): ");
					public_key = UART_ReceiveNumber(); //Enter public key (e)
					public_key_check = gcd(phi,public_key);
				}while(public_key_check !=1);
				public_key_check = 0; // so code can work again and we can enter next public key
				
				UART_SendString("Enter plain text: ");
				cipher_plain_text = UART_ReceiveNumber(); // enter plain text required to encrypt X				
				
        private_key = Extended_Eculdian_Algorthim(phi, public_key); //Generate private key (d) used for decryption Y
				
        cipher_plain_text = Encryption_Decryption(public_key, n, cipher_plain_text); //encryption plain text with e and store it in cipher text
        intToASCII(cipher_plain_text, temp); // convert number from integar to Ascii
        UART_SendString("Encryption: "); //send string text to uart
        UART_SendString(temp); //show cipher text in uart terminal
        UART_SendString("\r\n"); //make new line

        cipher_plain_text = Encryption_Decryption(private_key, n, cipher_plain_text);//decryption cipher text with d and store it in plain text
        intToASCII(cipher_plain_text, temp); // convert number from integar to Ascii
        UART_SendString("Decryption: "); //send string text to uart
        UART_SendString(temp); //show plain text in uart terminal
        UART_SendString("\r\n"); //make new line
				
    }
}
//==================================================================================================================
// RSA Functions

unsigned char primality_test(unsigned int prime_candidate)
{
    unsigned char a = 2; // value of a we will use for fermat test
		unsigned int reminder = 1; //reminder value we check to know if condition is true or not
		unsigned char i; //for loop itreation

		unsigned char security_test = 0; // number of test i need to check
		for(i = 0; i <= 5;i++)
		{
				reminder = square_mul(a,prime_candidate,(prime_candidate - 1)); // a^(p~-1) mod p~ = 1 mod p~ 
				if (reminder ==1)// if reminder is 1 then condition in comment in line 99
				{
					security_test++; // 1 test passed correct
					a++;  // next value of a to test
				}
				else
				{ // test failed due to only one failed
					break; //break for loop
				}
		}
		if(security_test >= 5) // check if test passed 5 times or more else test failed
		{
			return 1; //test passed
		}
		else
		{
			return 0; //test failed
		}
}

unsigned long square_mul(unsigned long number, unsigned long m, unsigned long power) {
    unsigned int result = 1;
    while (power) {
        if (power & 1) result = (result * number) % m;
        number = (number * number) % m;
        power >>= 1;
    }
    return result;
}

// ex 5 mod 3 ==> (5 % 3 + 3) % 3 ==> (2 + 3) % 3 ==> 5 % 3 ==> 2 mod 3 
// ex -5 mod 3 ==> (-5 % 3 + 3) % 3 ==> (-2 + 3) % 3 ==> (1) % 3 ==> 1 mod 3

//EEA function
unsigned long Extended_Eculdian_Algorthim(unsigned long phi, unsigned long e) {
    // Initialize variables:
    // r1 and r2 are used for calculating the greatest common divisor (GCD) using the Euclidean algorithm.
    signed int r1 = phi, r2 = e, t1 = 0, t2 = 1, temp;
    while (r2) {
        signed int q = r1 / r2;
        temp = r1 % r2;
        r1 = r2;
        r2 = temp;

        temp = t1 - q * t2;
        t1 = t2;
        t2 = temp;
    }
    return (t1 < 0) ? t1 + phi : t1;
}

//Encryption and Decryption Function
unsigned long Encryption_Decryption(unsigned long public_private_key, unsigned long n, unsigned int plain_cipher_text) {
    // Initialize result to the plain or cipher text input.
    unsigned long result = 1;
    // Declare a loop counter variable.
		result = square_mul(plain_cipher_text,n,public_private_key);
    // Return the final result, which is the encryption or decryption output.
    return result;
}


unsigned long gcd(unsigned long n1, unsigned long n2) // 27, 21 as example inputs
{
    while (n2) {
        unsigned int temp = n2;
        n2 = n1 % n2;
        n1 = temp;
    }
    return n1;
}

//==================================================================================================================
// UART Functions
void serial_init(void) {
    TMOD = 0x20; TH1 = 0xFD; SCON = 0x50; TR1 = 1;
}

void send_data_serial(char data_t) {
		TI = 0;
    SBUF = data_t;
    while (!TI);
    TI = 0;
}

char recive_data_serial(void) {
    while (!RI);
    RI = 0;
    return SBUF;
}

void UART_SendString(char *str) {
    while (*str) send_data_serial(*str++);
}

unsigned long UART_ReceiveNumber(void) {
    char buffer[10];          // Buffer to store received characters
    char c;                   // Character received from UART
    char index = 0;            // Current index in the buffer
    unsigned int number = 0; // Final integer value
		char i;
    while (1) {
        c = recive_data_serial(); // Receive a character from UART
        if (c == '\r') break;     // Stop on carriage return
        if (index < sizeof(buffer) - 1) buffer[index++] = c; // Store valid characters
    }

    buffer[index] = '\0'; // Null-terminate the buffer

    // Convert the ASCII string to an integer
    for (i = 0; buffer[i] >= '0' && buffer[i] <= '9'; i++) {
        number = number * 10 + (buffer[i] - '0');
    }

    return number;
}

void intToASCII(unsigned long number, char *asciiString) {
    char index = 0;
		char i = 0;
    if (number == 0) {
        asciiString[index++] = '0'; // Handle zero case
    } else {
        // Extract digits in reverse order
        while (number > 0) {
            asciiString[index++] = (number % 10) + '0';
            number /= 10;
        }

        // Reverse the string to correct order
        for (i = 0; i < index / 2; i++) {
            char temp = asciiString[i];
            asciiString[i] = asciiString[index - i - 1];
            asciiString[index - i - 1] = temp;
        }
    }

    asciiString[index] = '\0'; // Null-terminate the string
}