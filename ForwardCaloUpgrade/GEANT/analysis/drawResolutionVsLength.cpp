#include <cstdlib>
#include <iostream>

#include "CommonTools/DrawBase.h"
#include "CommonTools/fitTools.h"

#include "TCanvas.h"


std::pair<TH1D*,TH1D*> get_histos_vs_length( const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int nLayers );
void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3, TH1D* h1_4 );



int main() {


  std::string batchProd = "5k_v4";

  DrawBase* db = new DrawBase("db");

  std::string outputdir = "plots_" + batchProd;
  db->set_outputdir(outputdir);
  std::string mkdir_command = "mkdir -p " + outputdir;
  system(mkdir_command.c_str());


  int nLayers = 30;

  std::pair<TH1D*,TH1D*> pair_act5_abs5 =  get_histos_vs_length( batchProd, 5., 5., nLayers );
  std::pair<TH1D*,TH1D*> pair_act5_abs2 =  get_histos_vs_length( batchProd, 5., 2., nLayers );
  std::pair<TH1D*,TH1D*> pair_act10_abs5 =  get_histos_vs_length( batchProd, 10., 5., nLayers );
  std::pair<TH1D*,TH1D*> pair_act10_abs2 =  get_histos_vs_length( batchProd, 10., 2., nLayers );

  drawComparison( db, "resolution", "Resolution", pair_act5_abs5.second, pair_act5_abs2.second, pair_act10_abs5.second, pair_act10_abs2.second );
  drawComparison( db, "sf", "Sampling Fraction", pair_act5_abs5.first, pair_act5_abs2.first, pair_act10_abs5.first, pair_act10_abs2.first );

  return 0;

}


std::pair<TH1D*,TH1D*> get_histos_vs_length( const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int nLayers ) {


  char histoName_sf[200];
  sprintf( histoName_sf, "sf_vs_length_act%.0f_abs%.0f", activeLayerThickness, absorberLayerThickness );
  TH1D* h1_sf = new TH1D( histoName_sf, "", nLayers, 0.5, (float)nLayers+0.5 );

  char histoName_reso[200];
  sprintf( histoName_reso, "resolution_vs_length_act%.0f_abs%.0f", activeLayerThickness, absorberLayerThickness );
  TH1D* h1_reso = new TH1D( histoName_reso, "", nLayers, 0.5, (float)nLayers+0.5 );


  for( unsigned iLayer=1; iLayer<nLayers; ++iLayer ) {

    char fileName[200];
    sprintf( fileName, "../batchOutput_750MeV_%s/rootfiles/samplinghistos_n%d_act%.0f_abs%.0f.root", batchProd.c_str(), iLayer, activeLayerThickness, absorberLayerThickness );
    TFile* file = TFile::Open( fileName );

    if( file==0 ) {
      std::cout << "WARNING! Didn't find file: " << fileName << "   ... skipping" << std::endl;
      h1_sf->SetBinContent( iLayer, -1 );
      h1_reso->SetBinContent( iLayer, -1 );
      continue;
    }

    TH1D* h1_sf_tmp = (TH1D*)file->Get("h_sf_config0_750MeV");
    
    float sf_mean = h1_sf_tmp->GetMean();
    float sf_rms  = h1_sf_tmp->GetRMS();

    float sf_mean_err = h1_sf_tmp->GetMeanError();
    float sf_rms_err  = h1_sf_tmp->GetRMSError();

    float resolution = sf_rms/sf_mean;
    float resolution_err = sqrt( sf_rms_err*sf_rms_err/(sf_mean*sf_mean) + resolution*resolution*sf_mean_err*sf_mean_err/(sf_mean*sf_mean*sf_mean*sf_mean) );


    h1_reso->SetBinContent( iLayer, resolution );
    h1_sf->SetBinContent( iLayer, sf_mean );

    h1_sf->SetBinError( iLayer, sf_mean_err );
    h1_reso->SetBinError( iLayer, resolution_err );

    file->Close();

  }


  std::pair<TH1D*,TH1D*> returnPair;
  returnPair.first = h1_sf;
  returnPair.second = h1_reso;
  
  return returnPair;

}



void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3, TH1D* h1_4 ) {


  const char* name1 = h1_1->GetName();
  const char* name2 = h1_2->GetName();
  const char* name3 = h1_3->GetName();
  const char* name4 = h1_4->GetName();

  int act1, abs1;
  int act2, abs2;
  int act3, abs3;
  int act4, abs4;

  std::string scanfText = varName + "_vs_length_act%d_abs%d";
  sscanf( name1, scanfText.c_str(), &act1, &abs1 );
  sscanf( name2, scanfText.c_str(), &act2, &abs2 );
  sscanf( name3, scanfText.c_str(), &act3, &abs3 );
  sscanf( name4, scanfText.c_str(), &act4, &abs4 );

  float markerSize = 1.6;

  h1_1->SetMarkerStyle(20);
  h1_1->SetMarkerColor(46);
  h1_1->SetMarkerSize(markerSize);

  h1_2->SetMarkerStyle(21);
  h1_2->SetMarkerColor(kRed+3);
  h1_2->SetMarkerSize(markerSize);

  h1_3->SetMarkerStyle(24);
  h1_3->SetMarkerColor(kBlack);
  h1_3->SetMarkerSize(markerSize);

  h1_4->SetMarkerStyle(25);
  h1_4->SetMarkerColor(13);
  h1_4->SetMarkerSize(markerSize);



  float xMax = h1_1->GetXaxis()->GetXmax();
  float yMax = h1_1->GetMaximum();
  if( h1_2->GetMaximum() >yMax ) yMax = h1_2 ->GetMaximum();
  if( h1_3->GetMaximum()>yMax )  yMax = h1_3->GetMaximum();
  if( h1_4->GetMaximum()>yMax )  yMax = h1_4->GetMaximum();
  yMax *= 1.6;
  

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D("axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle("Number of Layers");
  h2_axes->SetYTitle(yAxisName.c_str());

  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.55, 0.63, 0.9, 0.9, "CeF_{3} / Lead" );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);

  char legendText1[200];
  char legendText2[200];
  char legendText3[200];
  char legendText4[200];

  sprintf( legendText1, "%d mm / %d mm", act1, abs1 );
  sprintf( legendText2, "%d mm / %d mm", act2, abs2 );
  sprintf( legendText3, "%d mm / %d mm", act3, abs3 );
  sprintf( legendText4, "%d mm / %d mm", act4, abs4 );

  legend->AddEntry( h1_1, legendText1, "P" );
  legend->AddEntry( h1_2, legendText2, "P" );
  legend->AddEntry( h1_3, legendText3, "P" );
  legend->AddEntry( h1_4, legendText4, "P" );
  legend->Draw("same");

  TPaveText* labelTop = db->get_labelTop();
  labelTop->Draw("same");

  h1_1->Draw("P same");
  h1_2->Draw("P same");
  h1_3->Draw("P same");
  h1_4->Draw("P same");


  std::string canvasName = db->get_outputdir() + "/" + varName +"_vs_length";
  std::string canvasName_eps = canvasName+".eps";
  std::string canvasName_png = canvasName+".png";

  c1->SaveAs(canvasName_eps.c_str());
  c1->SaveAs(canvasName_png.c_str());

  delete c1;
  delete h2_axes;
  delete legend;

}
