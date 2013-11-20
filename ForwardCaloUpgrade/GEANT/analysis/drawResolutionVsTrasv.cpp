#include <cstdlib>
#include <iostream>

#include "CommonTools/DrawBase.h"
#include "CommonTools/fitTools.h"

#include "TCanvas.h"


void drawStuffForOneVariable( DrawBase* db, const std::string& varName, const std::string&  axisName, const std::string& units, const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int minTrasv, int maxTrasv );
std::pair<TH1D*,TH1D*> get_histos_vs_trasv( const std::string& varName, const std::string& batchProd, int nLayers, float activeLayerThickness, float absorberLayerThickness, int minTrasv, int maxTrasv );
void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, const std::string& units, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3 );
float getMoliereRadius( float act, float abs );



int main() {


  std::string batchProd = "trasv_v3";

  DrawBase* db = new DrawBase("db");

  std::string outputdir = "plots_" + batchProd;
  db->set_outputdir(outputdir);
  std::string mkdir_command = "mkdir -p " + outputdir;
  system(mkdir_command.c_str());


  float activeLayerThickness = 10.;
  float absorberLayerThickness = 2.;
  int minTrasv = 1;
  int maxTrasv = 30;

  drawStuffForOneVariable( db, "sf",   "Sampling Fraction", "", batchProd, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );
  drawStuffForOneVariable( db, "ecal", "ECAL Energy", "MeV", batchProd, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );
  drawStuffForOneVariable( db, "e_dep","ECAL Energy", "MeV", batchProd, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );

  return 0;

}



void drawStuffForOneVariable( DrawBase* db, const std::string& varName, const std::string& axisName, const std::string& units, const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int minTrasv, int maxTrasv ) {

  std::pair<TH1D*,TH1D*> pair_act10_abs2 =  get_histos_vs_trasv( varName, batchProd, 15, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );
  std::pair<TH1D*,TH1D*> pair_act10_abs5 =  get_histos_vs_trasv( varName, batchProd, 10, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );
  std::pair<TH1D*,TH1D*> pair_act5_abs2  =  get_histos_vs_trasv( varName, batchProd, 25, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );

  drawComparison( db, varName+"res", axisName+" Resolution", "",    pair_act10_abs2.second, pair_act10_abs5.second, pair_act5_abs2.second );
  drawComparison( db, varName,       axisName,               units, pair_act10_abs2.first , pair_act10_abs5.first , pair_act5_abs2.first  );

}


std::pair<TH1D*,TH1D*> get_histos_vs_trasv( const std::string& varName, const std::string& batchProd, int nLayers, float activeLayerThickness, float absorberLayerThickness, int minTrasv, int maxTrasv ) {

  int nSteps = 1 + maxTrasv-minTrasv;

  char histoName_sf[200];
  sprintf( histoName_sf, "%s_vs_trasv_act%.0f_abs%.0f", varName.c_str(), activeLayerThickness, absorberLayerThickness );
  TH1D* h1_sf = new TH1D( histoName_sf, "", nSteps, (float)minTrasv-0.5, (float)maxTrasv+0.5 );

  char histoName_reso[200];
  sprintf( histoName_reso, "%sres_vs_trasv_act%.0f_abs%.0f", varName.c_str(), activeLayerThickness, absorberLayerThickness );
  TH1D* h1_reso = new TH1D( histoName_reso, "", nSteps, (float)minTrasv-0.5, (float)maxTrasv+0.5 );

  std::string histoName_get = "h_" + varName + "_config0_750MeV";

  for( unsigned iTrasv=minTrasv; iTrasv<=maxTrasv; ++iTrasv ) {

    int iBin = iTrasv-minTrasv+1;

    char fileName[200];
    sprintf( fileName, "../batchOutput_750MeV_%s/rootfiles/samplinghistos_n%d_act%.0f_abs%.0f_trasv%d.root", batchProd.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, iTrasv );
    TFile* file = TFile::Open( fileName );

    if( file==0 ) {
      std::cout << "WARNING! Didn't find file: " << fileName << "   ... skipping" << std::endl;
      h1_sf->SetBinContent( iBin, -1 );
      h1_reso->SetBinContent( iBin, -1 );
      continue;
    }

    TH1D* h1_sf_tmp = (TH1D*)file->Get(histoName_get.c_str());

    if( h1_sf_tmp==0 ) {
 
      TH1F::AddDirectory(kTRUE);

      if( iTrasv==minTrasv ) 
        std::cout << "-> Didn't find histogram: '" << histoName_get << ". Will go for the Cell tree." << std::endl;

      char fileName_temp[500];
      sprintf( fileName_temp, "../batchOutput_750MeV_%s/rootfiles/temp/temp_n%d_act%.0f_abs%.0f_trasv%d.root", batchProd.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, iTrasv );
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

    h1_reso->SetBinContent( iBin, resolution );
    h1_sf->SetBinContent( iBin, sf_mean );

    h1_sf->SetBinError( iBin, sf_mean_err );
    h1_reso->SetBinError( iBin, resolution_err );

    file->Close();

  }


  std::pair<TH1D*,TH1D*> returnPair;
  returnPair.first = h1_sf;
  returnPair.second = h1_reso;
  
  return returnPair;

}



void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, const std::string& units, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3 ) {


  TString yAxisName_tstr(yAxisName);
  bool isResolution = yAxisName_tstr.Contains("Resolution");

  const char* name1 = h1_1->GetName();
  const char* name2 = h1_2->GetName();
  const char* name3 = h1_3->GetName();

  std::string scanfText = varName + "_vs_trasv_act%d_abs%d";
  int act1, abs1;
  sscanf( name1, scanfText.c_str(), &act1, &abs1 );
  int act2, abs2;
  sscanf( name2, scanfText.c_str(), &act2, &abs2 );
  int act3, abs3;
  sscanf( name3, scanfText.c_str(), &act3, &abs3 );

  float markerSize = 1.6;

  int color1 = 46;
  h1_1->SetMarkerStyle(20);
  h1_1->SetMarkerColor(color1);
  h1_1->SetMarkerSize(markerSize);

  int color2 = kRed+3;
  h1_2->SetMarkerStyle(21);
  h1_2->SetMarkerColor(color2);
  h1_2->SetMarkerSize(markerSize);

  int color3 = kBlack;
  h1_3->SetMarkerStyle(24);
  h1_3->SetMarkerColor(color3);
  h1_3->SetMarkerSize(markerSize);



  float xMin = h1_1->GetXaxis()->GetXmin();
  float xMax = h1_1->GetXaxis()->GetXmax();
  float yMax = h1_1->GetMaximum();
  if( h1_2->GetMaximum() >yMax ) yMax = h1_2 ->GetMaximum();
  if( h1_3->GetMaximum()>yMax )  yMax = h1_3->GetMaximum();
  if( isResolution )
    yMax *= 1.1;
  else
    yMax *= 1.8;
  

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., yMax );
  h2_axes->SetXTitle("Transverse Cell Size [mm]");
  std::string totalAxisName = yAxisName;
  if( units!="" ) totalAxisName = totalAxisName + " [" + units + "]";
  h2_axes->SetYTitle(totalAxisName.c_str());

  h2_axes->Draw();

  float moliereRadius1 = getMoliereRadius( act1, abs1 );
  float moliereRadius2 = getMoliereRadius( act2, abs2 );
  float moliereRadius3 = getMoliereRadius( act3, abs3 );

  TLine* line_moliere1 = new TLine( moliereRadius1, 0., moliereRadius1, yMax );
  line_moliere1->SetLineColor(kBlack);
  line_moliere1->Draw("same");
  TPaveText* text_moliere1 = new TPaveText( moliereRadius1, 0., moliereRadius1, yMax );
  text_moliere1->SetFillColor(0);
  text_moliere1->SetTextSize(0.025);
  text_moliere1->SetTextAngle(90.);
  char moliereText1[200];
  sprintf( moliereText1, "R_{M}(%d/%dmm) = %.1f mm", act1, abs1, moliereRadius1 );
  text_moliere1->AddText(moliereText1);
  //text_moliere1->Draw("Same");
  TH1D* h1_moliere1 = new TH1D("moliere1", "", 10, 0., 1.);
  h1_moliere1->SetLineColor(color1);

  TLegend* legend = new TLegend( 0.28, 0.7, 0.6, 0.9, "CeF_{3} / Lead" );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);

  char legendText1[200];

  sprintf( legendText1, "%d mm / %d mm", act1, abs1 );

  legend->AddEntry( h1_1, legendText1, "P" );
  legend->AddEntry( line_moliere1, "Moliere Radius", "L" );
  legend->Draw("same");


  TPaveText* label_top = new TPaveText(0.4,0.953,0.96,0.975, "brNDC");
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right
  label_top->SetTextFont(62);
  label_top->AddText("Electron gun, E = 750 MeV");
  label_top->Draw("same");

  


  h1_1->Draw("P same");

  gPad->RedrawAxis();

  std::string canvasName = db->get_outputdir() + "/" + varName +"_vs_trasv";
  std::string canvasName_eps = canvasName+".eps";
  std::string canvasName_png = canvasName+".png";

  c1->SaveAs(canvasName_eps.c_str());
  c1->SaveAs(canvasName_png.c_str());

  delete c1;
  delete h2_axes;
  delete legend;

}


float getMoliereRadius( float act, float abs ) {

  float moliereRadius = 0.;
  if( act==10. && abs==2. )
    moliereRadius = 28.1;
  else if( act==10. && abs==5. )
    moliereRadius = 25.4;
  else if( act== 5. && abs==2. )
    moliereRadius = 22.6;
  else if( act== 5. && abs==5. )
    moliereRadius = 22.3;

  return moliereRadius;

}
