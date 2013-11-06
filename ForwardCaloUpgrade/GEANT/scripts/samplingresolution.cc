void samplingresolution(TString storename, TString tgraphname, TString outfilename, TString workdir, TString config){


  //gROOT->LoadMacro(workdir+"/scripts/style-CMSTDR.C");
  //setTDRStyle();
  
  TFile *g = new TFile(storename, "update");
  TGraph *graph = (TGraph*) g->Get(tgraphname);
  
  TF1 *f_fit = new TF1("f_fit","[0]/(sqrt(x))",0,200);
  f_fit->SetParameter(0,0.10);
  
  TCanvas *c1 = new TCanvas(config+"_samp_res","Sampling resolution fit",600,800);

  graph->GetXaxis()->SetTitle("E");
  cout << "TAG4" << endl;  

  c1->cd();
  graph->Fit("f_fit");
  graph->Draw("AP");
  //  graph->FitPanel();
  //f_fit->Draw("same");

  Double_t samp_res     = f_fit->GetParameter(0);
  Double_t samp_res_err = f_fit->GetParError(0);
  
  c1->Write();

  g->Close();

  //append to file
  ofstream out;
  out.open(outfilename,ios::app);
  out << "Sampling resolution of setup\t" << samp_res << " +/- " << samp_res_err << endl << endl;
  out.close();
}
