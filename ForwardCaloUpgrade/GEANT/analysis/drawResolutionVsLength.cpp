#include <iostream>

#include "CommonTools/DrawBase.h"
#include "CommonTools/fitTools.h"

#include "TCanvas.h"


TH1D* get_histo_vs_length( const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int nLayers );



int main() {


  DrawBase* db = new DrawBase("db");

  std::string batchProd = "provaNEW1";

  int nLayers = 30;
  float activeLayerThickness = 5.;
  float absorberLayerThickness = 5.;

  TH1D* h1_resolution_vs_length_act5_abs5 =  get_histo_vs_length( batchProd, activeLayerThickness, absorberLayerThickness, nLayers );


  return 0;

}


TH1D* get_histo_vs_length( const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int nLayers ) {


  char histoName[200];
  sprintf( histoName, "resolution_vs_length_act%.0f_abs%.0f", activeLayerThickness, absorberLayerThickness );
  TH1D* h1 = new TH1D( histoName, "", nLayers, 0., (float)nLayers );


  for( unsigned iLayer=1; iLayer<=nLayers; ++iLayer ) {

    char fileName[200];
    sprintf( fileName, "../batchDir_%s/rootfiles/samplinghistos_n%d_act%.0f_abs%.0f.root", batchProd.c_str(), iLayer, activeLayerThickness, absorberLayerThickness );
    TFile* file = TFile::Open( fileName );

    TH1D* h1_sf = (TH1D*)file->Get("h_sf_config0_750MeV");
    
    float sf_mean = h1_sf->GetMean();
    float sf_rms  = h1_sf->GetRMS();

    float resolution = sf_rms/sf_mean;

    h1->SetBinContent( iLayer, resolution );

    file->Close();

  }

  
  return h1;

}
