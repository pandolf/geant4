#include <cstdlib>

#include "TH1D.h"
#include "TFile.h"

#include "CommonTools/DrawBase.h"


TH1D* getHisto( const std::string& batchProd, int nLayers, float activeLayerThickness, float absorberLayerThickness );
void drawGaussians( DrawBase* db, const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness );


int main() {


  std::string batchProd = "2p5k_v4_withTemp";

  DrawBase* db = new DrawBase(batchProd);
  std::string outputdir = "plots_" + batchProd;
  db->set_outputdir(outputdir);
  std::string mkdir_command = "mkdir -p " + outputdir;
  system(mkdir_command.c_str());

  drawGaussians( db, batchProd, 10., 2. );
  drawGaussians( db, batchProd, 10., 5. );

  return 0;

}



void drawGaussians( DrawBase* db, const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness ) {

  TH1D* h1_6  = getHisto( batchProd,  6, activeLayerThickness, absorberLayerThickness );
  TH1D* h1_9  = getHisto( batchProd,  9, activeLayerThickness, absorberLayerThickness );
  TH1D* h1_12 = getHisto( batchProd, 12, activeLayerThickness, absorberLayerThickness );
  TH1D* h1_20 = getHisto( batchProd, 20, activeLayerThickness, absorberLayerThickness );
  TH1D* h1_25 = getHisto( batchProd, 25, activeLayerThickness, absorberLayerThickness );


  h1_6 ->Rebin( 2 );
  h1_9 ->Rebin( 2 );
  h1_12->Rebin( 2 );
  h1_20->Rebin( 2 );
  h1_25->Rebin( 2 );

  float yMax = h1_25->GetMaximum();
  //if( h1_12->GetMaximum()>yMax ) yMax = h1_12->GetMaximum();
  //if( h1_20->GetMaximum()>yMax ) yMax = h1_20->GetMaximum();
  //if( h1_25->GetMaximum()>yMax ) yMax = h1_25->GetMaximum();
  yMax *= 1.2;


  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  float xMax = (absorberLayerThickness<5.) ? 600. : 450.;
  TH2D* h2_axes = new TH2D("axes", "", 10, 100., xMax, 10, 0., yMax );
  h2_axes->SetXTitle( "ECAL Energy [MeV]");
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

  h1_6 ->SetLineColor( 38 );
  h1_9 ->SetLineColor( 30 );
  h1_12->SetLineColor( kBlack );
  h1_20->SetLineColor( 46 );
  h1_25->SetLineColor( 39 );

  h1_6 ->SetLineWidth( 2 );
  h1_9 ->SetLineWidth( 2 );
  h1_12->SetLineWidth( 2 );
  h1_20->SetLineWidth( 2 );
  h1_25->SetLineWidth( 2 );

  char legendName[200];
  sprintf( legendName, "%.0f mm CeF_{3} / %.0f mm Lead", activeLayerThickness, absorberLayerThickness );
  TLegend* legend = new TLegend( 0.2, 0.5, 0.5, 0.9, legendName );
  legend->SetTextSize( 0.038 );
  legend->SetFillColor( kWhite );
  legend->AddEntry( h1_6, "6 layers", "l" );
  legend->AddEntry( h1_9, "9 layers", "l" );
  legend->AddEntry( h1_12, "12 layers", "l" );
  legend->AddEntry( h1_20, "20 layers", "l" );
  if( absorberLayerThickness<5. )
    legend->AddEntry( h1_25, "25 layers", "l" );
  legend->Draw("same");

  float x1 = 0.4;
  float y1 = 0.953;
  float x2 = 0.96;
  float y2 = 0.975;

  TPaveText* label_top = new TPaveText(x1,y1,x2,y2, "brNDC");
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right
  label_top->SetTextFont(62);
  label_top->AddText("Electron gun, E = 750 MeV");
  label_top->Draw("same");

  if( absorberLayerThickness<5. )
    h1_25->Draw( "same" );
  h1_20->Draw( "same" );
  h1_12->Draw( "same" );
  h1_9 ->Draw( "same" );
  h1_6 ->Draw( "same" );

  gPad->RedrawAxis();

  char canvasName[200];
  sprintf( canvasName, "%s/ecal_gauss_act%.0f_abs%.0f", db->get_outputdir().c_str(), activeLayerThickness, absorberLayerThickness );
  std::string canvasName_str(canvasName);
  std::string canvasName_eps = canvasName_str+".eps";
  std::string canvasName_png = canvasName_str+".png";

  c1->SaveAs(canvasName_eps.c_str());
  c1->SaveAs(canvasName_png.c_str());

  delete c1;
  delete h2_axes;
  delete legend;
  delete label_top;

}


TH1D* getHisto( const std::string& batchProd, int nLayers, float activeLayerThickness, float absorberLayerThickness ) {

  char fileName[500];
  sprintf( fileName, "../batchOutput_750MeV_%s/rootfiles/samplinghistos_n%d_act%.0f_abs%.0f_trasv20.root", batchProd.c_str(), nLayers, activeLayerThickness, absorberLayerThickness );
  TFile* file = TFile::Open( fileName );

  std::string varName = "ecal";
  std::string histoName_get = "h_" + varName + "_config0_750MeV";
  TH1D* h1_sf_tmp = (TH1D*)file->Get(histoName_get.c_str());
  char histoName_out[300];
  sprintf( histoName_out, "ecal_n%d_act%.0f_abs%.0f", nLayers, activeLayerThickness, absorberLayerThickness );
  h1_sf_tmp->SetName(histoName_out);

  return h1_sf_tmp;

}
