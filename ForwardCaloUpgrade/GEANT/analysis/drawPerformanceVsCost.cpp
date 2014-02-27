#include <iostream>
#include <cstdlib>
#include <cmath>

#include "TMath.h"
#include "TGaxis.h"

#include "CommonTools/DrawBase.h"


// all lengths in mm




float getCompositeSum( int act_n, float act_l );
float getEffectiveX0( int act_n, float act_thick, float act_X0 );
float getCostPerSquareMeter( int n, float length, float cost );

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
  int   lysocal_act_n     = lysocal_abs_n+1;

  float lysocal_length_act = getCompositeSum( lysocal_act_n, lysocal_act_thick );
  float lysocal_length_abs = getCompositeSum( lysocal_abs_n, lysocal_abs_thick );
  float lysocal_length_tot = lysocal_length_act + lysocal_length_abs;

  float lysocal_X0_act = getEffectiveX0( lysocal_act_n, lysocal_act_thick, X0_lyso );
  float lysocal_X0_abs = getEffectiveX0( lysocal_abs_n, lysocal_abs_thick, X0_tung );
  float lysocal_X0_tot = lysocal_X0_act + lysocal_X0_abs;

  std::cout << "*** LYSO CAL: " << std::endl;
  std::cout << " Active material: " << lysocal_act_n << " layers of " << lysocal_act_thick << "mm of LYSO (X0 = " << X0_lyso << " mm)" << std::endl;
  std::cout << " Absorber       : " << lysocal_abs_n << " layers of " << lysocal_abs_thick << "mm of W    (X0 = " << X0_tung << " mm)" << std::endl;
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


  float lysocal_mol = 13.7;

  float lysocal_act_cost   = getCostPerSquareMeter( lysocal_act_n, lysocal_act_thick, lyso_costpercc );
  float lysocal_abs_cost   = getCostPerSquareMeter( lysocal_abs_n, lysocal_abs_thick, tung_costpercc );
  float lysocal_cost   = lysocal_act_cost + lysocal_abs_cost;


  // now go ahead and study performance of CeF3/W:

  double X0_target = 24.; // want to have at least 24 X0 length

  double detectorLength = 0.;

  double tung_thickness = 3.; // fixed
  double cef3_thickness = 1.; // will vary

  
  TH1D*  h1_Sres_vs_thickness = new TH1D("Sres_vs_thickness", "", 20, 0.5, 20.5);
  TH1D*  h1_cost_vs_thickness = new TH1D("cost_vs_thickness", "", 20, 0.5, 20.5);
  TH1D*  h1_mol_vs_thickness = new TH1D("mol_vs_thickness", "", 20, 0.5, 20.5);


  while ( detectorLength<220. ) {

    cef3_thickness = cef3_thickness + 1.; // start from 2mm

    int thisBin = h1_mol_vs_thickness->FindBin(cef3_thickness);
  
    double X0_tung_singleLayer = tung_thickness/X0_tung;
    double X0_cef3_singleLayer = cef3_thickness/X0_cef3;
  

    int nLayers = TMath::Ceil( (( X0_target - X0_cef3_singleLayer ) / ( X0_cef3_singleLayer + X0_tung_singleLayer )) );
    if( tung_thickness==3. && cef3_thickness==3. ) nLayers = 24;
    if( tung_thickness==3. && cef3_thickness==10. ) nLayers = 17;

    float X0_eff = ((float)nLayers+1.)*X0_cef3_singleLayer + (float)nLayers*X0_tung_singleLayer;
  
    detectorLength = (((float)nLayers+1)*cef3_thickness + (float)nLayers*tung_thickness);
    if( detectorLength>250. ) break;
    

    TFile* file_1GeV   = TFile::Open(Form(  "../batchOutput_%s/1GeV/rootfiles/samplinghistos_e1_n%d_act%.0f_abs3_trasv20.root", batchProd.c_str(), nLayers, cef3_thickness) );
    TFile* file_10GeV  = TFile::Open(Form( "../batchOutput_%s/10GeV/rootfiles/samplinghistos_e10_n%d_act%.0f_abs3_trasv20.root", batchProd.c_str(), nLayers, cef3_thickness) );
    TFile* file_5GeV = TFile::Open(Form("../batchOutput_%s/5GeV/rootfiles/samplinghistos_e5_n%d_act%.0f_abs3_trasv20.root", batchProd.c_str(), nLayers, cef3_thickness) );

    TH1D* h1_ecal_1GeV   = (file_1GeV!=0)   ? (TH1D*)file_1GeV  ->Get("h_ecal_config0_1000MeV")  : 0;
    TH1D* h1_ecal_10GeV  = (file_10GeV!=0)  ? (TH1D*)file_10GeV ->Get("h_ecal_config0_10000MeV") : 0;
    TH1D* h1_ecal_5GeV = (file_5GeV!=0) ? (TH1D*)file_5GeV->Get("h_ecal_config0_5000MeV"): 0;


    if( h1_ecal_1GeV==0 && h1_ecal_10GeV==0 && h1_ecal_5GeV==0 ) {
      std::cout << "WARNING! Didn't find anything for CeF3 = " << cef3_thickness << " mm. Skipping!" << std::endl;
      continue;
    }
      

    float sfrms_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetRMS() : 0.;
    float sfrms_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetRMS() : 0.;
    float sfrms_5GeV = (h1_ecal_5GeV!=0) ? h1_ecal_5GeV->GetRMS() : 0.;

    float sf_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetMean() : 0.;
    float sf_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetMean() : 0.;
    float sf_5GeV = (h1_ecal_5GeV!=0) ? h1_ecal_5GeV->GetMean() : 0.;

    float sfrmserr_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetRMSError() : 0.;
    float sfrmserr_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetRMSError() : 0.;
    float sfrmserr_5GeV = (h1_ecal_5GeV!=0) ? h1_ecal_5GeV->GetRMSError() : 0.;

    float sferr_1GeV   = (h1_ecal_1GeV  !=0) ? h1_ecal_1GeV  ->GetMeanError() : 0.;
    float sferr_10GeV  = (h1_ecal_10GeV !=0) ? h1_ecal_10GeV ->GetMeanError() : 0.;
    float sferr_5GeV = (h1_ecal_5GeV!=0) ? h1_ecal_5GeV->GetMeanError() : 0.;

    float Sres_1GeV   = (sf_1GeV>0.)   ? sfrms_1GeV  /sf_1GeV   : 0.;
    float Sres_10GeV  = (sf_10GeV>0.)  ? sfrms_10GeV /sf_10GeV  : 0.;
    float Sres_5GeV = (sf_5GeV>0.) ? sfrms_5GeV/sf_5GeV : 0.;

    float Sreserr_1GeV   = sqrt( sfrmserr_1GeV  *sfrmserr_1GeV   / (sf_1GeV*  sf_1GeV)   + sferr_1GeV  *sferr_1GeV   *sfrms_1GeV  *sfrms_1GeV   / (sf_1GeV*  sf_1GeV*  sf_1GeV*  sf_1GeV)   );
    float Sreserr_10GeV  = sqrt( sfrmserr_10GeV *sfrmserr_10GeV  / (sf_10GeV* sf_10GeV)  + sferr_10GeV *sferr_10GeV  *sfrms_10GeV *sfrms_10GeV  / (sf_10GeV* sf_10GeV* sf_10GeV* sf_10GeV)  );
    float Sreserr_5GeV = sqrt( sfrmserr_5GeV*sfrmserr_5GeV / (sf_5GeV*sf_5GeV) + sferr_5GeV*sferr_5GeV *sfrms_5GeV*sfrms_5GeV / (sf_5GeV*sf_5GeV*sf_5GeV*sf_5GeV) );


    TGraphErrors* gr_Sres = new TGraphErrors(0);
    if( Sres_1GeV>0. ) {
      gr_Sres->SetPoint( 0, 1.,   Sres_1GeV   );
      gr_Sres->SetPointError( 0, 0., Sreserr_1GeV   );
    }
    if( Sres_10GeV>0.) {
      gr_Sres->SetPoint( 1, 10.,  Sres_10GeV  );
      gr_Sres->SetPointError( 1, 0., Sreserr_10GeV  );
    }
    if( Sres_5GeV>0. ) {
      gr_Sres->SetPoint( 2, 5., Sres_5GeV );
      gr_Sres->SetPointError( 2, 0., Sreserr_5GeV );
    }


    TCanvas* c1 = new TCanvas("c1", "", 600, 600);
    c1->cd();
    c1->SetLogx();
    c1->SetLogy();

    TH2D* h2_axes = new TH2D("axes", "", 10, 0.5, 200., 10, 0.001, 0.7);
    h2_axes->SetXTitle("Electron Energy [GeV]");
    h2_axes->SetYTitle("Sampling Resolution");

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
    text_act->AddText(Form("CeF3 = %.0fmm", cef3_thickness));
    text_act->Draw("same");

    TPaveText* text_abs = new TPaveText(0.25, 0.3, 0.4, 0.35, "brNDC" );
    text_abs->SetFillColor(kWhite);
    text_abs->SetTextSize(0.035);
    text_abs->SetTextAlign(11); // align left
    text_abs->AddText(Form("W = %.0fmm", tung_thickness));
    text_abs->Draw("same");

    gr_Sres->SetMarkerStyle(20);
    gr_Sres->SetMarkerSize(1.3);
    gr_Sres->SetMarkerColor(46);
  
    TF1* f1_Sres = new TF1("Sres", "[0]/sqrt(x)", 0.1, 1000.);
    f1_Sres->SetLineColor(29);
    f1_Sres->SetLineWidth(2);

    gr_Sres->Draw("p same");
    if( gr_Sres->GetN()>1. ) {
      gr_Sres->Fit( f1_Sres, "R" );
      f1_Sres->Draw("L same");
    }

    //labelTop->Draw("same");

    gPad->RedrawAxis();

    c1->SaveAs(Form("%s/fit_Sres_n%d_act%.0f_abs3_trasv20.eps", db->get_outputdir().c_str(), nLayers, cef3_thickness) );
    c1->SaveAs(Form("%s/fit_Sres_n%d_act%.0f_abs3_trasv20.png", db->get_outputdir().c_str(), nLayers, cef3_thickness) );

    float Sres = f1_Sres->GetParameter(0);
    float Sreserr = f1_Sres->GetParError(0);

    h1_Sres_vs_thickness->SetBinContent( thisBin, Sres );
    h1_Sres_vs_thickness->SetBinError  ( thisBin, Sreserr );




    // moliere radius:

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


    c1->Clear();
    c1->SetLogx();
    c1->SetLogy();

    TH2D* h2_axes2 = new TH2D("axes2", "", 10, 0.5, 200., 10, 1., 50.);
    h2_axes2->SetXTitle("Electron Energy [GeV]");
    h2_axes2->SetYTitle("Moliere Radius [mm]");

    h2_axes2->Draw();

    label_top->Draw("same");
    text_act->Draw("same");
    text_abs->Draw("same");


    gr_mol->SetMarkerStyle(20);
    gr_mol->SetMarkerSize(1.3);
    gr_mol->SetMarkerColor(46);
  
    TF1* f1_mol = new TF1("mol", "[0]", 0.1, 1000.);
    f1_mol->SetLineColor(29);
    f1_mol->SetLineWidth(2);

    gr_mol->Draw("p same");
    if( gr_mol->GetN()>1. ) {
      gr_mol->Fit( f1_mol, "R" );
      f1_mol->Draw("L same");
    }

    //labelTop->Draw("same");

    gPad->RedrawAxis();

    c1->SaveAs(Form("%s/fit_mol_n%d_act%.0f_abs3_trasv20.eps", db->get_outputdir().c_str(), nLayers, cef3_thickness) );
    c1->SaveAs(Form("%s/fit_mol_n%d_act%.0f_abs3_trasv20.png", db->get_outputdir().c_str(), nLayers, cef3_thickness) );

    float mol = f1_mol->GetParameter(0);
    float molerr = f1_mol->GetParError(0);

    h1_mol_vs_thickness->SetBinContent( thisBin, mol );
    h1_mol_vs_thickness->SetBinError  ( thisBin, molerr );



    // cost:

    float cef3cal_act_cost   = getCostPerSquareMeter( nLayers+1, cef3_thickness, cef3_costpercc );
    float cef3cal_abs_cost   = getCostPerSquareMeter( nLayers,   tung_thickness, tung_costpercc ); 
    float cef3cal_cost   = cef3cal_act_cost + cef3cal_abs_cost;

    h1_cost_vs_thickness->SetBinContent( thisBin, cef3cal_cost/lysocal_cost );

    

    delete gr_Sres;
    delete f1_Sres;
    delete c1;
    delete h2_axes;
 
  }




  gStyle->SetPadLeftMargin(0.112);
  gStyle->SetPadTopMargin(0.07);
  gStyle->SetPadRightMargin(0.12);
  gStyle->SetPadTickY(0);
  gStyle->cd();


  drawDoublePlot( db, "Sres", "Sampling Resolution Term", h1_cost_vs_thickness, h1_Sres_vs_thickness, lysocal_Sres);
  drawDoublePlot( db, "mol",  "Moliere Radius [mm]",      h1_cost_vs_thickness, h1_mol_vs_thickness , lysocal_mol);


  return 0;

}


void drawDoublePlot( DrawBase* db, const std::string& name, const std::string& axisName, TH1D* h1_left, TH1D* h1_right, float lysocal_ref ) {

  float xMin = 1.5;
  float xMax = 10.5;

  TCanvas* c2 = new TCanvas("c2", "", 600, 600);
  c2->cd();


  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., 3.);
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
