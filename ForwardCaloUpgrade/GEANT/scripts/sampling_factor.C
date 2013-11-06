#include <iostream>
#include <sstream>
#include <string>

//------------------------------------------
//function to calculate ecal sampling factor
Double_t g4_ecal_sample(std::string fname, double energy=50., bool do_show=false, bool do_print=false){
	//make strings
	std::stringstream drawname, oname, elename;
	oname << "ecal_sample_" << energy << "gev_peak.png";
	elename << "e- " << energy << " GeV";

	//open file and tree
	TFile* _file;
	_file = TFile::Open(fname.c_str());
	TTree* totalTree = (TTree*)_file->Get("Total");

	//get histo from tree (no display)
	drawname << "(ecal)/1000>>hsam";
	std::string cutname = "";
	totalTree->Draw((drawname.str()).c_str(),cutname.c_str(),"hist goff");
	TH1F* hsam = (TH1F*)gDirectory->Get("hsam");

	//find peak using tspectrum
	TSpectrum* spec = new TSpectrum(5);
	spec->Search(hsam,5,"nodraw goff");
	Float_t* xpos = spec->GetPositionX();
	Float_t* ypos = spec->GetPositionY();

	Double_t m = xpos[0];
	Double_t N = hsam->GetEntries();
	std::stringstream s_mean;
	Double_t f = energy/m;
	s_mean << f;

	TPolyMarker* pm = new TPolyMarker(1, xpos, ypos);
	hsam->GetListOfFunctions()->Add(pm);
	pm->SetMarkerStyle(23);
	pm->SetMarkerColor(kRed);
	pm->SetMarkerSize(1.3);

	//plotting and printing
	if (do_show){
		TCanvas* can = new TCanvas("ecal_sample","ecal_sample",700,500);
		can->cd();
		
		hsam->SetTitle("");
		hsam->GetXaxis()->SetTitle("Energy [GeV]");
		hsam->Draw();
		
		std::stringstream Nname;
		Nname << "N = " << N;
		
		//determine placing of pave
		Double_t xmin;
		if (m/((hsam->GetXaxis()->GetXmax() + hsam->GetXaxis()->GetXmin())/2) < 1) xmin = 0.7;
		else xmin = 0.2;
		
		//legend
		TPaveText *pave = new TPaveText(xmin,0.65,xmin+0.2,0.85,"NDC");
		pave->AddText((elename.str()).c_str());
		pave->AddText((Nname.str()).c_str());
		pave->AddText("Peak sampling factor:");
		pave->AddText((s_mean.str()).c_str());
		pave->SetFillColor(0);
		pave->SetBorderSize(0);
		pave->SetTextFont(42);
		pave->SetTextSize(0.04);
		pave->Draw("same");

		std::cout << "f_ecal = " << f << std::endl;

		if(do_print) can->Print((oname.str()).c_str(),"png");
	}

	return f;
}

//------------------------------------------
//function to calculate hcal sampling factor
Double_t g4_hcal_sample(std::string fname, Double_t sam_ecal=1.0, double energy=50., bool do_show=false, bool do_print=false){
	//make strings
	std::stringstream drawname, cutname, oname, piname;
	oname << "hcal_sample_" << energy << "gev_peak.png";
	piname << "pi- " << energy << " GeV";

	//open file and tree
	TFile* _file;
	_file = TFile::Open(fname.c_str());
	TTree* totalTree = (TTree*)_file->Get("Total");

	//get histo from tree (no display)
	//define mip as sam_ecal*ecal < 1 gev = 1000 mev
	Double_t zeroWt = 0.5;
	drawname << "(hcal+" << zeroWt << "*zero)/1000>>hsam";
	cutname << "(" << sam_ecal << "*ecal)<1000";
	totalTree->Draw((drawname.str()).c_str(),(cutname.str()).c_str(),"hist goff");
	TH1F* hsam = (TH1F*)gDirectory->Get("hsam");

	//find peak using tspectrum
	TSpectrum* spec = new TSpectrum(5);
	spec->Search(hsam,5,"nodraw goff");
	Float_t* xpos = spec->GetPositionX();
	Float_t* ypos = spec->GetPositionY();

	Double_t m = xpos[0];
	Double_t N = hsam->GetEntries();
	std::stringstream s_mean;
	Double_t f = energy/m;
	s_mean << f;

	TPolyMarker* pm = new TPolyMarker(1, xpos, ypos);
	hsam->GetListOfFunctions()->Add(pm);
	pm->SetMarkerStyle(23);
	pm->SetMarkerColor(kRed);
	pm->SetMarkerSize(1.3);

	//plotting and printing
	if (do_show){
		TCanvas* can = new TCanvas("hcal_sample","hcal_sample",700,500);
		can->cd();
		
		hsam->SetTitle("");
		hsam->GetXaxis()->SetTitle("Energy [GeV]");
		hsam->Draw();
		
		std::stringstream Nname;
		Nname << "N = " << N;
		
		//determine placing of pave
		Double_t xmin;
		if (m/((hsam->GetXaxis()->GetXmax() + hsam->GetXaxis()->GetXmin())/2) < 1) xmin = 0.7;
		else xmin = 0.2;
		
		//legend
		TPaveText *pave = new TPaveText(xmin,0.65,xmin+0.2,0.85,"NDC");
		pave->AddText((piname.str()).c_str());
		pave->AddText((Nname.str()).c_str());
		pave->AddText("Peak sampling factor:");
		pave->AddText((s_mean.str()).c_str());
		pave->SetFillColor(0);
		pave->SetBorderSize(0);
		pave->SetTextFont(42);
		pave->SetTextSize(0.04);
		pave->Draw("same");

		std::cout << "f_hcal = " << f << std::endl;

		if(do_print) can->Print((oname.str()).c_str(),"png");
	}

	return f;
}

//------------------------------
//function to reconstruct energy
void g4_reconstruct_energy(std::string fname, Double_t sam_hcal, Double_t sam_ecal=1.0, bool do_print=false){
	//open file and tree
	TFile* _file;
	_file = TFile::Open(fname.c_str());
	TTree* totalTree = (TTree*)_file->Get("Total");
	
	//make tree drawing expressions
	Double_t zeroWt = 0.5;
	std::stringstream drawname;
	drawname << "(" << sam_ecal << "*ecal+" << sam_hcal << "*(hcal+" << zeroWt << "*zero))/1000>>htemp";
	std::string cutname = "";
	//draw w/ appropriate cut
	totalTree->Draw((drawname.str()).c_str(),cutname.c_str(),"hist goff");
	
	//get histo
	TH1F* h_res = (TH1F*)gDirectory->Get("htemp");
	
	//make canvas
	TCanvas* can = new TCanvas("energy","energy",700,500);
	can->cd();
	
	//plot histo
	h_res->SetStats(kTRUE);
	gStyle->SetOptStat("emr");
	h_res->SetTitle("");
	h_res->GetXaxis()->SetTitle("Energy [GeV]");
	h_res->Draw("hist");
	
	if(do_print) can->Print("reconstructed_energy.png","png");
}