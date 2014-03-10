#include "DrawBase.h"
#include <algorithm>
#include <fstream>



std::vector<TH1D*> getEcalDepositHistos( TTree* ecal_depth, const std::string& name, std::vector<float> *sf=0 );
TH1D* getSingleEcalHisto( TTree* ecal_depth, const std::string& name, std::vector<float> sf );
std::vector<float> cycleVector( std::vector<float> inputVector );



int main( int argc, char* argv[] ) {

 
  std::string SFname = "FN";
  if( argc>1 ) {
    std::string SFname_str(argv[1]);
    SFname = SFname_str;
  }


  
  ifstream sfFile(Form("data/lightYieldSF_%s.txt", SFname.c_str()));

  std::vector<float> lysf;
  while(sfFile.good()) {
    float sf;
    sfFile >> sf;
    lysf.push_back(sf);
  }

  lysf.pop_back(); // last element is repeated
  std::sort(lysf.begin(), lysf.end());


  // frascati configuration:
  //int nLayers = 10;
  //float actThickness = 10.;
  //float absThickness = 3.;
  //float trasvSize = 25.;

  TFile* file = TFile::Open("../batchOutput_frascati_v1/rootfiles/temp/frasctemp_n10_act10_abs3_trasv25.root");
  TTree* ecal_depth = (TTree*)file->Get("VectorEcal");

  std::vector<TH1D*> ecal_noSF = getEcalDepositHistos( ecal_depth, "noSF" );

  float ecalMean_noSF = ecal_noSF[0]->GetMean();
  float ecalRes_noSF =  ecal_noSF[0]->GetRMS()/ecalMean_noSF;

  std::vector<TH1D*> ecal_SF = getEcalDepositHistos( ecal_depth, "SF", &lysf );


  // and now plot
  DrawBase* db = new DrawBase("LYSF");
  

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  float xMin = -0.5;
  float xMax = 19.5;
  TH2D* axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., 0.15);
  axes->SetXTitle( "Configuration Number" );
  axes->SetYTitle( "ECAL Energy Resolution" );

  axes->Draw();

  TGraphErrors* gr_reso_vs_config = new TGraphErrors(0);

  float minReso=1.;
  int bestConfig = -1;
  for( unsigned i=0; i<ecal_SF.size(); ++i ) {
    float thisReso = ecal_SF[i]->GetRMS()/ecal_SF[i]->GetMean();
    gr_reso_vs_config->SetPoint( i, (float)i, thisReso );
    if( thisReso<minReso ) {
      minReso = thisReso;
      bestConfig = i;
    }
  }

  std::cout << "-> Best resolution (" << minReso << ") found for ordering N." << bestConfig << ":" << std::endl;
  for( unsigned i=0; i<bestConfig; ++i )
    lysf = cycleVector(lysf);
  for(unsigned j=0; j<lysf.size(); ++j )
    std::cout << lysf[j] << "\t";
  std::cout << std::endl;
  
  

  gr_reso_vs_config->SetMarkerStyle(20);
  gr_reso_vs_config->SetMarkerColor(kBlack);
  gr_reso_vs_config->SetMarkerSize(1.3);

  TLine* line_noSF = new TLine( xMin, ecalRes_noSF, xMax, ecalRes_noSF );
  line_noSF->SetLineWidth(2.);
  line_noSF->SetLineColor(46);
  line_noSF->Draw("same");

  gr_reso_vs_config->Draw("p same");


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

  TLegend* legend = new TLegend( 0.18, 0.2, 0.55, 0.4 );
  legend->SetTextSize(0.035);
  //legend->SetTextFont(22);
  legend->SetFillColor(0);
  legend->AddEntry( line_noSF, "Uniform Sampling", "L");
  legend->AddEntry( gr_reso_vs_config, "Non-uniform Sampling Configurations", "P" );
  legend->Draw("same");

  c1->SaveAs("resolutionVsFrascatiOrdering.eps");
  c1->SaveAs("resolutionVsFrascatiOrdering.png");

  return 0;

} 



std::vector<TH1D*> getEcalDepositHistos( TTree* ecal_depth, const std::string& name, std::vector<float> *sf ) {

   
  std::vector<TH1D*> returnVector;

  if( sf==0 ) {

    std::vector<float> unitarySF;
    for( unsigned i=0; i<10; ++i ) unitarySF.push_back(1.);
    TH1D* ecalHisto = getSingleEcalHisto( ecal_depth, name, unitarySF );
    returnVector.push_back(ecalHisto);

  } else {

    std::vector<float> tmp_sf;
    for( unsigned i=0; i<sf->size(); ++i ) tmp_sf.push_back(sf->at(i)); //copy sf


    for( unsigned i=0; i<tmp_sf.size(); ++i ) {
      std::cout << std::endl << "-> Starting configuration N." << i << ": " << std::endl;
      for(unsigned j=0; j<tmp_sf.size(); ++j )
        std::cout << tmp_sf[j] << " ";
      std::cout << std::endl;
      TH1D* ecalHisto = getSingleEcalHisto( ecal_depth, Form("%s_%d", name.c_str(), i), tmp_sf );
      returnVector.push_back(ecalHisto);
      tmp_sf = cycleVector(tmp_sf);
    }

    std::cout << std::endl << std::endl << "REVERSE" << std::endl;
    std::reverse(tmp_sf.begin(),tmp_sf.end()); 

    for( unsigned i=0; i<tmp_sf.size(); ++i ) {
      std::cout << std::endl << "-> Starting configuration N." << i+10 << ": " << std::endl;
      for(unsigned j=0; j<tmp_sf.size(); ++j )
        std::cout << tmp_sf[j] << " ";
      std::cout << std::endl;
      TH1D* ecalHisto = getSingleEcalHisto( ecal_depth, Form("%s_%d", name.c_str(), 10+i), tmp_sf );
      returnVector.push_back(ecalHisto);
      tmp_sf = cycleVector(tmp_sf);
    }

  }


  return returnVector;

}



TH1D* getSingleEcalHisto( TTree* ecal_depth, const std::string& name, std::vector<float> sf ) {

  if( sf.size()!=10 ) {
    std::cout << "Sorry hardwired to 10 layers for now. Exiting." << std::endl;
    exit(1191);
  }

  TH1D* returnHisto = new TH1D( name.c_str(), "", 1000., 0., 1000.);
  returnHisto->Sumw2();
  std::string cutString =  "e_vec_ecal[0]*%f+";
              cutString += "e_vec_ecal[1]*%f+";
              cutString += "e_vec_ecal[2]*%f+";
              cutString += "e_vec_ecal[3]*%f+";
              cutString += "e_vec_ecal[4]*%f+";
              cutString += "e_vec_ecal[5]*%f+";
              cutString += "e_vec_ecal[6]*%f+";
              cutString += "e_vec_ecal[7]*%f+";
              cutString += "e_vec_ecal[8]*%f+";
              cutString += "e_vec_ecal[9]*%f";

  ecal_depth->Project( name.c_str(), Form(cutString.c_str(), sf[0], sf[1], sf[2], sf[3], sf[4], sf[5], sf[6], sf[7], sf[8], sf[9]) );

  return returnHisto;

}


std::vector<float> cycleVector( std::vector<float> inputVector ) {

  std::vector<float> returnVector;

  for( unsigned i=0; i<inputVector.size(); ++i ) {
    if( i==0 )
      returnVector.push_back(inputVector[inputVector.size()-1]);
    else
      returnVector.push_back(inputVector[i-1]);
  }

  return returnVector;

}
