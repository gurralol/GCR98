#include "filter_faceswap.h"

#include <chrono>
#include <fstream>
#include <numeric>

FilterFaceswap::FilterFaceswap(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	PanelFilter(parent, id, pos, size)
{
	textTitle->SetLabel("Faceswap");

	boxSizerProvider = new wxBoxSizer(wxHORIZONTAL);
	boxSizerDetectionThreshold = new wxBoxSizer(wxHORIZONTAL);
	boxSizerSimilarityThreshold = new wxBoxSizer(wxHORIZONTAL);
	boxSizerAutoSwap = new wxBoxSizer(wxHORIZONTAL);
	boxSizerFacesTargetCtrls = new wxBoxSizer(wxHORIZONTAL);
	boxSizerFacesSourceCtrls = new wxBoxSizer(wxHORIZONTAL);

	textProvider = new wxStaticText(panelContent, wxID_ANY, "Provider", wxDefaultPosition, wxSize(72, 24));
	textDetectionThreshold = new wxStaticText(panelContent, wxID_ANY, "Detection Thresh", wxDefaultPosition, wxSize(112, 24));
	textSimilarityThreshold = new wxStaticText(panelContent, wxID_ANY, "Similarity Thresh", wxDefaultPosition, wxSize(112, 24));
	textAutoSwap = new wxStaticText(panelContent, wxID_ANY, "Auto-swap", wxDefaultPosition, wxSize(112, 24));
	textFacesTarget = new wxStaticText(panelContent, wxID_ANY, "Targets", wxDefaultPosition, wxSize(72, 24));
	textFacesSource = new wxStaticText(panelContent, wxID_ANY, "Sources", wxDefaultPosition, wxSize(72, 24));

	wxString arrProvider[] = {"CPU", "CUDA", "TensorRT"};
	choiceProvider = new wxChoice(panelContent, wxID_ANY, wxDefaultPosition, wxSize(72, 24), 3, arrProvider);
	choiceProvider->SetSelection(1);

	sliderDetectionThreshold = new wxSlider(panelContent, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxSize(92, 32));
	sliderSimilarityThreshold = new wxSlider(panelContent, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxSize(92, 32));

	checkBoxAutoSwap = new wxCheckBox(panelContent, wxID_ANY, "", wxDefaultPosition, wxSize(32, 32));

	btnFindFaces = new wxButton(panelContent, wxID_ANY, "Find faces", wxDefaultPosition, wxSize(72, 24));
	btnLoadFaces = new wxButton(panelContent, wxID_ANY, "Load faces", wxDefaultPosition, wxSize(72, 24));

	scrollWindowFacesTarget = new wxScrolledWindow(panelContent, wxID_ANY, wxDefaultPosition, wxSize(50, 58));
	scrollWindowFacesSource = new wxScrolledWindow(panelContent, wxID_ANY, wxDefaultPosition, wxSize(50, 58));
	scrollWindowFacesTarget->SetScrollRate(5, 5);
	scrollWindowFacesSource->SetScrollRate(5, 5);

	gridSizerFacesTarget = new wxGridSizer(10, 0, 0);
	gridSizerFacesSource = new wxGridSizer(10, 0, 0);

	scrollWindowFacesTarget->SetSizer(gridSizerFacesTarget);
	scrollWindowFacesSource->SetSizer(gridSizerFacesSource);

	for (int i = 0; i < 10; i++)
	{
		wxButton* btnT = new wxButton(scrollWindowFacesTarget, wxID_ANY, "faceT", wxDefaultPosition, wxSize(40, 40));
		gridSizerFacesTarget->Add(btnT, 0, wxALIGN_CENTER);

		wxButton* btnS = new wxButton(scrollWindowFacesSource, wxID_ANY, "faceS", wxDefaultPosition, wxSize(40, 40));
		gridSizerFacesSource->Add(btnS, 0, wxALIGN_CENTER);
	}

	boxSizerProvider->Add(textProvider, 0);
	boxSizerProvider->AddStretchSpacer();
	boxSizerProvider->Add(choiceProvider, 0);

	boxSizerDetectionThreshold->Add(textDetectionThreshold, 0);
	boxSizerDetectionThreshold->AddStretchSpacer();
	boxSizerDetectionThreshold->Add(sliderDetectionThreshold, 0);

	boxSizerSimilarityThreshold->Add(textSimilarityThreshold, 0);
	boxSizerSimilarityThreshold->AddStretchSpacer();
	boxSizerSimilarityThreshold->Add(sliderSimilarityThreshold, 0);

	boxSizerAutoSwap->Add(textAutoSwap, 0);
	boxSizerAutoSwap->AddStretchSpacer();
	boxSizerAutoSwap->Add(checkBoxAutoSwap, 0);

	boxSizerFacesTargetCtrls->Add(textFacesTarget, 0);
	boxSizerFacesTargetCtrls->AddStretchSpacer();
	boxSizerFacesTargetCtrls->Add(btnFindFaces, 0);

	boxSizerFacesSourceCtrls->Add(textFacesSource, 0);
	boxSizerFacesSourceCtrls->AddStretchSpacer();
	boxSizerFacesSourceCtrls->Add(btnLoadFaces, 0);

	boxSizerContent->Add(boxSizerProvider, 0, wxEXPAND);

	boxSizerContent->Add(boxSizerDetectionThreshold, 0, wxEXPAND);
	boxSizerContent->Add(boxSizerSimilarityThreshold, 0, wxEXPAND);

	boxSizerContent->Add(boxSizerAutoSwap, 0, wxEXPAND);

	boxSizerContent->Add(boxSizerFacesTargetCtrls, 0, wxEXPAND);
	boxSizerContent->Add(scrollWindowFacesTarget, 0, wxEXPAND);

	boxSizerContent->Add(boxSizerFacesSourceCtrls, 0, wxEXPAND);
	boxSizerContent->Add(scrollWindowFacesSource, 0, wxEXPAND);

	checkBoxAutoSwap->Bind(wxEVT_CHECKBOX, &FilterFaceswap::BtnAutoSwap, this);
	btnFindFaces->Bind(wxEVT_BUTTON, &FilterFaceswap::BtnFindFaces, this);
	btnLoadFaces->Bind(wxEVT_BUTTON, &FilterFaceswap::BtnLoadFaces, this);

	envDet10g = nullptr;
	sessionOptsDet10g = nullptr;
	sessionDet10g = nullptr;

	envW600k = nullptr;
	sessionOptsW600k = nullptr;
	sessionW600k = nullptr;

	envInswapper128 = nullptr;
	sessionOptsInswapper128 = nullptr;
	sessionInswapper128 = nullptr;

	InitDet10g();
	InitW600k();
	InitInswapper128();
}

FilterFaceswap::~FilterFaceswap()
{
}

wxImage FilterFaceswap::ApplyFilter(wxImage img)
{
	// Find faces in current frame
	cv::Mat mat = ImgUtils::cvFromWx(img);
	std::vector<bBox> faces = Det10g(mat);
	if (faces.empty()) return img;

	for (auto& foundFace : faces)
	{
		// Clean aligned crop
		int tempSizeX = foundFace.x2 - foundFace.x1;
		int tempSizeY = foundFace.y2 - foundFace.y1;

		bBox bBoxTemp;
		bBoxTemp.x1 = foundFace.x1 - (tempSizeX / 2);
		bBoxTemp.y1 = foundFace.y1 - (tempSizeY / 2);
		bBoxTemp.x2 = foundFace.x2 + (tempSizeX / 2);
		bBoxTemp.y2 = foundFace.y2 + (tempSizeY / 2);

		cv::Mat processed = CropFace(bBoxTemp, mat);

		double dy = foundFace.bY - foundFace.aY;
		double dx = foundFace.bX - foundFace.aX;
		double angle = atan2(dy, dx) * 180.0 / CV_PI;
		cv::Point2f center(processed.cols / 2, processed.rows / 2);
		cv::Mat rotMat = cv::getRotationMatrix2D(center, angle, 1.0);
		cv::warpAffine(processed, processed, rotMat, processed.size(), cv::INTER_CUBIC);

		cv::Rect cropRect(tempSizeX / 2, tempSizeY / 2, tempSizeX, tempSizeY);
		processed = processed(cropRect);

		std::vector<float> embeddingFound = W600k(processed);

		if (checkBoxAutoSwap->GetValue() == 1)
		{
			if (embeddingAutoSwap.empty()) return img;

			cv::Mat swap = Inswapper128(processed, embeddingAutoSwap);

			cv::resize(swap, swap, cv::Size(foundFace.x2 - foundFace.x1, foundFace.y2 - foundFace.y1));

			//		Paste back		//
			// Ensure swap has alpha
			cv::Mat swap_bgra;
			if (swap.channels() == 3)
				cv::cvtColor(swap, swap_bgra, cv::COLOR_BGR2BGRA);
			else
				swap_bgra = swap.clone();

			// Rotate with transparent background
			cv::Point2f center2(swap_bgra.cols / 2.0f, swap_bgra.rows / 2.0f);
			cv::Mat rotMat2 = cv::getRotationMatrix2D(center2, -angle, 1.0);
			cv::Scalar transparent(0, 0, 0, 0);
			cv::warpAffine(swap_bgra, swap_bgra, rotMat2, swap_bgra.size(), cv::INTER_CUBIC, cv::BORDER_CONSTANT, transparent);

			// Ensure mat has alpha channel
			if (mat.channels() == 3)
				cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);

			// Intended ROI (may go out of bounds)
			cv::Rect dst_roi(foundFace.x1, foundFace.y1, swap_bgra.cols, swap_bgra.rows);

			// Image bounds
			cv::Rect dst_bounds(0, 0, mat.cols, mat.rows);

			// Intersection of destination ROI and bounds
			cv::Rect clipped_dst_roi = dst_roi & dst_bounds;

			if (clipped_dst_roi.area() > 0) {
				// Calculate corresponding region from source image (swap_bgra)
				int offset_x = clipped_dst_roi.x - dst_roi.x;
				int offset_y = clipped_dst_roi.y - dst_roi.y;
				cv::Rect src_roi(offset_x, offset_y, clipped_dst_roi.width, clipped_dst_roi.height);

				cv::Mat src_region = swap_bgra(src_roi);
				cv::Mat dst_region = mat(clipped_dst_roi);

				// Alpha blending
				for (int y = 0; y < src_region.rows; ++y) {
					for (int x = 0; x < src_region.cols; ++x) {
						const cv::Vec4b& fg = src_region.at<cv::Vec4b>(y, x);
						cv::Vec4b& bg = dst_region.at<cv::Vec4b>(y, x);
						float alpha = fg[3] / 255.0f;
						for (int c = 0; c < 3; ++c) {
							bg[c] = static_cast<uchar>(fg[c] * alpha + bg[c] * (1.0f - alpha));
						}
						bg[3] = std::max(bg[3], fg[3]);  // optional
					}
				}
			}
		}
		else
		{
			if (arrFacesTarget.empty())
			{
				return img;
			}

			for (auto& targetFace : arrFacesTarget)
			{
				float dot = std::inner_product(embeddingFound.begin(), embeddingFound.end(), targetFace.embeddingSelf.begin(), 0.0f);
				float simValue = static_cast<float>(sliderSimilarityThreshold->GetValue()) / 100;

				if (dot > simValue)
				{
					cv::Mat swap = Inswapper128(processed, targetFace.embeddingSrc);

					cv::resize(swap, swap, cv::Size(foundFace.x2 - foundFace.x1, foundFace.y2 - foundFace.y1));

					//		Paste back		//
					// Ensure swap has alpha
					cv::Mat swap_bgra;
					if (swap.channels() == 3)
						cv::cvtColor(swap, swap_bgra, cv::COLOR_BGR2BGRA);
					else
						swap_bgra = swap.clone();

					// Rotate with transparent background
					cv::Point2f center2(swap_bgra.cols / 2.0f, swap_bgra.rows / 2.0f);
					cv::Mat rotMat2 = cv::getRotationMatrix2D(center2, -angle, 1.0);
					cv::Scalar transparent(0, 0, 0, 0);
					cv::warpAffine(swap_bgra, swap_bgra, rotMat2, swap_bgra.size(), cv::INTER_CUBIC, cv::BORDER_CONSTANT, transparent);

					// Ensure mat has alpha channel
					if (mat.channels() == 3)
						cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);

					// Intended ROI (may go out of bounds)
					cv::Rect dst_roi(foundFace.x1, foundFace.y1, swap_bgra.cols, swap_bgra.rows);

					// Image bounds
					cv::Rect dst_bounds(0, 0, mat.cols, mat.rows);

					// Intersection of destination ROI and bounds
					cv::Rect clipped_dst_roi = dst_roi & dst_bounds;

					if (clipped_dst_roi.area() > 0) {
						// Calculate corresponding region from source image (swap_bgra)
						int offset_x = clipped_dst_roi.x - dst_roi.x;
						int offset_y = clipped_dst_roi.y - dst_roi.y;
						cv::Rect src_roi(offset_x, offset_y, clipped_dst_roi.width, clipped_dst_roi.height);

						cv::Mat src_region = swap_bgra(src_roi);
						cv::Mat dst_region = mat(clipped_dst_roi);

						// Alpha blending
						for (int y = 0; y < src_region.rows; ++y) {
							for (int x = 0; x < src_region.cols; ++x) {
								const cv::Vec4b& fg = src_region.at<cv::Vec4b>(y, x);
								cv::Vec4b& bg = dst_region.at<cv::Vec4b>(y, x);
								float alpha = fg[3] / 255.0f;
								for (int c = 0; c < 3; ++c) {
									bg[c] = static_cast<uchar>(fg[c] * alpha + bg[c] * (1.0f - alpha));
								}
								bg[3] = std::max(bg[3], fg[3]);  // optional
							}
						}
					}
				}
			}
		}
	}
	wxImage result = ImgUtils::wxFromCv(mat);

	return result;
}

void FilterFaceswap::BtnAutoSwap(wxCommandEvent& event)
{
	if (checkBoxAutoSwap->GetValue() == 1)
	{
		textFacesTarget->Disable();
		btnFindFaces->Disable();
		scrollWindowFacesTarget->Disable();
	}
	else
	{
		textFacesTarget->Enable();
		btnFindFaces->Enable();
		scrollWindowFacesTarget->Enable();
	}
}

void FilterFaceswap::BtnFindFaces(wxCommandEvent& event)
{
	// Return if no media is playing
	if (!currFrame->IsOk()) return;

	cv::Mat cvFrame = ImgUtils::cvFromWx(currFrame->Copy());
	if (cvFrame.empty()) return;

	// Find faces
	std::vector<bBox> bBoxes = Det10g(cvFrame);
	if (bBoxes.empty()) return;

	// Clear target faces
	gridSizerFacesTarget->Clear(true);
	btnSelectedTarget = nullptr;
	arrFacesTarget.clear();

	// For every detected face...
	for (int i = 0; i < bBoxes.size(); i++)
	{
		// Crop it out
		cv::Mat crop = CropFace(bBoxes[i], cvFrame);
		if (crop.empty()) continue;

		// Create an embedding for it
		std::vector<float> embedding = W600k(crop);

		targetBtn targetBtnTemp;
		targetBtnTemp.embeddingSelf = embedding;
		arrFacesTarget.push_back(targetBtnTemp);

		size_t targetIndex = arrFacesTarget.size() - 1;

		//targetBtn* targetBtnPointer = &arrFacesTarget.back();

		// Create thumbnail for button
		wxImage thumb = ImgUtils::wxFromCv(crop);
		thumb = ImgUtils::Resize_Fit(thumb, 32, 32);
		ImgUtils::Borders_Rounded(thumb, 1);

		// Create button
		wxToggleButton* btn = new wxToggleButton(scrollWindowFacesTarget, wxID_ANY, "", wxDefaultPosition, wxSize(40, 40));
		btn->SetBitmap(wxBitmap(thumb));
		//btn->SetClientData(targetBtnPointer);
		btn->SetClientData(reinterpret_cast<void*>(targetIndex));

		// Create button binding
		btn->Bind(wxEVT_TOGGLEBUTTON, [this, btn](wxCommandEvent& event) {
			if (btnSelectedTarget != nullptr)
			{
				if (btn != btnSelectedTarget)
				{
					btnSelectedTarget->SetValue(false);
				}
			}
			btnSelectedTarget = btn;

			// Deselect all gridSizerSource buttons
			auto& list = gridSizerFacesSource->GetChildren();
			for (auto& item : list)
			{
				if (item->IsWindow())
				{
					wxWindow* win = item->GetWindow();
					if (auto button = dynamic_cast<wxToggleButton*>(win))
					{
						button->SetValue(false);
					}
				}
			}

			// Select all src buttons in the targets struct.
			size_t index = reinterpret_cast<size_t>(btn->GetClientData());
			targetBtn& btnStruct = arrFacesTarget[index];

			for (int i = 0; i < btnStruct.arrSrcBtns.size(); i++)
			{
				btnStruct.arrSrcBtns[i]->SetValue(true);
			}

			event.Skip();
			});

		// Add button to grid
		gridSizerFacesTarget->Add(btn, 0);
	}

	panelContent->Layout();
	event.Skip();
}

void FilterFaceswap::BtnLoadFaces(wxCommandEvent& event)
{
	wxDir dir = wxDirSelector();
	if (!dir.IsOpened()) return;

	wxArrayString files;
	dir.GetAllFiles(dir.GetName(), &files);

	gridSizerFacesSource->Clear(true);
	arrFacesSource.clear();

	for (int i = 0; i < files.GetCount(); i++)
	{
		cv::Mat img = cv::imread(files[i].utf8_string()); // Read image
		if (img.empty()) continue;

		std::vector<bBox> bBoxes = Det10g(img); // Find faces
		if (bBoxes.empty()) continue;

		for (int j = 0; j < bBoxes.size(); j++)
		{
			cv::Mat crop = CropFace(bBoxes[j], img); // Crop the first one
			if (crop.empty()) continue;

			std::vector<float> embedding = W600k(crop); // Create embedding from cropped

			face faceTemp;
			faceTemp.bBox.x1 = bBoxes[j].x1;
			faceTemp.bBox.y1 = bBoxes[j].y1;
			faceTemp.bBox.x2 = bBoxes[j].x2;
			faceTemp.bBox.y2 = bBoxes[j].y2;
			faceTemp.embedding = embedding;
			faceTemp.crop = crop;
			arrFacesSource.push_back(faceTemp);
			face* storedFace = &arrFacesSource.back();
			size_t faceIndex = arrFacesSource.size() - 1;

			wxImage thumb = ImgUtils::wxFromCv(crop);

			thumb = ImgUtils::Resize_Fit(thumb, 32, 32);
			ImgUtils::Borders_Rounded(thumb, 1);
			
			wxToggleButton* btn = new wxToggleButton(scrollWindowFacesSource, wxID_ANY, "", wxDefaultPosition, wxSize(40, 40));
			btn->SetBitmap(wxBitmap(thumb));
			//btn->SetClientData(storedFace);
			btn->SetClientData(reinterpret_cast<void*>(faceIndex));

			btn->Bind(wxEVT_TOGGLEBUTTON, [this, btn, storedFace](wxCommandEvent& event) {
				
				std::vector<float> embTemp(512);
				int srcFacesSelected = 0;
				auto& list = gridSizerFacesSource->GetChildren();
				for (auto& item : list)
				{
					if (item->IsWindow())
					{
						wxWindow* win = item->GetWindow();
						if (auto button = dynamic_cast<wxToggleButton*>(win))
						{
							if (button->GetValue() == 1)
							{
								size_t idx = reinterpret_cast<size_t>(button->GetClientData());
								face* faceTemp = &arrFacesSource[idx];

								for (int i = 0; i < faceTemp->embedding.size(); i++)
								{
									embTemp[i] += faceTemp->embedding[i];
								}
								srcFacesSelected++;
							}
						}
					}
				}
				if (srcFacesSelected > 0)
				{
					for (int i = 0; i < embTemp.size(); i++)
					{
						embTemp[i] /= srcFacesSelected;
					}
					embeddingAutoSwap = embTemp;
				}
				

				if (checkBoxAutoSwap->GetValue() == 1)
				{
					event.Skip();
					return;
				}

				if (btnSelectedTarget == nullptr)
				{
					return;
				}

				size_t index = reinterpret_cast<size_t>(btnSelectedTarget->GetClientData());
				targetBtn& btnStruct = arrFacesTarget[index];
				
				// Check if button exists in target struct
				bool btnExists = false;
				int existingIdx = -1;
				for (int i = 0; i < btnStruct.arrSrcBtns.size(); i++)
				{
					if (btnStruct.arrSrcBtns[i] == btn)
					{
						btnExists = true;
						existingIdx = i;
						break;
					}
				}

				if (btn->GetValue())
				{
					if (!btnExists)
					{
						btnStruct.arrSrcBtns.push_back(btn);
					}
				}
				else
				{
					if (btnExists)
					{
						btnStruct.arrSrcBtns.erase(btnStruct.arrSrcBtns.begin() + existingIdx);
					}
				}

				std::vector<float> embeddingTemp(512, 0.0f);
				for (int i = 0; i < btnStruct.arrSrcBtns.size(); i++)
				{
					size_t idx = reinterpret_cast<size_t>(btnStruct.arrSrcBtns[i]->GetClientData());
					face* faceTemp = &arrFacesSource[idx];

					for (int j = 0; j < faceTemp->embedding.size(); j++)
					{
						embeddingTemp[j] += faceTemp->embedding[j];
					}
				}

				for (int i = 0; i < embeddingTemp.size(); i++)
				{
					embeddingTemp[i] /= static_cast<float>(btnStruct.arrSrcBtns.size());
				}

				btnStruct.embeddingSrc = embeddingTemp;
				
				event.Skip();
				});

			gridSizerFacesSource->Add(btn, 0);
		}
	}

	panelContent->Layout();
	event.Skip();
}

cv::Mat FilterFaceswap::CropFace(bBox bBox, cv::Mat img)
{
	int cropWidth = bBox.x2 - bBox.x1;   // width
	int cropHeight = bBox.y2 - bBox.y1;  // height

	int x1 = bBox.x1;
	int y1 = bBox.y1;

	// Create black image of the desired crop size
	cv::Mat cropped = cv::Mat::zeros(cropHeight, cropWidth, img.type());

	// Calculate source ROI (intersection with image)
	int x_src_start = std::max(x1, 0);
	int y_src_start = std::max(y1, 0);
	int x_src_end = std::min(x1 + cropWidth, img.cols);
	int y_src_end = std::min(y1 + cropHeight, img.rows);

	if (x_src_start >= x_src_end || y_src_start >= y_src_end)
		return cropped; // bbox completely outside

	// Destination start (in black image)
	int x_dst_start = x_src_start - x1;
	int y_dst_start = y_src_start - y1;

	cv::Rect src_roi(x_src_start, y_src_start, x_src_end - x_src_start, y_src_end - y_src_start);
	cv::Rect dst_roi(x_dst_start, y_dst_start, x_src_end - x_src_start, y_src_end - y_src_start);

	img(src_roi).copyTo(cropped(dst_roi));

	return cropped;
}

// Inference
std::vector<float> FilterFaceswap::W600k(cv::Mat input)
{
	cv::Mat resized;
	cv::resize(input, resized, cv::Size(112, 112));
	
	//resized.convertTo(resized, CV_32F, 1.0 / 127.5, -1.0);
	resized.convertTo(resized, CV_32F, 1.0 / 255, -1.0);

	std::vector<cv::Mat> channels(3);
	cv::split(resized, channels);

	std::vector<float> inputVector;
	for (const auto& channel : channels) {
		inputVector.insert(inputVector.end(), (float*)channel.datastart, (float*)channel.dataend);
	}

	std::vector<int64_t> inputDims = { 1,3,112,112 };

	Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
	Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
		memoryInfo, inputVector.data(), inputVector.size(), inputDims.data(), inputDims.size());
	
	const char* inputName = "input.1";
	const char* outputName = "683";
	std::vector<const char*> inputNames(1, nullptr);
	std::vector<const char*> outputNames(1, nullptr);
	inputNames[0] = inputName;
	outputNames[0] = outputName;
	std::array<Ort::Value, 1> inputs = { std::move(inputTensor) };

	auto outputTensors = sessionW600k->Run(
		Ort::RunOptions{ nullptr }, &inputNames[0], inputs.data(), inputs.size(), &outputNames[0], 1);

	float* outputData = outputTensors[0].GetTensorMutableData<float>();
	std::vector<int64_t> outputDims = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
	size_t outputSize = outputDims[1];

	std::vector<float> result(512);
	for (int i = 0; i < outputSize; i++)
	{
		result[i] = outputData[i];
	}
	Eigen::VectorXf resultEig(512);
	for (int i = 0; i < outputSize; i++)
	{
		resultEig(i) = result[i];
	}

	Eigen::VectorXf n_e = resultEig / resultEig.norm();
	Eigen::RowVectorXf n_e_row = n_e.transpose();
	Eigen::RowVectorXf latent = n_e_row * *emap;

	resultEig = latent / latent.norm();
	for (int i = 0; i < outputSize; i++)
	{
		result[i] = resultEig(i);
	}

	return result;
}

std::vector<FilterFaceswap::bBox> FilterFaceswap::Det10g(cv::Mat input)
{
	cv::Mat resized = ImgUtils::ResizeCvMat(input, 640, 640);

	cv::Mat background = cv::Mat::zeros(cv::Size(640, 640), CV_32FC3);

	int posX = (640 - resized.cols) / 2;
	int posY = (640 - resized.rows) / 2;

	cv::Rect roi(posX, posY, resized.cols, resized.rows);

	resized.copyTo(background(roi));

	cv::Mat original = background.clone();

	background.convertTo(background, CV_32FC3, 1.0 / 255.0, -1.0);

	std::vector<float> inputVector(3 * 640 * 640);
	
	for (int c = 0; c < 3; c++)
	{
		for (int h = 0; h < 640; h++)
		{
			for (int w = 0; w < 640; w++)
			{
				float pixVal = background.at<cv::Vec3f>(h, w)[c];
				inputVector[c * 640 * 640 + h * 640 + w] = pixVal;
			}
		}
	}

	std::vector<int64_t> inputDims = { 1, 3, 640, 640 };
	Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
	Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
		memoryInfo, inputVector.data(), inputVector.size(), inputDims.data(), inputDims.size());

	const char* inputName = "input.1";
	const char* outputName0 = "448";
	const char* outputName1 = "471";
	const char* outputName2 = "494";
	const char* outputName3 = "451";
	const char* outputName4 = "474";
	const char* outputName5 = "497";
	const char* outputName6 = "454";
	const char* outputName7 = "477";
	const char* outputName8 = "500";

	std::vector<const char*> inputNames(1, nullptr);
	std::vector<const char*> outputNames(9, nullptr);
	inputNames[0] = inputName;
	outputNames[0] = outputName0;
	outputNames[1] = outputName1;
	outputNames[2] = outputName2;
	outputNames[3] = outputName3;
	outputNames[4] = outputName4;
	outputNames[5] = outputName5;
	outputNames[6] = outputName6;
	outputNames[7] = outputName7;
	outputNames[8] = outputName8;

	auto outputTensors = sessionDet10g->Run(
		Ort::RunOptions{ nullptr }, &inputNames[0], &inputTensor, inputNames.size(), &outputNames[0], 9);

	std::vector<bBox> arr_bBoxes;
	std::vector<anchorBox> arr_anchorBoxes;

	// Stepsize 8
	float* conf = outputTensors[0].GetTensorMutableData<float>();
	float* bBoxes = outputTensors[3].GetTensorMutableData<float>();
	float* landmarks = outputTensors[6].GetTensorMutableData<float>();
	size_t numDets = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape()[0];

	auto CreateAnchors = [&](std::vector<anchorBox>& arrAnchors, float step, std::vector<std::vector<float>> fMap) {
		int gridSize = 640 / step;

		for (int i = 0; i < gridSize; i++)
		{
			for (int j = 0; j < gridSize; j++)
			{
				for (auto& ref : fMap)
				{
					float cx = j * step;
					float cy = i * step;
					float sx = ref[0];
					float sy = ref[0];
					anchorBox aBox = { cx, cy, sx, sy };
					arrAnchors.push_back(aBox);
				}
			}
		}
	};

	auto SaveDetections = [&](int numDets, float confidence, float step) {
		for (int i = 0; i < numDets; i++)
		{
			if (conf[i] > confidence)
			{
				float ax1 = arr_anchorBoxes[i].cx;
				float ay1 = arr_anchorBoxes[i].cy;
				float x1 = bBoxes[i * 4 + 0] * step;
				float y1 = bBoxes[i * 4 + 1] * step;
				float x2 = bBoxes[i * 4 + 2] * step;
				float y2 = bBoxes[i * 4 + 3] * step;
				
				float rx1 = ax1 - x1;
				float ry1 = ay1 - y1;
				float rx2 = ax1 + x2;
				float ry2 = ay1 + y2;

				float aX = ax1 + (landmarks[i * 10 + 0] * step);
				float aY = ay1 + (landmarks[i * 10 + 1] * step);
				float bX = ax1 + (landmarks[i * 10 + 2] * step);
				float bY = ay1 + (landmarks[i * 10 + 3] * step);
				float cX = ax1 + (landmarks[i * 10 + 4] * step);
				float cY = ay1 + (landmarks[i * 10 + 5] * step);
				float dX = ax1 + (landmarks[i * 10 + 6] * step);
				float dY = ay1 + (landmarks[i * 10 + 7] * step);
				float eX = ax1 + (landmarks[i * 10 + 8] * step);
				float eY = ay1 + (landmarks[i * 10 + 9] * step);

				bBox box;
				box.score = conf[i];
				box.x1 = rx1;
				box.y1 = ry1;
				box.x2 = rx2;
				box.y2 = ry2;

				box.aX = aX;
				box.aY = aY;
				box.bX = bX;
				box.bY = bY;
				box.cX = cX;
				box.cY = cY;
				box.dX = dX;
				box.dY = dY;
				box.eX = eX;
				box.eY = eY;

				arr_bBoxes.push_back(box);
			}
		}
	};

	std::vector<std::vector<float>> fmap8 = { {-8.0, -8.0, 23.0, 23.0}, {0.0, 0.0, 15.0, 15.0} };
	std::vector<std::vector<float>> fmap16 = { {-56.0, -56.0, 71.0, 71.0}, {-24.0, -24.0, 39.0, 39.0} };
	std::vector<std::vector<float>> fmap32 = { {-248.0, -248.0, 263.0, 263.0}, {-120.0, -120.0, 135.0, 135.0} };

	float detValue = static_cast<float>(sliderDetectionThreshold->GetValue()) / 100;

	CreateAnchors(arr_anchorBoxes, 8, fmap8);
	SaveDetections(arr_anchorBoxes.size(), detValue, 8);

	// Stepsize 16
	arr_anchorBoxes.clear();
	conf = outputTensors[1].GetTensorMutableData<float>();
	bBoxes = outputTensors[4].GetTensorMutableData<float>();
	landmarks = outputTensors[7].GetTensorMutableData<float>();
	numDets = outputTensors[1].GetTensorTypeAndShapeInfo().GetShape()[0];

	CreateAnchors(arr_anchorBoxes, 16, fmap16);
	SaveDetections(arr_anchorBoxes.size(), detValue, 16);

	// Stepsize 32
	arr_anchorBoxes.clear();
	conf = outputTensors[2].GetTensorMutableData<float>();
	bBoxes = outputTensors[5].GetTensorMutableData<float>();
	landmarks = outputTensors[8].GetTensorMutableData<float>();
	numDets = outputTensors[2].GetTensorTypeAndShapeInfo().GetShape()[0];

	CreateAnchors(arr_anchorBoxes, 32, fmap32);
	SaveDetections(arr_anchorBoxes.size(), detValue, 32);

	// Remove overlapping
	float threshold = 50.0f;
	for (size_t i = 0; i < arr_bBoxes.size(); i++)
	{
		for (size_t j = i + 1; j < arr_bBoxes.size(); j++)
		{
			float centX1 = (arr_bBoxes[i].x1 + arr_bBoxes[i].x2) * 0.5;
			float centY1 = (arr_bBoxes[i].y1 + arr_bBoxes[i].y2) * 0.5;

			float centX2 = (arr_bBoxes[j].x1 + arr_bBoxes[j].x2) * 0.5;
			float centY2 = (arr_bBoxes[j].y1 + arr_bBoxes[j].y2) * 0.5;

			float distance = std::sqrt(std::pow(centX1 - centX2, 2) + std::pow(centY1 - centY2, 2));

			if (distance < threshold)
			{
				arr_bBoxes.erase(arr_bBoxes.begin() + j);
				j--;
			}
		}
	}

	// Scale back coords to input image
	for (int i = 0; i < arr_bBoxes.size(); i++)
	{
		float input_w = static_cast<float>(input.cols);
		float input_h = static_cast<float>(input.rows);

		float resized_w = static_cast<float>(background.cols);
		float resized_h = static_cast<float>(background.rows);

		// Calculate scale and padding used during resize + padding
		float ar = std::min(resized_w / input_w, resized_h / input_h);

		int pad_w = (resized_w - static_cast<int>(input_w * ar)) / 2;
		int pad_h = (resized_h - static_cast<int>(input_h * ar)) / 2;

		// Remove padding, then scale back to original size
		float x1 = (arr_bBoxes[i].x1 - pad_w) / ar;
		float y1 = (arr_bBoxes[i].y1 - pad_h) / ar;
		float x2 = (arr_bBoxes[i].x2 - pad_w) / ar;
		float y2 = (arr_bBoxes[i].y2 - pad_h) / ar;

		float aX = (arr_bBoxes[i].aX - pad_w) / ar;
		float aY = (arr_bBoxes[i].aY - pad_h) / ar;
		float bX = (arr_bBoxes[i].bX - pad_w) / ar;
		float bY = (arr_bBoxes[i].bY - pad_h) / ar;
		float cX = (arr_bBoxes[i].cX - pad_w) / ar;
		float cY = (arr_bBoxes[i].cY - pad_h) / ar;
		float dX = (arr_bBoxes[i].dX - pad_w) / ar;
		float dY = (arr_bBoxes[i].dY - pad_h) / ar;
		float eX = (arr_bBoxes[i].eX - pad_w) / ar;
		float eY = (arr_bBoxes[i].eY - pad_h) / ar;

		float centX = (x1 + x2) * 0.5;
		float centY = (y1 + y2) * 0.5;

		float size = static_cast<int>(y2 - y1) * 1.25;

		float newX1 = centX - (size / 2);
		float newY1 = centY - (size / 2);

		float newX2 = newX1 + size;
		float newY2 = newY1 + size;

		arr_bBoxes[i].x1 = newX1;
		arr_bBoxes[i].y1 = newY1;
		arr_bBoxes[i].x2 = newX2;
		arr_bBoxes[i].y2 = newY2;

		arr_bBoxes[i].aX = aX;
		arr_bBoxes[i].aY = aY;
		arr_bBoxes[i].bX = bX;
		arr_bBoxes[i].bY = bY;
		arr_bBoxes[i].cX = cX;
		arr_bBoxes[i].cY = cY;
		arr_bBoxes[i].dX = dX;
		arr_bBoxes[i].dY = dY;
		arr_bBoxes[i].eX = eX;
		arr_bBoxes[i].eY = eY;
	}

	return arr_bBoxes;
}

cv::Mat FilterFaceswap::Inswapper128(cv::Mat input, std::vector<float> embedding)
{
	cv::Mat resized;
	cv::resize(input, resized, cv::Size(128, 128), 0, 0, cv::INTER_CUBIC);

	resized.convertTo(resized, CV_32F, 1.0 / 255);

	std::vector<cv::Mat> channels(3);
	cv::split(resized, channels);

	std::vector<float> inputVector;
	for (const auto& channel : channels) {
		inputVector.insert(inputVector.end(), (float*)channel.datastart, (float*)channel.dataend);
	}

	std::vector<int64_t> targetDims = { 1,3,128,128 };
	std::vector<int64_t> embeddingDims = { 1, 512 };

	Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
	Ort::Value inputTensor1 = Ort::Value::CreateTensor<float>(
		memoryInfo, inputVector.data(), inputVector.size(), targetDims.data(), targetDims.size());
	Ort::Value inputTensor2 = Ort::Value::CreateTensor<float>(
		memoryInfo, embedding.data(), embedding.size(), embeddingDims.data(), embeddingDims.size());

	const char* inputName1 = "target";
	const char* inputName2 = "source";
	const char* outputName = "output";
	std::vector<const char*> inputNames(2, nullptr);
	std::vector<const char*> outputNames(1, nullptr);
	inputNames[0] = inputName1;
	inputNames[1] = inputName2;
	outputNames[0] = outputName;
	std::array<Ort::Value, 2> inputs = { std::move(inputTensor1), std::move(inputTensor2) };

	auto outputTensors = sessionInswapper128->Run(
		Ort::RunOptions{ nullptr }, &inputNames[0], inputs.data(), inputs.size(), &outputNames[0], 1);

	float* outputData = outputTensors[0].GetTensorMutableData<float>();
	std::vector<int64_t> outputDims = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();

	int height = static_cast<int>(outputDims[2]);
	int width = static_cast<int>(outputDims[3]);

	cv::Mat outputImg(height, width, CV_32FC3);
	for (int c = 0; c < 3; ++c) {
		for (int h = 0; h < height; ++h) {
			for (int w = 0; w < width; ++w) {
				outputImg.at<cv::Vec3f>(h, w)[c] = outputData[c * height * width + h * width + w];
			}
		}
	}

	outputImg.convertTo(outputImg, CV_8UC3, 255.0);

	return outputImg;
}

// Memory management
void FilterFaceswap::InitW600k()
{
	if (envW600k == nullptr)
	{
		envW600k = new Ort::Env(ORT_LOGGING_LEVEL_ERROR, "w600k");
		sessionOptsW600k = new Ort::SessionOptions;
	}
	sessionOptsW600k->SetInterOpNumThreads(1);
	sessionOptsW600k->SetIntraOpNumThreads(1);
	sessionOptsW600k->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

	OrtCUDAProviderOptions cudaOptions;
	cudaOptions.device_id = 0;
	cudaOptions.cudnn_conv_algo_search = OrtCudnnConvAlgoSearchHeuristic;
	cudaOptions.arena_extend_strategy = 0;
	cudaOptions.do_copy_in_default_stream = 0;
	sessionOptsW600k->AppendExecutionProvider_CUDA(cudaOptions);

	std::string modelPath = "models/w600k_r50.onnx";
	std::basic_string<ORTCHAR_T> modelChar(modelPath.begin(), modelPath.end());

	if (sessionW600k == nullptr)
	{
		sessionW600k = new Ort::Session(*envW600k, modelChar.c_str(), *sessionOptsW600k);
	}

	std::ifstream file("models/emap", std::ios::binary);

	if (!file.is_open())
	{
		std::cout << "Unable to open emap." << std::endl;
	}

	int rows, cols;
	file.read(reinterpret_cast<char*>(&rows), sizeof(int));
	file.read(reinterpret_cast<char*>(&cols), sizeof(int));

	Eigen::MatrixXf matrix(rows, cols);
	file.read(reinterpret_cast<char*>(matrix.data()), rows * cols * sizeof(float));
	file.close();

	emap = new Eigen::MatrixXf(matrix);
}

void FilterFaceswap::InitDet10g()
{
	if (envDet10g == nullptr)
	{
		envDet10g = new Ort::Env(ORT_LOGGING_LEVEL_ERROR, "det10g");
		sessionOptsDet10g = new Ort::SessionOptions;
	}
	sessionOptsDet10g->SetInterOpNumThreads(1);
	sessionOptsDet10g->SetIntraOpNumThreads(1);
	sessionOptsDet10g->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
	
	OrtCUDAProviderOptions cudaOptions;
	cudaOptions.device_id = 0;
	cudaOptions.cudnn_conv_algo_search = OrtCudnnConvAlgoSearchHeuristic;
	cudaOptions.arena_extend_strategy = 0;
	cudaOptions.do_copy_in_default_stream = 0;
	sessionOptsDet10g->AppendExecutionProvider_CUDA(cudaOptions);

	std::string modelPath = "models/det_10g.onnx";
	std::basic_string<ORTCHAR_T> modelChar(modelPath.begin(), modelPath.end());

	if (sessionDet10g == nullptr)
	{
		sessionDet10g = new Ort::Session(*envDet10g, modelChar.c_str(), *sessionOptsDet10g);
	}
}

void FilterFaceswap::InitInswapper128()
{
	if (envInswapper128 == nullptr)
	{
		envInswapper128 = new Ort::Env(ORT_LOGGING_LEVEL_ERROR, "Inswapper128");
		sessionOptsInswapper128 = new Ort::SessionOptions;
	}
	sessionOptsInswapper128->SetInterOpNumThreads(1);
	sessionOptsInswapper128->SetIntraOpNumThreads(1);
	sessionOptsInswapper128->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

	OrtCUDAProviderOptions cudaOptions;
	cudaOptions.device_id = 0;
	cudaOptions.cudnn_conv_algo_search = OrtCudnnConvAlgoSearchHeuristic;
	cudaOptions.arena_extend_strategy = 0;
	cudaOptions.do_copy_in_default_stream = 0;
	sessionOptsInswapper128->AppendExecutionProvider_CUDA(cudaOptions);

	std::string modelPath = "models/inswapper_128.fp16.onnx";
	std::basic_string<ORTCHAR_T> modelChar(modelPath.begin(), modelPath.end());

	if (sessionInswapper128 == nullptr)
	{
		sessionInswapper128 = new Ort::Session(*envInswapper128, modelChar.c_str(), *sessionOptsInswapper128);
	}
}

void FilterFaceswap::ClearCache()
{
}