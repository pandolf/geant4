#include <cstdlib>
#include <iostream>

#include "CommonTools/DrawBase.h"
#include "CommonTools/fitTools.h"

#include "TCanvas.h"
#include "TString.h"


bool isBrass=false;
bool isTung=false;


void drawStuffForOneVariable( DrawBase* db, const std::string& varName, const std::string&  axisName, const std::string& units, const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int minTrasv, int maxTrasv );
std::pair<TH1D*,TH1D*> get_histos_vs_trasv( const std::string& varName, const std::string& batchProd, int nLayers, float activeLayerThickness, float absorberLayerThickness, int minTrasv, int maxTrasv, bool nineCells=true );
void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, const std::string& units, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3 );
float getMoliereRadius( float act, float abs );



int main( int argc, char* argv[] ) {


  std::string batchProd = "trasv_v3";
  if( argc > 1 ) {
    std::string batchProd_str(argv[1]);
    batchProd = batchProd_str;
  }


  //if( argc > 2 ) {
  //  std::string isBrass_str(argv[2]);
  //  if( isBrass_str=="true" ) 
  //    isBrass = true;
  //}

  TString batchProd_tstr(batchProd);
  isTung = batchProd_tstr.Contains("tung");
  isBrass = batchProd_tstr.Contains("brass");

  if( isBrass )
    std::cout << "-> Using brass as absorber." << std::endl;
  if( isTung )
    std::cout << "-> Using tungsten as absorber." << std::endl;


  DrawBase* db = new DrawBase("db");

  std::string outputdir = "plots_" + batchProd;
  db->set_outputdir(outputdir);
  std::string mkdir_command = "mkdir -p " + outputdir;
  system(mkdir_command.c_str());


  float activeLayerThickness = 5.;
  //float activeLayerThickness = 10.;
  float absorberLayerThickness = 2.;
  if( isTung ) absorberLayerThickness=3.;
  if( isBrass ) absorberLayerThickness=5.;
  int minTrasv = 1;
  int maxTrasv = (isBrass) ? 40 : 30;

  drawStuffForOneVariable( db, "sf",   "Sampling Fraction", "", batchProd, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );
  drawStuffForOneVariable( db, "ecal", "ECAL Energy", "MeV", batchProd, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );
  drawStuffForOneVariable( db, "e_dep","ECAL Energy", "MeV", batchProd, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );

  return 0;

}



void drawStuffForOneVariable( DrawBase* db, const std::string& varName, const std::string& axisName, const std::string& units, const std::string& batchProd, float activeLayerThickness, float absorberLayerThickness, int minTrasv, int maxTrasv ) {


  int n1 = 15;
  int n2 = 10;
  int n3 = 25;

  if( isTung ) {

    n1 = 15;
    n2 = 15;
    n3 = 15;

  }

  std::pair<TH1D*,TH1D*> pair_1 = get_histos_vs_trasv( varName, batchProd, n1, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );
  std::pair<TH1D*,TH1D*> pair_2 = get_histos_vs_trasv( varName, batchProd, n2, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv, false );
  std::pair<TH1D*,TH1D*> pair_3 = get_histos_vs_trasv( varName, batchProd, n3, activeLayerThickness, absorberLayerThickness, minTrasv, maxTrasv );

  drawComparison( db, varName+"res", axisName+" Resolution", "",    pair_1.second, pair_2.second, pair_3.second );
  drawComparison( db, varName,       axisName,               units, pair_1.first , pair_2.first , pair_3.first  );

}


std::pair<TH1D*,TH1D*> get_histos_vs_trasv( const std::string& varName, const std::string& batchProd, int nLayers, float activeLayerThickness, float absorberLayerThickness, int minTrasv, int maxTrasv, bool nineCells ) {


  int nCells = (nineCells) ? 9 : 1;

  int nSteps = 1 + maxTrasv-minTrasv;

  char histoName_sf[200];
  sprintf( histoName_sf, "%s_vs_trasv_n%d_act%.0f_abs%.0f_ncells%d", varName.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, nCells );
  TH1D* h1_sf = new TH1D( histoName_sf, "", nSteps, (float)minTrasv-0.5, (float)maxTrasv+0.5 );

  char histoName_reso[200];
  sprintf( histoName_reso, "%sres_vs_trasv_n%d_act%.0f_abs%.0f_ncells%d", varName.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, nCells );
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

      std::string treeVarName = varName;
      if( nineCells ) {
        treeVarName += "[0]";
        for( unsigned icell=1; icell<n_cells; ++icell ) { //add for all cells
          char anotherCellName[100];
          sprintf( anotherCellName, "+%s[%d]", varName.c_str(), icell );
          std::string anotherCellName_str(anotherCellName);
          treeVarName += anotherCellName_str;
        }
      } else {
        treeVarName += "[4]"; // only central cell
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

  std::string scanfText = varName + "_vs_trasv_n%d_act%d_abs%d_ncells%d";
  int act1, abs1, n1, nCells1;
  int act2, abs2, n2, nCells2;
  int act3, abs3, n3, nCells3;
  
  sscanf( name1, scanfText.c_str(), &n1, &act1, &abs1, &nCells1 );
  sscanf( name2, scanfText.c_str(), &n2, &act2, &abs2, &nCells2 );
  sscanf( name3, scanfText.c_str(), &n3, &act3, &abs3, &nCells3 );

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

  std::string legendTitle = "CeF_{3} / ";
  if( isBrass ) legendTitle += "Brass";
  else if( isTung ) legendTitle += "Tungsten";
  else  legendTitle += "Lead";
  TLegend* legend = new TLegend( 0.28, 0.63, 0.6, 0.9, legendTitle.c_str() );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);

  char legendText1[200];
  char legendText2[200];

  sprintf( legendText1, "%d mm / %d mm (%d layers, %.0fx%.0f)", act1, abs1, n1, sqrt(nCells1), sqrt(nCells1) );
  sprintf( legendText2, "%d mm / %d mm (%d layers, %.0fx%.0f)", act2, abs2, n2, sqrt(nCells2), sqrt(nCells2) );

  legend->AddEntry( h1_1, legendText1, "P" );
  legend->AddEntry( h1_2, legendText2, "P" );
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
  h1_2->Draw("P same");

  gPad->RedrawAxis();

  std::string canvasName = db->get_outputdir() + "/" + varName +"_vs_trasv";
  std::string canvasName_eps = canvasName+".eps";
  std::string canvasName_png = canvasName+".png";

  c1->SaveAs(canvasName_eps.c_str());
  c1->SaveAs(canvasName_png.c_str());

  delete c1;
  delete h2_axes;
  delete legend;
  delete h1_moliere1;

}


float getMoliereRadius( float act, float abs ) {

  float moliereRadius = 0.;
  if( isBrass ) {
    if( act==10. && abs==5. )
      moliereRadius = 31.1;
    else if( act==10. && abs==12. )
      moliereRadius = 29.8;
    else if( act== 5. && abs==5. )
      moliereRadius = 29.8;
    else if( act== 5. && abs==12. )
      moliereRadius = 29.2;
  } else if( isTung ) {
    if( act==10. && abs==3. )
      moliereRadius = 23.1;
    else if( act==5. && abs==3. )
      moliereRadius = 19.0;
  } else {
    if( act==10. && abs==2. )
      moliereRadius = 28.1;
    else if( act==10. && abs==5. )
      moliereRadius = 25.4;
    else if( act== 5. && abs==2. )
      moliereRadius = 22.6;
    else if( act== 5. && abs==5. )
      moliereRadius = 22.3;
  }

  return moliereRadius;

}
