/*
  VGAREGS.H

  Esta librer da acceso al control de los registros de la VGA. Es
  necesario leer el artulo o tener conocimientos sobre el tema para
  poder hacer algo bien con la VGA.

  #include "VGAREGS.H"

*/

#define ATTR_ADDR               0x3c0
#define MISC_ADDR               0x3c2
#define VGAENABLE_ADDR          0x3c3
#define SEQU_ADDR               0x3c4
#define GRAC_ADDR               0x3ce
#define CRTC_ADDR               0x3d4
#define STATUS_ADDR             0x3da
#define CHIPSTECH_ADDR          0x3d6

/*
膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊
膊                       DECLARACIONES DE FUNCIONES                     膊
膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊
*/

void UnProtectCRTC( void );
unsigned char RegisterIn( int, char );
void RegisterOut( int, char, unsigned char );

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Funciones para no usar dos.h : */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void OutPortb( int , unsigned char );              /* = outportb y outp */
void OutPortw( int , int );                        /* = outport y outpw */
unsigned char InPortb( int );                      /* = inportb */
unsigned int InPortw( int );                       /* = inport */


/*
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
北 UnProtectCRTC();                                                     北
北                                                                      北
北 Desprotege el CRTC para poder modificar los registros que controla.  北
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
*/

void UnProtectCRTC( void )
{
 int v ;
	OutPortb( CRTC_ADDR, 0x11 );         /* dice = 11h */
	v = InPortb( CRTC_ADDR + 1 );        /* 3d5h */
	v = v & 0x7F;                        /* bit 7 = 0 */
	OutPortb( CRTC_ADDR, 0x11 );         /* dice = 11h */
	OutPortw( CRTC_ADDR, 0x11 | ( v << 8 ) );        /* desproteger */
}


/*
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
北 RegisterIn();                                                        北
北                                                                      北
北 Devuelve el valor que contiene el registro especificado.             北
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
*/

unsigned char RegisterIn( int Controlador, char index )
{
 unsigned char valor;

  switch ( Controlador )
   {
      case MISC_ADDR:
	      valor = InPortb(0x3cc);
	      break;
		      /* 0x3c2 es de escritura y 0x3cc de lectura */

      case ATTR_ADDR:
	      InPortb(STATUS_ADDR);   /* reseteamos el flip-flop */
				      /* el dice *DEBE* tener activado el */
				      /* bit 5. ( p. ej. sum爊dole 32 ); */
	      OutPortb(ATTR_ADDR, index);
	      valor = InPortb(ATTR_ADDR+1);
	      break;

      case SEQU_ADDR:
      case GRAC_ADDR:
      case CRTC_ADDR:
      case CHIPSTECH_ADDR:
	      OutPortb(Controlador, index);     /* las 3 son iguales */
	      valor = InPortb(Controlador+1);
	      break;

      case VGAENABLE_ADDR:
      default:
	      valor = InPortb(Controlador);     /* resto de puertos */
	      break;
   } /* fin de switch */

 OutPortb( ATTR_ADDR, 0x20 );         /* permitir escritura a pantalla */
 return(valor);        /* devolvemos el valor */
}



/*
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
北 RegisterOut();                                                       北
北                                                                      北
北 Escribe el valor "valor" en el registro especificado.                北
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
*/

void RegisterOut( int Controlador, char index, unsigned char valor )
{
  switch (Controlador)
    {
      case ATTR_ADDR:
	   InPortb(STATUS_ADDR);           /* resetear flip-flop */
	   OutPortb(ATTR_ADDR, index);
	   OutPortb(ATTR_ADDR, valor);  /* hecho ... */
	   break;

      case SEQU_ADDR:
	   if (index == 1)                 /* este es el caso m爏 extra */
	    {
	     OutPortw(SEQU_ADDR, 0x0100);           /* resetear el secuenc. */
	     OutPortw(SEQU_ADDR, valor<<8 | 1);     /* enviar valor */
	     OutPortw(SEQU_ADDR, 0x0300);
	     break;
	    };
      case GRAC_ADDR:
      case CRTC_ADDR:
      case CHIPSTECH_ADDR:
	   OutPortw( Controlador , index | valor<<8);
	   break;

      case MISC_ADDR:
      case VGAENABLE_ADDR:
      default:
	   OutPortb(Controlador, valor);
	   break;
    } /* fin de switch */

   OutPortb( ATTR_ADDR, 0x20 );         /* permitir escritura a pantalla */
}



/*
膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊
膊                    FUNCIONES DE PUERTOS MS-DOS                       膊
膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊
*/


/*
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
北 InPortb( puerto );                                                   北
北 Lee del puerto "puerto" y devuelve el valor que tiene. ( 1 byte ).   北
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
*/

unsigned char InPortb( int port )
{
asm mov dx , [port]
asm in al , dx

return ( _AL );
}

/*
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
北 InPortw( puerto );                                                   北
北 Lee del puerto "puerto" y devuelve el valor que tiene. ( 1 word ).   北
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
*/

unsigned int InPortw( int port )
{
asm {
	mov dx , [port]
	in ax , dx
    }
return ( _AX );
}


/*
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
北 OutPortb( puerto, valor );                                           北
北 Escribe el byte valor en el puerto "puerto".                         北
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
*/

void OutPortb ( int port, unsigned char dato )
{
asm {
	mov dx , [port]
	mov al , [dato]
	out dx , al
    }
}

/*
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
北 OutPortw( puerto, valor );                                           北
北 Escribe el word valor en el puerto "puerto".                         北
北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
*/

void OutPortw ( int port, int dato )
{
asm {
	mov dx , [port]
	mov ax , [dato]
	out dx , ax
    }
}

