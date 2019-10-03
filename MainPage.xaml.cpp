﻿//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace GongFuTimer;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

	//Fill the combo boxes with all possible second values
	for (int i = 0; i <= 60; ++i)
	{
		infSecsComboBox->Items->Append(i);
		baseSecsComboBox->Items->Append(i);
	}

	infSecsComboBox->SelectedIndex = 0;
	baseSecsComboBox->SelectedIndex = 0;

	//Init timer so it displays at 0 seconds at the start
	teaTimer.reset();

	//Load alarmSound from file
	Windows::Storage::StorageFolder^ folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
	concurrency::create_task(folder->GetFileAsync("Assets\\Alarm.wav")).then(
		[this](Windows::Storage::StorageFile^ file)
	{
		if (nullptr != file)
		{
			concurrency::create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then(
				[this, file](Windows::Storage::Streams::IRandomAccessStream^ stream)
			{
				if (nullptr != stream)
				{
					alarmSound = ref new Windows::UI::Xaml::Controls::MediaElement();
					alarmSound->AutoPlay = false;
					alarmSound->SetSource(stream, file->ContentType);
				}
			});
		}
	});

	//Get Dispatcher for main loop
	CoreWindow^ appwindow = CoreWindow::GetForCurrentThread();
	appdispatcher = appwindow->Dispatcher;

	//---------------------  Main Loop  ----------------------------------------------------------------------------------------------------------
	appdispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
	{

		while (true)
		{
			//Increment ticks
			++ticks;

			//Run update function
			GongFuTimer::MainPage::Update();

			//Process any UI events if needed
			if (appdispatcher->ShouldYield())
			{
				appdispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);
			}
		}
	}));
}

//Converts the given string into a float
float GongFuTimer::MainPage::StrToF(Platform::String^ str)
{
	std::wstring textToConvert = str->Data();
	wchar_t* end;
	return std::wcstof(textToConvert.c_str(), &end);
}

//Converts the given float into the desired format, ensuring that the numbers are always two characters in width by adding a zero if it is only one figure
Platform::String ^ GongFuTimer::MainPage::FormatFloat(float f)
{
	Platform::String^ formattedStr;

	if (f < 10.0f)
	{
		formattedStr = "0" + f.ToString();
	}
	else
	{
		formattedStr = f.ToString();
	}

	return formattedStr;
}

void GongFuTimer::MainPage::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//target Seconds = base time + (additional infusion time * infusion number)
	targetSeconds = StrToF(baseSecsComboBox->SelectedValue->ToString()) + (StrToF(infSecsComboBox->SelectedValue->ToString()) * (float)infNumber);

	//debugText = targetSeconds.ToString();

	teaTimer.start();
}

void GongFuTimer::MainPage::Update()
{
	//Check if timer is complete
	if (teaTimer.isRunning && teaTimer.elapsedSeconds() > targetSeconds)
	{
		teaTimer.reset();
		alarmSound->Play();
		//Increment number of infusions
		++infNumber;
		infNumText->Text = infNumber.ToString();
		//Change startbutton content
		startButton->Content = "Next Infusion";
	}


	//Splitting the second value from the timer into minutes, seconds, and milliseconds for the timer display
	float seconds = 0.0f;
	float minutes = 0.0f;
	float milliseconds = 0.0f;
	if (teaTimer.isRunning)
	{
		seconds = targetSeconds - teaTimer.elapsedSeconds();
		if (seconds >= 60.0f)
		{
			minutes = seconds / 60.0f;
			minutes = std::floorf(minutes);
			seconds -= minutes * 60.0f;
		}
		milliseconds = (seconds - std::floorf(seconds))*100.0f;
		seconds = std::floorf(seconds);
		milliseconds = std::floorf(milliseconds);

		minuteText->Text = FormatFloat(minutes);
		secondText->Text = FormatFloat(seconds);
		millisecondText->Text = FormatFloat(milliseconds);
	}

	debugTextBlock->Text = debugText;
}

void GongFuTimer::MainPage::Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	teaTimer.reset();
	infNumber = 0;
	alarmSound->Stop();
	infNumText->Text = "0";
	//also reset startbutton text
	startButton->Content = "Start";

}