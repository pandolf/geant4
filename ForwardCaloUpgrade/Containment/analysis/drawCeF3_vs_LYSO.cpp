#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TH1D.h"
#include "TPaveText.h"
#include "TStyle.h"



class DataFile {

 public:

  DataFile( const std::string& fileName, const std::string& dataDir="./" );

  std::string actType;
  int nLayers;
  float absThickness;
  TFile* file;

 private:
};



void setStyle();
void drawCompare( const std::string& outputdir, std::vector<TGraphErrors*> graphs );
TGraphErrors* getMoliereGraph( std::vector<DataFile> dataFiles );
std::pair<TGraphErrors*, TGraphErrors*> getResponseGraphs( std::vector<DataFile> dataFiles );
TGraphErrors* getVolumeGraph( std::vector<DataFile> dataFiles );
TGraphErrors* getRatioGraph( TGraphErrors* graph );
std::string chopUnits( std::string title );
void drawSingleGraph( const std::string& outputdir, TGraphErrors* graph );
TPaveText* getLabelTop();



int main() {


  setStyle();

  std::string dataDir = "../data";

  std::vector<DataFile> dataFiles; 

  dataFiles.push_back(DataFile("EEShash_LYSO_nLayers28_tung25.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers34_tung15.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers28_tung20.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers24_tung25.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers21_tung30.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers19_tung35.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers17_tung40.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers16_tung45.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers14_tung50.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers13_tung55.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers12_tung65.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers12_tung60.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers11_tung70.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers10_tung80.root", dataDir) );
  dataFiles.push_back(DataFile("EEShash_CeF3_nLayers10_tung75.root", dataDir) );


  TGraphErrors* gr_mol = getMoliereGraph( dataFiles );
  TGraphErrors* gr_vol = getVolumeGraph( dataFiles );
  std::pair<TGraphErrors*, TGraphErrors*> gr_resp_reso = getResponseGraphs( dataFiles );

  std::vector< TGraphErrors* > graphs;
  graphs.push_back( gr_mol );
  graphs.push_back( gr_vol );
  graphs.push_back( gr_resp_reso.first );
  graphs.push_back( gr_resp_reso.second );

  std::string outputdir = "Plots_CeF3_vs_LYSO";
  system( Form("mkdir -p %s", outputdir.c_str()) );
  drawCompare( outputdir, graphs );

  return 0;

}




DataFile::DataFile( const std::string& fileName, const std::string& dataDir ) {

  std::stringstream ss(fileName);
  std::vector<std::string> parts;
  std::string item;
  while(std::getline(ss, item, '_')) {
    parts.push_back(item);
  }  

  // name should be EEShash_[actType]_nLayers[n]_tung[a*10].root
  actType = parts[1];

  sscanf( parts[2].c_str(), "nLayers%d", &nLayers );
  int absThickness_x10;
  sscanf( parts[3].c_str(), "tung%d", &absThickness_x10);
  absThickness = (float)absThickness_x10/10.;

  file = TFile::Open( Form("%s/%s", dataDir.c_str(), fileName.c_str() ) );

}



TGraphErrors* getMoliereGraph( std::vector<DataFile> dataFiles ) {

  TGraphErrors* graph = new TGraphErrors(0);

  for( unsigned i=0; i<dataFiles.size(); ++i ) {

    Double_t x = (i==0) ? -1 : dataFiles[i].absThickness;

    Double_t y = ((TH1D*)(dataFiles[i].file->Get("Rmoliere")))->GetMean();
    Double_t yerr = ((TH1D*)(dataFiles[i].file->Get("Rmoliere")))->GetMeanError();

    graph->SetPoint(i, x, y);
    graph->SetPointError(i, 0, yerr);

  }

  graph->SetName( "mol" );
  graph->SetTitle( "Moliere Radius [mm]" );

  return graph;

}



TGraphErrors* getVolumeGraph( std::vector<DataFile> dataFiles ) {

  TGraphErrors* graph = new TGraphErrors(0);

  for( unsigned i=0; i<dataFiles.size(); ++i ) {

    Double_t x = (i==0) ? -1 : dataFiles[i].absThickness;

    float actThickness = (dataFiles[i].actType=="LYSO") ? 1.5 : 5.;
    Double_t y = actThickness*dataFiles[i].nLayers;

    graph->SetPoint(i,x,y);

  }

  graph->SetName( "vol" );
  graph->SetTitle( "Active Volume [au]");

  return graph;

}



std::pair<TGraphErrors*, TGraphErrors*> getResponseGraphs( std::vector<DataFile> dataFiles ) {

  std::pair<TGraphErrors*, TGraphErrors*> gr_resp_reso;
  gr_resp_reso.first  = new TGraphErrors(0); // sampling fraction
  gr_resp_reso.second = new TGraphErrors(0); // resolution

  for( unsigned i=0; i<dataFiles.size(); ++i ) {

    Double_t x = (i==0) ? -1 : dataFiles[i].absThickness;

    float mean = ((TH1D*)(dataFiles[i].file->Get("edep_act")))->GetMean();
    float meanErr = ((TH1D*)(dataFiles[i].file->Get("edep_act")))->GetMeanError();

    float rms = ((TH1D*)(dataFiles[i].file->Get("edep_act")))->GetRMS();
    float rmsErr = ((TH1D*)(dataFiles[i].file->Get("edep_act")))->GetRMSError();

    gr_resp_reso.first->SetPoint(i,x,mean/20000.); // 20 GeV beam
    gr_resp_reso.first->SetPointError(i,0.,meanErr/20000.); // 20 GeV beam

    float reso = rms/mean;
    float nPhotoElectrons = mean/30.;
    float photoStat_reso = 1./sqrt(nPhotoElectrons);
    Double_t reso_tot = sqrt( reso*reso + photoStat_reso*photoStat_reso );

    Double_t reso_err = sqrt( rmsErr*rmsErr/(mean*mean) + rms*rms*meanErr*meanErr/(mean*mean*mean*mean) );

    gr_resp_reso.second->SetPoint(i,x,reso_tot);
    gr_resp_reso.second->SetPoint(i,x,reso_err);

  }

  gr_resp_reso.first->SetName( "sfrac" );
  gr_resp_reso.second->SetName( "eres" );

  gr_resp_reso.first->SetTitle( "Sampling Fraction" );
  gr_resp_reso.second->SetTitle( "Energy Resolution [%]" );

  return gr_resp_reso;

}



void drawCompare( const std::string& outputdir, std::vector<TGraphErrors*> graphs ) {

  for( unsigned i=0; i<graphs.size(); ++i ) 
    drawSingleGraph( outputdir, graphs[i] ); 

  //drawCompareAll( outputdir, graphs );

}



void drawSingleGraph( const std::string& outputdir, TGraphErrors* graph ) {

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  float yMax = 0.;
  for( unsigned i =1; i<graph->GetN(); ++i ) {
    Double_t x, y;
    graph->GetPoint( i, x, y); 
    if( y>yMax ) yMax = y;
  }
  yMax *= 1.2;

  TH2D* h2_axes = new TH2D("axes", "", 10, 0., 8., 10, 0., yMax );
  h2_axes->SetYTitle( graph->GetTitle() );
  h2_axes->SetXTitle( "Tungsten Thickness [mm]" );
  h2_axes->Draw();

  graph->SetMarkerStyle( 20 );
  graph->SetMarkerSize( 1.5 );
  graph->SetMarkerColor( 29 );
  graph->Draw("p same");

  TPaveText* labelTop = getLabelTop();
  labelTop->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form( "%s/%s.eps", outputdir.c_str(), graph->GetName() ) );
  c1->SaveAs( Form( "%s/%s.png", outputdir.c_str(), graph->GetName() ) );
  c1->SaveAs( Form( "%s/%s.pdf", outputdir.c_str(), graph->GetName() ) );


  c1->Clear();

  std::string title_chop = chopUnits( graph->GetTitle() );

  TH2D* h2_axes_ratio = new TH2D( "axes_ratio", "", 10, 0., 8., 10, 0., 2. );
  h2_axes_ratio->SetXTitle( "Tungsten Thickness [mm]" );
  h2_axes_ratio->SetYTitle( Form("%s Ratio", title_chop.c_str()) ); 
  h2_axes_ratio->Draw();

  TGraphErrors* gr_ratio = getRatioGraph( graph );

  gr_ratio->SetMarkerStyle( 20 );
  gr_ratio->SetMarkerSize( 1.5 );
  gr_ratio->SetMarkerColor( 29 );
  gr_ratio->Draw("p same");

  labelTop->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form( "%s/%s_ratio.eps", outputdir.c_str(), graph->GetName() ) );
  c1->SaveAs( Form( "%s/%s_ratio.png", outputdir.c_str(), graph->GetName() ) );
  c1->SaveAs( Form( "%s/%s_ratio.pdf", outputdir.c_str(), graph->GetName() ) );

  delete c1;
  delete h2_axes; 
  delete h2_axes_ratio; 

}



TGraphErrors* getRatioGraph( TGraphErrors* graph ) {
 
  TGraphErrors* gr_ratio = new TGraphErrors(0);

  Double_t y_ref, x_ref;
  graph->GetPoint(0, x_ref, y_ref );

  for( unsigned i =1; i<graph->GetN(); ++i ) {

    Double_t x, y;
    graph->GetPoint(i, x, y );

    gr_ratio->SetPoint( i-1, x, y/y_ref );

  }

  return gr_ratio;

}


std::string chopUnits( std::string title ) {

  std::stringstream ss(title);
  std::vector<std::string> parts;
  std::string item;
  while(std::getline(ss, item, '_')) {
    parts.push_back(item);
  }  

  if( parts.size()==2 ) return title;

  std::string choppedTitle = parts[0] + " " + parts[1]; // remove units
  return choppedTitle;

}



void setStyle() {

  // set the TStyle
  TStyle* style = new TStyle("DrawBaseStyle", "");
  style->SetCanvasColor(0);
  style->SetPadColor(0);
  style->SetFrameFillColor(0);
  style->SetStatColor(0);
  style->SetOptStat(0);
  style->SetTitleFillColor(0);
  style->SetCanvasBorderMode(0);
  style->SetPadBorderMode(0);
  style->SetFrameBorderMode(0);
  style->SetPadBottomMargin(0.12);
  style->SetPadLeftMargin(0.12);
  style->cd();
  // For the canvas:
  style->SetCanvasBorderMode(0);
  style->SetCanvasColor(kWhite);
  style->SetCanvasDefH(600); //Height of canvas
  style->SetCanvasDefW(600); //Width of canvas
  style->SetCanvasDefX(0); //POsition on screen
  style->SetCanvasDefY(0);
  // For the Pad:
  style->SetPadBorderMode(0);
  style->SetPadColor(kWhite);
  style->SetPadGridX(false);
  style->SetPadGridY(false);
  style->SetGridColor(0);
  style->SetGridStyle(3);
  style->SetGridWidth(1);
  // For the frame:
  style->SetFrameBorderMode(0);
  style->SetFrameBorderSize(1);
  style->SetFrameFillColor(0);
  style->SetFrameFillStyle(0);
  style->SetFrameLineColor(1);
  style->SetFrameLineStyle(1);
  style->SetFrameLineWidth(1);
  // Margins:
  style->SetPadTopMargin(0.05);
  style->SetPadBottomMargin(0.15);//0.13);
  style->SetPadLeftMargin(0.15);//0.16);
  style->SetPadRightMargin(0.05);//0.02);
  // For the Global title:
  style->SetOptTitle(0);
  style->SetTitleFont(42);
  style->SetTitleColor(1);
  style->SetTitleTextColor(1);
  style->SetTitleFillColor(10);
  style->SetTitleFontSize(0.05);
  // For the axis titles:
  style->SetTitleColor(1, "XYZ");
  style->SetTitleFont(42, "XYZ");
  style->SetTitleSize(0.05, "XYZ");
  style->SetTitleXOffset(1.15);//0.9);
  style->SetTitleYOffset(1.3); // => 1.15 if exponents
  // For the axis labels:
  style->SetLabelColor(1, "XYZ");
  style->SetLabelFont(42, "XYZ");
  style->SetLabelOffset(0.007, "XYZ");
  style->SetLabelSize(0.045, "XYZ");
  // For the axis:
  style->SetAxisColor(1, "XYZ");
  style->SetStripDecimals(kTRUE);
  style->SetTickLength(0.03, "XYZ");
  style->SetNdivisions(510, "XYZ");
  style->SetPadTickX(1); // To get tick marks on the opposite side of the frame
  style->SetPadTickY(1);
  // for histograms:
  style->SetHistLineColor(0);
  style->cd();

}


TPaveText* getLabelTop() {

  TPaveText* label_top = new TPaveText(0.4,0.953,0.975,0.975, "brNDC");
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right
  label_top->SetTextFont(62);
  label_top->AddText("20 GeV Electron Gun");
  return label_top;

}
