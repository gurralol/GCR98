#pragma once

#include <wx/wxprec.h>
#include <wx/dir.h>
#include <onnxruntime_cxx_api.h>
#include <Eigen/Dense>

#include "../gui/panel_filter.h"
#include "../utilities/img_utils.h"

class FilterFaceswap : public PanelFilter
{
public:
	FilterFaceswap(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size = wxSize(50, 400));
	~FilterFaceswap();

	// GUI
	wxBoxSizer* boxSizerProvider;
	wxBoxSizer* boxSizerDetectionThreshold;
	wxBoxSizer* boxSizerSimilarityThreshold;
	wxBoxSizer* boxSizerAutoSwap;
	wxBoxSizer* boxSizerFacesTargetCtrls;
	wxBoxSizer* boxSizerFacesSourceCtrls;

	wxStaticText* textProvider;
	wxStaticText* textDetectionThreshold;
	wxStaticText* textSimilarityThreshold;
	wxStaticText* textAutoSwap;
	wxStaticText* textFacesTarget;
	wxStaticText* textFacesSource;

	wxChoice* choiceProvider;
	wxSlider* sliderDetectionThreshold;
	wxSlider* sliderSimilarityThreshold;
	wxCheckBox* checkBoxAutoSwap;
	wxButton* btnFindFaces;
	wxButton* btnLoadFaces;

	wxScrolledWindow* scrollWindowFacesTarget;
	wxScrolledWindow* scrollWindowFacesSource;

	wxGridSizer* gridSizerFacesTarget;
	wxGridSizer* gridSizerFacesSource;

	// Variables
	Ort::Env* envDet10g;
	Ort::SessionOptions* sessionOptsDet10g;
	Ort::Session* sessionDet10g;

	Ort::Env* envW600k;
	Ort::SessionOptions* sessionOptsW600k;
	Ort::Session* sessionW600k;

	Ort::Env* envInswapper128;
	Ort::SessionOptions* sessionOptsInswapper128;
	Ort::Session* sessionInswapper128;

	Eigen::MatrixXf* emap;

	struct anchorBox
	{
		float cx, cy, sx, sy;
	};

	struct bBox
	{
		float score;
		int x1;
		int y1;
		int x2;
		int y2;

		int aX;
		int aY;
		int bX;
		int bY;
		int cX;
		int cY;
		int dX;
		int dY;
		int eX;
		int eY;
	};

	struct face
	{
		bBox bBox;
		std::vector<wxToggleButton*> btnBindings;
		std::vector<float> embedding;
		cv::Mat crop;
	};

	struct targetBtn
	{
		std::vector<float> embeddingSelf;
		std::vector<wxToggleButton*> arrSrcBtns;
		std::vector<float> embeddingSrc;
	};

	// Variables
	std::vector<targetBtn> arrFacesTarget;
	std::vector<face> arrFacesSource;
	wxImage* currFrame;

	wxToggleButton* btnSelectedTarget = nullptr;
	std::vector<float> embeddingAutoSwap;

	// Override
	wxImage ApplyFilter(wxImage img) override;

	// Buttons
	void BtnAutoSwap(wxCommandEvent& event);
	void BtnFindFaces(wxCommandEvent& event);
	void BtnLoadFaces(wxCommandEvent& event);

	// Utilities
	void FindFaces();
	void CompareFaces();
	void SwapFaces();

	cv::Mat CropFace(bBox bBox, cv::Mat img);
	
	// Backend
	std::vector<float> W600k(cv::Mat input);
	std::vector<bBox> Det10g(cv::Mat input);
	cv::Mat Inswapper128(cv::Mat input, std::vector<float> embedding);

	void InitW600k();
	void InitDet10g();
	void InitInswapper128();

	void ClearCache();
};