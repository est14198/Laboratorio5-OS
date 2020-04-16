#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include<omp.h>

// Array de sudoku
int array[9][9];
// Flag para indicar error
bool error = false;


// Funcion de verificacion de columnas
void *verificacionColumna(){
	omp_set_nested(true);
	omp_set_num_threads(9);
	int i;
	int j;
	int k;
	#pragma omp parallel for
	for(j=0;j<9;j++)
	{
		int verCol[9] = {0,0,0,0,0,0,0,0,0};
		printf("En la revision de columnas el siguiente es un thread en ejecucion: %d\n", syscall(SYS_gettid));
		for(i=0;i<9;i++){
			for(k=0;k<9;k++){
				if(verCol[k] == array[i][j]){
					printf("Sudoku incorrecto\n");
					error = true;
				}
			}
			verCol[i] = array[i][j];
		}
	}
}

// Funcion de verificacion de filas
void *verificacionFila(){
	omp_set_nested(true);
	omp_set_num_threads(9);
	int i;
	int j;
	int k;
	#pragma omp parallel for schedule(dynamic)
	for(i=0;i<9;i++)
	{
		int verFila[9] = {0,0,0,0,0,0,0,0,0};
		for(j=0;j<9;j++){
			for(k=0;k<9;k++){
				if(verFila[k] == array[i][j]){
					printf("Sudoku incorrecto\n");
					error = true;
				}
			}
			verFila[i] = array[i][j];
		}
	}
}


// main
int main(){
	omp_set_num_threads(1);
	// Abrir y leer archivo de sudoku
    	FILE *filePointer;
    	char ch;
	char solucion[81];
	int contador = 0;
    	filePointer = fopen("sudoku", "r");
    	if (filePointer == NULL) {
        	printf("No se encuentra el archivo \n");
    	}
    	else {
        	while ((ch = fgetc(filePointer)) != EOF || contador < 82) {
			solucion[contador] = ch;
			contador++;
        	}
    	}
    	fclose(filePointer);

	// Ingresar lo leido en el archivo a un array bidimensional
	int i;
	int j;
	for(i=0;i<9;i++) {
		for(j=0;j<9;j++) {
	    		array[i][j]=(int)solucion[(i*9)+j]-'0';
	    	}
	}

	// Forks solicitados en el lab
	int pid = getpid();
	char buf[8];
	snprintf(buf, 8, "%d", pid);
	int var1 = fork();
	// Proceso hijo
	if ( var1 == 0 ) {
		execlp("ps", "ps", "-p", buf, "-lLf", (char *) NULL);
	}
	// Proceso padre
	else {
		pthread_t thread;
		pthread_create(&thread, NULL, verificacionColumna, NULL);
		pthread_join(thread, NULL);
		printf("El thread en el que se ejecuta main es: %d\n", syscall(SYS_gettid));
		wait(NULL);
		verificacionFila();
		if(error){
			printf("\nSudoku incorecto\n");
		}
		else {
			printf("\nSudoku correcto\n");
		}
		int var2 = fork();
		// Proceso hijo
		if ( var2 == 0 ) {
			execlp("ps", "ps", "-p", buf, "-lLf", (char *) NULL);
		}
		// Proceso padre
		else {
			wait(NULL);
		}
	}

	return 0;
}
