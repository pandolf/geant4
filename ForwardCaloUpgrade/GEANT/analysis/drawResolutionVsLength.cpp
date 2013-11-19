#include <cstdlib>
#include <iostream>

#include "CommonTools/DrawBase.h"
#include "CommonTools/fitTools.h"

#include "TCanvas.h"


void drawStuffForOneVariable( DrawBase* db, const std::string& varName, const std::string& axisName, const std::string& units, const std::string& batchProd, int nLayers );
std::pair<TH1D*,TH1D*> get_histos_vs_length( const std::string& varName, const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int nLayers );
void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, const std::string& units, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3, TH1D* h1_4 );



int main() {


  std::string batchProd = "2p5k_v4_withTemp";
  //std::string batchProd = "5k_v4";

  DrawBase* db = new DrawBase("db");

  std::string outputdir = "plots_" + batchProd;
  db->set_outputdir(outputdir);
  std::string mkdir_command = "mkdir -p " + outputdir;
  system(mkdir_command.c_str());


  int nLayers = 30;

  drawStuffForOneVariable( db, "sf",         "Sampling Fraction", "", batchProd, nLayers );
  drawStuffForOneVariable( db, "ecal",       "ECAL Energy", "MeV", batchProd, nLayers );
  drawStuffForOneVariable( db, "e_dep",      "ECAL Energy", "MeV", batchProd, nLayers );

  return 0;

}


void drawStuffForOneVariable( DrawBase* db, const std::string& varName, const std::string& axisName, const std::string& units, const std::string& batchProd, int nLayers ) {

  std::pair<TH1D*,TH1D*> pair_act5_abs5  =  get_histos_vs_length( varName, batchProd, 5., 5., nLayers );
  std::pair<TH1D*,TH1D*> pair_act5_abs2  =  get_histos_vs_length( varName, batchProd, 5., 2., nLayers );
  std::pair<TH1D*,TH1D*> pair_act10_abs5 =  get_histos_vs_length( varName, batchProd, 10., 5., nLayers );
  std::pair<TH1D*,TH1D*> pair_act10_abs2 =  get_histos_vs_length( varName, batchProd, 10., 2., nLayers );

  drawComparison( db, varName+"res", axisName + " Resolution",    "", pair_act5_abs5.second, pair_act5_abs2.second, pair_act10_abs5.second, pair_act10_abs2.second );
  drawComparison( db, varName,       axisName,                 units, pair_act5_abs5.first,  pair_act5_abs2.first,  pair_act10_abs5.first,  pair_act10_abs2.first );

}


std::pair<TH1D*,TH1D*> get_histos_vs_length( const std::string& varName, const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int nLayers ) {


  char histoName_sf[200];
  sprintf( histoName_sf, "%s_vs_length_act%.0f_abs%.0f", varName.c_str(), activeLayerThickness, absorberLayerThickness );
  TH1D* h1_sf = new TH1D( histoName_sf, "", nLayers, 0.5, (float)nLayers+0.5 );

  char histoName_reso[200];
  sprintf( histoName_reso, "%sres_vs_length_act%.0f_abs%.0f", varName.c_str(), activeLayerThickness, absorberLayerThickness );
  TH1D* h1_reso = new TH1D( histoName_reso, "", nLayers, 0.5, (float)nLayers+0.5 );

  std::string histoName_get = "h_" + varName + "_config0_750MeV";



  for( unsigned iLayer=1; iLayer<nLayers; ++iLayer ) {

    if( activeLayerThickness==10. && absorberLayerThickness==5. && iLayer>26 ) continue; // EE is too long (>400 cm)

    char fileName[500];
    sprintf( fileName, "../batchOutput_750MeV_%s/rootfiles/samplinghistos_n%d_act%.0f_abs%.0f_trasv20.root", batchProd.c_str(), iLayer, activeLayerThickness, absorberLayerThickness );
    TFile* file = TFile::Open( fileName );

    if( file==0 ) {
      std::cout << "WARNING! Didn't find file: " << fileName << "   ... skipping" << std::endl;
      h1_sf->SetBinContent( iLayer, -1 );
      h1_reso->SetBinContent( iLayer, -1 );
      continue;
    }

    TH1D* h1_sf_tmp = (TH1D*)file->Get(histoName_get.c_str());

    if( h1_sf_tmp==0 ) {
 
      TH1F::AddDirectory(kTRUE);

      if( iLayer==1 ) 
        std::cout << "-> Didn't find histogram: '" << histoName_get << ". Will go for the Cell tree." << std::endl;

      char fileName_temp[500];
      sprintf( fileName_temp, "../batchOutput_750MeV_%s/rootfiles/temp/temp_n%d_act%.0f_abs%.0f_trasv20.root", batchProd.c_str(), iLayer, activeLayerThickness, absorberLayerThickness );
      TFile* file_temp = TFile::Open( fileName_temp );

      TTree* tree_cell = (TTree*)file_temp->Get("Cell");

      int n_cells;
      tree_cell->SetBranchAddress( "n_cells", &n_cells);
      tree_cell->GetEntry(1);

      std::string treeVarName = varName + "[0]";
      for( unsigned icell=1; icell<n_cells; ++icell ) { //add for all cells
        char anotherCellName[100];
        sprintf( anotherCellName, "+%s[%d]", varName.c_str(), icell );
        std::string anotherCellName_str(anotherCellName);
        treeVarName += anotherCellName_str;
      }

      h1_sf_tmp = new TH1D("sf_tmp", "", 200, 0., 750.); //eMax = 750 mev
      
      tree_cell->Project( "sf_tmp", treeVarName.c_str() );

    }

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



void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, const std::string& units, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3, TH1D* h1_4 ) {

  TString yAxisName_tstr(yAxisName);
  bool isResolution = yAxisName_tstr.Contains("Resolution");


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
  float scaleFactor = 1.5;
  if( isResolution ) scaleFactor = 1.1;
  yMax *= scaleFactor;
  
  if(varName=="ecal"||varName=="e_dep") yMax = 900.;

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D("axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle("Number of Layers");
  std::string totalAxisName = yAxisName;
  if( units!="" ) totalAxisName = totalAxisName + " [" + units + "]";
  h2_axes->SetYTitle(totalAxisName.c_str());
  
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.58, 0.63, 0.9, 0.9, "CeF_{3} / Lead" );
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


  h1_1->Draw("P same");
  h1_2->Draw("P same");
  h1_3->Draw("P same");
  h1_4->Draw("P same");

  gPad->RedrawAxis();

  std::string canvasName = db->get_outputdir() + "/" + varName +"_vs_length";
  std::string canvasName_eps = canvasName+".eps";
  std::string canvasName_png = canvasName+".png";

  c1->SaveAs(canvasName_eps.c_str());
  c1->SaveAs(canvasName_png.c_str());

  delete c1;
  delete h2_axes;
  delete legend;

}
