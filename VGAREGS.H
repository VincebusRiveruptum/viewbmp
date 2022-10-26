/*
  VGAREGS.H

  Esta librer�a da acceso al control de los registros de la VGA. Es
  necesario leer el art�culo o tener conocimientos sobre el tema para
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
��������������������������������������������������������������������������
��                       DECLARACIONES DE FUNCIONES                     ��
��������������������������������������������������������������������������
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
��������������������������������������������������������������������������
�� UnProtectCRTC();                                                     ��
��                                                                      ��
�� Desprotege el CRTC para poder modificar los registros que controla.  ��
��������������������������������������������������������������������������
*/

void UnProtectCRTC( void )
{
 int v ;
	OutPortb( CRTC_ADDR, 0x11 );         /* �ndice = 11h */
	v = InPortb( CRTC_ADDR + 1 );        /* 3d5h */
	v = v & 0x7F;                        /* bit 7 = 0 */
	OutPortb( CRTC_ADDR, 0x11 );         /* �ndice = 11h */
	OutPortw( CRTC_ADDR, 0x11 | ( v << 8 ) );        /* desproteger */
}


/*
��������������������������������������������������������������������������
�� RegisterIn();                                                        ��
��                                                                      ��
�� Devuelve el valor que contiene el registro especificado.             ��
��������������������������������������������������������������������������
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
				      /* el �ndice *DEBE* tener activado el */
				      /* bit 5. ( p. ej. sum�ndole 32 ); */
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
��������������������������������������������������������������������������
�� RegisterOut();                                                       ��
��                                                                      ��
�� Escribe el valor "valor" en el registro especificado.                ��
��������������������������������������������������������������������������
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
	   if (index == 1)                 /* este es el caso m�s extra�o */
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
��������������������������������������������������������������������������
��                    FUNCIONES DE PUERTOS MS-DOS                       ��
��������������������������������������������������������������������������
*/


/*
��������������������������������������������������������������������������
�� InPortb( puerto );                                                   ��
�� Lee del puerto "puerto" y devuelve el valor que tiene. ( 1 byte ).   ��
��������������������������������������������������������������������������
*/

unsigned char InPortb( int port )
{
asm mov dx , [port]
asm in al , dx

return ( _AL );
}

/*
��������������������������������������������������������������������������
�� InPortw( puerto );                                                   ��
�� Lee del puerto "puerto" y devuelve el valor que tiene. ( 1 word ).   ��
��������������������������������������������������������������������������
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
��������������������������������������������������������������������������
�� OutPortb( puerto, valor );                                           ��
�� Escribe el byte valor en el puerto "puerto".                         ��
��������������������������������������������������������������������������
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
��������������������������������������������������������������������������
�� OutPortw( puerto, valor );                                           ��
�� Escribe el word valor en el puerto "puerto".                         ��
��������������������������������������������������������������������������
*/

void OutPortw ( int port, int dato )
{
asm {
	mov dx , [port]
	mov ax , [dato]
	out dx , ax
    }
}
