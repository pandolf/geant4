#include <iostream>
#include <cstdlib>
#include <cmath>

#include "CommonTools/DrawBase.h"


// all lengths in mm




float getCompositeSum( int act_n, float act_l );
float getEffectiveX0( int act_n, float act_thick, float act_X0 );





int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./drawPerformanceVsCost [batchProd]" << std::endl;
    exit(11);
  }

  
  std::string batchProd;
  if( argc>1 ) {
    std::string batchProd_str(argv[1]);
    batchProd = batchProd_str;
  }


  DrawBase* db = new DrawBase("costs");
  db->set_outputdir(Form("CostPlots_%s", batchProd.c_str()));


  float X0_tung = 3.5;
  float X0_lyso = 11.4;
  float X0_cef3 = 16.8;



  // first, compute stuff for the LYSO calorimeter
  float lysocal_act_thick = 1.5;
  float lysocal_abs_thick = 2.5;
  int   lysocal_act_n     = 29;
  int   lysocal_abs_n     = 28;

  float lysocal_length_act = getCompositeSum( lysocal_act_n, lysocal_act_thick );
  float lysocal_length_abs = getCompositeSum( lysocal_abs_n, lysocal_abs_thick );
  float lysocal_length_tot = lysocal_length_act + lysocal_length_abs;

  float lysocal_X0_act = getEffectiveX0( lysocal_act_n, lysocal_act_thick, X0_lyso );
  float lysocal_X0_abs = getEffectiveX0( lysocal_abs_n, lysocal_abs_thick, X0_tung );
  float lysocal_X0_tot = lysocal_X0_act + lysocal_X0_abs;

  std::cout << "*** LYSO CAL: " << std::endl;
  std::cout << " Active material: " << lysocal_act_n << " layers of LYSO (X0 = " << X0_lyso << " mm)" << std::endl;
  std::cout << " Absorber       : " << lysocal_abs_n << " layers of W    (X0 = " << X0_tung << " mm)" << std::endl;
  std::cout << " Total length   : " << lysocal_length_tot << " mm" << std::endl;
  std::cout << " Total X0       : " << lysocal_X0_tot     << std::endl;

  float lysocal_Sres = 0.1;
  TF1* f1_lysocal_Sres = new TF1("f1_lysocal_Sres", "[0]/sqrt(x)", 0.1, 1000.); // taken from slide 20 of https://indico.cern.ch/event/292529/session/0/contribution/1/material/slides/
  f1_lysocal_Sres->SetParameter(0,lysocal_Sres);
  std::cout << "Sampling Resolution: " << std::endl;
  std::cout << "  750 MeV: " << f1_lysocal_Sres->Eval(0.75) << std::endl;
  std::cout << "  1 GeV: "   << f1_lysocal_Sres->Eval(1.)   << std::endl;
  std::cout << "  10 GeV: "  << f1_lysocal_Sres->Eval(10.)  << std::endl;
  std::cout << "  100 GeV: " << f1_lysocal_Sres->Eval(100.) << std::endl;

  //float lysocal_cost   = getCostPerSquareMeter( lysocal_act_n, X0_lyso, lysocal_abs_n, X0_tung );



  // now go ahead and study performance of CeF3/W:

  float X0_target = 24.; // want to have at least 24 X0 length

  float detectorLength = 0.;

  float tung_thickness = 3.; // fixed
  float cef3_thickness = 1.; // will vary


  while ( detectorLength<220. ) {

    cef3_thickness = cef3_thickness + 1.; // start from 2mm
  
    float X0_tung_singleLayer = tung_thickness/X0_tung;
    float X0_cef3_singleLayer = cef3_thickness/X0_cef3;
  
    int nLayers = ceil( (( X0_target - X0_cef3_singleLayer ) / ( X0_cef3_singleLayer + X0_tung_singleLayer )) );
    float X0_eff = (nLayers+1)*X0_cef3_singleLayer + nLayers*X0_tung_singleLayer;
  
    detectorLength = ((nLayers+1)*cef3_thickness + nLayers*tung_thickness);
    if( detectorLength>220. ) break;

    TFile* file_1GeV   = TFile::Open(Form(  "../batchOutput_%s/1GeV/rootfiles/samplinghistos_e1_n%d_act%.0f_abs3_trasv20.root", batchProd.c_str(), nLayers, cef3_thickness) );
    TFile* file_10GeV  = TFile::Open(Form( "../batchOutput_%s/10GeV/rootfiles/samplinghistos_e10_n%d_act%.0f_abs3_trasv20.root", batchProd.c_str(), nLayers, cef3_thickness) );
    TFile* file_100GeV = TFile::Open(Form("../batchOutput_%s/100GeV/rootfiles/samplinghistos_e100_n%d_act%.0f_abs3_trasv20.root", batchProd.c_str(), nLayers, cef3_thickness) );

    TH1D* h1_ecal_1GeV   = (file_1GeV!=0)   ? (TH1D*)file_1GeV  ->Get("h_ecal_config0_1000MeV")  : 0;
    TH1D* h1_ecal_10GeV  = (file_10GeV!=0)  ? (TH1D*)file_10GeV ->Get("h_ecal_config0_10000MeV") : 0;
    TH1D* h1_ecal_100GeV = (file_100GeV!=0) ? (TH1D*)file_100GeV->Get("h_ecal_config0_100000MeV"): 0;

    if( h1_ecal_1GeV==0 && h1_ecal_10GeV==0 && h1_ecal_100GeV==0 ) {
      std::cout << "WARNING! Didn't find anything for CeF3 = " << cef3_thickness << " mm. Skipping!" << std::endl;
      continue;
    }
      

    float sfrms_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetRMS() : 0.;
    float sfrms_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetRMS() : 0.;
    float sfrms_100GeV = (h1_ecal_100GeV!=0) ? h1_ecal_100GeV->GetRMS() : 0.;

    float sf_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetMean() : 0.;
    float sf_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetMean() : 0.;
    float sf_100GeV = (h1_ecal_100GeV!=0) ? h1_ecal_100GeV->GetMean() : 0.;

    float sfrmserr_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetRMSError() : 0.;
    float sfrmserr_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetRMSError() : 0.;
    float sfrmserr_100GeV = (h1_ecal_100GeV!=0) ? h1_ecal_100GeV->GetRMSError() : 0.;

    float sferr_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetMeanError() : 0.;
    float sferr_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetMeanError() : 0.;
    float sferr_100GeV = (h1_ecal_100GeV!=0) ? h1_ecal_100GeV->GetMeanError() : 0.;

    float Sres_1GeV   = (sf_1GeV>0.)   ? sfrms_1GeV  /sf_1GeV   : 0.;
    float Sres_10GeV  = (sf_10GeV>0.)  ? sfrms_10GeV /sf_10GeV  : 0.;
    float Sres_100GeV = (sf_100GeV>0.) ? sfrms_100GeV/sf_100GeV : 0.;

    float Sreserr_1GeV   = sqrt( sfrmserr_1GeV  *sfrmserr_1GeV   / (sf_1GeV*  sf_1GeV)   + sferr_1GeV  *sferr_1GeV   *sfrms_1GeV  *sfrms_1GeV   / (sf_1GeV*  sf_1GeV*  sf_1GeV*  sf_1GeV)   );
    float Sreserr_10GeV  = sqrt( sfrmserr_10GeV *sfrmserr_10GeV  / (sf_10GeV* sf_10GeV)  + sferr_10GeV *sferr_10GeV  *sfrms_10GeV *sfrms_10GeV  / (sf_10GeV* sf_10GeV* sf_10GeV* sf_10GeV)  );
    float Sreserr_100GeV = sqrt( sfrmserr_100GeV*sfrmserr_100GeV / (sf_100GeV*sf_100GeV) + sferr_100GeV*sferr_100GeV *sfrms_100GeV*sfrms_100GeV / (sf_100GeV*sf_100GeV*sf_100GeV*sf_100GeV) );


    TGraphErrors* gr_Sres = new TGraphErrors(0);
    if( Sres_1GeV>0. ) {
      gr_Sres->SetPoint( 0, 1.,   Sres_1GeV   );
      gr_Sres->SetPointError( 0, 0., Sreserr_1GeV   );
    }
    if( Sres_10GeV>0.) {
      gr_Sres->SetPoint( 1, 10.,  Sres_10GeV  );
      gr_Sres->SetPointError( 1, 0., Sreserr_10GeV  );
    }
    if( Sres_100GeV>0. ) {
      gr_Sres->SetPoint( 2, 100., Sres_100GeV );
      gr_Sres->SetPointError( 2, 0., Sreserr_100GeV );
    }


    TCanvas* c1 = new TCanvas("c1", "", 600, 600);
    c1->cd();
    c1->SetLogx();
    c1->SetLogy();

    TH2D* h2_axes = new TH2D("axes", "", 10, 0.5, 200., 10, 0.001, 0.7);
    h2_axes->SetXTitle("Electron Energy [GeV]");
    h2_axes->SetYTitle("Sampling Resolution");

    h2_axes->Draw();


    gr_Sres->SetMarkerStyle(20);
    gr_Sres->SetMarkerSize(1.3);
    gr_Sres->SetMarkerColor(46);
  
    TF1* f1_Sres = new TF1("Sres", "[0]/sqrt(x)", 0.1, 1000.);
    f1_Sres->SetLineColor(29);
    f1_Sres->SetLineWidth(2);

    gr_Sres->Draw("p same");
    if( gr_Sres->GetN()>1. ) {
      gr_Sres->Fit( f1_Sres );
      f1_Sres->Draw("L same");
    }

    c1->SaveAs(Form("%s/fit_n%d_act%.0f_abs3_trasv20.eps", db->get_outputdir().c_str(), nLayers, cef3_thickness) );
    c1->SaveAs(Form("%s/fit_n%d_act%.0f_abs3_trasv20.png", db->get_outputdir().c_str(), nLayers, cef3_thickness) );

    delete gr_Sres;
    delete f1_Sres;
    delete c1;
    delete h2_axes;
 
  }





  return 0;

}




float getCompositeSum( int act_n, float act_l ) {

  float sum = (float)act_n*act_l;

  return sum;

}


float getEffectiveX0( int act_n, float act_thick, float act_X0 ) {

  float X0 = (float)act_n*act_thick/act_X0;

  return X0;

}


//float lysocal_cost   = getCostPerSquareMeter( lysocal_act_n, X0_lyso, lysocal_abs_n, X0_tung );
