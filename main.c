#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>

#define PI 3.14159265
#define SIZE 64
#define R1 1
#define R2 2
#define d_phi 0.02
#define d_theta 0.07
#define K2 10
#define K1 SIZE*K2*3/(8*(R1+R2))

const char values[] = ".,-~:;=!*#$@";

void print_box (char output[SIZE][SIZE]) {
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			printf("%c", output[i][j]);
		}
		printf("\n");
	}
}

double luminence (double sinA, double cosA, 
		double sinB, double cosB, 
		double sin_phi, double cos_phi,
		double sin_theta, double cos_theta,
		double x, double y, double z) {
	return x*(cos_phi*cosA*cosB+sin_phi*sin_theta*sinA*sinB+sin_phi*sin_theta*sinA)+
	       y*(cos_phi*sinB-sin_phi*sin_theta*cosB)+
	       z*(cosA*sin_phi*sin_theta-sin_phi*sin_theta*sinA*sinB-cos_phi*sinA*cosB);
}

void render_frame (double A, double B, char output[SIZE][SIZE]) {
	double cosA = cos(A), sinA = sin(A);
	double cosB = cos(B), sinB = sin(B);
	for (double phi = 0; phi < 2*PI; phi += d_phi) {
		double sin_phi = sin(phi);
		double cos_phi = cos(phi);
		double M = R2+R1*sin_phi;
		for (double theta = 0; theta < 2*PI; theta += d_theta) { 
			double cos_theta = cos(theta), sin_theta = sin(theta);

			double x = R1*cos_phi*cosA*cosB + sin_theta*sinB*cosA*M+sinA*cos_theta*M;
			double y = R1*cos_phi*sinB - sin_theta*cosB*M;
			double z = K2 + cos_theta*cosA*M - sin_theta*sinA*sinB*M - R1*cos_phi*sinA*cosB;

			double inv_z = 1/z;

			int x_prime = (int)(SIZE/2 + (K1*x)*inv_z);
			int y_prime = (int)(SIZE/2 - (K1*y)*inv_z);

			double LX = -1.0;
			double LY = 1.0;
			double LZ = 1.0;

			double lighting_index = luminence(sinA, cosA, sinB, cosB, sin_phi, cos_phi, sin_theta, cos_theta, -1.0, -1.0, 0.0);
				
			if (lighting_index > 0) {
				int L = 11 * lighting_index/sqrt(LX*LX+LY*LY+LZ*LZ);
				output[y_prime > SIZE - 1 ? SIZE - 1 : y_prime][x_prime > SIZE - 1 ? SIZE - 1 : x_prime] = values[L];
			}
		}
	}
}

void clear (char output[SIZE][SIZE]) {
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			output[i][j] = ' ';
		}
	}
}

double atof_safe (char *str) {
	double ret = atof(str);

	if (ret == 0.0) {
		if (str[0] != '0') {
			printf("%s is not a valid floating-point value\n", str);
			exit(1);
		}
	}

	return ret;
}

int main (int argc, char* argv[]) {
	char output[SIZE][SIZE];

	clear(output);

	double A = 0.0, B = 0.0;
	double rateA, rateB;

	switch (argc) {
		case 1:
			rateA = 0.02;
			rateB = 0.02;
			break;
		case 2:
			rateA = atof_safe(argv[1]);
			rateB = 0.0;
			break;
		case 3:
			rateA = atof_safe(argv[1]);
			rateB = atof_safe(argv[2]);
			break;
		default:
			printf("Too many parameters\n");
			exit(1);
			break;
	}

	printf("\x1b[H");
	while (true) {
		render_frame(A, B, output);
		print_box(output);

		A += rateA;
		B += rateB;
		
		clear(output);
	}

	return 0;
}

