#include <iostream>
#include <cstdlib>
#include <cmath>

#include "TMath.h"
#include "TGaxis.h"

#include "CommonTools/DrawBase.h"


// all lengths in mm


struct CaloParameters {

  float sf;
  float res;
  float mol;

  float sferr;
  float reserr;
  float molerr;

};



float getCompositeSum( int act_n, float act_l );
float getEffectiveX0( int act_n, float act_thick, float act_X0 );
float getCostPerSquareMeter( int n, float length, float cost );
std::string getStringWithDecimal( float act );

CaloParameters drawSingleConfiguration( DrawBase* db, const std::string& batchProd, const std::string& actType, int nLayers, float act, float abso );
TF1* drawAndFit( DrawBase* db, TGraphErrors* gr_S, const std::string& name, const std::string& varName, const std::string& formula, int nLayers, const std::string& actType, float act, float abso );

void drawDoublePlot( DrawBase* db, const std::string& name, const std::string& axisName, TH1D* h1_left, TH1D* h1_right, float lysocal_ref );




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

  TPaveText* labelTop = db->get_labelTop();



  float X0_tung = 3.5;
  float X0_lyso = 11.4;
  float X0_cef3 = 16.8;

  float tung_costpercc = 4.7; // in dollars
  float cef3_costpercc = 30.; // in dollars
  float lyso_costpercc = 30.; // in dollars

  // first, compute stuff for the LYSO calorimeter
  float lysocal_act_thick = 1.5;
  float lysocal_abs_thick = 2.5;
  int   lysocal_abs_n     = 28;
  int   lysocal_act_n     = lysocal_abs_n;
  //int   lysocal_act_n     = lysocal_abs_n+1;

  float lysocal_length_act = getCompositeSum( lysocal_act_n, lysocal_act_thick );
  float lysocal_length_abs = getCompositeSum( lysocal_abs_n, lysocal_abs_thick );
  float lysocal_length_tot = lysocal_length_act + lysocal_length_abs;

  float lysocal_X0_act = getEffectiveX0( lysocal_act_n, lysocal_act_thick, X0_lyso );
  float lysocal_X0_abs = getEffectiveX0( lysocal_abs_n, lysocal_abs_thick, X0_tung );
  float lysocal_X0_tot = lysocal_X0_act + lysocal_X0_abs;


  CaloParameters lysocal = drawSingleConfiguration( db, batchProd, "LYSO", lysocal_act_n, lysocal_act_thick, lysocal_abs_thick );
  std::cout << "LYSOCAL: " << std::endl;
  std::cout << "sf: " << lysocal.sf << std::endl;
  std::cout << "mol: " << lysocal.mol << std::endl;
  std::cout << "res: " << lysocal.res << std::endl;

  //std::cout << "*** LYSO CAL: " << std::endl;
  //std::cout << " Active material: " << lysocal_act_n << " layers of " << lysocal_act_thick << "mm of LYSO (X0 = " << X0_lyso << " mm)" << std::endl;
  //std::cout << " Absorber       : " << lysocal_abs_n << " layers of " << lysocal_abs_thick << "mm of W    (X0 = " << X0_tung << " mm)" << std::endl;
  //std::cout << " Total length   : " << lysocal_length_tot << " mm" << std::endl;
  //std::cout << " Total X0       : " << lysocal_X0_tot     << std::endl;

  //float lysocal_res = 0.1;
  //TF1* f1_lysocal_res = new TF1("f1_lysocal_res", "[0]/sqrt(x)", 0.1, 1000.); // taken from slide 20 of https://indico.cern.ch/event/292529/session/0/contribution/1/material/slides/
  //f1_lysocal_res->SetParameter(0,lysocal_res);
  //std::cout << "Sampling Resolution: " << std::endl;
  //std::cout << "  750 MeV: " << f1_lysocal_res->Eval(0.75) << std::endl;
  //std::cout << "  1 GeV: "   << f1_lysocal_res->Eval(1.)   << std::endl;
  //std::cout << "  10 GeV: "  << f1_lysocal_res->Eval(10.)  << std::endl;
  //std::cout << "  100 GeV: " << f1_lysocal_res->Eval(100.) << std::endl;



  float lysocal_act_cost   = getCostPerSquareMeter( lysocal_act_n, lysocal_act_thick, lyso_costpercc );
  float lysocal_abs_cost   = getCostPerSquareMeter( lysocal_abs_n, lysocal_abs_thick, tung_costpercc );
  float lysocal_cost   = lysocal_act_cost + lysocal_abs_cost;


  // now go ahead and study performance of CeF3/W:

  double X0_target = 24.; // want to have at least 24 X0 length

  double detectorLength = 0.;

  double tung_thickness = 2.5; // fixed
  //double tung_thickness = 3.; // fixed
  double cef3_thickness = 0.5; // will vary

  
  TH1D*  h1_res_vs_thickness = new TH1D("res_vs_thickness", "",   40, 0.75, 20.75);
  TH1D*  h1_sf_vs_thickness = new TH1D("sf_vs_thickness", "",     40, 0.75, 20.75);
  TH1D*  h1_cost_vs_thickness = new TH1D("cost_vs_thickness", "", 40, 0.75, 20.75);
  TH1D*  h1_mol_vs_thickness = new TH1D("mol_vs_thickness", "",   40, 0.75, 20.75);


  while ( detectorLength<220. ) {

    cef3_thickness = cef3_thickness + 0.5; // start from 1mm

    int thisBin = h1_mol_vs_thickness->FindBin(cef3_thickness);
  
    double X0_tung_singleLayer = tung_thickness/X0_tung;
    double X0_cef3_singleLayer = cef3_thickness/X0_cef3;
  

    // this was for N+1:
    //int nLayers = TMath::Ceil( (( X0_target - X0_cef3_singleLayer ) / ( X0_cef3_singleLayer + X0_tung_singleLayer )) );
    //if( tung_thickness==3. && cef3_thickness==3. ) nLayers = 24;
    //if( tung_thickness==3. && cef3_thickness==10. ) nLayers = 17;
    //float X0_eff = ((float)nLayers+1.)*X0_cef3_singleLayer + (float)nLayers*X0_tung_singleLayer;
    //detectorLength = (((float)nLayers+1)*cef3_thickness + (float)nLayers*tung_thickness);

    int nLayers = TMath::Ceil( X0_target / ( X0_cef3_singleLayer + X0_tung_singleLayer ) );
    if( tung_thickness==3. && cef3_thickness==3. ) nLayers = 24;
    if( tung_thickness==3. && cef3_thickness==9. ) nLayers = 17;
    if( tung_thickness==3. && cef3_thickness==10. ) nLayers = 17;
    float X0_eff = (float)nLayers*X0_cef3_singleLayer + (float)nLayers*X0_tung_singleLayer;
    detectorLength = (float)nLayers*cef3_thickness + (float)nLayers*tung_thickness;
    if( detectorLength>250. ) break;
    
    CaloParameters thiscal = drawSingleConfiguration( db, batchProd, "CeF3", nLayers, cef3_thickness, tung_thickness );

    std::cout << std::endl;
    std::cout << "sf: " << thiscal.sf << std::endl;
    std::cout << "res: " << thiscal.res << std::endl;
    std::cout << "mol: " << thiscal.mol << std::endl;
    if( thiscal.sf>0. ) {
      h1_sf_vs_thickness->SetBinContent( thisBin, thiscal.sf );
      //h1_sf_vs_thickness->SetBinError  ( thisBin, thiscal.sferr );
    }

    if( thiscal.res>0. ) {
      h1_res_vs_thickness->SetBinContent( thisBin, thiscal.res );
      //h1_res_vs_thickness->SetBinError  ( thisBin, thiscal.reserr );
    }

    if( thiscal.mol>0. ) {
      h1_mol_vs_thickness->SetBinContent( thisBin, thiscal.mol );
      //h1_mol_vs_thickness->SetBinError  ( thisBin, thiscal.molerr );
    }



    // cost:

    float cef3cal_act_cost   = getCostPerSquareMeter( nLayers, cef3_thickness, cef3_costpercc );
    float cef3cal_abs_cost   = getCostPerSquareMeter( nLayers, tung_thickness, tung_costpercc ); 
    float cef3cal_cost   = cef3cal_act_cost + cef3cal_abs_cost;

    h1_cost_vs_thickness->SetBinContent( thisBin, cef3cal_cost );

  }


  // draw all ratio plots:
  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  float xMinPlot = 0.75;
  float xMaxPlot = 8.25;
  
  TH2D* h2_axes = new TH2D("axes", "", 10, xMinPlot, xMaxPlot, 10, 0., 4.);
  h2_axes->SetXTitle( "CeF_{3} Thickness [mm]");
  h2_axes->SetYTitle( "Ratio to LYSO Calorimeter" );

  h2_axes->Draw();

  TPaveText* label_top = new TPaveText(0.4,0.953,0.975,0.975, "brNDC");
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right
  label_top->SetTextFont(62);
  label_top->AddText("CMS Simulation, Electron gun");
  label_top->Draw("same");




  TLine* line_one = new TLine( xMinPlot, 1., xMaxPlot, 1. );
  line_one->SetLineColor(kBlack);
  line_one->Draw("same");

  TH1D* h1_cost_vs_thickness_norm = new TH1D(*h1_cost_vs_thickness);
  TH1D* h1_mol_vs_thickness_norm = new TH1D(*h1_mol_vs_thickness);
  TH1D* h1_res_vs_thickness_norm = new TH1D(*h1_res_vs_thickness);
  TH1D* h1_sf_vs_thickness_norm = new TH1D(*h1_sf_vs_thickness);

  h1_cost_vs_thickness_norm->Scale(1./lysocal_cost);
  h1_mol_vs_thickness_norm ->Scale(1./lysocal.mol);
  h1_res_vs_thickness_norm ->Scale(1./lysocal.res);
  h1_sf_vs_thickness_norm  ->Scale(1./lysocal.sf);
  //h1_mol_vs_thickness_norm->Divide(lysocal_cost);
  //h1_res_vs_thickness_norm->Divide(lysocal_cost);
  //h1_sf_vs_thickness_norm->Divide(0.3);


  h1_cost_vs_thickness_norm->SetLineColor(kBlack);
  h1_res_vs_thickness_norm ->SetLineColor(46);
  h1_sf_vs_thickness_norm  ->SetLineColor(29);
  h1_mol_vs_thickness_norm ->SetLineColor(38);

  h1_cost_vs_thickness_norm->SetLineWidth(2);
  h1_res_vs_thickness_norm->SetLineWidth(2);
  h1_sf_vs_thickness_norm->SetLineWidth(2);
  h1_mol_vs_thickness_norm->SetLineWidth(2);

  //h1_cost_vs_thickness_norm->SetMarkerSize(1.6);
  //h1_res_vs_thickness_norm ->SetMarkerSize(1.6);
  //h1_sf_vs_thickness_norm  ->SetMarkerSize(1.6);
  //h1_mol_vs_thickness_norm ->SetMarkerSize(1.6);

  //h1_cost_vs_thickness_norm->SetMarkerStyle(20);
  //h1_res_vs_thickness_norm ->SetMarkerStyle(21);
  //h1_sf_vs_thickness_norm  ->SetMarkerStyle(24);
  //h1_mol_vs_thickness_norm ->SetMarkerStyle(25);

  TLegend* legend = new TLegend( 0.2, 0.65, 0.6, 0.9 );
  legend->SetTextSize(0.037);
  legend->SetFillColor(0);
  legend->AddEntry(h1_cost_vs_thickness_norm, "Cost", "L");
  legend->AddEntry(h1_sf_vs_thickness_norm, "Sampling Fraction", "L");
  legend->AddEntry(h1_mol_vs_thickness_norm, "Moliere Radius", "L");
  legend->AddEntry(h1_res_vs_thickness_norm, "Resolution", "L");
  legend->Draw("Same");

  h1_cost_vs_thickness_norm->Draw("L same");
  h1_res_vs_thickness_norm->Draw("L same");
  h1_sf_vs_thickness_norm ->Draw("L same");
  h1_mol_vs_thickness_norm->Draw("L same");


  c1->SaveAs(Form("%s/CeF3_vs_LYSO.eps", db->get_outputdir().c_str()));
  c1->SaveAs(Form("%s/CeF3_vs_LYSO.png", db->get_outputdir().c_str()));

  gStyle->SetPadLeftMargin(0.112);
  gStyle->SetPadTopMargin(0.07);
  gStyle->SetPadRightMargin(0.12);
  gStyle->SetPadTickY(0);
  gStyle->cd();


  drawDoublePlot( db, "res", "Sampling Resolution Term", h1_cost_vs_thickness, h1_res_vs_thickness, lysocal.res);
  drawDoublePlot( db, "mol",  "Moliere Radius [mm]",      h1_cost_vs_thickness, h1_mol_vs_thickness , lysocal.mol);


  return 0;

}


void drawDoublePlot( DrawBase* db, const std::string& name, const std::string& axisName, TH1D* h1_left, TH1D* h1_right, float lysocal_ref ) {

  float xMin = 1.5;
  float xMax = 10.5;

  TCanvas* c2 = new TCanvas("c2", "", 600, 600);
  c2->cd();


  TH2D* h2_axes = new TH2D("axes2", "", 10, xMin, xMax, 10, 0., 4.);
  h2_axes->SetXTitle("CeF_{3} Layer Thickness [mm]");
  h2_axes->GetYaxis()->SetTitleOffset(1.4);
  h2_axes->GetYaxis()->SetTitleFont();
  h2_axes->GetYaxis()->SetTitleSize(0.04);
  h2_axes->SetYTitle("Cost Ratio");

  h2_axes->Draw();

  TLine* line_lysocal = new TLine( xMin, 1., xMax, 1. );
  line_lysocal->Draw("same");

  TPaveText* text_lysocal = new TPaveText(6.5, 1.06, 10., 1.11);
  text_lysocal->SetFillColor(0);
  text_lysocal->SetTextSize(0.035);
  text_lysocal->AddText("LYSO Calorimeter");
  text_lysocal->Draw("same");

  //h1_cost_vs_thickness->SetLineColor(46);
  h1_left->SetLineWidth(2);
  h1_left->Draw("same");


  c2->Update();

  float rightmax = lysocal_ref*3.;
  float scale = gPad->GetUymax()/rightmax;
  h1_right->Scale(scale);

  h1_right->SetMarkerStyle(20);
  h1_right->SetMarkerSize(2.);
  h1_right->SetMarkerColor(46);
  h1_right->Draw("p EX0 same");
   
  //draw an axis on the right side
  TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),
        gPad->GetUxmax(), gPad->GetUymax(),0,rightmax,510,"+L");
  axis->SetLineColor(46);
  axis->SetLabelColor(46);
  axis->SetTextColor(46);
  axis->SetTextSize(0.04);
  axis->SetTitleOffset(1.5);
  axis->SetTitle(axisName.c_str());
  axis->Draw();


  TPaveText* label_top = new TPaveText(0.2,0.94,0.9,0.96, "brNDC");
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right
  label_top->SetTextFont(62);
  label_top->AddText("CMS Simulation, Electron gun");
  label_top->Draw("same");



  c2->SaveAs(Form("%s/%s_vs_tickness.eps", db->get_outputdir().c_str(), name.c_str()) );
  c2->SaveAs(Form("%s/%s_vs_tickness.png", db->get_outputdir().c_str(), name.c_str()) );

  delete c2;
  delete h2_axes;

}





float getCompositeSum( int act_n, float act_l ) {

  float sum = (float)act_n*act_l;

  return sum;

}


float getEffectiveX0( int act_n, float act_thick, float act_X0 ) {

  float X0 = (float)act_n*act_thick/act_X0;

  return X0;

}


float getCostPerSquareMeter( int n, float length, float cost ) { // cost is dollars per cube cm

  float totalLength = (float)n*length;

  float convertedCost = cost / 1000.;

  return convertedCost*totalLength;

}



CaloParameters drawSingleConfiguration( DrawBase* db, const std::string& batchProd, const std::string& actType, int nLayers, float act, float abso ) {


  std::string extraString_actType = (actType=="CeF3") ? "" : "/"+actType;

  std::string act_str = getStringWithDecimal(act);
  std::string abs_str = getStringWithDecimal(abso);


  TFile* file_1GeV   = TFile::Open(Form("../batchOutput_%s/1GeV%s/rootfiles/samplinghistos_e1_n%d_act%s_abs%s_trasv20.root",   batchProd.c_str(), extraString_actType.c_str(), nLayers, act_str.c_str(), abs_str.c_str()) );
  TFile* file_10GeV  = TFile::Open(Form("../batchOutput_%s/10GeV%s/rootfiles/samplinghistos_e10_n%d_act%s_abs%s_trasv20.root", batchProd.c_str(), extraString_actType.c_str(), nLayers, act_str.c_str(), abs_str.c_str()) );
  TFile* file_5GeV   = TFile::Open(Form("../batchOutput_%s/5GeV%s/rootfiles/samplinghistos_e5_n%d_act%s_abs%s_trasv20.root",   batchProd.c_str(), extraString_actType.c_str(), nLayers, act_str.c_str(), abs_str.c_str()) );


  // first sampling fraction:

  TH1D* h1_sf_1GeV   = (file_1GeV!=0)   ? (TH1D*)file_1GeV  ->Get("h_sf_config0_1000MeV")  : 0;
  TH1D* h1_sf_10GeV  = (file_10GeV!=0)  ? (TH1D*)file_10GeV ->Get("h_sf_config0_10000MeV") : 0;
  TH1D* h1_sf_5GeV   = (file_5GeV!=0)   ? (TH1D*)file_5GeV  ->Get("h_sf_config0_5000MeV"): 0;


  float sf_1GeV   = (h1_sf_1GeV  !=0) ? h1_sf_1GeV  ->GetMean() : 0.;
  float sf_10GeV  = (h1_sf_10GeV !=0) ? h1_sf_10GeV ->GetMean() : 0.;
  float sf_5GeV   = (h1_sf_5GeV!=0)   ? h1_sf_5GeV  ->GetMean() : 0.;

  float sferr_1GeV   = (h1_sf_1GeV  !=0) ? h1_sf_1GeV  ->GetMeanError() : 0.;
  float sferr_10GeV  = (h1_sf_10GeV !=0) ? h1_sf_10GeV ->GetMeanError() : 0.;
  float sferr_5GeV   = (h1_sf_5GeV!=0)   ? h1_sf_5GeV  ->GetMeanError() : 0.;

  TGraphErrors* gr_sf = new TGraphErrors(0);
  if( sf_1GeV>0. ) {
    gr_sf->SetPoint( 0, 1.,   sf_1GeV   );
    gr_sf->SetPointError( 0, 0., sferr_1GeV   );
  }
  if( sf_10GeV>0.) {
    gr_sf->SetPoint( 1, 10.,  sf_10GeV  );
    gr_sf->SetPointError( 1, 0., sferr_10GeV  );
  }
  if( sf_5GeV>0. ) {
    gr_sf->SetPoint( 2, 5., sf_5GeV );
    gr_sf->SetPointError( 2, 0., sferr_5GeV );
  }



  // then ecal energy resolution:

  TH1D* h1_ecal_1GeV   = (file_1GeV!=0)   ? (TH1D*)file_1GeV  ->Get("h_ecal_config0_1000MeV")  : 0;
  TH1D* h1_ecal_10GeV  = (file_10GeV!=0)  ? (TH1D*)file_10GeV ->Get("h_ecal_config0_10000MeV") : 0;
  TH1D* h1_ecal_5GeV   = (file_5GeV!=0)   ? (TH1D*)file_5GeV  ->Get("h_ecal_config0_5000MeV")  : 0;


  if( h1_ecal_1GeV==0 && h1_ecal_10GeV==0 && h1_ecal_5GeV==0 ) {
    std::cout << "WARNING! Didn't find anything for " + actType + " = " << act << " mm. Skipping!" << std::endl;
    CaloParameters cp1;
    cp1.res = -1.;
    cp1.mol = -1.;
    cp1.sf = -1.;
    return cp1;
  }
      

  float ecalrms_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetRMS() : 0.;
  float ecalrms_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetRMS() : 0.;
  float ecalrms_5GeV = (h1_ecal_5GeV!=0) ? h1_ecal_5GeV->GetRMS() : 0.;

  float ecal_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetMean() : 0.;
  float ecal_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetMean() : 0.;
  float ecal_5GeV = (h1_ecal_5GeV!=0) ? h1_ecal_5GeV->GetMean() : 0.;

  float ecalrmserr_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetRMSError() : 0.;
  float ecalrmserr_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetRMSError() : 0.;
  float ecalrmserr_5GeV = (h1_ecal_5GeV!=0) ? h1_ecal_5GeV->GetRMSError() : 0.;

  float ecalerr_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetMeanError() : 0.;
  float ecalerr_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetMeanError() : 0.;
  float ecalerr_5GeV = (h1_ecal_5GeV!=0) ? h1_ecal_5GeV->GetMeanError() : 0.;

  float res_1GeV   = (ecal_1GeV>0.)   ? ecalrms_1GeV  /ecal_1GeV   : 0.;
  float res_10GeV  = (ecal_10GeV>0.)  ? ecalrms_10GeV /ecal_10GeV  : 0.;
  float res_5GeV = (ecal_5GeV>0.) ? ecalrms_5GeV/ecal_5GeV : 0.;

  float reserr_1GeV   = sqrt( ecalrmserr_1GeV  *ecalrmserr_1GeV   / (ecal_1GeV*  ecal_1GeV)   + ecalerr_1GeV  *ecalerr_1GeV   *ecalrms_1GeV  *ecalrms_1GeV   / (ecal_1GeV*  ecal_1GeV*  ecal_1GeV*  ecal_1GeV)   );
  float reserr_10GeV  = sqrt( ecalrmserr_10GeV *ecalrmserr_10GeV  / (ecal_10GeV* ecal_10GeV)  + ecalerr_10GeV *ecalerr_10GeV  *ecalrms_10GeV *ecalrms_10GeV  / (ecal_10GeV* ecal_10GeV* ecal_10GeV* ecal_10GeV)  );
  float reserr_5GeV = sqrt( ecalrmserr_5GeV*ecalrmserr_5GeV / (ecal_5GeV*ecal_5GeV) + ecalerr_5GeV*ecalerr_5GeV *ecalrms_5GeV*ecalrms_5GeV / (ecal_5GeV*ecal_5GeV*ecal_5GeV*ecal_5GeV) );


  TGraphErrors* gr_res = new TGraphErrors(0);
  if( res_1GeV>0. ) {
    gr_res->SetPoint( 0, 1.,   res_1GeV   );
    gr_res->SetPointError( 0, 0., reserr_1GeV   );
  }
  if( res_10GeV>0.) {
    gr_res->SetPoint( 1, 10.,  res_10GeV  );
    gr_res->SetPointError( 1, 0., reserr_10GeV  );
  }
  if( res_5GeV>0. ) {
    gr_res->SetPoint( 2, 5., res_5GeV );
    gr_res->SetPointError( 2, 0., reserr_5GeV );
  }



  // then moliere radius:

  TH1D* h1_mol_1GeV   = (file_1GeV!=0)   ? (TH1D*)file_1GeV  ->Get("h_mol_config0_1000MeV")  : 0;
  TH1D* h1_mol_10GeV  = (file_10GeV!=0)  ? (TH1D*)file_10GeV ->Get("h_mol_config0_10000MeV") : 0;
  TH1D* h1_mol_5GeV   = (file_5GeV!=0)   ? (TH1D*)file_5GeV  ->Get("h_mol_config0_5000MeV"): 0;


  float mol_1GeV   = (h1_mol_1GeV  !=0) ? h1_mol_1GeV  ->GetMean() : 0.;
  float mol_10GeV  = (h1_mol_10GeV !=0) ? h1_mol_10GeV ->GetMean() : 0.;
  float mol_5GeV   = (h1_mol_5GeV!=0)   ? h1_mol_5GeV  ->GetMean() : 0.;

  float molerr_1GeV   = (h1_mol_1GeV  !=0) ? h1_mol_1GeV  ->GetMeanError() : 0.;
  float molerr_10GeV  = (h1_mol_10GeV !=0) ? h1_mol_10GeV ->GetMeanError() : 0.;
  float molerr_5GeV   = (h1_mol_5GeV!=0)   ? h1_mol_5GeV  ->GetMeanError() : 0.;

  TGraphErrors* gr_mol = new TGraphErrors(0);
  if( mol_1GeV>0. ) {
    gr_mol->SetPoint( 0, 1.,   mol_1GeV   );
    gr_mol->SetPointError( 0, 0., molerr_1GeV   );
  }
  if( mol_10GeV>0.) {
    gr_mol->SetPoint( 1, 10.,  mol_10GeV  );
    gr_mol->SetPointError( 1, 0., molerr_10GeV  );
  }
  if( mol_5GeV>0. ) {
    gr_mol->SetPoint( 2, 5., mol_5GeV );
    gr_mol->SetPointError( 2, 0., molerr_5GeV );
  }




  // here we go:

  TF1* f1_sf  = drawAndFit( db, gr_sf,   "sf",    "Sampling Fraction",      "[0]",         nLayers, actType, act, abso );
  TF1* f1_res = drawAndFit( db, gr_res, "sfres", "ECAL Energy Resolution", "[0]/sqrt(x)", nLayers, actType, act, abso );
  TF1* f1_mol = drawAndFit( db, gr_mol,   "mol",  "Moliere Radius",         "[0]",         nLayers, actType, act, abso );

 
  //std::cout << std::endl << std::endl;
  //std::cout << "*** NEW CONFIGURATION" << std::endl;
  //std::cout << nLayers << " layers of: " << actType << " = " << act << "mm / W = " << abso << "mm" << std::endl;
  //std::cout << "Sampling Fraction: " << 100.*sf << " %" << std::endl;
  //std::cout << "ECAL Energy Resolution (Stochastic term): " << 100.*res << " %" << std::endl;
  //std::cout << "Moliere Radius: " << mol << " mm" << std::endl;

  CaloParameters cp;
  cp.sf  = f1_sf ->GetParameter(0);
  cp.res = f1_res->GetParameter(0);
  cp.mol = f1_mol->GetParameter(0);

  cp.sferr  = f1_sf ->GetParError(0);
  cp.reserr = f1_res->GetParError(0);
  cp.molerr = f1_mol->GetParError(0);

  return cp;

}



TF1* drawAndFit( DrawBase* db, TGraphErrors* gr_S, const std::string& name, const std::string& varName, const std::string& formula, int nLayers, const std::string& actType, float act, float abso ) {


  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();
  c1->SetLogx();

  float yMax = (name=="mol") ? 25. : 0.7;
  if( name=="sf" ) yMax = 0.4;
  float yLog = (name=="sfres") ? true : false;
  if( yLog ) c1->SetLogy();

  TH2D* h2_axes = new TH2D("axes", "", 10, 0.5, 200., 10, 0.001, yMax);
  h2_axes->SetXTitle("Electron Energy [GeV]");
  h2_axes->SetYTitle(varName.c_str());

  h2_axes->Draw();


  TPaveText* label_top = new TPaveText(0.4,0.953,0.975,0.975, "brNDC");
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right
  label_top->SetTextFont(62);
  label_top->AddText("CMS Simulation, Electron gun");
  label_top->Draw("same");


  TPaveText* text_act = new TPaveText(0.25, 0.35, 0.4, 0.4, "brNDC" );
  text_act->SetFillColor(kWhite);
  text_act->SetTextSize(0.035);
  text_act->SetTextAlign(11); // align left
  text_act->AddText(Form("%s = %.1fmm", actType.c_str(), act));
  text_act->Draw("same");

  TPaveText* text_abs = new TPaveText(0.25, 0.3, 0.4, 0.35, "brNDC" );
  text_abs->SetFillColor(kWhite);
  text_abs->SetTextSize(0.035);
  text_abs->SetTextAlign(11); // align left
  text_abs->AddText(Form("W = %.0fmm", abso));
  text_abs->Draw("same");

  gr_S->SetMarkerStyle(20);
  gr_S->SetMarkerSize(1.3);
  gr_S->SetMarkerColor(46);
  
  TF1* f1_res = new TF1(Form("f1_fit_%s", name.c_str()), formula.c_str(), 0.1, 1000.);
  f1_res->SetLineColor(29);
  f1_res->SetLineWidth(2);

  gr_S->Draw("p same");
  if( gr_S->GetN()>1. ) {
    gr_S->Fit( f1_res, "R" );
    f1_res->Draw("L same");
  }

  //labelTop->Draw("same");

  gPad->RedrawAxis();

  std::string act_str;
  double intpart;
  if( modf(act, &intpart) > 0. ) {
    char act_str_char[100];
    sprintf( act_str_char, "%.0fp%.0f", act, 10*modf(act, &intpart) );
    std::string act_str_tmp(act_str_char);
    act_str = act_str_tmp;
  } else {
    char act_str_char[100];
    sprintf( act_str_char, "%.0f", act );
    std::string act_str_tmp(act_str_char);
    act_str = act_str_tmp;
  }


  c1->SaveAs(Form("%s/fit_%s_n%d_%s_act%s_abs%.0f.eps", db->get_outputdir().c_str(), name.c_str(), nLayers, actType.c_str(), act_str.c_str(), abso) );
  c1->SaveAs(Form("%s/fit_%s_n%d_%s_act%s_abs%.0f.png", db->get_outputdir().c_str(), name.c_str(), nLayers, actType.c_str(), act_str.c_str(), abso) );

  delete c1;
  delete h2_axes;
  delete label_top;
  delete text_abs;
  delete text_act;

  return f1_res;

}


std::string getStringWithDecimal( float act ) {

  std::string act_str;
  double intpart, fracpart;
  fracpart = modf(act, &intpart);
  if( fracpart > 0. ) {
    char act_str_char[100];
    sprintf( act_str_char, "%.0fp%.0f", intpart, 10*fracpart );
    std::string act_str_tmp(act_str_char);
    act_str = act_str_tmp;
  } else {
    char act_str_char[100];
    sprintf( act_str_char, "%.0f", act );
    std::string act_str_tmp(act_str_char);
    act_str = act_str_tmp;
  }

  return act_str;

}
