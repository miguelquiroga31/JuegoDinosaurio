#include "Ave.h"

Ave::Ave(int posicionInicial) {  
  x = posicionInicial;  
}

void Ave::mover() {   
  if(x > 0) {
    x = x - 1;
  }
  else {
    x = 20;
  }  
}

int Ave::obtenerPosicion() {
  return x;
}