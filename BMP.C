/* Programa que visualiza imagenes BMP, hecho por vincebus 2018 */

/* EVITAR A TODA COSTA STRCPY, USAR STRNCPY, estuve peleando un dia entero por
una maldita linea de codigo con strcpy */

/* Version 0.1 Alpha */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "modox.h"

struct FileHeader{
	unsigned char ID[2];
	long Size;
	int Res1[2];
	long Offset;	/* Offset de la imagen donde se encuentra */
};

struct InfoHeader{
	/* Header tipo W3.0 */
	long Hsize;
	long X;
	long Y;
	int NumColorPlanes;
	int BitsPerPixel;
	long CompressionMethod;
	long ImgSize;
	long ResX;
	long ResY;
	long NumColors;
	long NumImportantColors;
};

struct Color{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char i;
};

struct ImagenBMP{
	struct FileHeader fh;
	struct InfoHeader ih;
	struct Color *Paleta;
	unsigned char **imagen;
};

int AbrirBMP(char *nombreArchivo, struct ImagenBMP **img);
void SetPaleta(struct ImagenBMP **img);
void DibujarIMGMSK(struct ImagenBMP **img, int x, int y, unsigned maskcolor);
void DibujarIMG(struct ImagenBMP **img, int x, int y);
void LiberarImagen(struct ImagenBMP **img);

int AbrirBMP(char *nombreArchivo, struct ImagenBMP **img){
	FILE *fp = NULL;

	int y,i,j,padding;
	char *id = (char*)calloc(3,1);
	unsigned char pixel;

	fp = fopen(nombreArchivo, "rb");

	if(fp==NULL){
		printf("\nError al abrir archivo!");
		return 0;
	}

	/* Dimension de imagen */
	if((*img)==NULL){
		(*img) = (struct ImagenBMP *)malloc(sizeof(struct ImagenBMP));
		(*img)->Paleta = (struct Color*)malloc(sizeof(struct Color) * 255);
	}

	fread(id, 2, 1,fp);

	/* Valida si el archivo es BMP */
	if(strcmp(id, "BM")){
		printf("\n%s", id);
		printf("\nArchivo invalido!");
		return 0;
	}

	strncpy(((*img)->fh.ID), id,2);
	fread(&(*img)->fh, 12, 1,fp);
	fread(&(*img)->ih, 40, 1,fp);
	fread((*img)->Paleta, 1024,1, fp); /* Lectura de Paleta e imagen */

	(*img)->imagen = (unsigned char **)malloc(sizeof(unsigned char*) * (*img)->ih.Y);

	if((*img)->imagen==NULL){
		printf("\nNo se pudo dimensionar la imagen Y");
		return 0;
	}

	padding = 0;

	while(((*img)->ih.X + padding) % 4 != 0){
		padding++;
	};

	for(y=(*img)->ih.Y-1; y>=0;y--){
		(*img)->imagen[y] = (unsigned char *)malloc(sizeof(unsigned char) * (*img)->ih.X);
		if((*img)->imagen[y] == NULL){
			printf("\nNo se pudo dimensionar la imagen en el eje X, Iteracion : %d", y);
			return 0;
		}else{
			fread((*img)->imagen[y], (*img)->ih.X + padding, 1,fp);
		}
	}
	free(id);
	fclose(fp);
	return 1;
}

void SetPaleta(struct ImagenBMP **img){
	int i;

	struct ImagenBMP *ptr = (*img);

	/* Si no hay imagen, no se ejecuta la funcion*/
	if(ptr==NULL) return;

	for(i=0;i<256;i++){
		SetPal(i, ptr->Paleta[i].r >> 2,ptr->Paleta[i].g >> 2,ptr->Paleta[i].b >> 2);
	}
}

/* Dibuja una imagen en la posicion X Y considerando un color de mascara
transparente */

void DibujarIMGMSK(struct ImagenBMP **img, int x, int y, unsigned maskcolor){
	int i,j, xm, ym;

	struct ImagenBMP *rec = *img;
	unsigned char color;

	/* Si no hay imagen, no se imprime */
	if(rec==NULL) return;

	xm =rec->ih.X;
	ym =rec->ih.Y;

	for(j=0;j<ym;j++){
		for(i=0;i<xm;i++){
			color =rec->imagen[j][i];

			if(color!=maskcolor){
				PutPixelX(i + x ,j + y,color);
			}

		}
	}
}

/* Dibuja una imagen en la posicion X Y */
void DibujarIMG(struct ImagenBMP **img, int x, int y){
	int i,j, xm, ym;

	unsigned char color;

	struct ImagenBMP *rec = *img;

	/* Si no hay imagen, no se imprime */
	if(rec==NULL) return;

	xm = rec->ih.X;
	ym = rec->ih.Y;

	for(j=0;j<ym;j++){
		for(i=0;i<xm;i++){
			color = rec->imagen[j][i];
			PutPixelX(i + x ,j + y,color);
		}
	}
}

void LiberarImagen(struct ImagenBMP **img){
	int y;

	if((*img)==NULL)return;

	for(y=(*img)->ih.Y-1; y>=0;y--){
		free((*img)->imagen[y]);
	}

	free((*img)->imagen);
	free((*img));


}

/************************ MAIN	***************************/

int main(int argc, char *argv[]){
	struct ImagenBMP *img = NULL;
	char *bufferNombreArchivo = NULL;
	int buffsiz;
	if(argc<2){
		printf("\nUso : bmp.exe <NombreDeArchivo.bmp>\n /p : Dibuja la paleta.\n");
		return 0;
	}

	buffsiz = strlen(argv[1])+1;
	bufferNombreArchivo = (char*)calloc(buffsiz, 1);
	strncpy(bufferNombreArchivo, argv[1], buffsiz);

	if(!AbrirBMP(bufferNombreArchivo, &img)){
		return 0;
	}

	Set_320x200X();
	SetPaleta(&img);
	DibujarIMG(&img, 0,0);

	getch();

	if(strcmp(argv[2], "/p")==0){
		DibujarPaleta();
	}
	free(bufferNombreArchivo);
	LiberarImagen(&img);

	getch();
	TXTModo();
	return 0;
}





