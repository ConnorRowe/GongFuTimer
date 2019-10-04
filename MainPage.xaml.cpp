//
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

	LastFrameTime = std::chrono::system_clock::now();

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
	Windows::UI::Core::CoreWindow^ appwindow = CoreWindow::GetForCurrentThread();
	appdispatcher = appwindow->Dispatcher;

	//Add window activated event handler to keep track of the window's focus status
	appwindow->Activated += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::WindowActivatedEventArgs ^>(this, &GongFuTimer::MainPage::OnActivated);

	//---------------------  Main Loop  ----------------------------------------------------------------------------------------------------------
	appdispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
	{
		while (true)
		{
			//Increment ticks
			++ticks;

			//calculate frame delta to prevent more than 30 frames from occuring every second
			delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - LastFrameTime).count();

			if (delta >= 1000.0 / 30.0)
			{
				//Run update function
				GongFuTimer::MainPage::Update();

				//reset last frame time
				LastFrameTime = std::chrono::system_clock::now();
			}

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
	//debugText = targetSeconds.ToString();
	Start();
}

//Fires every single frame
void GongFuTimer::MainPage::Update()
{
	//Only check for input if the window is focused
	if (isFocused)
	{
		//Check for enter KeyUp
		if ((Window::Current->CoreWindow->GetKeyState(Windows::System::VirtualKey::Enter) & Windows::UI::Core::CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down)
		{
			Start();
		}
	}

	//Check if timer is complete
	if (teaTimer.isRunning && teaTimer.elapsedSeconds() > targetSeconds)
	{
		//---- Timer has fired!

		teaTimer.reset();
		alarmSound->Play();
		//Increment number of infusions
		++infNumber;
		infNumText->Text = infNumber.ToString();
		//Change startbutton content
		startButton->Content = "Next Infusion";
		//Clear timer text
		minuteText->Text = "00";
		secondText->Text = "00";
		millisecondText->Text = "00";
	}

	//Splitting the second value from the timer into minutes, seconds, and milliseconds for the timer display
	float seconds = 0.0f;
	float minutes = 0.0f;
	float milliseconds = 0.0f;

	//Only update text UI stuff if window is focused
	if (isFocused)
	{
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
}

void GongFuTimer::MainPage::Start()
{
	//target Seconds = base time + (additional infusion time * infusion number)
	targetSeconds = StrToF(baseSecsComboBox->SelectedValue->ToString()) + (StrToF(infSecsComboBox->SelectedValue->ToString()) * (float)infNumber);

	//Start Timer
	teaTimer.start();
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

//Keep track of window the window focus status
void GongFuTimer::MainPage::OnActivated(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowActivatedEventArgs ^args)
{
	if (args->WindowActivationState == Windows::UI::Core::CoreWindowActivationState::Deactivated)
	{
		isFocused = false;
	}
	else
	{
		isFocused = true;
	}
}
