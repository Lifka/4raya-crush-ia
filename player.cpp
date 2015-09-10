/*

********************************************

  Javier Izquierdo Vera
  javierizquierdovera@gmail.com
  UGR

********************************************

*/#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"
#include <vector>

#define aceptamos 50
#define malo 0

      // 0 --> vacío
      // 1 --> J1
      // 2 --> J2
      // 4 --> BOOM J1
      // 5 --> BOOM J2

#define vacio 0
#define J1 1
#define J2 2
#define BOOMJ1 4
#define BOOMJ2 5

#define total 7

#define dos 2
#define tres 6
#define escala 2
#define ganar 8000.0
#define perder -9999.0

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

// LINEA VERDE EN LA ESQUINA INFERIOR DERECHA
// COn tablero vacío minimax 9 put 4

// Prefiere situar fichas que se encuentren situadas en el tablero, cuanto más cercano de 4 mejor
double Puntuacion(int jugador, const Environment &estado){
    double suma=0;


    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}

// No modificar
// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else 
          return Puntuacion(jugador,estado);
}


vector<vector<char> > getMatrixMov(const Environment &tablero){
  vector<vector<char> > matrix;

  matrix.resize(total);
  matrix[0].resize(total);
  matrix[1].resize(total);
  matrix[2].resize(total);
  matrix[3].resize(total);
  matrix[4].resize(total);
  matrix[5].resize(total);
  matrix[6].resize(total);

  for(int i = 0; i < total; i++)
    for(int j = 0; j < total; j++)
        matrix[i][j] = tablero.See_Casilla(i,j);

  return matrix;

}

bool esMiBomba(int pos, int jugador){

  if (pos == 4 && jugador == 1){
    return true;
  } else if(pos == 5 && jugador == 2)
    return true;
  else
    return false;

}


double puntuacion_(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador || esMiBomba(estado.See_Casilla(i,j), jugador)){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


int enemigoDe(int jugador){
    if (jugador == 1)
      return 1;
    else
      return 2;
}


bool comprobarPosClave(vector<vector<char> > matrix, int jugador, int i, int j){
  if (matrix[i][j] == jugador) // La casilla es del jugador
    if (matrix[i][j+1] == jugador){ // la siguiente es del jugador
      if(matrix[i][j+2] == 0 && matrix[i][j-1] == 0) // las de los lados están libres
        if (i > 0){ // Si estamos por arriba
          if (matrix[i-1][j+2] != 0 && matrix[i-1][j-1] != 0) // comprobar que hay suelo
            return true;
        } else // Si no estamos arriba, hay suelo
          return true;
        

    } else if (matrix[i][j-1] == jugador){ // La anterior es del jugador
       if(matrix[i][j-2] == 0 && matrix[i][j+1] == 0) // las de los lados están libres
        if (i > 0){ // Si estamos por arriba
          if (matrix[i-1][j-2] != 0 && matrix[i-1][j+1] != 0) // comprobar que hay suelo
            return true;
        } else // Si no estamos arriba, hay suelo
          return true;


    }
    return false;

}

int evalua_tablero(int jugador, const Environment &estado){

  vector<vector<char> > matrix = getMatrixMov(estado);

  int suma = 0;
  int contar= 0;
  int contar2 = 0;


  for (int i = 0; i < total; i++){
    for(int j = 0; j < total; j++){
      if(matrix[i][j] == jugador)
        contar++;
      else
        contar = 0;


      if (contar == 2 && (
        ((j+1) < total && (j-1) > 0) ||
        ((j+2) < total) ||
        ((j-2) > 0))
        )
        suma += dos;
      else if(contar > 2 && (
        ((j+1) < total) ||
        ((j-1) > 0))
        )
        suma += tres;

      if (comprobarPosClave(matrix, jugador, i, j)){
        suma += tres;
      }

      if(matrix[j][i] == jugador)
        contar2++;
      else
        contar2 = 0;

      if (contar2 == 2 && (
        ((i+1) < total && (i-1) > 0) ||
        ((i+2) < total) ||
        ((i-2) > 0))
        )
        suma += dos;
      else if(contar2 > 2 && (
        ((i+1) < total) ||
        ((i-1) > 0))
        )
        suma += tres;


    }
  }


contar = 0;

  for(int i = 0; i < total; i++){
    for(int j = 0; j < total-i; j++){

      if(matrix[j][j+i] == jugador)
        contar++;
      else
        contar = 0;


      if (contar == 2)
        suma += dos*2;
      else if(contar > 2)
        suma += tres*2;

    }
  }

contar = 0;

  for(int i = 1; i < total; i++){
    for(int j = 1; j < i; j++){

      if(matrix[j][j-i] == jugador)
        contar++;
      else
        contar = 0;


      if (contar == 2)
        suma += dos*2;
      else if(contar > 2)
        suma += tres*2;


    }
  }

  return contar;


}



int evalua_enemigo(int enemigo, const Environment &estado){
  int valoracion = 0;

  valoracion += puntuacion_(enemigo,estado);
  valoracion += evalua_tablero(enemigo,estado);

  return valoracion;
}


int evaluame(int jugador, const Environment &estado){
  int valoracion = 0;

  if (jugador == 2)
    valoracion += puntuacion_(jugador,estado)*2;
  else
    valoracion += puntuacion_(jugador,estado);
  valoracion += evalua_tablero(jugador,estado);

  return valoracion;

}

int fichasDe(const Environment &estado, int jugador){

  int enemigo = enemigoDe(jugador);
  int casillas_jugador = 0;
  vector<vector<char> > matrix = getMatrixMov(estado);

  for (int i = 0; i < total; i++)
    for(int j = 0; j < total; j++){
      if(matrix[i][j] == jugador || esMiBomba(matrix[i][j],jugador))
        casillas_jugador++;
    }

  return (casillas_jugador);


}


// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Valoracion(const Environment &estado, int jugador){
  int ganador = estado.RevisarTablero();
  int enemigo = enemigoDe(ganador);
  int valoracion = 0;


  if (ganador==jugador)
       return ganar; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return perder; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else{

            

      valoracion = evaluame(jugador,estado) - evalua_enemigo(enemigo,estado);


      if (fichasDe(estado,jugador) > (fichasDe(estado,enemigo)+1))
         valoracion *= escala;
       else
          valoracion -= escala * 2;

          
      return valoracion;
    }

}





// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}


double PodaAlfaBeta(const Environment & tablero, int jugador, int profundidad, int profLimite, Environment::ActionType & accion, double alfa, double beta){
      Environment::ActionType ult_accion;
      

       //Profundidad máxima o fin del juego, se acabó
       if(profundidad == profLimite || tablero.JuegoTerminado())   
           return Valoracion(tablero, jugador);
       else{ // Hijos                                                                                                 
          int ult_act = -1;
          //Generar siguiente nodo tablero hijo
           Environment hijo = tablero.GenerateNextMove(ult_act);  

           //Profundidad par --> nodo MAX
          if(profundidad%2 == 0){                                                                             

           //Recorrido de nodos hijos
          while(!(hijo==tablero)){                                              

            double aux = PodaAlfaBeta(hijo, jugador, profundidad+1,profLimite,ult_accion,alfa,beta);

             // if (profundidad == 0)
                //cout << "Acción -> " << ult_act << endl;
              if(aux > alfa){
                alfa = aux;
                if (profundidad == 0)
                  accion = static_cast<Environment::ActionType> (ult_act);       
              } 

              if(beta <= alfa){                                                   
                return alfa; //poda
              } else {
             //Generación del siguiente nodo tablero hijo
                hijo = tablero.GenerateNextMove(ult_act); 
              }

          }

          return alfa; // no poda
          

          }else{                                                                     
            //Nivel impar -> nodo MIN
            while(!(hijo==tablero)){                                                

              double aux = PodaAlfaBeta(hijo, jugador, profundidad+1, profLimite, ult_accion, alfa, beta);

              if(aux < beta)
                beta = aux;       

              if(beta <= alfa){                                                 
                return beta;  //poda
              } else{
                hijo = tablero.GenerateNextMove(ult_act);          
              }                 

          }

          return beta; // no poda

      }


    }
}




// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acción que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;


    //--------------------- COMENTAR Desde aqui
   /* cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

   */ //--------------------- COMENTAR Hasta aqui


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    valor = PodaAlfaBeta(actual_, jugador_, 0, PROFUNDIDAD_ALFABETA, accion, menosinf, masinf);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;
// hacer el minmax y adaptarlo a esta (copiarlo) y luego convertirlo en alphabeta
    
    return accion;
}

