#!/usr/bin/python



lengthOK = true

while (lengthOK):

  X0_tung = 3.5
  X0_cef3 = 16.8


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
  

    python sendOnBatch.py $1 15 10 2 $i
    python sendOnBatch.py $1 20 10 2 $i
    python sendOnBatch.py $1 10 10 5 $i
    python sendOnBatch.py $1 25 5  2 $i
done
