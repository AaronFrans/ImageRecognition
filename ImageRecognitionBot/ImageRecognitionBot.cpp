#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>


#include <iostream>
#include <string>
#include <atlstr.h>


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define TESTING

#ifdef TESTING

//#define RECT
#ifdef RECT
void RectangleTest()
{

	std::string image_path = "C:\\Users\\azdef\\OneDrive\\Pictures\\Complete.jpg";
	std::string image_path2 = "C:\\Users\\azdef\\OneDrive\\Pictures\\Cropped.jpg";

	Mat image = imread(image_path, IMREAD_COLOR);
	Mat image2 = imread(image_path2, IMREAD_COLOR);

	Mat results{};
	matchTemplate(image, image2, results, TM_CCORR_NORMED);


	cv::Point min_Loc(0, 0), max_Loc(0, 0);

	double max_Val;

	minMaxLoc(results, nullptr, &max_Val, &min_Loc, &max_Loc);

	std::cout << "MaxLoc: " << max_Loc << std::endl;
	std::cout << "MaxVal: " << max_Val << std::endl;

	int w = image2.size[1];
	int h = image2.size[0];

	rectangle(image, max_Loc, cv::Point{ max_Loc.x + w, max_Loc.y + h }, { 0,255,255 });

	imshow("Vision", image);
	waitKey(0);
}
#endif // RECT


#define WINDOW_CAPTURE
#ifdef WINDOW_CAPTURE

#endif // WINDOW_CAPTURE


#endif // TESTING


#pragma region Functions
constexpr int NR_OF_IMAGES{ 5 };
constexpr float MIN_TRESHOLD{ 0.99f };


void SetupImages(std::vector<cv::Mat>& toFindImages)
{
	toFindImages.reserve(NR_OF_IMAGES);
	std::string imageFolderPath;
	do {
		std::cout << "Enter the path to the folder with enemies: ";
		std::getline(std::cin, imageFolderPath);

		// Validate the path (you may want to add more sophisticated validation)
		if (imageFolderPath.length() == 0 || GetFileAttributesA(imageFolderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
			std::cout << "Entered path: " << imageFolderPath << std::endl;
			std::cout << "Invalid path. Try again.\n";
		}

	} while (GetFileAttributesA(imageFolderPath.c_str()) == INVALID_FILE_ATTRIBUTES);


	for (int i = 1; i <= NR_OF_IMAGES; i++)
	{
		cv::Mat image = cv::imread(imageFolderPath + '\\' + std::to_string(i) + ".png", CV_8UC4);

		cv::resize(image, image, { image.size[1] * 8, image.size[0] * 8 });
		toFindImages.emplace_back(image);
	}

}


void TakeScreenshot(cv::Mat& mat)
{
	auto w = GetSystemMetrics(SM_CXFULLSCREEN);
	auto h = GetSystemMetrics(SM_CYFULLSCREEN);
	auto hdc = GetDC(HWND_DESKTOP);
	auto hbitmap = CreateCompatibleBitmap(hdc, w, h);
	auto memdc = CreateCompatibleDC(hdc);
	auto oldbmp = SelectObject(memdc, hbitmap);
	BitBlt(memdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

	mat.create(h, w, CV_8UC4);
	BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
	GetDIBits(hdc, hbitmap, 0, h, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	SelectObject(memdc, oldbmp);
	DeleteDC(memdc);
	DeleteObject(hbitmap);
	ReleaseDC(HWND_DESKTOP, hdc);
}




#pragma endregion

int main()
{
	using namespace cv;

#ifdef TESTING

#ifdef RECT
	RectangleTest();
#endif // RECT


#ifdef WINDOW_CAPTURE

	try
	{
		std::vector<cv::Mat> enemies{};

		SetupImages(enemies);

		Mat screenshot{};
		std::vector<std::chrono::milliseconds> durations{};
		std::chrono::time_point<std::chrono::system_clock> start{}, end{};
		std::cout << "----------Starting the image recognition----------\n";
		while (true)
		{
			start = std::chrono::system_clock::now();
			TakeScreenshot(screenshot);

			for (const auto& enemyImage : enemies)
			{

				Mat results{};

				matchTemplate(screenshot, enemyImage, results, TM_CCORR_NORMED);

				cv::Point min_Loc(0, 0), max_Loc(0, 0);

				double max_Val;
				minMaxLoc(results, nullptr, &max_Val, &min_Loc, &max_Loc);

				//if (max_Val < MIN_TRESHOLD)
				//	continue;


				int w = enemyImage.size[1];
				int h = enemyImage.size[0];

				rectangle(screenshot, max_Loc, cv::Point{ max_Loc.x + w, max_Loc.y + h }, { 0,255,255 });
			}


			resize(screenshot, screenshot, { 1280, 720 });

			imshow("Computer Vision", screenshot);

			end = std::chrono::system_clock::now();
			durations.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - start));

			if (waitKey(1) && GetKeyState('Q'))
				break;
		}
		std::cout << "----------Ending the image recognition----------\n";


		size_t trimSize = durations.size() / 10;
		durations.erase(durations.begin(), durations.begin() + trimSize);
		durations.erase(durations.end() - trimSize, durations.end());

		std::chrono::milliseconds sumDuration(0);
		for (const auto& duration : durations) {
			sumDuration += duration;
		}

		std::chrono::milliseconds averageDuration = sumDuration / durations.size();


		std::cout << "Average Duration: " << averageDuration.count() << " milliseconds\n";


		std::cout << "Done" << std::endl;


	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

#endif // WINDOW_CAPTURE


#endif // TESTING


	return 0;
}
