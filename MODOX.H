/* Modificado por vincebus, creditos a compiler software */

#include "vgaregs.h"

void Set_320x200X ( void );
void PutPixelX( int, int, char );
unsigned char GetPixelX( int, int );
void SetPal( char color, unsigned char r, unsigned char g, unsigned char b );
void DibujarPaleta();
void TXTModo();

/*
 Set_320x200X();                                                                                                                            ±±
 Inicializa el Modo 13X de 320x200 a 256 colores en sistema 1x4.
*/
void Set_320x200X ( )
  {
    asm  mov ax , 0x13                          /* inicializamos modo 13h est ndar */
    asm  int 0x10

	OutPortw( SEQU_ADDR , 0x0604 );       /* ponemos el bit CHAIN-4 a 0 */
	OutPortw( CRTC_ADDR , 0xE317);        /* ajuste del modo de palabras */
	OutPortw( CRTC_ADDR , 0x0014);        /* ajuste del modo de dobles palabras */
	OutPortw( SEQU_ADDR , 0x0F02);        /* seleccionar los cuatro planos para...*/

    asm {                                    /* ...borrar la pantalla */
	 mov ax , 0xA000
	 mov es , ax
	 xor di , di
	 xor ax , ax
	 mov cx , 0x8000
	 rep stosw
	}
  }

/*
 PutPixelX();
 Coloca un punto en la primera p gina en (X,Y) de color color.
*/

void PutPixelX( int x, int y, char color )
{
unsigned int offs;

OutPortb( SEQU_ADDR , 0x02 );                  /* funci¢n seleccionar plano */
OutPortb( SEQU_ADDR+1, 0x01 << ( x & 3 ) );    /* esto calcula autom ticamente
						  el plano a partir de los 2
						  £ltimos bits de la coord.x*/

offs = (y<<6) + (y<<4) + (x>>2);                 /* calculamos el offset */
						 /* esta l¡nea equivale a
						    off = (80*y) + (x/4) ;
						   pero es MUCHO m s r pida.*/
asm {
	mov ax , 0xa000
	mov es , ax
	mov di , offs
	mov al , color
	stosb                                      /* ponemos el punto */
    }
}

/*
 GetPixelX();
 Devuelve el color de un pixel en (X,Y) en sistema 1x4.
*/

unsigned char GetPixelX(int x, int y)
	{
unsigned int offs;

	OutPortw(GRAC_ADDR, 0x04);
	OutPortw(GRAC_ADDR+1, x & 3 );     /* n§ de plano */

offs = (y<<6) + (y<<4) + (x>>2);                 /* calculamos el offset */
						 /* esta l¡nea equivale a
						   off = (80*y) + (x/4) ;
						   pero es MUCHO m s r pida.*/
asm {
	mov ax , 0xa000
	mov es , ax
	mov di , offs                          /* lo leemos */
	mov al, es:[di]
    }

   return( _AL );                              /* y lo devolvemos */
}

void SetPal( char color, unsigned char r, unsigned char g, unsigned char b ){
	OutPortb( 0x3c8, color );
	OutPortb( 0x3c9, r     );
	OutPortb( 0x3c9, g     );
	OutPortb( 0x3c9, b     );
}

/* Cambia al modo de texto, solucion rapida */

void TXTModo(){
	asm xor ax, ax
	asm int 16h
	asm mov ax , 3
	asm int 10h
}

/* Dibuja la paleta en pantalla en el punto 0,0 */

void DibujarPaleta(){
	int x,d,y,i;	i=0;

	for(y=0;y<64;y++){
		for(x=0;x<64;x++){

			if(x%4==0 && y%4==0){
				d=y + 3;

				PutPixelX(x,d,i);
				PutPixelX(x+1,d,i);
				PutPixelX(x+2,d,i);
				PutPixelX(x+3,d,i);
				PutPixelX(x,d+1,i);
				PutPixelX(x+1,d+1,i);
				PutPixelX(x+2,d+1,i);
				PutPixelX(x+3,d+1,i);
				PutPixelX(x,d+2,i);
				PutPixelX(x+1,d+2,i);
				PutPixelX(x+2,d+2,i);
				PutPixelX(x+3,d+2,i);
				PutPixelX(x,d+3,i);
				PutPixelX(x+1,d+3,i);
				PutPixelX(x+2,d+3,i);
				PutPixelX(x+3,d+3,i);

				i++;
			}
		}
	}
}
