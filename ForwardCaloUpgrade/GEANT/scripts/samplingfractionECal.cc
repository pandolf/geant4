#include "Riostream.h"

void samplingfractionECal(TString workdir, TString rootfile, TString particle, TString Sthickabs, TString Sthicksens, TString Snlayers, TString Senergy, TString outfilename, TString runnumber, TString config, TString storename, TString tgraphname, TString absorber, TString sensitiv){
  
  Double_t energy    = Senergy.Atof();
  Double_t nlayers   = Snlayers.Atof();
  Double_t thickabs  = Sthickabs.Atof();
  Double_t thicksens = Sthicksens.Atof();


  //works on rootfile definde my script simulation.sh
  //  gROOT->LoadMacro(workdir+"/scripts/style-CMSTDR.C");
  // setTDRStyle();

  //only for output
  //----------------------------------------------------------------------
  //radiation length for materials Tungsten and CeF3 in mm!
  if(absorber=="Tungsten")
    Double_t xabs = 3.5;
  else if(absorber == "Lead")
    Double_t xabs = 5.6;
  else{
    cout << "Absorber X_0 not defined!" << endl;
    return;
  }  
  
  if(sensitiv=="CeF3")
    Double_t xsens = 16.8;     
  else if(sensitiv=="LYSO")
    Double_t xsens = 11.4;    
  else if(sensitiv=="YSO")
    Double_t xsens = 31.1;     
  else{
    cout << "Scintillator X_0 not defined!" << endl;
    return;
  }  
  
  //calculate setup parameters
  Double_t lengthmm = (thicksens+thickabs)*nlayers;
  Double_t lengthx  = (thicksens/xsens+thickabs/xabs)*nlayers;
  //--------------------------------------------------------------------------
  
  TFile *f  = new TFile(workdir+"/"+rootfile);
  TTree *mytree = (TTree*) f->Get("Total");


  //------------------------------------------------------------
  //Book histos
  Double_t EcalMaxBin = 50000;
  Double_t AbseMaxBin = 50000;
  Double_t EcalMinBin = 0;
  Double_t AbseMinBin = 0;
  if(particle=="mu-")
    Double_t Divider    = 15;
  else
    Double_t Divider    = 80;

  char suffix_cstr[200];
  sprintf( suffix_cstr, "config%s_%.0fMeV", config.Data(), (float)(1000.*energy) );
  TString suffix(suffix_cstr);
  
  TH1F *H_ecal       = new TH1F("h_ecal_"+suffix,"",EcalMaxBin/Divider,EcalMinBin,EcalMaxBin);
  TH1F *H_abse       = new TH1F("h_abse_"+suffix,"",AbseMaxBin/Divider,AbseMinBin,AbseMaxBin);

  TH1F *H_ecal_cut   = new TH1F("h_ecal_cut_"+suffix,"",EcalMaxBin/Divider,EcalMinBin,EcalMaxBin);
  TH1F *H_abse_cut   = new TH1F("h_abse_cut_"+suffix,"" ,AbseMaxBin/Divider,AbseMinBin,AbseMaxBin);

  TH1F *H_sf         = new TH1F("h_sf_"+suffix,"",200,0,1);

  TH1D *H_mol        = new TH1D();
  

  //------------------------------------------------------------------------------
  //Histo labeling/cosmetics
  
  H_ecal->GetYaxis()->SetTitle("#events") ;
  H_abse->GetYaxis()->SetTitle("#events") ;
  H_ecal_cut->GetYaxis()->SetTitle("#events") ;
  H_abse_cut->GetYaxis()->SetTitle("#events") ;
  H_sf ->GetYaxis()->SetTitle("#events") ;
  H_mol->GetYaxis()->SetTitle("#events") ;

  H_ecal->GetXaxis()->SetTitle("Energy [MeV]") ;
  H_abse-> GetXaxis()->SetTitle("Energy [MeV]") ;
  H_ecal_cut->GetXaxis()->SetTitle("Energy [MeV]") ;
  H_abse_cut->GetXaxis()->SetTitle("Energy [MeV]") ;
  H_sf ->GetXaxis()->SetTitle("Sampling fraction") ;
  H_mol->GetXaxis()->SetTitle("Moliere radius [mm]") ;
  
  TString ti_ecal     = "Energy deposited in "+Sthickabs+" mm "+absorber+" ("+Snlayers+" layers) by "+particle+" ("+Senergy+" GeV)";    
  TString ti_abse     = "Energy deposited in "+Sthicksens+" mm "+sensitiv+" ("+Snlayers+" layers) by "+particle+" ("+Senergy+" GeV)";    
  TString ti_ecal_cut = "Energy deposited in "+Sthickabs+" mm "+absorber+" by "+particle+" ("+Senergy+" GeV) after radiative tail cut";    
  TString ti_abse_cut = "Energy deposited in "+Sthickabs+" mm "+absorber+" by "+particle+" ("+Senergy+" GeV) adter radiative tail cut";    
  TString ti_sf       = "Sampling fraction of "+particle+" in "+absorber+"-"+sensitiv+" configuration ("+Sthickabs+" mm - "+Sthicksens+" mm)";
  TString ti_mol      = "Moliere radius of "+particle+" in "+sensitiv+" ("+Sthicksens+" mm, "+Snlayers+" layers)" ;
  
  H_ecal->SetTitle(ti_ecal) ;
  H_abse->SetTitle(ti_abse) ;
  H_sf ->SetTitle(ti_sf) ;
  H_mol->SetTitle(ti_mol) ;


  //-------------------------------------------------------------
  //Define analysis variable

  TFitResultPtr result; //in case samp_frac fit is not successful, still continue

  Double_t   E_ecal            = 0;    
  Double_t   E_abse            = 0;    
  Double_t   S_ecal            = 0;    
  Double_t   S_ecal_cut        = 0;    

  Double_t   samp_frac         = 0;
  Double_t   samp_width        = 0;
  Double_t   samp_frac_err     = 0;
  Double_t   samp_width_err    = 0;
  
  mytree->SetBranchAddress("ecal",&E_ecal);
  mytree->SetBranchAddress("abse",&E_abse);
  
    
  //Fill histos
  Long64_t nentries = mytree->GetEntries();
  
  for (Long64_t i = 0; i < nentries; i++){
    mytree->GetEntry(i);    
    H_ecal->Fill(E_ecal);
    H_abse->Fill(E_abse);
    H_sf->Fill(E_ecal/(E_ecal+E_abse));
    if(i==0){
      Double_t hrange=1000;
      if(particle=="mu-")
	hrange=400;
      H_sf->SetAxisRange((E_ecal/(E_ecal+E_abse))-0.3,(E_ecal/(E_ecal+E_abse))+0.3);
      H_ecal->SetAxisRange(E_ecal-hrange,E_ecal+hrange);
      H_abse->SetAxisRange(E_abse-hrange,E_abse+hrange);
      H_ecal_cut->SetAxisRange(E_ecal-hrange,E_ecal+hrange);
      H_abse_cut->SetAxisRange(E_abse-hrange,E_abse+hrange);
    }
  }
  
  //-----------------------------------------------------------
  //cut radiative tails in case of muons

  if (particle == "mu-"){

    cout << "CUTTING RADIATIVE TAIL" << endl;

    TH1F *H_sf_nocut   = (TH1F*) H_sf->Clone();
    H_sf_nocut->SetName("h_sf_nocut"+config+"_"+Senergy);
    H_sf_nocut->SetTitle("ECal Sampling Fraction (MIP no cut)");
    H_sf_nocut->Fit("gaus");
    H_sf->Reset();

    //fit landau & gaus
    TF1 *f_lan1 = new TF1("f_lan1", "[0]*TMath::Landau(x,[1],[2])",0,50000);
    f_lan1->SetParNames("l1","l2","l3");
    f_lan1->SetParameters(H_ecal->GetMaximum(),H_ecal->GetBinCenter(H_ecal->GetMaximumBin()),3); 
    TF1 *f_gaus1 = new TF1("f_gaus1", "[0]*TMath::Gaus(x,[1],[2])",0,H_ecal->GetBinCenter(H_ecal->GetMaximumBin()));
    f_gaus1->SetParameters(H_ecal->GetMaximum(),H_ecal->GetBinCenter(H_ecal->GetMaximumBin()),3); 
    f_gaus1->SetLineColor(kBlue);
    TF1 *f_lan2 = new TF1("f_lan2", "[0]*TMath::Landau(x,[1],[2])",0,50000);
    f_lan2->SetParNames("l1","l2","l3");
    f_lan2->SetParameters(H_abse->GetMaximum(),H_abse->GetBinCenter(H_abse->GetMaximumBin()),3); 
    TF1 *f_gaus2 = new TF1("f_gaus2", "[0]*TMath::Gaus(x,[1],[2])",0,H_abse->GetBinCenter(H_abse->GetMaximumBin()));
    f_gaus2->SetParameters(H_abse->GetMaximum(),H_abse->GetBinCenter(H_abse->GetMaximumBin()),3); 
    f_gaus2->SetLineColor(kBlue);

    TCanvas c1(config+"_ecal_tailcut",ti_ecal_cut);
    c1.cd();
    H_ecal->Fit("f_gaus1","R");
    H_ecal->Fit("f_lan1","+");
    f_gaus1->SetRange(0,50000);
    f_gaus1->Draw("SAME");

    TCanvas c2(config+"_abse_tailcut",ti_abse_cut);
    c2.cd();
    H_abse->Fit("f_gaus2","R");
    H_abse->Fit("f_lan2","+");
    f_gaus2->SetRange(0,50000);
    f_gaus2->Draw("SAME");

    Double_t x_cut_ecal = 0;
    Double_t x_cut_abse = 0;
    
    //find x_cut
    for(Double_t x=f_gaus1->GetMaximumX();x<5000;x++){
      if(10*f_gaus1->Eval(x)<f_lan1->Eval(x)){
	x_cut_ecal=x;
	break;
      }
    }
    for(Double_t x=f_gaus2->GetMaximumX();x<5000;x++){
      if(10*f_gaus2->Eval(x)<f_lan2->Eval(x)){
	x_cut_abse=x;
	break;
      }
    }
    
    TLine *t_ecal = new TLine(x_cut_ecal,0,x_cut_ecal,H_ecal->GetMaximum());
    TLine *t_abse = new TLine(x_cut_abse,0,x_cut_abse,H_abse->GetMaximum());
    t_ecal->SetLineWidth(3);    
    t_abse->SetLineWidth(3);    

    c1.cd();
    t_ecal->Draw("SAME");
    c2.cd();
    t_abse->Draw("SAME");


    cout << x_cut_ecal <<  "  "  << x_cut_abse << endl;
    
    //cut tails
    Bool_t ecal,abse;
    
    for (Long64_t i = 0; i < nentries; i++){
      mytree->GetEntry(i);    
      ecal=false;
      abse=false;
      if(E_ecal<x_cut_ecal){
	H_ecal_cut->Fill(E_ecal);
	ecal=true;
      }
      if(E_abse < x_cut_abse){
	H_abse_cut->Fill(E_abse);
	abse=true;
      }
      if(abse && ecal){
	H_sf->Fill(E_ecal/(E_ecal+E_abse));
      }
    }
    
    //workaround to not destroy canvases above
    TCanvas c3("dummy","dummy");
    c3.cd();
    result =  H_sf->Fit("gaus","S");
    
  }
  
  //for electron just use original distribution 
  else {
    result =  H_sf->Fit("gaus","S");
  }
  
  //---------------------------------------------------------------------------
  //calculate parameters

  if(result->IsValid()){  //if cutting the tails has lead to an invalid fit make simulation go on!
    samp_frac       =  H_sf->GetFunction("gaus")->GetParameter(1);
    samp_width      =  H_sf->GetFunction("gaus")->GetParameter(2);
    samp_frac_err   =  H_sf->GetFunction("gaus")->GetParError(1);
    samp_width_err  =  H_sf->GetFunction("gaus")->GetParError(2);
  }
  else{
    cout << "Fit to sampling fraction was not successful" << endl;
  }

  //Get Moliere Radius for e-
  if(particle=="e-"){
    H_mol = (TH1D*) f->Get("EcalSensMol");
    H_mol->Fit("gaus");
    Double_t rad_mol = H_mol->GetFunction("gaus")->GetParameter(1);
    H_mol->SetName("h_mol_"+suffix);
    H_mol->SetTitle("ECal Sens Moliere Radius");
  }
  else{
    Double_t rad_mol=0;
  }


  //------------------------------------------------------------------------------

  //TFile to save the histos
  TFile g(storename, "update");
  H_sf->Write();
  H_abse->Write();
  H_ecal->Write();
  if(particle=="e-")  
    H_mol->Write();
  else if(particle="mu-"){
    c1.Write();
    c2.Write();
    H_ecal_cut->Write();
    H_abse_cut->Write();
    H_sf_nocut->Write();
  }
  g.Write();
  g.Close();
  
  //Make sampling resolution plot
  if(particle == "e-"){
    Double_t y_value = samp_width/samp_frac;
    Double_t y_value_err = sqrt(pow((samp_width_err/samp_frac),2)+pow((samp_width*samp_frac_err)/(pow(samp_frac,2)),2));
    make_plot(config,y_value,y_value_err,energy,storename,tgraphname);
  }

  
  //Write File
  ofstream out;
  out.open(outfilename,ios::app);
  out << thicksens << "\t\t"<< thickabs  << "\t\t" << nlayers << "\t" << setprecision(3) << lengthmm << "\t" << lengthx << "\t" << particle << "\t\t" << Senergy << "\t" << samp_frac << "\t" << setw(8) << samp_width << "\t" << rad_mol << endl;  
  
  out.close();
  f->Close();
  
}

//------------------------------------------------------------------------------



void make_plot(TString config, Double_t y_value, Double_t y_value_err, Double_t energy, TString storename, TString tgraphname){

  TFile *g = new TFile(storename, "update");

  //Check if TGraph exists, of not create one
   
  if(!(g->GetListOfKeys()->Contains(tgraphname)))
    TGraphErrors *graph = new TGraphErrors();
  else     
    TGraphErrors *graph = g->Get(tgraphname);
  
  //Remove old Tgraph
  g->Delete(tgraphname+";1");
  

  //Add new point
  graph->SetPoint(graph->GetN(),energy,y_value);
  graph->SetPointError(graph->GetN()-1,0,y_value_err);
  
  graph->Write(tgraphname);

  }
