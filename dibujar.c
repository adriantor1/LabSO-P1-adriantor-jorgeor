#include <stdio.h>
#include <stdlib.h>

/* Codigo de ejemplo para generar un gráfico PNG a partir de los resultados de la simulación.
* los datos dependerán de la cantidad de procesos y la secuencia en que se asigne la CPU.
* 
* La configuración de máximos de x e y depende del tiempo máximo de simulación (x) y la cantidad de procesos (n + 1)
* 
* el comando "set arrow" requiere un número secuencial para cada flecha, que se puede generar como:
* 
* int current_arrow;
* 
* current_arrow = 1;
* 
* fprintf(fp, "set arrow %d ..... ", current_arrow, ...);
* current_arrow++;
*/


int main(int argc, char *argv[]) {
  
  FILE * fp = fopen("plot.plt", "w");
  
  if (fp == NULL) {
    fprintf(stderr, "No se pudo crear el archivo\n");
    exit(EXIT_FAILURE);
  }
  
  fprintf(fp, "set term pngcairo dashed size 1024,768\n");
  fprintf(fp, "set output 'plot.png'\n");
  fprintf(fp, "set style fill solid\n");
  fprintf(fp, "set xrange [0:45]\n");
  fprintf(fp, "set yrange [0:7]\n");
  fprintf(fp, "unset ytics\n");
  fprintf(fp, "set title 'Planificacion'\n");
  fprintf(fp, "set ytics('a.txt' 1,'b.txt' 2,'c.txt' 3,'d.txt' 4,'e.txt' 5,'f.txt' 6)\n");
  fprintf(fp, "set xtics 5\n");
  fprintf(fp, "unset key\n");
  fprintf(fp, "set xlabel 'Tiempo'\n");
  fprintf(fp, "set ylabel 'Proceso'\n");
  fprintf(fp, "set style line 1 lt 1 lw 2 lc rgb '#00ff00'\n");
  fprintf(fp, "set style line 2 lt 1 lw 2 lc rgb '#ff0000'\n");
  fprintf(fp, "set style line 3 lt 1 lw 1 lc rgb '#202020'\n");
  fprintf(fp, "set style arrow 1 heads size screen 0.008,90 ls 1\n");
  fprintf(fp, "set style arrow 2 heads size screen 0.008,90 ls 2\n");
  fprintf(fp, "set style arrow 3 heads size screen 0.008,100 ls 3\n");
  fprintf(fp, "set arrow 1 from 0,2 to 3,2 as 1\n");
  fprintf(fp, "set arrow 2 from 0,1 to 3,1 as 3\n");
  fprintf(fp, "set arrow 3 from 0,3 to 3,3 as 3\n");
  fprintf(fp, "set arrow 4 from 0,4 to 3,4 as 3\n");
  fprintf(fp, "set arrow 5 from 0,5 to 3,5 as 3\n");
  fprintf(fp, "set arrow 6 from 0,6 to 3,6 as 3\n");
  fprintf(fp, "set arrow 7 from 3,6 to 7,6 as 1\n");
  fprintf(fp, "set arrow 8 from 3,1 to 7,1 as 3\n");
  fprintf(fp, "set arrow 9 from 3,3 to 7,3 as 3\n");
  fprintf(fp, "set arrow 10 from 3,4 to 7,4 as 3\n");
  fprintf(fp, "set arrow 11 from 3,5 to 7,5 as 3\n");
  fprintf(fp, "set arrow 12 from 7,5 to 12,5 as 1\n");
  fprintf(fp, "set arrow 13 from 7,1 to 12,1 as 3\n");
  fprintf(fp, "set arrow 14 from 7,3 to 12,3 as 3\n");
  fprintf(fp, "set arrow 15 from 7,4 to 12,4 as 3\n");
  fprintf(fp, "set arrow 16 from 12,1 to 17,1 as 1\n");
  fprintf(fp, "set arrow 17 from 12,3 to 17,3 as 3\n");
  fprintf(fp, "set arrow 18 from 12,4 to 17,4 as 3\n");
  fprintf(fp, "set arrow 19 from 17,1 to 19,1 as 1\n");
  fprintf(fp, "set arrow 20 from 17,3 to 19,3 as 3\n");
  fprintf(fp, "set arrow 21 from 17,4 to 19,4 as 3\n");
  fprintf(fp, "set arrow 22 from 19,4 to 24,4 as 1\n");
  fprintf(fp, "set arrow 23 from 19,3 to 24,3 as 3\n");
  fprintf(fp, "set arrow 24 from 24,4 to 27,4 as 1\n");
  fprintf(fp, "set arrow 25 from 24,3 to 27,3 as 3\n");
  fprintf(fp, "set arrow 26 from 27,3 to 32,3 as 1\n");
  fprintf(fp, "set arrow 27 from 32,3 to 37,3 as 1\n");
  fprintf(fp, "set arrow 28 from 37,3 to 40,3 as 1\n");
  fprintf(fp, "plot NaN\n");
  
  fclose(fp);
  //Generar el grafico PNG
  system("gnuplot plot.plt");
  
  //Abrir el grafico PNG
  system("start plot.png");
	
	return 0;
}

