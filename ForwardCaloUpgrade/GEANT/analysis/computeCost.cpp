#include <iostream>

#include "CommonTools/DrawBase.h"


// all lengths in mm




float getCompositeSum( int act_n, float act_l );
float getEffectiveX0( int act_n, float act_thick, float act_X0 );

int main() {


  float X0_tung = 3.5;
  float X0_lyso = 11.4;
  float X0_cef3 = 16.8;


  // first, compute stuff for the LYSO calorimeter
  float lysocal_act_thick = 1.5;
  float lysocal_abs_thick = 2.5;
  int   lysocal_act_n     = 29;
  int   lysocal_abs_n     = 28;

  float lysocal_length_act = getCompositeSum( lysocal_act_n, lysocal_act_thick );
  float lysocal_length_abs = getCompositeSum( lysocal_abs_n, lysocal_abs_thick );
  float lysocal_length_tot = lysocal_length_act + lysocal_length_abs;

  float lysocal_X0_act = getEffectiveX0( lysocal_act_n, lysocal_act_thick, X0_lyso );
  float lysocal_X0_abs = getEffectiveX0( lysocal_abs_n, lysocal_abs_thick, X0_tung );
  float lysocal_X0_tot = lysocal_X0_act + lysocal_X0_abs;

  std::cout << "*** LYSO CAL: " << std::endl;
  std::cout << " Active material: " << lysocal_act_n << " layers of LYSO (X0 = " << X0_lyso << " mm)" << std::endl;
  std::cout << " Absorber       : " << lysocal_abs_n << " layers of W    (X0 = " << X0_tung << " mm)" << std::endl;
  std::cout << " Total length   : " << lysocal_length_tot << " mm" << std::endl;
  std::cout << " Total X0       : " << lysocal_X0_tot     << std::endl;

  //float lysocal_cost   = getCostPerSquareMeter( lysocal_act_n, X0_lyso, lysocal_abs_n, X0_tung );






  return 0;

}




float getCompositeSum( int act_n, float act_l ) {

  float sum = (float)act_n*act_l;

  return sum;

}


float getEffectiveX0( int act_n, float act_thick, float act_X0 ) {

  float X0 = (float)act_n*act_thick/act_X0;

  return X0;

}


//float lysocal_cost   = getCostPerSquareMeter( lysocal_act_n, X0_lyso, lysocal_abs_n, X0_tung );
