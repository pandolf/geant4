#include <cstdlib>
#include <iostream>

#include "CommonTools/DrawBase.h"
#include "CommonTools/fitTools.h"

#include "TCanvas.h"


std::pair<TH1D*,TH1D*> get_histos_vs_length( const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int nLayers );
void drawComparison( DrawBase* db, const std::string& saveName, const std::string& yAxisName, TH1D* h1_act5_abs5, TH1D* h1_act5_abs2, TH1D* h1_act10_abs5, TH1D* h1_act10_abs2 );



int main() {


  DrawBase* db = new DrawBase("db");

  std::string outputdir = "plots";
  db->set_outputdir(outputdir);
  std::string mkdir_command = "mkdir -p " + outputdir;
  system(mkdir_command.c_str());

  std::string batchProd = "provaNEW1";

  int nLayers = 30;

  std::pair<TH1D*,TH1D*> pair_act5_abs5 =  get_histos_vs_length( batchProd, 5., 5., nLayers );
  std::pair<TH1D*,TH1D*> pair_act5_abs2 =  get_histos_vs_length( batchProd, 5., 2., nLayers );
  std::pair<TH1D*,TH1D*> pair_act10_abs5 =  get_histos_vs_length( batchProd, 10., 5., nLayers );
  std::pair<TH1D*,TH1D*> pair_act10_abs2 =  get_histos_vs_length( batchProd, 10., 2., nLayers );

  drawComparison( db, "reso", "Resolution", pair_act5_abs5.second, pair_act5_abs2.second, pair_act10_abs5.second, pair_act10_abs2.second );

  return 0;

}


std::pair<TH1D*,TH1D*> get_histos_vs_length( const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int nLayers ) {


  char histoName_sf[200];
  sprintf( histoName_sf, "sf_vs_length_act%.0f_abs%.0f", activeLayerThickness, absorberLayerThickness );
  TH1D* h1_sf = new TH1D( histoName_sf, "", nLayers, 0., (float)nLayers );

  char histoName_reso[200];
  sprintf( histoName_reso, "resolution_vs_length_act%.0f_abs%.0f", activeLayerThickness, absorberLayerThickness );
  TH1D* h1_reso = new TH1D( histoName_reso, "", nLayers, 0., (float)nLayers );


  for( unsigned iLayer=1; iLayer<=nLayers; ++iLayer ) {

    char fileName[200];
    sprintf( fileName, "../batchDir_%s/rootfiles/samplinghistos_n%d_act%.0f_abs%.0f.root", batchProd.c_str(), iLayer, activeLayerThickness, absorberLayerThickness );
    TFile* file = TFile::Open( fileName );

    TH1D* h1_sf = (TH1D*)file->Get("h_sf_config0_750MeV");
    
    float sf_mean = h1_sf->GetMean();
    float sf_rms  = h1_sf->GetRMS();

    float resolution = sf_rms/sf_mean;

    h1_sf->SetBinContent( iLayer, sf_mean );
    h1_reso->SetBinContent( iLayer, resolution );

    file->Close();

  }

  std::pair<TH1D*,TH1D*> returnPair;
  returnPair.first = h1_sf;
  returnPair.second = h1_reso;
  
  return returnPair;

}



void drawComparison( DrawBase* db, const std::string& saveName, const std::string& yAxisName, TH1D* h1_act5_abs5, TH1D* h1_act5_abs2, TH1D* h1_act10_abs5, TH1D* h1_act10_abs2 ) {


  h1_act5_abs5->SetMarkerStyle(20);
  h1_act5_abs5->SetMarkerColor(46);
  h1_act5_abs5->SetMarkerSize(1.3);

  h1_act5_abs2->SetMarkerStyle(21);
  h1_act5_abs2->SetMarkerColor(kRed+1);
  h1_act5_abs2->SetMarkerSize(1.3);

  h1_act10_abs5->SetMarkerStyle(24);
  h1_act10_abs5->SetMarkerColor(29);
  h1_act10_abs5->SetMarkerSize(1.3);

  h1_act10_abs2->SetMarkerStyle(25);
  h1_act10_abs2->SetMarkerColor(13);
  h1_act10_abs2->SetMarkerSize(1.3);


  float xMax = h1_act5_abs5->GetXaxis()->GetXmax();
  float yMax = h1_act5_abs5->GetMaximum();
  if( h1_act5_abs2->GetMaximum()>yMax ) yMax = h1_act5_abs2->GetMaximum();
  if( h1_act10_abs5->GetMaximum()>yMax ) yMax = h1_act10_abs5->GetMaximum();
  if( h1_act10_abs2->GetMaximum()>yMax ) yMax = h1_act10_abs2->GetMaximum();
  yMax *= 1.1;
  

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D("axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle("Number of Layers");
  h2_axes->SetYTitle(yAxisName.c_str());

  h2_axes->Draw();

  h1_act5_abs5->Draw("P same");
  h1_act5_abs2->Draw("P same");
  h1_act10_abs5->Draw("P same");
  h1_act10_abs2->Draw("P same");


  std::string canvasName = db->get_outputdir() + "/" + saveName +"_vs_length";
  std::string canvasName_eps = canvasName+".eps";
  std::string canvasName_png = canvasName+".png";

  c1->SaveAs(canvasName_eps.c_str());
  c1->SaveAs(canvasName_png.c_str());

  delete c1;
  delete h2_axes;

}
