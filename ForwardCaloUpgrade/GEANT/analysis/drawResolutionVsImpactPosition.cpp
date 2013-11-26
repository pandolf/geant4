#include <cstdlib>
#include <iostream>

#include "CommonTools/DrawBase.h"
#include "CommonTools/fitTools.h"

#include "TCanvas.h"


bool isBrass=false;
bool isTung=false;


void drawStuffForOneVariable( DrawBase* db, const std::string& varName, const std::string& axisName, const std::string& units, const std::string& batchProd, int maxImpactPosition );
std::pair<TH1D*,TH1D*> get_histos_vs_impactPosition( const std::string& varName, const std::string& batchProd, int nLayers, float activeLayerThickness, float absorberLayerThickness, float trasv, int maxImpactPosition, bool nineCells=true );
void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, const std::string& units, float trasv, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3, TH1D* h1_4 );



int main( int argc, char* argv[] ) {


  std::string batchProd = "impact_v2";
  if( argc > 1 ) {
    std::string batchProd_str(argv[1]);
    batchProd = batchProd_str;
  }

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


  int maxImpactPosition = 30;

  drawStuffForOneVariable( db, "sf",         "Sampling Fraction", "", batchProd, maxImpactPosition );
  drawStuffForOneVariable( db, "ecal",       "ECAL Energy", "MeV", batchProd, maxImpactPosition );
  drawStuffForOneVariable( db, "e_dep",      "ECAL Energy", "MeV", batchProd, maxImpactPosition );

  return 0;

}


void drawStuffForOneVariable( DrawBase* db, const std::string& varName, const std::string& axisName, const std::string& units, const std::string& batchProd, int maxImpactPosition ) {

  int n1=20;
  int n2=10;

  float act1=10.;
  float act2=10.;

  float abs1=2.;
  float abs2=5.;

  float trasv = 20.;

  if( isTung ) {

    n1=15;
    n2=15;

    act1=5.;
    act2=10.;

    abs1=3.;
    abs2=3.;

    trasv = 20.;
    //trasv = 25.;

  }

  std::pair<TH1D*,TH1D*> pair_1 =  get_histos_vs_impactPosition( varName, batchProd, n1, act1, abs1, trasv, maxImpactPosition );
  std::pair<TH1D*,TH1D*> pair_2 =  get_histos_vs_impactPosition( varName, batchProd, n2, act2, abs2, trasv, maxImpactPosition );

  std::pair<TH1D*,TH1D*> pair_3 =  get_histos_vs_impactPosition( varName, batchProd, n1, act1, abs1, trasv, maxImpactPosition, false );
  std::pair<TH1D*,TH1D*> pair_4 =  get_histos_vs_impactPosition( varName, batchProd, n2, act2, abs2, trasv, maxImpactPosition, false );

  drawComparison( db, varName+"res", axisName + " Resolution",    "", trasv, pair_1.second, pair_2.second, pair_3.second, pair_4.second );
  drawComparison( db, varName,       axisName,                 units, trasv, pair_1.first,  pair_2.first,  pair_3.first , pair_4.first );

}


std::pair<TH1D*,TH1D*> get_histos_vs_impactPosition( const std::string& varName, const std::string& batchProd, int nLayers, float activeLayerThickness, float absorberLayerThickness, float trasv, int maxImpactPosition, bool nineCells ) {


  int nCells = (nineCells) ? 9 : 1;

  char histoName_sf[200];
  sprintf( histoName_sf, "%s_vs_impact_n%d_act%.0f_abs%.0f_ncells%d", varName.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, nCells );
  TH1D* h1_sf = new TH1D( histoName_sf, "", maxImpactPosition, 0.5, (float)maxImpactPosition+0.5 );

  char histoName_reso[200];
  sprintf( histoName_reso, "%sres_vs_impact_n%d_act%.0f_abs%.0f_ncells%d", varName.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, nCells );
  TH1D* h1_reso = new TH1D( histoName_reso, "", maxImpactPosition, 0.5, (float)maxImpactPosition+0.5 );

  std::string histoName_get = "h_" + varName + "_config0_750MeV";



  for( unsigned iImpact=1; iImpact<maxImpactPosition; ++iImpact ) {

    char fileName[500];
    if( iImpact>0 )
      sprintf( fileName, "../batchOutput_750MeV_%s/rootfiles/samplinghistos_n%d_act%.0f_abs%.0f_trasv%.0f_impact%d.root", batchProd.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, trasv, iImpact );
    else
      sprintf( fileName, "../batchOutput_750MeV_%s/rootfiles/samplinghistos_n%d_act%.0f_abs%.0f_trasv%.0f.root", batchProd.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, trasv );
    TFile* file = TFile::Open( fileName );

    if( file==0 ) {
      std::cout << "WARNING! Didn't find file: " << fileName << "   ... skipping" << std::endl;
      h1_sf->SetBinContent( iImpact, -1 );
      h1_reso->SetBinContent( iImpact, -1 );
      continue;
    }

    TH1D* h1_sf_tmp = (TH1D*)file->Get(histoName_get.c_str());

    if( h1_sf_tmp==0 ) {
 
      TH1F::AddDirectory(kTRUE);

      if( iImpact==1 ) 
        std::cout << "-> Didn't find histogram: '" << histoName_get << ". Will go for the Cell tree." << std::endl;

      char fileName_temp[500];
      if( iImpact>0 )
        sprintf( fileName_temp, "../batchOutput_750MeV_%s/rootfiles/temp/temp_n%d_act%.0f_abs%.0f_trasv%.0f_impact%d.root", batchProd.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, trasv, iImpact );
      else
        sprintf( fileName_temp, "../batchOutput_750MeV_%s/rootfiles/temp/temp_n%d_act%.0f_abs%.0f_trasv%.0f.root", batchProd.c_str(), nLayers, activeLayerThickness, absorberLayerThickness, trasv );
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


    h1_reso->SetBinContent( iImpact, resolution );
    h1_sf->SetBinContent( iImpact, sf_mean );

    h1_sf->SetBinError( iImpact, sf_mean_err );
    h1_reso->SetBinError( iImpact, resolution_err );

    file->Close();

  }


  std::pair<TH1D*,TH1D*> returnPair;
  returnPair.first = h1_sf;
  returnPair.second = h1_reso;
  
  return returnPair;

}



void drawComparison( DrawBase* db, const std::string& varName, const std::string& yAxisName, const std::string& units, float trasv, TH1D* h1_1, TH1D* h1_2, TH1D* h1_3, TH1D* h1_4 ) {

  TString yAxisName_tstr(yAxisName);
  bool isResolution = yAxisName_tstr.Contains("Resolution");


  const char* name1 = h1_1->GetName();
  const char* name2 = h1_2->GetName();
  const char* name3 = h1_3->GetName();
  const char* name4 = h1_4->GetName();

  int act1, abs1, n1, nCells1;
  int act2, abs2, n2, nCells2;
  int act3, abs3, n3, nCells3;
  int act4, abs4, n4, nCells4;

  std::string scanfText = varName + "_vs_impact_n%d_act%d_abs%d_ncells%d";
  sscanf( name1, scanfText.c_str(), &n1, &act1, &abs1, &nCells1 );
  sscanf( name2, scanfText.c_str(), &n2, &act2, &abs2, &nCells2 );
  sscanf( name3, scanfText.c_str(), &n3, &act3, &abs3, &nCells3 );
  sscanf( name4, scanfText.c_str(), &n4, &act4, &abs4, &nCells4 );

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
  h1_4->SetMarkerColor(40);
  h1_4->SetMarkerSize(markerSize);



  float xMax = h1_1->GetXaxis()->GetXmax();
  float yMax = h1_1->GetMaximum();
  if( h1_2->GetMaximum() >yMax ) yMax = h1_2 ->GetMaximum();
  if( h1_3->GetMaximum()>yMax )  yMax = h1_3->GetMaximum();
  if( h1_4->GetMaximum()>yMax )  yMax = h1_4->GetMaximum();
  float scaleFactor = 1.8;
  yMax *= scaleFactor;
  
  if(varName=="ecal"||varName=="e_dep") yMax = 700.;
  //if(varName=="ecalres"||varName=="e_depres") yMax = 1.2;

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D("axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle("Beam Offset [mm]");
  std::string totalAxisName = yAxisName;
  if( units!="" ) totalAxisName = totalAxisName + " [" + units + "]";
  h2_axes->SetYTitle(totalAxisName.c_str());
  
  h2_axes->Draw();

  std::string absMaterial = "Lead";
  if( isTung ) absMaterial = "Tungsten";
  if( isBrass ) absMaterial = "Brass";

  char legendTitle[300];
  sprintf(legendTitle, "CeF_{3} / %s (%.0fx%.0f Cells)", absMaterial.c_str(), trasv, trasv );

  TLegend* legend = new TLegend( 0.2, 0.66, 0.7, 0.9, legendTitle );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);

  char legendText1[200];
  char legendText2[200];
  char legendText3[200];
  char legendText4[200];

  sprintf( legendText1, "%d mm / %d mm (%d layers, %dx%d)", act1, abs1, n1, nCells1, nCells1 );
  sprintf( legendText2, "%d mm / %d mm (%d layers, %dx%d)", act2, abs2, n2, nCells2, nCells2 );
  sprintf( legendText3, "%d mm / %d mm (%d layers, %dx%d)", act3, abs3, n3, nCells3, nCells3 );
  sprintf( legendText4, "%d mm / %d mm (%d layers, %dx%d)", act4, abs4, n4, nCells4, nCells4 );

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


  h1_2->Draw("P same");
  h1_1->Draw("P same");
  h1_3->Draw("P same");
  h1_4->Draw("P same");

  gPad->RedrawAxis();

  std::string canvasName = db->get_outputdir() + "/" + varName +"_vs_impact";
  std::string canvasName_eps = canvasName+".eps";
  std::string canvasName_png = canvasName+".png";

  c1->SaveAs(canvasName_eps.c_str());
  c1->SaveAs(canvasName_png.c_str());

  delete c1;
  delete h2_axes;
  delete legend;

}
