#include "paquete.h"

void limpiarPaquete(Paquete* paquete){
  paquete->stx=0;
  paquete->op=0;
  paquete->tam=0;
  memset(paquete->datos, 0, sizeof paquete->datos);
  paquete->etx=0;
  paquete->cantDatos=0;
  paquete->estado=ESPERANDO_STX;
}

int agregarCaracter(Paquete* paquete,uint8_t* caracter){

switch (paquete->estado) {
  case ESPERANDO_STX:
    if(*caracter-48 == STX){
      paquete->stx= *caracter-48;
      paquete->estado=ESPERANDO_OP;
    }
    break;
  case ESPERANDO_OP:
  if(*caracter-48<=APP){
    paquete->op= *caracter-48;
    paquete->estado=ESPERANDO_TAM;
  }
  else{
    limpiarPaquete(paquete);
  }
    break;
  case ESPERANDO_TAM:
    paquete->tam= *caracter-48;
    paquete->estado=ESPERANDO_DATOS;
    break;
  case ESPERANDO_DATOS:
    if(paquete->cantDatos < paquete->tam){
      paquete->datos[paquete->cantDatos++]=*caracter;
      if(paquete->cantDatos == paquete->tam){
        paquete->estado=ESPERANDO_ETX;
      }
      else paquete->estado=ESPERANDO_DATOS;

    }
    else{
      limpiarPaquete(paquete);
    }
    break;
  case ESPERANDO_ETX:
    if(*caracter-48 == ETX){
      paquete->etx= *caracter-48;
      paquete->estado=COMPLETO;
    }
    else{
      limpiarPaquete(paquete);
    }
    break;
  case COMPLETO:
      printf("Paquete completo\n");
      return 1;
      break;
  default:
    break;
}

  return 0;
}

void toString(Paquete* paquete){
  printf("Paquete:\n\tSTX: %d\n\tOP: %d\n\tTAM: %d\n\tDATOS: %s\n\tcantDatos: %d\n\tETX: %d\n\tESTADO: %d\n",paquete->stx,paquete->op,paquete->tam,paquete->datos,paquete->cantDatos,paquete->etx,paquete->estado );
}

void toStringModificado(Paquete* paquete){
  printf("Paquete modificado:\n\tSTX: %d\n\tOP: %d\n\tTAM: %d\n\tDATOS: %s\n\tcantDatos: %d\n\tETX: %d\n\tESTADO: %d\n",paquete->stx,paquete->op,paquete->tam,paquete->datos,paquete->cantDatos,paquete->etx,paquete->estado );
}
void mayusculizar(Paquete *original,Paquete *mayusculizado){
mayusculizado->stx=original->stx;
mayusculizado->op=original->op;
mayusculizado->tam=original->tam;
mayusculizado->etx=original->etx;
mayusculizado->cantDatos=original->cantDatos;
mayusculizado->estado=original->estado;

int desp='a'-'A';

for (size_t i = 0; i < original->tam; i++) {
  if(original->datos[i]>='a'&&original->datos[i]<='z'){
    mayusculizado->datos[i]=original->datos[i]-desp;;
  }
  else
    mayusculizado->datos[i]=original->datos[i];

  }

}

void minusculizar(Paquete *original,Paquete *minusculizado){

  minusculizado->stx=original->stx;
  minusculizado->op=original->op;
  minusculizado->tam=original->tam;
  minusculizado->etx=original->etx;
  minusculizado->cantDatos=original->cantDatos;
  minusculizado->estado=original->estado;

  int desp='a'-'A';

  for (size_t i = 0; i < original->tam; i++) {
    if(original->datos[i]>='A'&&original->datos[i]<='Z'){
      minusculizado->datos[i]=original->datos[i]+desp;;
    }
    else
      minusculizado->datos[i]=original->datos[i];
    }

}


void realizarOperacion(Paquete *original,Paquete *aModificar){

  switch (original->op) {
    case MAYUSCULA:
    mayusculizar(original,aModificar);
      break;
    case MINUSCULA:
    minusculizar(original,aModificar);
      break;
    case STACK:
      break;
    case HEAP:
      break;
    case APP:
      break;
    default:
      break;
    }
}
