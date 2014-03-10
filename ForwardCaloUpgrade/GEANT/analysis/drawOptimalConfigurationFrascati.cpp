#include "DrawBase.h"
#include <algorithm>
#include <fstream>



std::vector<TH1D*> getEcalDepositHistos( TTree* ecal_depth, const std::string& name, std::vector<float> *sf=0 );
TH1D* getSingleEcalHisto( TTree* ecal_depth, const std::string& name, std::vector<float> sf );
std::vector<float> cycleVector( std::vector<float> inputVector );



int main( int argc, char* argv[] ) {

 
  std::string SFname = "dummy";
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

  TFile* prova = TFile::Open("prova.root", "recreate");
  prova->cd();
  ecal_noSF[0]->Write();
  prova->Close();

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
      TH1D* ecalHisto = getSingleEcalHisto( ecal_depth, Form("%s_%d", name.c_str(), i), tmp_sf );
      returnVector.push_back(ecalHisto);
      tmp_sf = cycleVector(tmp_sf);
    }
    std::reverse(tmp_sf.begin(),tmp_sf.end()); 

    for( unsigned i=0; i<tmp_sf.size(); ++i ) {
      TH1D* ecalHisto = getSingleEcalHisto( ecal_depth, Form("%s_%d", name.c_str(), 10+i), tmp_sf );
      returnVector.push_back(ecalHisto);
      tmp_sf = cycleVector(tmp_sf);
    }
//  for(unsigned i=0; i<lysf.size(); ++i )
//    std::cout << i << " " << lysf[i] << std::endl;

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
